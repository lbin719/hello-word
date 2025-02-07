#include "wl_task.h"
#include "sys_task.h"
#include "cmsis_os.h"
#include "rtc_timer.h"

#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "sys_task.h"
#include "str.h"
#include "rtc_timer.h"
#include "mj8000.h"
#include "wtn6040.h"
#include "wl_priv_data.h"
#include "wl_task.h"
#include "stdlib.h"


#define WL_RX_BUFFER_SIZE   (256)
uint8_t wl_rx_buf[WL_RX_BUFFER_SIZE+1];

static bool wl_get_cimi = false;
static osThreadId WL_ThreadHandle;

int32_t wl_ossignal_notify(int32_t signals)
{
    if(!WL_ThreadHandle)
    {
        return -1;
    }

    return osSignalSet(WL_ThreadHandle, signals);
}


#define WL_ATRESPOND_TIMEOUT_MS         (2000)
#define WL_HEART_TIMEOUT_MS             (60*1000)

wl_t wl = {
    .status = 0,
    .rssi = 99,
};

volatile uint8_t priv_send_event = 0;

void wl_set_status_bit(uint32_t status)
{
    wl.status |= status;
}

bool wl_get_status_bit(uint32_t status)
{
    return (wl.status & status) ? true : false;
}

void wl_clear_status_bit(uint32_t status)
{
    wl.status &= ~status;
}

bool wl_ctrl_cmd(int argc, char *argv[])
{
    if (strncmp((const char*)argv[0], "+QIURC", 5) == 0)
    {
        if(argc == 4 && strcmp((const char*)argv[1], "recv") == 0)
        {
//            uint16_t recv_len = atoi(argv[3]);
            wl_set_status_bit(WL_STATUS_RECV_BIT);
            return true;
        }

        if(argc == 3 && strcmp((const char*)argv[1], "closed") == 0)
        {
            LOG_I("wl close and reset\r\n");
            wl_ossignal_notify(WL_NOTIFY_WLRESET_BIT);
            return true;
        }
    }

    if (strncmp((const char*)argv[0], "+CGSN", 5) == 0)
    {
        memcpy(wl.sn, argv[1], WL_SN_LEN);
        wl.sn[WL_SN_LEN] = '\0';
        wl_set_status_bit(WL_STATUS_CGSN_BIT);
		return true;
    }

    if (strncmp((const char*)argv[0], "+QCCID", 6) == 0)
    {
        memcpy(wl.iccid, argv[1], WL_ICCID_LEN);
        wl.iccid[WL_ICCID_LEN] = '\0';
        wl_set_status_bit(WL_STATUS_QCCID_BIT);
		return true;
    }

    if (strncmp((const char*)argv[0], "+CPIN", 6) == 0)
    {
        if (strncmp((const char*)argv[1], "READY", 5) == 0)
        {
            wl_set_status_bit(WL_STATUS_CPIN_BIT);
            return true;
        }
        else
        {
            wl_clear_status_bit(WL_STATUS_CPIN_BIT);
            return false;
        }
    }

    if (strncmp((const char*)argv[0], "+CSQ", 4) == 0)
    {
        wl.rssi = atoi(argv[1]);
        if(wl.rssi < 99)
        {
            wl_set_status_bit(WL_STATUS_CSQ_BIT);
            ui_ossignal_notify(UI_NOTIFY_SIGNEL_BIT);
        }
        return true;
    }

    if (strncmp((const char*)argv[0], "+CGREG", 6) == 0)
    {
        if(argv[1][0] == '0' && (argv[2][0] == '1' || argv[2][0] == '5')) //返回正常 或漫游
        {
            wl_set_status_bit(WL_STATUS_CGREG_BIT);
            ui_ossignal_notify(UI_NOTIFY_SIGNEL_BIT);
            return true;
        }
        else
        {
            wl_clear_status_bit(WL_STATUS_CGREG_BIT);
            ui_ossignal_notify(UI_NOTIFY_SIGNEL_BIT);
            return false;
        }
    }

    if (strncmp((const char*)argv[0], "+CEREG", 6) == 0)
    {
        if(argv[1][0] == '0' && (argv[2][0] == '1' || argv[2][0] == '5')) //返回正常 或漫游
        {
            wl_set_status_bit(WL_STATUS_CEREG_BIT);
            return true;
        }
        else
        {
            wl_clear_status_bit(WL_STATUS_CEREG_BIT);
            return false;
        }
    }

    if (strncmp((const char*)argv[0], "+QLTS", 5) == 0)
    {
        uint32_t m_argc = 0;
        char *m_argv[ARGC_LIMIT] = { (char *)0 };
        m_argc = str_split((char*)argv[1], strlen(argv[1]), m_argv, 0);
#if 1
        LOG_I("[WL]margc: %d margv:", m_argc);
        for (int i = 0; i < m_argc; i++)
            LOG_I_NOTICK(" %s", m_argv[i]);
        LOG_I_NOTICK("\r\n");
#endif
        clock_date_t tm = {0};
        tm.year = atoi(m_argv[0]);
        tm.month = atoi(m_argv[1]);
        tm.day = atoi(m_argv[2]);
        tm.hour = atoi(m_argv[3]);
        tm.minute = atoi(m_argv[4]);
        tm.second = atoi(m_argv[5]);
        tm.timezone = 32;
        set_timestamp(date_to_seconds(&tm));
        LOG_I("tm time: %d-%d-%d %d:%d:%d timestamp:%d\r\n", 
               tm.year, tm.month, tm.day,
               tm.hour, tm.minute, tm.second,
               get_timestamp());
        wl_set_status_bit(WL_STATUS_QLTS_BIT);
        return true;
    }

    if (strncmp((const char*)argv[0], "+QIACT", 6) == 0)
    {
        int len = MIN(strlen(argv[4]), WL_IP_LEN);
        memcpy(wl.ip, argv[4], len);
        wl.ip[len] = '\0';
        wl_set_status_bit(WL_STATUS_QIACT_BIT);
        return true;
    }

    if (strncmp((const char*)argv[0], "+QIOPEN", 7) == 0)
    {
        if((argc >= 3) && (argv[1][0] == '0') &&  (argv[2][0] == '0'))
        {
            wl_set_status_bit(WL_STATUS_QIOPEN_BIT);
            return true;
        }
        else
        {
            // wl_clear_status_bit(WL_STATUS_QIOPEN_BIT);
            return false;
        }
    }

    if ((argc >= 3) && (strncmp((const char*)argv[0], "+CME", 7) == 0) && (strncmp((const char*)argv[1], "ERROR", 7) == 0))
    {
        wl.cme_error = atoi(argv[1]);
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

    argc = str_split((char*)ptr, len, argv, 0);
    if(argc == 0)
    {
        LOG_I("[WL]str_split error\r\n");
        return false;
    }

#if 0
    LOG_I("[WL]argc: %d argv:", argc);
    for (int i = 0; i < argc; i++)
        LOG_I_NOTICK(" %s", argv[i]);
    LOG_I_NOTICK("\r\n");
#endif


	if(priv_data) // 私有协议
	{
        return wl_priv_rx_parse(argc, argv);
	}

    if ((argc == 1) && argv[0][0] == '>')
    {
        wl_set_status_bit(WL_STATUS_ENTERSEND_BIT);
        return true;
    }

	if ((argc == 1) && (strncmp((const char*)argv[0], "OK", 2) == 0))
		return true;

	if ((argc == 2) && (strncmp((const char*)argv[0], "SEND", 4) == 0))
    {
        if(strncmp((const char*)argv[1], "OK", 2) == 0)
        {
            wl_set_status_bit(WL_STATUS_SENDFINSH_BIT);
            return true;
        }
        else
        {
            wl_clear_status_bit(WL_STATUS_SENDFINSH_BIT);
            return false;
        }
    }

	if ((argc == 1) && (strncmp((const char*)argv[0], "ERROR", 5) == 0))
    {
		return true;
    }

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
        {
            memcpy(wl.imsi, (char *)argv[0], WL_IMSI_LEN);
            wl.imsi[WL_IMSI_LEN] = '\0';
            wl_set_status_bit(WL_STATUS_CIMI_BIT);
            return true;
        }
	}

    LOG_I("%s error\r\n", __FUNCTION__);
    return false;
}

bool wl_rx_handle(uint8_t *buf, int16_t len)
{
	uint8_t *ptr = NULL;
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
                if(wl_rx_parse((char *)ptr, ptr_len) == false)//长度不包括\r\n
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
        if(wl_rx_parse((char *)ptr, ptr_len) == false)//长度不包括\r\n
            return false;
    }

    return true;
}


void wl_event_clear(void)
{
    ec800e_clear_rx_buf();

    osEvent event = {0};
    event = osSignalWait(UI_TASK_NOTIFY, 1);
    if(event.status == osEventSignal)
    {

    }
}

static bool wl_module_init(void)
{
    uint16_t retry_cnt;
    uint32_t act_len;

    wl.status = 0;
    wl.cme_error = 0;

    ec800e_hw_reset();
    // 通信检测
    retry_cnt = 10;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        if(ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE))
        {
            if(strstr((const char*)wl_rx_buf,(const char*)"OK"))
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
        osDelay(500);
    }while(1);

    // 关回显
    retry_cnt = 5;
    wl_event_clear();
    do{
        ec800e_uart_printf("ATE0\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
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
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+CGSN=1\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_CGSN_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 检测SIM卡
    retry_cnt = 20;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+CPIN?\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_CPIN_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
        osDelay(500);
    }while(1);

    // 获取SIM卡编号
    retry_cnt = 5;
    wl_event_clear();
    do{
        wl_get_cimi = true;
        ec800e_uart_printf("AT+CIMI\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_CIMI_BIT)
                break;
        }
        if(--retry_cnt == 0)
        {
            wl_get_cimi = false;
            goto exit;
        }
    }while(1);
    wl_get_cimi = false;

    // 获取信号强度
    retry_cnt = 20;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+CSQ\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_CSQ_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
        osDelay(500);
    }while(1);

    // 获取网络状态
    retry_cnt = 10;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CGREG?\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_CGREG_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
        osDelay(1000);
    }while(1);

    // 获取网络状态
    retry_cnt = 10;
    do{
        wl_event_clear();
        ec800e_uart_printf("AT+CEREG?\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_CEREG_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
        osDelay(1000);
    }while(1);

    // 获取网络时间
    retry_cnt = 5;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+QLTS=2\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_QLTS_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 断开连接
    retry_cnt = 5;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+QICLOSE=1\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        if(ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE))
        {
            if(strstr((const char*)wl_rx_buf,(const char*)"OK"))
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 激活 PDP 场景
    retry_cnt = 3;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+QIACT=1\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        if(ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE))
        {
            if(strstr((const char*)wl_rx_buf,(const char*)"OK"))
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 查询 PDP 场景
    retry_cnt = 5;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+QIACT?\r\n");
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_QIACT_BIT)
                break;
        }
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    // 打开 Socket 服务
    retry_cnt = 5;
    wl_event_clear();
    do{
        ec800e_uart_printf("AT+QIOPEN=1,0,\"TCP\",\"120.79.137.150\",10181,0,1\r\n");
wait_qiopen:
        osSignalWait(WL_NOTIFY_RECEIVE_BIT, WL_WAIT_RECEIVE_TIMEOUT); //wait receive
        act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
        if(act_len)
        {
            wl_rx_handle(wl_rx_buf, act_len);
            if(wl.status & WL_STATUS_QIOPEN_BIT)
                break;
            else
                goto wait_qiopen;
        }
            
        if(--retry_cnt == 0)
            goto exit;
    }while(1);

    LOG_I("[WL]sim: %s, rssi: %d\r\n", wl.status & WL_STATUS_CPIN_BIT ? "success" : "fail", wl.rssi);
    LOG_I("[WL]sn:%s\r\n", wl.sn);
    LOG_I("[WL]imsi:%s\r\n", wl.imsi);           
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
    if(!wl_module_init())
    {
        sys_ossignal_notify(SYS_NOTIFY_WLLX_BIT);
        osDelay(20000);
        LOG_I("module reset\r\n");
        goto wl_reset;
    }
    else
    {
        sys_ossignal_notify(SYS_NOTIFY_WLLJ_BIT); // 设备连接成功
        wl_ossignal_notify(WL_NOTIFY_PRIVSEND_RIGISTER_BIT); // 发送注册指令
    }

    while(1)
    {
        event = osSignalWait(WL_TASK_NOTIFY, osWaitForever);
        if(event.status == osEventSignal)
        {
            if(event.value.signals & WL_NOTIFY_RECEIVE_BIT) // 接收到数据包处理
            {
                uint32_t act_len = ec800e_get_rx_buf(wl_rx_buf, WL_RX_BUFFER_SIZE);
                if(act_len && wl_rx_handle(wl_rx_buf, act_len))
                {
                }
            }

            if(event.value.signals & WL_NOTIFY_PRIVSEND_RIGISTER_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_RIGISTER_EVENT);
            }
            if(event.value.signals & WL_NOTIFY_PRIVSEND_BUHUO_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_BUHUO_EVENT);
            }
            if(event.value.signals & WL_NOTIFY_PRIVSEND_BUHUOEND_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_BHWEIGHT_EVENT);
            }            
            if(event.value.signals & WL_NOTIFY_PRIVSEND_BANGPAN_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_BANGPAN_EVENT);
            }
            if(event.value.signals & WL_NOTIFY_PRIVSEND_BANGPANEND_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_BPWEIGHT_EVENT);
            }
            if(event.value.signals & WL_NOTIFY_PRIVSEND_IWEIGHT_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_IWEIGHT_EVENT);
            }
            if(event.value.signals & WL_NOTIFY_PRIVSEND_HOTST_BIT)
            {
                wl_priv_tx(WL_PRIVSEND_HOTST_EVENT);
            }
            
            if(event.value.signals & WL_NOTIFY_PRIVSEND_HEART_BIT)
            {
                // ec800e_uart_printf("AT+CSQ\r\n"); // 获取信号强度
                wl_priv_tx(WL_PRIVSEND_HEART_EVENT);
            }   

            if(event.value.signals & WL_NOTIFY_WLRESET_BIT)
            {
                goto wl_reset;
            }  
                    
        }
    }
}

void wl_init(void)
{
    wlpriv_init();

    osThreadDef(WLThread, WL_Thread, osPriorityAboveNormal, 0, 512);
    WL_ThreadHandle = osThreadCreate(osThread(WLThread), NULL);
}
