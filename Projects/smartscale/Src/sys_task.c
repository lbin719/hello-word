#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "mj8000.h"
#include "sys_task.h"
#include "str.h"
#include "rtc_timer.h"
#include "wtn6040.h"
#include "wl_priv_data.h"
#include "wl_task.h"
#include "cmsis_os.h"
#include "uart.h"
#include "stmencrypt.h"

extern void fct_task_handle(void);

#define SYS_BPLIVE_TIMEOUT          (3000) 
#define SYS_WEIGHT_TIMEOUT          (200) 

#define SYS_IWEIGHT_TIMEOUT         (4 * 5)// 4s
#define SYS_BWEIGHT_TIMEOUT         (20 * 5)// 20s


static osThreadId Sys_ThreadHandle;
static osTimerId bp_timehandle = NULL;
static osTimerId weight_timehandle = NULL;

static sys_status_e sys_status = SYS_STATUS_ZZDL;

bool quest_enter_banpan = false;

static int last_weight = 0;
static int sys_weight = 0;
static int change_weight = 0;
static uint32_t upload_weight_num = 0;

int upload_cweight = 0;
int upload_sweight = 0;

static void bp_ostimercallback(void const * argument)
{
    (void) argument;
    sys_ossignal_notify(SYS_NOTIFY_MJBPEXIT_BIT);
}

static void weight_ostimercallback(void const * argument)
{
    (void) argument;
    sys_ossignal_notify(SYS_NOTIFY_WEIGHTTIME_BIT);
}

uint8_t get_sys_status(void)
{
    return sys_status;
}
void set_sys_status(uint8_t status)
{
    sys_status = status;
    if(sys_status == SYS_STATUS_SBZC)
    {
        led_control(0, 0);
    }
    else
    {
        led_control(true, 50);
    }
}

int get_change_weight(void)
{
    return change_weight;
}

int get_sys_weight(void)
{
    return sys_weight;
}

void weight_init(void)
{
    // get sys_weight
    sys_weight = hx711_get_weight();
    last_weight = sys_weight;

    osTimerDef(weight_timer, weight_ostimercallback);
    weight_timehandle = osTimerCreate(osTimer(weight_timer), osTimerPeriodic, NULL);
    assert_param(weight_timehandle);
}

void weight_clear(void)
{
    last_weight = 0;
    upload_weight_num = 0;

    sys_weight = 0;
    change_weight = 0;
}

void weight_upload(void)
{
    upload_weight_num = 0;

    sys_weight += change_weight;

    upload_cweight = change_weight;
    upload_sweight = sys_weight;
    change_weight = 0; 

    if(sys_status == SYS_STATUS_BHZ)
    {
         wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUOEND_BIT);
    }
    else if(sys_status == SYS_STATUS_QQC)
    {
         wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BANGPANEND_BIT);
    }
    else if(sys_status == SYS_STATUS_QBDCP || sys_status == SYS_STATUS_QXFHCP)// 空闲状态下重量未变化
    {
        if(upload_cweight)
        {
            wl_ossignal_notify(WL_NOTIFY_PRIVSEND_IWEIGHT_BIT);
        }
    }
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
            set_sys_status(SYS_STATUS_QXFHCP);
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            wtn6040_play(WTN_WSBDCP_PLAY);
        }
    }
    else if(sys_status == SYS_STATUS_QXFHCP)// 空闲状态下重量未变化
    {
        if(++upload_weight_num >= SYS_IWEIGHT_TIMEOUT)// 空闲状态下重量变化 并上传重量
        {
            weight_upload();
            set_sys_status(SYS_STATUS_SBZC);
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
        }
    }
    else if(sys_status == SYS_STATUS_BHZ)// 补货中重量未变化
    {
        if(++upload_weight_num >= SYS_BWEIGHT_TIMEOUT) // 超时退出补货 并上传重量
        {
            weight_upload();
            set_sys_status(SYS_STATUS_SBZC);
            ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
            wtn6040_play(WTN_BHWC_PLAY);
        }
    }
    else if(change_weight && (sys_status == SYS_STATUS_SBZC))// 等待到设备正常状态
    {
        set_sys_status(SYS_STATUS_QXFHCP);
        ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
        wtn6040_play(WTN_WSBDCP_PLAY);
    }
    else
    {
        upload_weight_num = 0;
    }

    last_weight = weight;
}


char mj_str[MJ_STR_MAX_LEN + 1];
uint32_t mj_parse_lasttime = 0;

void mj_buhuo_handle(void)
{
    if((osKernelSysTick() - mj_parse_lasttime) < 1600)
        return ;// ingore
    mj_parse_lasttime = osKernelSysTick();

    if(sys_status == SYS_STATUS_SBZC) // quest enter
    {
        wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BUHUO_BIT);

        // weight_upload();
        // sys_status = SYS_STATUS_BHZ;
        // ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
        // wtn6040_play(WTN_KSBH_PLAY);
    }
    else if(sys_status == SYS_STATUS_BHZ) // exit
    {
        weight_upload();
        set_sys_status(SYS_STATUS_SBZC);
        ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
        wtn6040_play(WTN_BHWC_PLAY);
    }
}

void mj_banpan_handle(void)
{
    if(sys_status == SYS_STATUS_SBZC) // enter
    {
        osTimerStart(bp_timehandle, SYS_BPLIVE_TIMEOUT);
        if(false == quest_enter_banpan)
        {
            quest_enter_banpan = true;
            wl_ossignal_notify(WL_NOTIFY_PRIVSEND_BANGPAN_BIT);
        }
    }
    else if(sys_status == SYS_STATUS_QQC || sys_status == SYS_STATUS_QBDCP)
    {
        osTimerStart(bp_timehandle, SYS_BPLIVE_TIMEOUT);
    }
}

void mj8000_rx_parse(const char *buf, uint16_t len)
{
    uint16_t ptr_len = MIN(MJ_STR_MAX_LEN, len);
    memcpy(mj_str, buf, ptr_len);
    mj_str[ptr_len] = '\0';

    if(buf[0] == '0')
    {
        if(buf[1] == '1') // 补货
        {
            mj_buhuo_handle();
        }
        else if(buf[1] == '2') // 绑盘
        {
            mj_banpan_handle();
        }
    }
}

void mj_rx_handle(void)
{
    if(mj_uart_rx_frame.finsh)
    {
        if(mj_uart_rx_frame.len  > 1 && mj_uart_rx_frame.buf[mj_uart_rx_frame.len -1] == '\r')
            mj_uart_rx_frame.len --;
        mj_uart_rx_frame.buf[mj_uart_rx_frame.len] = '\0';
        LOG_I("[MJ]recv len:%d,data:%s\r\n", mj_uart_rx_frame.len, mj_uart_rx_frame.buf);
        mj8000_rx_parse((const char *)mj_uart_rx_frame.buf, mj_uart_rx_frame.len);

        mj_uart_rx_frame.finsh = 0;
        uart4_recive_dma(mj_uart_rx_frame.buf, MJ8000_UART_RX_BUF_SIZE);
    }
}

bool send_banpan_cmd = false;

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

            if(event.value.signals & SYS_NOTIFY_WLBPENTER_BIT)
            {
                if(sys_status == SYS_STATUS_SBZC) // enter
                {
                    if(wlpriv_banpan_result)
                    {
                        weight_upload();
                        set_sys_status(SYS_STATUS_QQC);
                        ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_USERNUM_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
                        wtn6040_play(WTN_SMCGQQC_PLAY);
                    }
                    else
                    {
                        set_sys_status(SYS_STATUS_QBDCP);
                        ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_USERNUM_BIT);
                        wtn6040_play(WTN_CPHWBD_PLAY);
                    }
                }
            } 
            if(event.value.signals & SYS_NOTIFY_MJBUHUOENTER_BIT)//发起补货回复
            {
                if(sys_status == SYS_STATUS_SBZC) // enter
                {
                    if(wlpriv_buhuo_result)
                    {
                        weight_upload();
                        set_sys_status(SYS_STATUS_BHZ);
                        ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
                        wtn6040_play(WTN_KSBH_PLAY);
                    }
                    else
                    {
                        // wtn6040_play(WTN_CPHWBD_PLAY); // test
                    }
                }
            } 

            if(event.value.signals & SYS_NOTIFY_MJBPEXIT_BIT)
            {
                quest_enter_banpan = false;

                if(sys_status == SYS_STATUS_QQC || sys_status == SYS_STATUS_QBDCP)
                {
                    weight_upload();
                    set_sys_status(SYS_STATUS_SBZC);
                    ui_ossignal_notify(UI_NOTIFY_STATUS_BIT | UI_NOTIFY_USERNUM_BIT | UI_NOTIFY_WEIGHT_BIT | UI_NOTIFY_SUM_PRICE_BIT | UI_NOTIFY_SUMSUM_PRICE_BIT);
                }
            } 

            if(event.value.signals & SYS_NOTIFY_WLLJ_BIT)
            {
                if(sys_status == SYS_STATUS_ZZDL || sys_status == SYS_STATUS_SBLX)
                {
                    // set_sys_status(SYS_STATUS_SBZC);
                    // ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
                }
            }
            if(event.value.signals & SYS_NOTIFY_WLLX_BIT)
            {
                set_sys_status(SYS_STATUS_SBLX);
                ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            }
            if(event.value.signals & SYS_NOTIFY_WLREGRET_BIT)
            {
                if(wl_get_status_bit(WL_STATUS_PRIVREGISTER_BIT))
                    set_sys_status(SYS_STATUS_SBZC);
                else
                    set_sys_status(SYS_STATUS_ZCSB);
                ui_ossignal_notify(UI_NOTIFY_STATUS_BIT);
            }

            if(event.value.signals & SYS_NOTIFY_WEIGHTTIME_BIT)
            {
                weight_period_handle(); // input
            }

            if(event.value.signals & SYS_NOTIFY_WEIGHZERO_BIT)
            {
                weight_clear();
                hx711_set_zero();
                wtn6040_play(WTN_CZTQLTX_PLAY);
            }
            if(event.value.signals & SYS_NOTIFY_WEIGHCALI_BIT)
            {
                weight_clear();
                hx711_set_calibration(hx711_cali_value);
            }            
            if(event.value.signals & SYS_NOTIFY_WLCAIPING_BIT)
            {
                wtn6040_play(WTN_CPQHCG_PLAY);
            }              
        }
    }
}

void sys_init(void)
{
    weight_init();

    osTimerDef(bp_timer, bp_ostimercallback);
    bp_timehandle = osTimerCreate(osTimer(bp_timer), osTimerOnce, NULL);
    assert_param(bp_timehandle);

    osThreadDef(SYSThread, SYS_Thread, osPriorityAboveNormal, 0, 512);
    Sys_ThreadHandle = osThreadCreate(osThread(SYSThread), NULL);
}
