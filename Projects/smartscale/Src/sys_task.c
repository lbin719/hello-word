#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "timer.h"
#include "sys_task.h"
#include "str.h"
#include "rtc_timer.h"
#include "mj8000.h"
#include "wtn6040.h"
#include "wl_priv_data.h"
#include "wl_task.h"
#include "cmsis_os.h"

static osThreadId Sys_ThreadHandle;

sys_status_e sys_status = SYS_STATUS_ZZDL;


uint32_t update_timer = 0;
int current_weight = 0;
int change_weight = 0;
int last_dis_weight = 0;

int get_change_weight(void)
{
    return change_weight;
}

void get_weight_period(void)
{
	int weight = hx711_get_weight();
    if(abs(weight - current_weight) > caiping_data.zhendongwucha)
    {
        change_weight = weight - current_weight;
        current_weight = weight;
    }
}

#define MJ_STATUS_IDLE              (0)
#define MJ_STATUS_BUHUO             (1)
#define MJ_STATUS_BANGPAN           (2)

#define MJ_BP_LIVE_TIMEOUT          (1500) 
#define MJ_STR_MAX_LEN              (MJ8000_UART_RX_BUF_SIZE)
char mj_str[MJ_STR_MAX_LEN + 1];
uint8_t mj_len = 0;
uint32_t mj_bp_time = 0;

uint8_t mj_status = MJ_STATUS_IDLE;

static osTimerId mj_timehandle = NULL;

static void mj_ostimercallback(void const * argument)
{
    (void) argument;

    mj_status = MJ_STATUS_IDLE;
    LOG_I("[MJ] timeout exit\r\n");
}

void mj8000_rx_parse(const char *buf, uint16_t len)
{
    uint16_t ptr_len = MIN(MJ_STR_MAX_LEN, len);
    memcpy(mj_str, buf, ptr_len);
    mj_str[ptr_len] = '\0';

    if(strstr((const char*)buf,"user")) // 补货
    {
        if(mj_status != MJ_STATUS_BUHUO)
            mj_status = MJ_STATUS_BUHUO;
        else
            mj_status = MJ_STATUS_IDLE;
        sys_ossignal_notify(SYS_NOTIFY_MJBUHUO_BIT);
        LOG_I("[MJ] buhuo\r\n");
        return ;
    }
    else if(strstr((const char*)buf,"zh")) // 绑盘
    {
        osTimerStart(mj_timehandle, MJ_BP_LIVE_TIMEOUT);
        mj_status = MJ_STATUS_BANGPAN;
        LOG_I("[MJ] pangpan\r\n");
        return ;
    }
}

void mj_rx_handle(void)
{
    if(mj_uart_rx_frame.finsh)
    {
        mj_uart_rx_frame.buf[mj_uart_rx_frame.len] = '\0';
        LOG_I("[MJ]recv len:%d,data:%s\r\n", mj_uart_rx_frame.len, mj_uart_rx_frame.buf);
        mj8000_rx_parse(mj_uart_rx_frame.buf, mj_uart_rx_frame.len);

        mj_uart_rx_frame.finsh = 0;
        uart4_recive_dma(mj_uart_rx_frame.buf, MJ8000_UART_RX_BUF_SIZE);
    }
}

bool send_banpan_cmd = false;

void sys_task_handle(void)
{
    if(sys_status == SYS_STATUS_ZZDL || sys_status == SYS_STATUS_SBLX)
    {
        if(change_weight)
        {
            ui_ossignal_notify(UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
        }

        if(wl.priv_register)//注册成功
        {
            sys_status = SYS_STATUS_SBZC;
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
        }
    }
    else if(sys_status == SYS_STATUS_SBZC)
    {
        if(mj_status == MJ_STATUS_BUHUO)
        {
            sys_status = SYS_STATUS_BHZ;
            // todu 显示重量清零
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            wtn6040_play(WTN_KSBH_PLAY);
            return ;
        }
        else if(mj_status == MJ_STATUS_BANGPAN)
        {
            // 发送绑盘指令
            send_banpan_cmd = true;
            // sys_status = SYS_STATUS_BHZ;
            // // todu 显示重量清零
            // ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            // wtn6040_play(WTN_KSBH_PLAY);
            return ;
        }

        if(change_weight)
        {
            sys_status = SYS_STATUS_QQFHCP;
            ui_ossignal_notify(UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT | UI_NOTIFY_STATUS_BIT);
            wtn6040_play(WTN_WSBDCP_PLAY);
        }
    }
    else if(sys_status == SYS_STATUS_BHZ)
    {
        if(mj_status == MJ_STATUS_IDLE)
        {
            wtn6040_play(WTN_BHWC_PLAY);
            sys_status = SYS_STATUS_SBZC;
            return ;
        }
    }
    else if(sys_status == SYS_STATUS_QQFHCP)
    {
        if(timer_isexpired(update_timer))
        {
            //上报服务器
            change_weight = 0;
            sys_status = SYS_STATUS_SBZC;
            ui_ossignal_notify(UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT | UI_NOTIFY_STATUS_BIT);
        }
    }
}




uint8_t get_sys_status(void)
{
    return sys_status;
}

int32_t sys_ossignal_notify(int32_t signals)
{
    if(!Sys_ThreadHandle)
        return -1;

    return osSignalSet(Sys_ThreadHandle, signals);
}

void SYS_Thread(void const *argument)
{
    osEvent event = {0};

    while(1) 
    {
        event = osSignalWait(SYS_TASK_NOTIFY, 200);
        if(event.status == osEventSignal)
        {
            if(event.value.signals & SYS_NOTIFY_FCT_BIT)
            {
                fct_task_handle();
            }

            if(get_stmencrypt_status() == false)// 解密失败，后面的模块不运行
            {
                LOG_I("warnning: key error\r\n");
                osDelay(5000);
                continue;
            }

            if(event.value.signals & SYS_NOTIFY_MJRX_BIT)
            {
                mj_rx_handle();
            }

            if(event.value.signals & SYS_NOTIFY_MJBUHUO_BIT)
            {
                if(sys_status == SYS_STATUS_SBZC)
                {
                    sys_status = SYS_STATUS_BHZ;
                    wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUO_BIT);
                    ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
                    wtn6040_play(WTN_KSBH_PLAY);
                }
                else if(sys_status == SYS_STATUS_BHZ)
                {
                    sys_status = SYS_STATUS_SBZC;
                    wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUOEND_BIT);
                    ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
                    wtn6040_play(WTN_BHWC_PLAY);
                }
            }
            
            if(event.value.signals & SYS_NOTIFY_MJBANPANG_BIT)
            {
                if(sys_status == SYS_STATUS_SBZC)
                {

                }
            } 

            if(event.value.signals & SYS_NOTIFY_WLREGISTER_BIT)
            {
                if(sys_status == SYS_STATUS_ZZDL || sys_status == SYS_STATUS_SBLX)
                {
                    sys_status = SYS_STATUS_SBZC;
                    ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
                }
            }

            if(event.value.signals & SYS_NOTIFY_WLLX_BIT)
            {
                sys_status = SYS_STATUS_SBLX;
                ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            }

        }
       else if(event.status == osEventTimeout)
       {
            get_weight_period(); // input
       }
    }
}

void sys_init(void)
{
    osTimerDef(mj_timer, mj_ostimercallback);
    mj_timehandle = osTimerCreate(osTimer(mj_timer), osTimerOnce, NULL);
    assert_param(mj_timehandle);

  osThreadDef(SYSThread, SYS_Thread, osPriorityAboveNormal, 0, 256);
  Sys_ThreadHandle = osThreadCreate(osThread(SYSThread), NULL);
}
