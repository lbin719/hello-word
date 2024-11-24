#include <stdio.h>
#include <stdlib.h>
#include "wl_priv_data.h"
#include "ui_task.h"
#include "main.h"
#include "hx711.h"
#include "ec800e.h"
#include "sys_task.h"
#include "str.h"
#include "rtc_timer.h"
#include "mj8000.h"
#include "wtn6040.h"
#include "hot.h"
#include "wl_task.h"
#include "system_info.h"
#include "cmsis_os.h"

bool wlpriv_banpan_result = false;
bool wlpriv_buhuo_result = false;

uint32_t hx711_cali_value = 0;

wlpriv_t wlpriv = {0};
static osTimerId heart_timehandle = NULL;

static osTimerId priv_timehandle = NULL;
wl_privsend_e   timeout_event = 0;

static void heart_ostimercallback(void const * argument)
{
    (void) argument;
    wl_ossignal_notify(WL_NOTIFY_PRIVSEND_HEART_BIT);
    osTimerStart(heart_timehandle, WL_HEART_PERIOD_MS);
}

static void priv_ostimercallback(void const * argument)
{
    (void) argument;

    if(timeout_event == WL_PRIVSEND_RIGISTER_EVENT) // 注册失败
    {
        vPortEnterCritical();
        wl_clear_status_bit(WL_STATUS_PRIVREGISTER_BIT);
        vPortExitCritical();
        sys_ossignal_notify(SYS_NOTIFY_WLREGRET_BIT); // 通知sys，设设备注册结果设备连接成功
    }

    timeout_event = 0;
}

void wl_priv_timestart_event(wl_privsend_e event)
{
    timeout_event = event;
    osTimerStart(priv_timehandle, WLPRIV_WAIT_TIMEOUT);
}

void wl_priv_timestop_event(void)
{
    timeout_event = 0;
    osTimerStop(priv_timehandle);
}

static bool wl_priv_set_caiping(int argc, char *argv[])
{
    caiping_data_t rx_caiping = {0};

    rx_caiping.mode = atoi(argv[2]);
    // if(strlen(argv[3])*2 <= STRING_DISH_LEN)
    //     str_tohex(argv[3], rx_caiping.dish_str);
    memcpy(rx_caiping.dish_str, argv[3], MIN(strlen(argv[3]), STRING_DISH_LEN));

    rx_caiping.price_unit = atoi(argv[4]);
    rx_caiping.tool_weight = atoi(argv[5]);
    rx_caiping.price = strtof(argv[6], NULL);
    rx_caiping.zhendongwucha = atoi(argv[7]);
    rx_caiping.devicenum = atoi(argv[8]);

    wlpriv.res_result = WL_OK;
    if(rx_caiping.mode > 1)
    {
        wlpriv.res_result = WL_ERROR;
        goto exit;
    }

    LOG_I("[WL] set caiping: %d %s %d, %d, %.2f, %d, %d\r\n",   
            rx_caiping.mode, 
            rx_caiping.dish_str, 
            rx_caiping.price_unit, 
            rx_caiping.tool_weight, 
            rx_caiping.price,
            rx_caiping.zhendongwucha,
            rx_caiping.devicenum);

    portENTER_CRITICAL();
    memcpy(&caiping_data, &rx_caiping, sizeof(caiping_data_t));
    portEXIT_CRITICAL();
    sysinfo_store_caipin(&caiping_data);
    ui_ossignal_notify(UI_NOTIFY_DISH_BIT | UI_NOTIFY_PRICE_BIT | UI_NOTIFY_PRICE_UNIT_BIT | UI_NOTIFY_ALL_BIT);
    sys_ossignal_notify(SYS_NOTIFY_WLCAIPING_BIT);
exit:
    wl_priv_tx(WL_PRIVRSEND_SETCAIPING_EVENT);

    return true;
}

static bool wl_priv_set_qupi(int argc, char *argv[])
{
    wtn6040_play(WTN_QQKTPTX_PLAY);
    osDelay(4000);
    sys_ossignal_notify(SYS_NOTIFY_WEIGHZERO_BIT);
    osDelay(500);
    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_QUPI_EVENT);
    return true;
}

static bool wl_priv_set_jiaozhun(int argc, char *argv[])
{
    hx711_cali_value = atoi(argv[2]);
    sys_ossignal_notify(SYS_NOTIFY_WEIGHCALI_BIT);

    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_JIAOZHUN_EVENT);
    return true;
}

static bool wl_priv_get_weight(int argc, char *argv[])
{
    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_GETWEIGHT_EVENT);
    return true;
}

static bool wl_priv_get_status(int argc, char *argv[])
{
    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_GETSTATUS_EVENT);
    return true;
}

static bool wl_priv_set_saomatou(int argc, char *argv[])
{
    mj8000_setconfig();

    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETSAOMATOU_EVENT);
    return true;
}

static bool wl_priv_set_voice(int argc, char *argv[])
{
    if(argc != 3)
    {
        LOG_I("Error argc:%d\r\n", argc);
        return false;
    }

    uint8_t level = atoi(argv[2]);
    wtn6040_set_voice_store(level);

    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETVOICE_EVENT);

    return true;
}

static bool wl_priv_set_hot(int argc, char *argv[])
{
    if(argc != 4)
    {
        LOG_I("Error argc:%d\r\n", argc);
        return false;
    }

    uint8_t mode = atoi(argv[2]);
    uint8_t time = atoi(argv[3]);    
    hot_ctrl_store(mode, time);

    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETHOT_EVENT);
    return true;
}

static bool wl_priv_set_hottimer(int argc, char *argv[])
{
//    uint32_t hot_timer = atoi(argv[2]);

    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETHOTTIMER_EVENT);
    return true;
}

static bool wl_priv_set_reboot(int argc, char *argv[])
{

    wlpriv.res_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_REBOOT_EVENT);
    return true;
}

/* ------------------------------------ */
static bool wl_priv_res_buhuo(int argc, char *argv[])
{
    if(argv[2][0] == '0')
        wlpriv_buhuo_result = true;
    else
        wlpriv_buhuo_result = false;
    sys_ossignal_notify(SYS_NOTIFY_MJBUHUOENTER_BIT);
    return true;
}

static bool wl_priv_res_weight(int argc, char *argv[])
{
    return true;
}

static bool wl_priv_res_user(int argc, char *argv[])
{
    if(argv[2][0] == '0')
    {
        wlpriv_banpan_result = true;
        wlpriv.user_sumprice = strtof(argv[3], NULL);
    }
    else
        wlpriv_banpan_result = false;

    sys_ossignal_notify(SYS_NOTIFY_WLBPENTER_BIT);
    return true;
}

static bool wl_priv_res_register(int argc, char *argv[])
{
    wl_priv_timestop_event();

    if(argv[2][0] == '0') // 设备注册成功，
    {
        wl_set_status_bit(WL_STATUS_PRIVREGISTER_BIT);
        osTimerStart(heart_timehandle, 0); // 开始发送心跳包
    }
    else
    {
        wl_clear_status_bit(WL_STATUS_PRIVREGISTER_BIT);
    }
    
    sys_ossignal_notify(SYS_NOTIFY_WLREGRET_BIT); // 通知sys，设设备注册结果设备连接成功
    return true;
}


static const priv_func_mapping_t priv_func[]={
    //服务器主动出发的指令解析
    {WL_PRIV_FCAIPING_CMD,          wl_priv_set_caiping},   // 4	服务器设置菜品
    {WL_PRIV_FQUPI_CMD,             wl_priv_set_qupi},      // 5	传感器操作（去皮）
    {WL_PRIV_FJIAOZHUN_CMD,         wl_priv_set_jiaozhun},  // 6	传感器操作（校准）
    {WL_PRIV_FWEIGHT_CMD,           wl_priv_get_weight},    // 7	传感器操作（获取重量）
    {WL_PRIV_FGETSTATUS_CMD,        wl_priv_get_status},    // 8    获取传感器稳定状态
    {WL_PRIV_FSAOMATUO_CMD,         wl_priv_set_saomatou},  // 9	扫码头默认参数设置
    {WL_PRIV_FSETVOICE_CMD,         wl_priv_set_voice},     // 10   设置音量大小
    {WL_PRIV_FHOT_CMD,              wl_priv_set_hot},       // 11	设置加热状态
    {WL_PRIV_FHOTTIMER_CMD,         wl_priv_set_hottimer},  // 12   设置加热等级
    {WL_PRIV_FREBOOT_CMD,           wl_priv_set_reboot},    // 13   设备重启

    // 发起指令回复的数据解析
    {WL_PRIV_DBUHUO_RECMD,          wl_priv_res_buhuo},     // (1 + 128) // 1	设备发起补货
    {WL_PRIV_DBHWEIGHT_RECMD,       wl_priv_res_weight},    // (2 + 128) // 2	设备补货完成传感器重量变化上报
    {WL_PRIV_DUSER_RECMD,           wl_priv_res_user},      // (3 + 128) // 3	设备发起用户绑盘称重
    {WL_PRIV_DREGISTER_RECMD,       wl_priv_res_register},  // (14 + 128)// 14	设备注册
};

bool wl_priv_rx_parse(int argc, char *argv[])
{
    wlpriv.rx_pnum++;

    bool ret = false;
    int cmd = atoi(argv[0]);
    wlpriv.service_num = atoi(argv[1]);

    for (uint32_t i = 0; i < (sizeof(priv_func) / sizeof(priv_func_mapping_t)); i++)
    {
        if (cmd == priv_func[i].cmd)
        {
            LOG_I("[WL]found cmd[%d]: %d\r\n", i, priv_func[i].cmd);
            if (priv_func[i].func)
                ret = priv_func[i].func(argc, argv);
            break;
        }
    }

    return ret;
}

void wl_priv_tx(uint8_t event)
{
    wlpriv.tx_pnum++;
	ec800e_uart_printf("AT+QISEND=0\r\n");
	osDelay(2);
    if(event == WL_PRIVSEND_RIGISTER_EVENT)
    {
        // ec800e_uart_printf("{%d,%d,865269073414190,460088340106940,}\r\n", WL_PRIV_DREGISTER_CMD, ++wlpriv.device_num);        //test
        // ec800e_uart_printf("{%d,%d,862584075695205,460088340106940,}\r\n", WL_PRIV_DREGISTER_CMD, ++wlpriv.device_num);        //test
        ec800e_uart_printf("{%d,%d,%s,%s,}\r\n", WL_PRIV_DREGISTER_CMD, ++wlpriv.device_num, wl.sn, wl.imsi);
        wl_priv_timestart_event(event);
    }
    else if(event == WL_PRIVSEND_HEART_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,0,0,%d,%d,%d,}\r\n", 
                             WL_PRIV_DXINTIAOBAO_CMD, ++wlpriv.device_num, 
                             get_sys_weight(), 
                             get_current_hot_status(), sysinfo_get_hottime(),
                             get_timestamp());
    }
    else if(event == WL_PRIVSEND_BUHUO_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%s,%d,}\r\n", WL_PRIV_DBUHUO_CMD, ++wlpriv.device_num, mj_str, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BHWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,%d,}\r\n", WL_PRIV_DBHWEIGHT_CMD, ++wlpriv.device_num, upload_cweight, upload_sweight, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BANGPAN_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%s,%d,}\r\n", WL_PRIV_DUSER_CMD, ++wlpriv.device_num, mj_str, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BPWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%s,%d,%d,}\r\n", WL_PRIV_DBPWEIGHT_CMD, ++wlpriv.device_num, mj_str, upload_cweight, get_timestamp());
    }  
    else if(event == WL_PRIVSEND_IWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,%d,}\r\n", WL_PRIV_DIWEIGHT_CMD, ++wlpriv.device_num, upload_cweight, upload_sweight, get_timestamp());
    } 

    else if(event == WL_PRIVRSEND_SETCAIPING_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FCAIPING_RECMD, wlpriv.service_num, wlpriv.res_result);
    }
    else if(event == WL_PRIVRSEND_QUPI_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FQUPI_RECMD, wlpriv.service_num, wlpriv.res_result);
    }
    else if(event == WL_PRIVRSEND_JIAOZHUN_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FJIAOZHUN_RECMD, wlpriv.service_num, wlpriv.res_result);
    }
    else if(event == WL_PRIVRSEND_GETWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,}\r\n", WL_PRIV_FWEIGHT_RECMD, wlpriv.service_num, wlpriv.res_result, get_sys_weight());
    }
    else if(event == WL_PRIVRSEND_GETSTATUS_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,}\r\n", WL_PRIV_FGETSTATUS_RECMD, wlpriv.service_num, wlpriv.res_result, 0);
    }    
    else if(event == WL_PRIVRSEND_SETSAOMATOU_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FSAOMATUO_RECMD, wlpriv.service_num, wlpriv.res_result);
    }   
    else if(event == WL_PRIVRSEND_SETVOICE_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FSETVOICE_RECMD, wlpriv.service_num, wlpriv.res_result);
    }
    else if(event == WL_PRIVRSEND_SETHOT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FHOT_RECMD, wlpriv.service_num, wlpriv.res_result);
    }
    else if(event == WL_PRIVRSEND_SETHOTTIMER_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FHOTTIMER_RECMD, wlpriv.service_num, wlpriv.res_result);
    }
    else if(event == WL_PRIVRSEND_REBOOT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FREBOOT_RECMD, wlpriv.service_num, wlpriv.res_result);
        ec800e_uart_printf("%c", 0x1A);//发送完成函数

        LOG_I("system reboot\r\n");
        HAL_Delay(500);
        NVIC_SystemReset();
    }
    else
    {
        return ;
    } 
    ec800e_uart_printf("%c", 0x1A);//发送完成函数          
    // priv_send_event = 0;
}

void wlpriv_init(void)
{
    osTimerDef(heart_timer, heart_ostimercallback);
    heart_timehandle = osTimerCreate(osTimer(heart_timer), osTimerOnce, NULL);
    assert_param(heart_timehandle);

    osTimerDef(priv_timer, priv_ostimercallback);
    priv_timehandle = osTimerCreate(osTimer(priv_timer), osTimerOnce, NULL);
    assert_param(priv_timehandle);
}
