#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "timer.h"
#include "application.h"
#include "str.h"

uint32_t weitht_lasttime = 0;
uint32_t last_weight = 0;

void weight_task_handle(void)
{
    if(HAL_GetTick() - weitht_lasttime < 200)
        return ;
    weitht_lasttime = HAL_GetTick();

	uint32_t current_weight = hx711_get_weight();
	if(current_weight != last_weight)
	{
		int len = snprintf(ui_draw.weight_str, STRING_NUM_LEN, "%ld", current_weight);
		if(len > STRING_NUM_LEN)
			Error_Handler();
		ui_draw.weight_str[len] = '\0';
		// LOG_I("ui_draw.weight_str:%s\r\n", ui_draw.weight_str);
		set_draw_update_bit(DRAW_UPDATE_WEIGHT_BIT);

		last_weight = current_weight;
	}
}

#define wl_set_state(st) do { wl.state = (st); LOG_I("[WL]State: %s (%d)\r\n", #st, st); } while (0)

#define WL_ATRESPOND_TIMEOUT_MS         (2000)


static wl_t wl = {
    .connect = false,
    .state = WL_STATE_INIT,
    .priv_count = 0,
    .priv_register = false,
    .send_status = false,
};

volatile uint32_t priv_send_bit = 0;

void set_privsend_bit(uint32_t data_bit)
{
  priv_send_bit |= data_bit;
}

#define WL_RX_BUFFER_SIZE   (256)
uint8_t wl_rx_buf[WL_RX_BUFFER_SIZE+1];

typedef struct
{
	const unsigned char *content;
	uint16_t length;
	uint16_t offset;
} parse_buffer;

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

#define ARGC_LIMIT      (12)

//tyepdef struct {
//	char *cmd;	/* AT指令 */
//    char *r_cmd;
//	int (*deal_func)(int opt, int argc, char *argv[]);
//}at_cmd_t;
//
//// int deal_uart_func(int opt, int argc, char *argv[]);
//
//at_cmd_t at_table[] = {
//	{"AT",              "OK",       NULL},
//	// {"ATE0",            "OK",       NULL},
//	{"AT+CGSN=1",       "+CGSN:"    NULL},// +CGSN: "862584073708935"
//    {"AT+CPIN?",        "+CPIN:",   NULL},// +CPIN: READY
//    {"AT+CMIM",         "0",       NULL},// 460081925003317
//    {"AT+CSQ",          "+CSQ:",    NULL},// +CSQ: 31,0 信号强度
//    // {"AT+QICLOSE=0",    "OK",       NULL},// ok
//    {"AT+CGREG?",       "+CGREG:",  NULL},// +CGREG: 0,1
//    {"AT+CEREG?",       "+CEREG:",  NULL},// +CEREG: 0,1
//	// AT+QICSGP=1,1,"CMIOT","","",1  OK
//	// AT+QIACT=1 OK
//	// AT+QIACT? +QIACT: 1,1,1,"10.27.82.43" OK
//	// AT+QIOPEN=1,0,"TCP","39.106.91.24",10181,0,1  OK
//	// AT+QISEND=0 \r\n hello ... SEND OK
//};

bool rx_priv_parse(const char *ptr, uint16_t len)
{
    int argc = ARGC_LIMIT;
    char *argv[ARGC_LIMIT] = { (char *)0 };
    // ptr += 1;
    argc = string_split((char*)ptr, len, ',', argv, argc);
#if 1
    LOG_I("[WL]argc: %d argv:", argc);
    for (int i = 0; i < argc; i++)
        LOG_I_NOTICK(" %s", argv[i]);
    LOG_I_NOTICK("\r\n");
#endif

    int cmd = str_toint(argv[0]);
    switch(cmd)
    {
        case WL_PRIV_DREGISTER_RECMD:
            wl.priv_register = true;
            break;
        default:break;
    }

    return true;
}

bool rx_cmd_parse(parse_buffer * const input_buffer)
{
	if ((input_buffer == NULL) || (input_buffer->content == NULL))
	{
		return false; /* no input */
	}

#if 0
    uint8_t print_buf[64];
    memcpy(print_buf, buffer_at_offset(input_buffer), input_buffer->length);
    print_buf[input_buffer->length] = '\0';
    LOG_I("[WL]parse:%s \r\n", );
#endif

	/* parse the different types of values */

	/* object */
	if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{'))
	{
        // prive
        return rx_priv_parse((buffer_at_offset(input_buffer)+1), (input_buffer->length - input_buffer->offset));
		// return parse_object(item, input_buffer);
	}

	if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '>'))
    {
        wl.send_status = true;
        return true;
    }


	if (can_read(input_buffer, 2) && (strncmp((const char*)buffer_at_offset(input_buffer), "OK", 2) == 0))
		return true;

	if (can_read(input_buffer, 7) && (strncmp((const char*)buffer_at_offset(input_buffer), "SEND OK", 7) == 0))
		return true;

	if (can_read(input_buffer, 5) && (strncmp((const char*)buffer_at_offset(input_buffer), "ERROR", 5) == 0))
		return false;

	if (can_read(input_buffer, 2) && (strncmp((const char*)buffer_at_offset(input_buffer), "AT", 2) == 0))
		return true;

    if (can_read(input_buffer, 8 + WL_SN_LEN) && (strncmp((const char*)buffer_at_offset(input_buffer), "+CGSN: \"", 8) == 0))
    {
        memcpy(wl.sn, (char *)(buffer_at_offset(input_buffer) + 8), WL_SN_LEN);
        wl.sn[WL_SN_LEN] = '\0';
		return true;
    }

    if (can_read(input_buffer, 8 + WL_ICCID_LEN) && (strncmp((const char*)buffer_at_offset(input_buffer), "+QCCID: ", 8) == 0))
    {
        memcpy(wl.iccid, (char *)(buffer_at_offset(input_buffer) + 8), WL_ICCID_LEN);
        wl.iccid[WL_ICCID_LEN] = '\0';
		return true;
    }

    if (can_read(input_buffer, 6) && (strncmp((const char*)buffer_at_offset(input_buffer), "+CPIN: READY", 12) == 0))
    {
		return true;
    }

    if (can_read(input_buffer, 6) && (strncmp((const char*)buffer_at_offset(input_buffer), "+CSQ: ", 6) == 0))
    {
        char *rssi_buf = buffer_at_offset(input_buffer) + strlen("+CSQ: ");
        wl.rssi = str_toint(rssi_buf);
		return true;
    }

    if (can_read(input_buffer, 8) && (strncmp((const char*)buffer_at_offset(input_buffer), "+CGREG: ", 8) == 0))
    {
        if(strstr((buffer_at_offset(input_buffer)+8), "0,1") || // 返回正常
           strstr((buffer_at_offset(input_buffer)+8), "0,5"))   // 返回正常，漫游
        {
            return true;
        }
		return false;
    }

    if (can_read(input_buffer, 8) && (strncmp((const char*)buffer_at_offset(input_buffer), "+CEREG: ", 8) == 0))
    {
        if(strstr((buffer_at_offset(input_buffer)+8), "0,1") || // 返回正常
           strstr((buffer_at_offset(input_buffer)+8), "0,5"))   // 返回正常，漫游
        {
            return true;
        }
		return false;
    }

    if (can_read(input_buffer, 8) && (strncmp((const char*)buffer_at_offset(input_buffer), "+QIACT: ", 8) == 0))
    {
        // +QIACT: 1,1,1,"100.110.182.156"
        int offest = str_char_offest(buffer_at_offset(input_buffer), '\"');
        char *ip = buffer_at_offset(input_buffer) + offest;
        int len = MIN(strlen(ip), WL_IP_LEN);
        memcpy(wl.ip, ip, len);
        wl.ip[len] = '\0';
		return true;
    }

    if (can_read(input_buffer, 12) && (strncmp((const char*)buffer_at_offset(input_buffer), "+QIOPEN: 0,0", 12) == 0))
    {
        // +QIOPEN: 0,0
        wl.connect = true;
		return true;
    }

	/* number */
	if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9'))))
	{
        if(wl.state == WL_STATE_WAIT_CIMI)
        {
            memcpy(wl.imsi, (char *)(buffer_at_offset(input_buffer)), WL_IMSI_LEN);
            wl.imsi[WL_IMSI_LEN] = '\0';
            return true;
        }
		// return parse_number(item, input_buffer);
	}

    LOG_I("%s error\r\n", __FUNCTION__);
    return false;
}

bool wl_rx_handle(uint8_t *buf, uint16_t len)
{
    bool result = false;
    bool index_header = false;

    parse_buffer p_buffer = {
        .content = (const unsigned char *)buf,
        .length = 0,
        .offset = 0,
    };

    do{
        if(index_header == false) //还未找到head
        {
            if(p_buffer.content[p_buffer.length] == '\0' ||
               p_buffer.content[p_buffer.length] == '\r' ||
               p_buffer.content[p_buffer.length] == '\n')
            {
                p_buffer.offset++; //filter
            }
            else
            {
                index_header = true;
            }
            p_buffer.length++;
        }
        else //已经找到head
        {
            if(p_buffer.content[p_buffer.length] == '\r' ||
               p_buffer.content[p_buffer.length] == '\0' )
            {
                p_buffer.length++;
                result = rx_cmd_parse(&p_buffer);//长度不包括\r\n
                if(result == false)
                    return result;

                p_buffer.offset = p_buffer.length;
                index_header = false;
            }
            else
            {
                p_buffer.length++;
            }
        }
    }while(--len > 1);

    return true;
}


void wl_priv_send(void)
{
    if(priv_send_bit & WL_PRIVSEND_RIGISTER_BIT)
    {
        ec800e_uart_printf("{%d,%d,%s,%s,}\r\n", WL_PRIV_DREGISTER_CMD, ++wl.priv_count, wl.sn, wl.imsi);
        
    }
    priv_send_bit = 0;
}

void wireless_task_handle(void)
{
    switch(wl.state)
    {
        case WL_STATE_INIT: {
           ec800e_clear_rx_buf();
           ec800e_uart_printf("AT\r\n");
           wl_set_state(WL_STATE_WAIT_UART_CONNECT);
           timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_UART_CONNECT: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CLOSE_DIS);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_INIT);
            }
        }
        break;
        case WL_STATE_CLOSE_DIS: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("ATE0\r\n");
            wl_set_state(WL_STATE_WAIT_CLOSE_DIS);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CLOSE_DIS: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CGSN);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CLOSE_DIS);
            }
        }
        break;
        case WL_STATE_CGSN: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CGSN=1\r\n");
            wl_set_state(WL_STATE_WAIT_CGSN);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CGSN: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CHECK_SIM);
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CGSN);
            }
        }
        break;
        case WL_STATE_CHECK_SIM: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CPIN?\r\n");
            wl_set_state(WL_STATE_WAIT_CHECK_SIM);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;    
        case WL_STATE_WAIT_CHECK_SIM: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CHECK_CSQ);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CHECK_SIM);
            }
        }
        break;
        case WL_STATE_CHECK_CSQ: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CSQ\r\n");
            wl_set_state(WL_STATE_WAIT_CHECK_CSQ);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CHECK_CSQ: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CIMI);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CHECK_CSQ);
            }
        }
        break;
        case WL_STATE_CIMI: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CIMI\r\n");
            wl_set_state(WL_STATE_WAIT_CIMI);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CIMI: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_QCCID);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CIMI);
            }
        }
        break;
        case WL_STATE_QCCID: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QCCID\r\n");
            wl_set_state(WL_STATE_WAIT_QCCID);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QCCID: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_QICLOSE);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QCCID);
            }
        }
        break;
        case WL_STATE_QICLOSE: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QICLOSE=0\r\n");
            wl_set_state(WL_STATE_WAIT_QICLOSE);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QICLOSE: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CGREG);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QICLOSE);
            }
        }
        break;
        case WL_STATE_CGREG: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CGREG?\r\n");
            wl_set_state(WL_STATE_WAIT_CGREG);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CGREG: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_CEREG);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CGREG);
            }
        }
        break;
        case WL_STATE_CEREG: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CEREG?\r\n");
            wl_set_state(WL_STATE_WAIT_CEREG);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CEREG: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_QIACT);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CEREG);
            }
        }
        case WL_STATE_QIACT: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QIACT=1\r\n");
            wl_set_state(WL_STATE_WAIT_QIACT);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QIACT: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_GET_QIACT);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QIACT);
            }
        }
        break;
        case WL_STATE_GET_QIACT: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QIACT?\r\n");
            wl_set_state(WL_STATE_WAIT_GET_QIACT);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_GET_QIACT: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_set_state(WL_STATE_GET_QIOPEN);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QIACT);
            }
        }
        break;
        case WL_STATE_GET_QIOPEN: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QIOPEN=1,0,\"TCP\",\"39.106.91.24\",10181,0,1\r\n");
            wl_set_state(WL_STATE_WAIT_QIOPEN);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QIOPEN: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                if(wl.connect == true)
                {
                    LOG_I("[WL]sim_status: %s, rssi: %d\r\n", wl.sim_status ? "fail" : "sucess", wl.rssi);
                    LOG_I("[WL]sn:%s\r\n", wl.sn);
                    LOG_I("[WL]imsi:%s\r\n", wl.imsi);
                    LOG_I("[WL]iccid:%s\r\n", wl.iccid);            
                    LOG_I("[WL]ip:%s\r\n", wl.ip);
                    wl_set_state(WL_STATE_TXRX);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_GET_QIOPEN);
            }
        }
        break;
        case WL_STATE_PRIV_ENTER_SEND: {
            ec800e_uart_printf("AT+QISEND=0\r\n");
            wl_set_state(WL_STATE_PRIV_SEND);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_PRIV_SEND: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                wl_priv_send();
                ec800e_uart_printf("%c", 0x1A);
                wl.send_status = false;
                wl_set_state(WL_STATE_TXRX);
                break;
            }
            if(timer_isexpired(wl.respond_timer))
            {
                ec800e_uart_printf("%c", 0x1A);
                wl.send_status = false;
                wl_set_state(WL_STATE_PRIV_ENTER_SEND);
            }
        }
        break;
        case WL_STATE_TXRX: {
            HAL_Delay(2000);
            if(wl.priv_register == false)
            {
                set_privsend_bit(WL_PRIVSEND_RIGISTER_BIT);
                wl_set_state(WL_STATE_PRIV_ENTER_SEND);
                break;
            }

            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                // wl_set_state(WL_STATE_CONNECT_FINISH);
                // break;
            }
        }
        break;
        default: break;
    }

}

