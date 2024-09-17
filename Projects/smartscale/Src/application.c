#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "timer.h"
#include "application.h"
#include "str.h"
#include "rtc_timer.h"

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
//		int len = snprintf(ui_draw.weight_str, STRING_NUM_LEN, "%ld", current_weight);
//		if(len > STRING_NUM_LEN)
//			Error_Handler();
//		ui_draw.weight_str[len] = '\0';
//		// LOG_I("ui_draw.weight_str:%s\r\n", ui_draw.weight_str);
//		set_draw_update_bit(DRAW_UPDATE_WEIGHT_BIT);

		last_weight = current_weight;
	}
}

#define WL_SET_STATE(st) do { wl.state = (st); LOG_I("[WL]State: %s (%d)\r\n", #st, st); } while (0)

#define WL_TIMEOUT_STATE(st)    if(timer_isexpired(wl.respond_timer)) WL_SET_STATE(st)

#define WL_ATRESPOND_TIMEOUT_MS         (2000)
#define WL_HEART_TIMEOUT_MS             (10*1000)


static wl_t wl = {
    .connect = false,
    .state = WL_STATE_INIT,
    .priv_dnum = 0,
    .priv_register = false,
    .send_status = false,
    .device_status = WL_DSTATUS_ZZDL,
};

volatile uint32_t priv_send_bit = 0;

void set_privsend_bit(uint32_t data_bit)
{
    priv_send_bit |= data_bit;
}

uint8_t wl_get_device_status(void)
{
    return wl.device_status;
}

#define WL_RX_BUFFER_SIZE   (256)
uint8_t wl_rx_buf[WL_RX_BUFFER_SIZE+1];

bool rx_priv_parse(int argc, char *argv[])
{
    int cmd = str_toint(argv[0]);
    // wl.priv_fnum = str_toint(argv[1]);
    switch(cmd)
    {
        case WL_PRIV_FCAIPING_CMD:// 4	服务器设置菜品
        break;
        case WL_PRIV_FQUPI_CMD:// 5	传感器操作（去皮）
        break;
        case WL_PRIV_FJIAOZHUN_CMD:// 6	传感器操作（校准）
        break;
        case WL_PRIV_FWEIGHT_CMD:// 7	传感器操作（获取重量）
        break;
        case WL_PRIV_FGETSTATUS_CMD:// 8 获取传感器稳定状态
        break;
        case WL_PRIV_FSAOMATUO_CMD:// 9	扫码头默认参数设置
        break;
        case WL_PRIV_FSETVOICE_CMD:// 10 设置音量大小
        break;
        case WL_PRIV_FHOT_CMD:// 11	设置加热状态
        break;
        case WL_PRIV_FHOTTIMER_CMD:// 12 设置加热等级
        break;
        case WL_PRIV_FREBOOT_CMD:// 13 设备重启
        break;

        case WL_PRIV_DREGISTER_RECMD:{
            wl.priv_register = true;
            wl.device_status = WL_DSTATUS_SBZC;
            set_draw_update_bit(DRAW_UPDATE_STATUS_BIT);
        }
        break;
        default:break;
    }

    return true;
}

bool wl_ctrl_cmd(int argc, char *argv[])
{
    if (strncmp((const char*)argv[0], "+CGSN", 5) == 0)
    {
        memcpy(wl.sn, argv[1], WL_SN_LEN);
        wl.sn[WL_SN_LEN] = '\0';
		return true;
    }

    if (strncmp((const char*)argv[0], "+QCCID", 6) == 0)
    {
        memcpy(wl.iccid, argv[1], WL_ICCID_LEN);
        wl.iccid[WL_ICCID_LEN] = '\0';
		return true;
    }

    if (strncmp((const char*)argv[0], "+CPIN", 6) == 0)
    {
        if (strncmp((const char*)argv[1], "READY", 5) == 0)
            return true;
        else
            return false;
    }

    if (strncmp((const char*)argv[0], "+CSQ", 4) == 0)
    {
        wl.rssi = str_toint(argv[1]);
        return true;
    }

    if (strncmp((const char*)argv[0], "+CGREG", 6) == 0)
    {
        if(argv[1][0] == '0' && (argv[2][0] == '1' || argv[2][0] == '5')) //返回正常 或漫游
            return true;
        else
            return false;
    }

    if (strncmp((const char*)argv[0], "+CEREG", 6) == 0)
    {
        if(argv[1][0] == '0' && (argv[2][0] == '1' || argv[2][0] == '5')) //返回正常 或漫游
            return true;
        else
            return false;
    }

    if (strncmp((const char*)argv[0], "+QLTS", 5) == 0)
    {
        uint32_t m_argc = 0;
        char *m_argv[ARGC_LIMIT] = { (char *)0 };
        m_argc = str_split((char*)argv[1], strlen(argv[1]), m_argv, m_argc);
#if 1
        LOG_I("[WL]margc: %d margv:", m_argc);
        for (int i = 0; i < m_argc; i++)
            LOG_I_NOTICK(" %s", m_argv[i]);
        LOG_I_NOTICK("\r\n");
#endif
        clock_date_t tm = {0};
        tm.year = str_toint(m_argv[0]);
        tm.month = str_toint(m_argv[1]);
        tm.day = str_toint(m_argv[2]);
        tm.hour = str_toint(m_argv[3]);
        tm.minute = str_toint(m_argv[4]);
        tm.second = str_toint(m_argv[5]);
        tm.timezone = 32;
        set_timestamp(date_to_seconds(&tm));
        LOG_I("tm time: %d-%d-%d %d:%d:%d timestamp:%d\r\n", 
               tm.year, tm.month, tm.day,
               tm.hour, tm.minute, tm.second,
               get_timestamp());
        return true;
    }

    if (strncmp((const char*)argv[0], "+QIACT", 6) == 0)
    {
        int len = MIN(strlen(argv[4]), WL_IP_LEN);
        memcpy(wl.ip, argv[4], len);
        wl.ip[len] = '\0';
        return true;
    }

    if (strncmp((const char*)argv[0], "+QIOPEN", 7) == 0)
    {
        if((argc >= 3) && (argv[1][0] == '0') &&  (argv[2][0] == '0'))
        {
            wl.connect = true;
            return true;
        }
        else
            return false;
    }

}

bool wl_rx_parse(parse_buffer * const input_buffer)
{
	if ((input_buffer == NULL) || (input_buffer->content == NULL))
	{
		return false; /* no input */
	}

    bool priv_data = false;
    uint32_t argc = 0;
    char *argv[ARGC_LIMIT] = { (char *)0 };

    char *ptr = buffer_at_offset(input_buffer);
    uint16_t len = input_buffer->length - input_buffer->offset;
    if(buffer_at_offset(input_buffer)[0] == '{')
    {
        priv_data = true;
        ptr++;
        if(len > 0)
            len--;
    }

    argc = str_split((char*)buffer_at_offset(input_buffer), len, argv, argc);

#if 1
    LOG_I("[WL]parse:%s \r\n", buffer_at_offset(input_buffer));
    LOG_I("[WL]argc: %d argv:", argc);
    for (int i = 0; i < argc; i++)
        LOG_I_NOTICK(" %s", argv[i]);
    LOG_I_NOTICK("\r\n");
#endif

    if(argc == 0)
        return false;

	if (priv_data)
	{
        return rx_priv_parse(argc, argv);
	}

    if ((argc == 1) && argv[0][0] == '>')
    {
        wl.send_status = true;
        return true;
    }

	if ((argc == 1) && (strncmp((const char*)argv[0], "OK", 2) == 0))
		return true;

	if ((argc == 2) && (strncmp((const char*)argv[0], "SEND", 4) == 0))
    {
        if(strncmp((const char*)argv[1], "OK", 2) == 0)
            return true;
        else
            return false;
    }

	if ((argc == 1) && (strncmp((const char*)argv[0], "ERROR", 5) == 0))
		return true;

	if ((argc == 1) && (strncmp((const char*)argv[0], "AT", 2) == 0))
		return true;

    if(argv[0][0] == '+')
    {
        return wl_ctrl_cmd(argc, argv);
    }

	/*  one number */
    if((argc == 1) && argv[0][0] >= '0' && argv[0][0] <= '9')
	{
        if(wl.state == WL_STATE_WAIT_CIMI)
        {
            memcpy(wl.imsi, (char *)argv[0], WL_IMSI_LEN);
            wl.imsi[WL_IMSI_LEN] = '\0';
            return true;
        }
	}

    LOG_I("%s error\r\n", __FUNCTION__);
    return false;
}




bool wl_rx_handle(uint8_t *buf, int16_t len)
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

            if((len == 1) && (p_buffer.offset != p_buffer.length))//特殊处理，就一个字符
            {
                result = wl_rx_parse(&p_buffer);//长度不包括\r\n
                if(result == false)
                    return result;
            }
        }
        else //已经找到head
        {
            if(p_buffer.content[p_buffer.length] == '\r' ||
               p_buffer.content[p_buffer.length] == '\0' )
            {
                p_buffer.length++;
                result = wl_rx_parse(&p_buffer);//长度不包括\r\n
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
    }while(--len >= 0);

    return true;
}


void wl_priv_send(void)
{
    if(priv_send_bit & WL_PRIVSEND_RIGISTER_BIT)
    {
        ec800e_uart_printf("{%d,%d,%s,%s,}\r\n", WL_PRIV_DREGISTER_CMD, ++wl.priv_dnum, wl.sn, wl.imsi);
    }
    else if(priv_send_bit & WL_PRIVSEND_HEART_BIT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_DXINTIAOBAO_CMD, ++wl.priv_dnum, get_timestamp());
    }
    priv_send_bit = 0;
}

void wl_priv_txrx(void)
{
    HAL_Delay(3000);
    if(wl.priv_register == false)
    {
        set_privsend_bit(WL_PRIVSEND_RIGISTER_BIT);
        ec800e_uart_printf("AT+QISEND=0\r\n");
        WL_SET_STATE(WL_STATE_PRIV_SEND);
        timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);

        wl.priv_register = true;//todu test
        return ;
    }

    if(timer_isexpired(wl.heart_timer))//心跳包
    {
        set_privsend_bit(WL_PRIVSEND_HEART_BIT);
        ec800e_uart_printf("AT+QISEND=0\r\n");
        WL_SET_STATE(WL_STATE_PRIV_SEND);
        timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        timer_start(wl.heart_timer, WL_HEART_TIMEOUT_MS);
        return ;
    }


    uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
    if(act_len && wl_rx_handle(wl_rx_buf, act_len))
    {
        // WL_SET_STATE(WL_STATE_CONNECT_FINISH);
        // break;
    }
}

void wl_task_handle(void)
{
    switch(wl.state)
    {
        case WL_STATE_INIT: {
           ec800e_clear_rx_buf();
           ec800e_uart_printf("AT\r\n");
           WL_SET_STATE(WL_STATE_WAIT_UART_CONNECT);
           timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_UART_CONNECT: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CLOSE_DIS);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_INIT);
        }
        break;
        case WL_STATE_CLOSE_DIS: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("ATE0\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CLOSE_DIS);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CLOSE_DIS: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CGSN);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_CLOSE_DIS);
        }
        break;
        case WL_STATE_CGSN: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CGSN=1\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CGSN);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CGSN: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CHECK_SIM);
            }
            WL_TIMEOUT_STATE(WL_STATE_CGSN);
        }
        break;
        case WL_STATE_CHECK_SIM: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CPIN?\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CHECK_SIM);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;    
        case WL_STATE_WAIT_CHECK_SIM: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CHECK_CSQ);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_CHECK_SIM);
        }
        break;
        case WL_STATE_CHECK_CSQ: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CSQ\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CHECK_CSQ);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CHECK_CSQ: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CIMI);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_CHECK_CSQ);
        }
        break;
        case WL_STATE_CIMI: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CIMI\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CIMI);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CIMI: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_QCCID);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_CIMI);
        }
        break;
        case WL_STATE_QCCID: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QCCID\r\n");
            WL_SET_STATE(WL_STATE_WAIT_QCCID);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QCCID: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_QICLOSE);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_QCCID);
        }
        break;
        case WL_STATE_QICLOSE: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QICLOSE=0\r\n");
            WL_SET_STATE(WL_STATE_WAIT_QICLOSE);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QICLOSE: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CGREG);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_QICLOSE);
        }
        break;
        case WL_STATE_CGREG: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CGREG?\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CGREG);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CGREG: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_CEREG);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_CGREG);
        }
        break;
        case WL_STATE_CEREG: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+CEREG?\r\n");
            WL_SET_STATE(WL_STATE_WAIT_CEREG);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CEREG: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_QLTS);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_CEREG);
        }
        break;
        case WL_STATE_QLTS: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QLTS=2\r\n");
            WL_SET_STATE(WL_STATE_WAIT_QLTS);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QLTS: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_QIACT);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_QLTS);
        }
        break;
        case WL_STATE_QIACT: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QIACT=1\r\n");
            WL_SET_STATE(WL_STATE_WAIT_QIACT);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QIACT: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_GET_QIACT);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_QIACT);
        }
        break;
        case WL_STATE_GET_QIACT: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QIACT?\r\n");
            WL_SET_STATE(WL_STATE_WAIT_GET_QIACT);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_GET_QIACT: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                WL_SET_STATE(WL_STATE_GET_QIOPEN);
                break;
            }
            WL_TIMEOUT_STATE(WL_STATE_QIACT);
        }
        break;
        case WL_STATE_GET_QIOPEN: {
            ec800e_clear_rx_buf();
            ec800e_uart_printf("AT+QIOPEN=1,0,\"TCP\",\"39.106.91.24\",10181,0,1\r\n");
            WL_SET_STATE(WL_STATE_WAIT_QIOPEN);
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
                    WL_SET_STATE(WL_STATE_TXRX);
                    break;
                }
            }
            WL_TIMEOUT_STATE(WL_STATE_GET_QIOPEN);
        }
        break;

        case WL_STATE_PRIV_SEND: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                if(wl.send_status)
                {
                    wl_priv_send();
                    ec800e_uart_printf("%c", 0x1A);
                    wl.send_status = false;
                    WL_SET_STATE(WL_STATE_TXRX);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                ec800e_uart_printf("%c", 0x1A);
                wl.send_status = false;
                WL_SET_STATE(WL_STATE_TXRX);
            }
        }
        break;
        case WL_STATE_TXRX: {
            wl_priv_txrx();
        }
        break;

        default: break;
    }
}

