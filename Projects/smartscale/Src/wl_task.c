#include "wl_task.h"
#include "application.h"
#include "cmsis_os.h"
#include "rtc_timer.h"

#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "timer.h"
#include "application.h"
#include "str.h"
#include "rtc_timer.h"
#include "mj8000.h"
#include "wtn6040.h"
#include "wl_priv_data.h"
#include "wl_task.h"

static osThreadId WL_ThreadHandle;
static bool wl_moudle_status = false;
static bool wl_get_cimi = false;
#define WL_RX_BUFFER_SIZE   (256)
uint8_t wl_rx_buf[WL_RX_BUFFER_SIZE+1];

int32_t wl_ossignal_notify(int32_t signals)
{
    if(!WL_ThreadHandle)
    {
        return -1;
    }

    return osSignalSet(WL_ThreadHandle, signals);
}

#define WL_SET_STATE(st) do { wl.state = (st); LOG_I("[WL]State: %s (%d)\r\n", #st, st); } while (0)

#define WL_TIMEOUT_STATE(st)    if(timer_isexpired(wl.respond_timer)) WL_SET_STATE(st)

#define WL_ATRESPOND_TIMEOUT_MS         (2000)
#define WL_HEART_TIMEOUT_MS             (60*1000)


wl_t wl = {
    .connect = false,
    .state = WL_STATE_INIT,
    .priv_dnum = 0,
    .priv_register = false,
    .send_status = false,
    .device_status = SYS_STATUS_ZZDL,
};

volatile uint8_t priv_send_event = 0;

bool wl_ctrl_cmd(int argc, char *argv[])
{
    if (strncmp((const char*)argv[0], "+QIURC", 5) == 0)
    {
        if(argc == 4 && strcmp((const char*)argv[1], "recv") == 0)
        {
            uint16_t recv_len = str_toint(argv[3]);
            return true;
        }

        if(argc == 3 && strcmp((const char*)argv[1], "closed") == 0)
        {
            return true;
        }
    }

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

    if ((argc >= 3) && (strncmp((const char*)argv[0], "+CME", 7) == 0) && (strncmp((const char*)argv[1], "ERROR", 7) == 0))
    {
        if(strncmp((const char*)argv[1], "10", 2) == 0)
        {
            // 未插入 SIM 卡
        }
        return false;
    }

    return false;
}

bool wl_rx_parse(char *ptr, uint16_t len)
{
    bool priv_data = false;
    uint32_t argc = 0;
    char *argv[ARGC_LIMIT] = { (char *)0 };

	if ((ptr == NULL) || (len == 0))
		return false; /* no input */

#if 0
    LOG_I("[WL]parse: len:%d ptr:%s \r\n", len, ptr);
#endif
    if(ptr[0] == '{')
        priv_data = true;

    argc = str_split((char*)ptr, len, argv, argc);
#if 1
    LOG_I("[WL]argc: %d argv:", argc);
    for (int i = 0; i < argc; i++)
        LOG_I_NOTICK(" %s", argv[i]);
    LOG_I_NOTICK("\r\n");
#endif

    if(argc == 0)
        return false;

	if(priv_data) // 私有协议
	{
        return wl_rx_priv_parse(argc, argv);
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
        {
            wl.wait_send_status = true;
            return true;
        }
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
    	if(wl_get_cimi)
//        if(wl.state == WL_STATE_WAIT_CIMI)
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
    char *ptr = NULL;
    uint16_t ptr_len = 0;

    do{
        if(ptr == NULL)
        {
            if(*buf != '\0' && *buf != '\r' && *buf != '\n')
            {
                ptr = buf;
                ptr_len++;
            }
        }
        else
        {
             if(*buf == '\r')
             {
                if(wl_rx_parse(ptr, ptr_len) == false)//长度不包括\r\n
                    return false;

                ptr = NULL;
                ptr_len = 0;
             }
             else
             {
                ptr_len++;
             }
        }
        buf++;
    }while(--len);

    if(ptr != NULL && ptr_len > 0)
    {
        if(wl_rx_parse(ptr, ptr_len) == false)//长度不包括\r\n
            return false;
    }

    return true;
}

void wl_set_priv_send(uint8_t event)
{
    priv_send_event = event;
    ec800e_uart_printf("AT+QISEND=0\r\n");
    wl.send_status = false;
    WL_SET_STATE(WL_STATE_PRIV_SEND);
    timer_start(wl.respond_timer, WL_ATRESPOND_TIMEOUT_MS);
}

void wl_priv_send(void)
{
// #define WL_PRIV_DBUHUO_RECMD            (1 + 128) // 1	设备发起补货
// #define WL_PRIV_DWEIGHT_RECMD           (2 + 128) // 2	设备传感器重量变化上报
// #define WL_PRIV_DUSER_RECMD             (3 + 128) // 3	设备发起用户绑盘称重
// #define WL_PRIV_DREGISTER_RECMD         (14 + 128) // 14	设备注册
// #define WL_PRIV_DXINTIAOBAO_RECMD       (15 + 128) // 15	设备发送心跳包
    if(priv_send_event == WL_PRIVSEND_RIGISTER_EVENT)
    {
        //test
        ec800e_uart_printf("{%d,%d,862584075695577,460074425636505,}\r\n", WL_PRIV_DREGISTER_CMD, ++wl.priv_dnum);
        // ec800e_uart_printf("{%d,%d,%s,%s,}\r\n", WL_PRIV_DREGISTER_CMD, ++wl.priv_dnum, wl.sn, wl.imsi);
    }
    else if(priv_send_event == WL_PRIVSEND_HEART_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_DXINTIAOBAO_CMD, ++wl.priv_dnum, get_timestamp());
    }

    else if(priv_send_event == WL_PRIVRSEND_SETCAIPING_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_DXINTIAOBAO_CMD, wl.priv_fnum, wl.respond_result);
    }


    else if(priv_send_event == WL_PRIVRSEND_QUPI_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FQUPI_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(priv_send_event == WL_PRIVRSEND_JIAOZHUN_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FJIAOZHUN_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(priv_send_event == WL_PRIVRSEND_GETWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,}\r\n", WL_PRIV_FWEIGHT_RECMD, wl.priv_fnum, wl.respond_result, 152); //todu
    }
    else if(priv_send_event == WL_PRIVRSEND_GETSTATUS_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,}\r\n", WL_PRIV_FGETSTATUS_RECMD, wl.priv_fnum, wl.respond_result, 0);
    }    
    else if(priv_send_event == WL_PRIVRSEND_SETSAOMATOU_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FSAOMATUO_RECMD, wl.priv_fnum, wl.respond_result);
    }   
    else if(priv_send_event == WL_PRIVRSEND_SETVOICE_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FSETVOICE_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(priv_send_event == WL_PRIVRSEND_SETHOT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FHOT_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(priv_send_event == WL_PRIVRSEND_SETHOTTIMER_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FHOTTIMER_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(priv_send_event == WL_PRIVRSEND_REBOOT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FREBOOT_RECMD, wl.priv_fnum, wl.respond_result);

        LOG_I("system reboot\r\n");
        HAL_Delay(500);
        NVIC_SystemReset();
    }        
    priv_send_event = 0;
}

void wl_priv_txrx(void)
{
    if(wl.priv_register == false)
    {
        wl_set_priv_send(WL_PRIVSEND_RIGISTER_EVENT);

        wl.priv_register = true;
        return ;
    }

    if(timer_isexpired(wl.heart_timer))//心跳包
    {
        wl_set_priv_send(WL_PRIVSEND_HEART_EVENT);

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

#if 0
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
            WL_TIMEOUT_STATE(WL_STATE_QICLOSE);// 跳转到close
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
                    wl.wait_send_status = false;
                    WL_SET_STATE(WL_STATE_PRIV_WAIT_SEND);
                    break;
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
                ec800e_uart_printf("%c", 0x1A);
                WL_SET_STATE(WL_STATE_TXRX);
            }
        }
        break;
        case WL_STATE_PRIV_WAIT_SEND: {
            uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
            if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            {
                if(wl.wait_send_status)
                {
                    WL_SET_STATE(WL_STATE_TXRX);
                }
            }
            if(timer_isexpired(wl.respond_timer))
            {
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
#endif

// #define WL_RETRY_
void wl_event_clear(void)
{
    ec800e_clear_rx_buf();

    osEvent event = {0};
    event = osSignalWait(UI_TASK_NOTIFY, 1);
    if(event.status == osEventSignal)
    {

    }
}

#define WL_WAIT_RECEIVE_TIMEOUT      (500)

static bool module_init(void)
{
    osEvent event = {0};

    char *strx;
    uint16_t retry_cnt;
    uint32_t act_len;

    // 通信检测
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        if(ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE))
        {
            if(strstr((const char*)wl_rx_buf,(const char*)"OK"))
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 关回显
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("ATE0\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        if(ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE))
        {
            if(strstr((const char*)wl_rx_buf,(const char*)"OK"))
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 获取设备SN
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CGSN=1\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 检测SIM卡
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CPIN?\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 获取SIM卡编号
    retry_cnt = 5;
    do{
        wl_event_clear();
        wl_get_cimi = true;
        ec800e_uart_printf("AT+CIMI\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
        {
            wl_get_cimi = false;
            goto exit;
        }
    }while(1);
    wl_get_cimi = false;

    // 获取信号强度
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CSQ\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 断开连接
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+QICLOSE=1\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 获取网络状态
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CGREG?\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
        osDelay(500);
    }while(1);

    // 获取网络状态
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CEREG?\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
        osDelay(500);
    }while(1);

    // 获取网络时间
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+QLTS=2\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 激活 PDP 场景
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+QIACT=1\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 查询 PDP 场景
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+QIACT?\r\n");
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
            break;

        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 打开 Socket 服务
    retry_cnt = 5;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+QIOPEN=1,0,\"TCP\",\"39.106.91.24\",10181,0,1\r\n");
wait_qiopen:
        event = osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len && wl_rx_handle(wl_rx_buf, act_len))
        {
            if(wl.connect)
                break;
            else
                goto wait_qiopen;
        }
            
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    LOG_I("[WL]sim_status: %s, rssi: %d\r\n", wl.sim_status ? "fail" : "sucess", wl.rssi);
    LOG_I("[WL]sn:%s\r\n", wl.sn);
    LOG_I("[WL]imsi:%s\r\n", wl.imsi);
    LOG_I("[WL]iccid:%s\r\n", wl.iccid);            
    LOG_I("[WL]ip:%s\r\n", wl.ip);
    LOG_I("[WL]init success\r\n");
    return true;

exit:
    LOG_I("[WL]init fail\r\n");
    return false;
}

void WL_Thread(void const *argument)
{
    osEvent event = {0};

wl_reset:
    if(module_init() == false)
    {
        osDelay(20000);
        LOG_I("module reset\r\n");
        goto wl_reset;
    }

    while(1)
    {
        event = osSignalWait(UI_TASK_NOTIFY, UI_TASK_DELAY);
        if(event.status == osEventSignal)
        {
            if(event.value.signals == WL_NOTIFY_RECEIVE_BIT)
            {
                // wl.wait_send_status = true;
                // WL_SET_STATE(WL_STATE_PRIV_SEND);
            }
 
        }
    }
}


void wl_init(void)
{

  osThreadDef(WLThread, WL_Thread, osPriorityAboveNormal, 0, 512);
  WL_ThreadHandle = osThreadCreate(osThread(WLThread), NULL);
}
