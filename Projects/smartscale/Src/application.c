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
    .state = WL_STATE_INIT,
};




void wireless_task_handle(void)
{
    switch(wl.state)
    {
        case WL_STATE_INIT: {
           ec800e_start_recv();
           ec800e_uart_printf("AT\r\n");
           wl_set_state(WL_STATE_WAIT_UART_CONNECT);
           timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_UART_CONNECT: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    wl_set_state(WL_STATE_CLOSE_DIS);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_INIT);
            }
        }
        break;
        case WL_STATE_CLOSE_DIS: {
            ec800e_start_recv();
            ec800e_uart_printf("ATE0\r\n");
            wl_set_state(WL_STATE_WAIT_CLOSE_DIS);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CLOSE_DIS: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    wl_set_state(WL_STATE_CGSN);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CLOSE_DIS);
            }
        }
        break;
        case WL_STATE_CGSN: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+CGSN\r\n");
            wl_set_state(WL_STATE_WAIT_CGSN);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CGSN: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    int offest = str_num_offest(g_uart_rx_frame.buf);
                    if(offest > 0)
                    {
                        memcpy(wl.sn, (char *)(g_uart_rx_frame.buf + offest), WL_SN_LEN);
                        wl.sn[WL_SN_LEN] = '\0';
                        wl_set_state(WL_STATE_CHECK_SIM);
                    }
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CGSN);
            }
        }
        case WL_STATE_CHECK_SIM: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+CPIN?\r\n");
            wl_set_state(WL_STATE_WAIT_CHECK_SIM);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;    
        case WL_STATE_WAIT_CHECK_SIM: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "+CPIN: READY")) //检查字符串中是否有OK
                {
                    wl.sim_status = true;  
                    wl_set_state(WL_STATE_CHECK_CSQ);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CHECK_SIM);
            }
        }
        break;
        case WL_STATE_CHECK_CSQ: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+CSQ\r\n");
            wl_set_state(WL_STATE_WAIT_CHECK_CSQ);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CHECK_CSQ: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    char *rssi_buf = g_uart_rx_frame.buf + strlen("+CSQ: ");
                    wl.rssi = str_toint(rssi_buf);
                    wl_set_state(WL_STATE_CIMI);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CHECK_CSQ);
            }
        }
        break;
        case WL_STATE_CIMI: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+CIMI\r\n");
            wl_set_state(WL_STATE_WAIT_CIMI);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CIMI: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    int offest = str_num_offest(g_uart_rx_frame.buf);
                    if(offest > 0)
                    {
                        memcpy(wl.imsi, (char *)(g_uart_rx_frame.buf + offest), WL_IMSI_LEN);
                        wl.sn[WL_IMSI_LEN] = '\0';
                        wl_set_state(WL_STATE_QCCID);
                    }
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CIMI);
            }
        }
        break;
        case WL_STATE_QCCID: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+QCCID\r\n");
            wl_set_state(WL_STATE_WAIT_QCCID);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QCCID: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    // +QCCID: 898604991023C0032817
                    char *buf = g_uart_rx_frame.buf + strlen("+QCCID: ");
                    uint8_t len = MIN(strlen(buf), WL_ICCID_LEN);
                    memcpy(wl.iccid, buf, len);
                    wl.iccid[len] = '\0';
                    wl_set_state(WL_STATE_QICLOSE);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QCCID);
            }
        }
        break;
        case WL_STATE_QICLOSE: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+QICLOSE=0\r\n");
            wl_set_state(WL_STATE_WAIT_QICLOSE);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QICLOSE: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    wl_set_state(WL_STATE_CGREG);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QICLOSE);
            }
        }
        break;
        case WL_STATE_CGREG: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+CGREG?\r\n");
            wl_set_state(WL_STATE_WAIT_CGREG);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CGREG: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "+CGREG: 0,1") || // 返回正常
                   strstr(g_uart_rx_frame.buf, "+CGREG: 0,5"))   // 返回正常，漫游
                {
                    wl_set_state(WL_STATE_CEREG);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CGREG);
            }
        }
        break;
        case WL_STATE_CEREG: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+CEREG?\r\n");
            wl_set_state(WL_STATE_WAIT_CEREG);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_CEREG: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "+CEREG: 0,1") || // 返回正常
                   strstr(g_uart_rx_frame.buf, "+CEREG: 0,5"))   // 返回正常，漫游
                {
                    wl_set_state(WL_STATE_QIACT);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_CEREG);
            }
        }
        case WL_STATE_QIACT: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+QIACT=1\r\n");
            wl_set_state(WL_STATE_WAIT_QIACT);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QIACT: {
            if(ec800e_wait_recv_data())
            {
                // don't check
                wl_set_state(WL_STATE_GET_QIACT);
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QIACT);
            }
        }
        break;
        case WL_STATE_GET_QIACT: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+QIACT?\r\n");
            wl_set_state(WL_STATE_WAIT_GET_QIACT);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_GET_QIACT: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    // +QIACT: 1,1,1,"100.110.182.156"
                    int offest = str_char_offest(g_uart_rx_frame.buf, '\"');
                    char *ip = g_uart_rx_frame.buf + offest;
                    int len = MIN(strlen(ip), WL_IP_LEN);
                    memcpy(wl.ip, ip, len);
                    wl.ip[len] = '\0';
                    wl_set_state(WL_STATE_GET_QIOPEN);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_QIACT);
            }
        }
        break;
        case WL_STATE_GET_QIOPEN: {
            ec800e_start_recv();
            ec800e_uart_printf("AT+QIOPEN=1,0,\"TCP\",\"39.106.91.24\",10181,0,1\r\n");
            wl_set_state(WL_STATE_WAIT_QIOPEN);
            timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
        }
        break;
        case WL_STATE_WAIT_QIOPEN: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {
                    wl_set_state(WL_STATE_CONNECT_FINISH);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                wl_set_state(WL_STATE_GET_QIOPEN);
            }
        }
        break;
        case WL_STATE_CONNECT_FINISH: {
            LOG_I("[WL]sim_status: %d, rssi: %d\r\n", wl.sim_status, wl.rssi);
            LOG_I("[WL]sn:%s\r\n", wl.sn);
            LOG_I("[WL]imsi:%s\r\n", wl.imsi);
            LOG_I("[WL]iccid:%s\r\n", wl.iccid);            
            LOG_I("[WL]ip:%s\r\n", wl.ip);
            ec800e_start_recv();    
            wl_set_state(WL_STATE_TXRX);
        }
        break;
        case WL_STATE_TXRX: {
            if(ec800e_wait_recv_data())
            {
                if(strstr(g_uart_rx_frame.buf, "OK")) //检查字符串中是否有OK
                {

                }
                ec800e_start_recv();
            }
            ec800e_uart_printf("AT+CSQ\r\n");
            HAL_Delay(2000);
        }
        break;
        default: break;
    }


    // if(HAL_GetTick() - ec_lasttime < 2000)
    //     return ;
    // ec_lasttime = HAL_GetTick();
    // ec800e_uart_printf("AT\r\n");
}

