#include "wl_priv_data.h"
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
#include "hot.h"
#include "wl_task.h"
#include <stdlib.h>

bool wlpriv_banpan_result = false;
uint32_t hx711_cali_value = 0;

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

    wl.respond_result = WL_OK;
    if(rx_caiping.mode > 1)
    {
        wl.respond_result = WL_ERROR;
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

    memcpy(&caiping_data, &rx_caiping, sizeof(caiping_data_t));
    sysinfo_store_caipin(&caiping_data);
    ui_ossignal_notify(UI_NOTIFY_DISH_BIT | UI_NOTIFY_PRICE_BIT | UI_NOTIFY_PRICE_UNIT_BIT | UI_NOTIFY_ALL_BIT);

exit:
    wl_priv_tx(WL_PRIVRSEND_SETCAIPING_EVENT);

    return true;
}

static bool wl_priv_set_qupi(int argc, char *argv[])
{
    sys_ossignal_notify(SYS_NOTIFY_WEIGHZERO_BIT);

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_QUPI_EVENT);
    return true;
}

static bool wl_priv_set_jiaozhun(int argc, char *argv[])
{
    uint32_t hx711_cali_value = atoi(argv[2]);
    sys_ossignal_notify(SYS_NOTIFY_WEIGHZERO_BIT);

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_JIAOZHUN_EVENT);
    return true;
}

static bool wl_priv_get_weight(int argc, char *argv[])
{
    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_GETWEIGHT_EVENT);
    return true;
}

static bool wl_priv_get_status(int argc, char *argv[])
{
    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_GETSTATUS_EVENT);
    return true;
}

static bool wl_priv_set_saomatou(int argc, char *argv[])
{
    mj8000_setconfig();

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETSAOMATOU_EVENT);
    return true;
}

static bool wl_priv_set_voice(int argc, char *argv[])
{
    uint8_t level = atoi(argv[2]);

    wtn6040_set_voice_store(level);

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETVOICE_EVENT);

    return true;
}

static bool wl_priv_set_hot(int argc, char *argv[])
{
    uint8_t status = atoi(argv[2]);
    hot_ctrl(status);

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETHOT_EVENT);
    return true;
}

static bool wl_priv_set_hottimer(int argc, char *argv[])
{
    uint32_t hot_timer = atoi(argv[2]);

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_SETHOTTIMER_EVENT);
    return true;
}

static bool wl_priv_set_reboot(int argc, char *argv[])
{

    wl.respond_result = WL_OK;
    wl_priv_tx(WL_PRIVRSEND_REBOOT_EVENT);
    return true;
}

/* ------------------------------------ */
static bool wl_priv_res_buhuo(int argc, char *argv[])
{
    return true;
}

static bool wl_priv_res_weight(int argc, char *argv[])
{
    return true;
}

static bool wl_priv_res_user(int argc, char *argv[])
{
    if(argv[2][0] == '0')
        wlpriv_banpan_result = true;
    else
        wlpriv_banpan_result = false;

    sys_ossignal_notify(SYS_NOTIFY_WLBPENTER_BIT);
    return true;
}

static bool wl_priv_res_register(int argc, char *argv[])
{
    wl.priv_register = true;
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
    bool ret = true;

   int cmd = atoi(argv[0]);
   wl.priv_fnum = atoi(argv[1]);

   for (uint32_t i = 0; i < (sizeof(priv_func) / sizeof(priv_func_mapping_t)); i++)
   {
       if (cmd == priv_func[i].cmd)
       {
           LOG_I("[WL]found cmd[%d]: %d\r\n", i, priv_func[i].cmd);
           if (priv_func[i].func)
           {
               ret = priv_func[i].func(argc, argv);
           }
           break;
       }
   }

    return ret;
}

void wl_priv_tx(uint8_t event)
{
	ec800e_uart_printf("AT+QISEND=0\r\n");
	osDelay(2);
    if(event == WL_PRIVSEND_RIGISTER_EVENT)
    {
        //test
        ec800e_uart_printf("{%d,%d,862584075695577,460074425636505,}\r\n", WL_PRIV_DREGISTER_CMD, ++wl.priv_dnum);
        // ec800e_uart_printf("{%d,%d,%s,%s,}\r\n", WL_PRIV_DREGISTER_CMD, ++wl.priv_dnum, wl.sn, wl.imsi);
    }
    else if(event == WL_PRIVSEND_HEART_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_DXINTIAOBAO_CMD, ++wl.priv_dnum, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BUHUO_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%s,%d,}\r\n", WL_PRIV_DBUHUO_CMD, ++wl.priv_dnum, mj_str, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BHWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,%d,}\r\n", WL_PRIV_DBHWEIGHT_CMD, ++wl.priv_dnum, upload_cweight, upload_sweight, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BANGPAN_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%s,%d,}\r\n", WL_PRIV_DUSER_CMD, ++wl.priv_dnum, mj_str, get_timestamp());
    }
    else if(event == WL_PRIVSEND_BPWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%s,%d,%d,}\r\n", WL_PRIV_DBPWEIGHT_CMD, ++wl.priv_dnum, mj_str, upload_cweight, get_timestamp());
    }  
    else if(event == WL_PRIVSEND_IWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,%d,}\r\n", WL_PRIV_DIWEIGHT_CMD, ++wl.priv_dnum, upload_cweight, upload_sweight, get_timestamp());
    } 



    else if(event == WL_PRIVRSEND_SETCAIPING_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_DXINTIAOBAO_CMD, wl.priv_fnum, wl.respond_result);
    }
    else if(event == WL_PRIVRSEND_QUPI_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FQUPI_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(event == WL_PRIVRSEND_JIAOZHUN_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FJIAOZHUN_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(event == WL_PRIVRSEND_GETWEIGHT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,}\r\n", WL_PRIV_FWEIGHT_RECMD, wl.priv_fnum, wl.respond_result, 152); //todu
    }
    else if(event == WL_PRIVRSEND_GETSTATUS_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,%d,}\r\n", WL_PRIV_FGETSTATUS_RECMD, wl.priv_fnum, wl.respond_result, 0);
    }    
    else if(event == WL_PRIVRSEND_SETSAOMATOU_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FSAOMATUO_RECMD, wl.priv_fnum, wl.respond_result);
    }   
    else if(event == WL_PRIVRSEND_SETVOICE_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FSETVOICE_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(event == WL_PRIVRSEND_SETHOT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FHOT_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(event == WL_PRIVRSEND_SETHOTTIMER_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FHOTTIMER_RECMD, wl.priv_fnum, wl.respond_result);
    }
    else if(event == WL_PRIVRSEND_REBOOT_EVENT)
    {
        ec800e_uart_printf("{%d,%d,%d,}\r\n", WL_PRIV_FREBOOT_RECMD, wl.priv_fnum, wl.respond_result);

        LOG_I("system reboot\r\n");
        HAL_Delay(500);
        NVIC_SystemReset();
    } 
    ec800e_uart_printf("%c", 0x1A);//发送完成函数          
    // priv_send_event = 0;
}
