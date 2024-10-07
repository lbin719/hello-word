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

sys_status_e sys_status = SYS_STATUS_ZZDL;

uint32_t weitht_lasttime = 0;

uint32_t update_timer = 0;
int current_weight = 0;
int change_weight = 0;
int last_dis_weight = 0;

int get_change_weight(void)
{
    return change_weight;
}

void weight_task_handle(void)
{
    if(HAL_GetTick() - weitht_lasttime < 100)
        return ;
    weitht_lasttime = HAL_GetTick();

	int weight = hx711_get_weight();
    if(abs(weight - current_weight) > caiping_data.zhendongwucha)
    {
        change_weight = weight - current_weight;
        current_weight = weight;
        timer_start(update_timer, 2000); 
    }
}


#define MJ_BP_LIVE_TIMEOUT          (1500) 
#define MJ_STR_MAX_LEN              (MJ8000_UART_RX_BUF_SIZE)
char mj_str[MJ_STR_MAX_LEN + 1];
uint8_t mj_len = 0;
uint32_t mj_bp_time = 0;

#define MJ_STATUS_IDLE              (0)
#define MJ_STATUS_BUHUO             (1)
#define MJ_STATUS_BANGPAN           (2)
uint8_t mj_status = MJ_STATUS_IDLE;

void mj8000_rx_parse(const char *buf, uint16_t len)
{
    char *strx = NULL;
    if(strstr((const char*)buf,"user")) // 补货
    {
        if(mj_status != MJ_STATUS_BUHUO)
            mj_status = MJ_STATUS_BUHUO;
        else
            mj_status = MJ_STATUS_IDLE;

        LOG_I("[MJ] buhuo\r\n");
        return ;
    }
    else if(strstr((const char*)buf,"zh")) // 绑盘
    {
        timer_start(mj_bp_time, MJ_BP_LIVE_TIMEOUT);
        mj_status = MJ_STATUS_BANGPAN;
        LOG_I("[MJ] pangpan\r\n");
        return ;
    }
}

void mj8000_task_handle(void)
{
    if(mj_uart_rx_frame.finsh)
    {
        mj_uart_rx_frame.buf[mj_uart_rx_frame.len] = '\0';
        LOG_I("[MJ]recv len:%d,data:%s\r\n", mj_uart_rx_frame.len, mj_uart_rx_frame.buf);
        mj8000_rx_parse(mj_uart_rx_frame.buf, mj_uart_rx_frame.len);

        mj_uart_rx_frame.finsh = 0;
        uart4_recive_dma(mj_uart_rx_frame.buf, MJ8000_UART_RX_BUF_SIZE);
    }

    if((mj_status == MJ_STATUS_BANGPAN) && (timer_isexpired(mj_bp_time)))
    {
        mj_status = MJ_STATUS_IDLE;
        LOG_I("[MJ] exit\r\n");
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


