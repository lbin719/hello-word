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

#define SYS_BPLIVE_TIMEOUT          (1500) 
#define SYS_WEIGHT_TIMEOUT          (200) 

#define SYS_IWEIGHT_TIMEOUT         (4 * 5)// 4s
#define SYS_BWEIGHT_TIMEOUT         (8 * 5)// 8s

static osThreadId Sys_ThreadHandle;
static osTimerId bp_timehandle = NULL;
static osTimerId weight_timehandle = NULL;

static sys_status_e sys_status = SYS_STATUS_ZZDL;

bool quest_enter_banpan = false;

static int last_weight = 0;
static int sys_weight = 0;
static int change_weight = 0;
static int upload_weight = 0;
static uint32_t upload_weight_num = 0;

static void bp_ostimercallback(void const * argument)
{
    (void) argument;
    sys_ossignal_notify(SYS_NOTIFY_MJBPEXIT_BIT);
    LOG_I("[MJ] timeout exit\r\n");
}

static void weight_ostimercallback(void const * argument)
{
    (void) argument;
    sys_ossignal_notify(SYS_NOTIFY_WEIGHTTIME_BIT);
}



int get_change_weight(void)
{
    return change_weight;
}

void weight_upload(void)
{
    upload_weight_num = 0;

    sys_weight += change_weight;

    upload_weight = change_weight;
    change_weight = 0;
    // wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUOEND_BIT);      
}

void weight_period_handle(void) // 200ms 周期
{
	int weight = hx711_get_weight();
    if(abs(weight - last_weight) > caiping_data.zhendongwucha) // update
    {
        upload_weight_num = 0;
        change_weight = weight - sys_weight;
        ui_ossignal_notify(UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);

        if(sys_status == SYS_STATUS_SBZC)
        {
            sys_status = SYS_STATUS_QBDCP;
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            wtn6040_play(WTN_WSBDCP_PLAY);
        }
    }
    else if(change_weight && (sys_status == SYS_STATUS_QBDCP))// 空闲状态下重量未变化
    {
        if(++upload_weight_num >= SYS_IWEIGHT_TIMEOUT)
        {
            //状态更新 上传重量
            sys_status = SYS_STATUS_SBZC;
            weight_upload();
            wl_ossignal_notify(WL_NOTIFY_PRIVSEND_IWEIGHT_BIT); 
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
        }
    }
    else if(change_weight && (sys_status == SYS_STATUS_BHZ))// 补货中重量未变化
    {
        if(++upload_weight_num >= SYS_BWEIGHT_TIMEOUT)
        {
            //状态更新 上传重量
            sys_status = SYS_STATUS_SBZC;
            weight_upload();
            wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUOEND_BIT); 
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
        }
    }
    else
    {
        upload_weight_num = 0;
    }

    last_weight = weight;
}



#define MJ_STR_MAX_LEN              (MJ8000_UART_RX_BUF_SIZE)
char mj_str[MJ_STR_MAX_LEN + 1];
uint8_t mj_len = 0;

uint32_t mj_parse_lasttime = 0;

void mj8000_rx_parse(const char *buf, uint16_t len)
{
    uint16_t ptr_len = MIN(MJ_STR_MAX_LEN, len);
    memcpy(mj_str, buf, ptr_len);
    mj_str[ptr_len] = '\0';

    if(strstr((const char*)buf,"user")) // 补货
    {
        if((osKernelSysTick() - mj_parse_lasttime) < 1600)
            return ;// ingore
        mj_parse_lasttime = osKernelSysTick();

        sys_ossignal_notify(SYS_NOTIFY_MJBUHUO_BIT);
        return ;
    }
    else if(strstr((const char*)buf,"zh")) // 绑盘
    {
        sys_ossignal_notify(SYS_NOTIFY_MJBPENTER_BIT);
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

    osTimerStart(weight_timehandle, SYS_WEIGHT_TIMEOUT);

    while(1) 
    {
        event = osSignalWait(SYS_TASK_NOTIFY, osWaitForever);
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
                if(sys_status == SYS_STATUS_SBZC) // enter
                {
                    // 重量要清零
                    sys_status = SYS_STATUS_BHZ;
                    wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUO_BIT);
                    ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
                    wtn6040_play(WTN_KSBH_PLAY);
                }
                else if(sys_status == SYS_STATUS_BHZ) // exit
                {
                    sys_status = SYS_STATUS_SBZC;
                    weight_upload();
                    wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUOEND_BIT);
                    ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
                    wtn6040_play(WTN_BHWC_PLAY);
                }
            }
            
            if(event.value.signals & SYS_NOTIFY_MJBPENTER_BIT)
            {
                if(sys_status == SYS_STATUS_SBZC) // enter
                {
                    osTimerStart(bp_timehandle, SYS_BPLIVE_TIMEOUT);
                    if(quest_enter_banpan)
                        return;
                    quest_enter_banpan = true;
                    wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BANGPAN_BIT);
                }
                else if(sys_status == SYS_STATUS_BHZ)
                {
                    sys_status == SYS_STATUS_SBZC;
                }
            } 
            if(event.value.signals & SYS_NOTIFY_MJBPEXIT_BIT)
            {
                // else if(sys_status == SYS_STATUS_BHZ)
                // {
                //     sys_status == SYS_STATUS_SBZC;
                // }
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

            if(event.value.signals & SYS_NOTIFY_WEIGHTTIME_BIT)
            {
                weight_period_handle(); // input
            }
        }
    }
}

void sys_init(void)
{
    osTimerDef(bp_timer, bp_ostimercallback);
    bp_timehandle = osTimerCreate(osTimer(bp_timer), osTimerOnce, NULL);
    assert_param(bp_timehandle);

    osTimerDef(weight_timer, weight_ostimercallback);
    weight_timehandle = osTimerCreate(osTimer(weight_timer), osTimerPeriodic, NULL);
    assert_param(weight_timehandle);

    osThreadDef(SYSThread, SYS_Thread, osPriorityAboveNormal, 0, 256);
    Sys_ThreadHandle = osThreadCreate(osThread(SYSThread), NULL);
}
