#include "wl_priv_data.h"
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
#include "application.h"
#include "hot.h"


static bool wl_priv_set_caiping(int argc, char *argv[])
{
    caiping_data_t rx_caiping = {0};

    rx_caiping.mode = str_toint(argv[2]);
    if(strlen(argv[3])*2 <= STRING_DISH_LEN)
        str_tohex(argv[3], rx_caiping.dish_str);
    rx_caiping.price_unit = str_toint(argv[4]);
    rx_caiping.tool_weight = str_toint(argv[5]);
    rx_caiping.price = str_toint(argv[6]);
    rx_caiping.zhendongwucha = str_toint(argv[7]);
    rx_caiping.devicenum = str_toint(argv[8]);

    wl.respond_result = WL_OK;
    if(rx_caiping.mode > 1)
    {
        wl.respond_result = WL_ERROR;
        goto exit;
    }

    //TODU 
    memcpy(&caiping_data, &rx_caiping, sizeof(caiping_data_t));
    sysinfo_store_caipin(&caiping_data);
    set_draw_update_bit(DRAW_UPDATE_DISH_BIT | DRAW_UPDATE_PRICE_BIT | DRAW_UPDATE_PRICE_UNIT_BIT | DRAW_UPDATE_ALL_BIT);

exit:
    wl_set_priv_send(WL_PRIVRSEND_SETCAIPING_EVENT);

    return true;
}

static bool wl_priv_set_qupi(int argc, char *argv[])
{
    hx711_set_zero();

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_QUPI_EVENT);
    return true;
}

static bool wl_priv_set_jiaozhun(int argc, char *argv[])
{
    uint32_t cali = str_toint(argv[2]);
    hx711_set_calibration(cali);

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_JIAOZHUN_EVENT);
    return true;
}

static bool wl_priv_get_weight(int argc, char *argv[])
{
    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_GETWEIGHT_EVENT);
    return true;
}

static bool wl_priv_get_status(int argc, char *argv[])
{
    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_GETSTATUS_EVENT);
    return true;
}

static bool wl_priv_set_saomatou(int argc, char *argv[])
{
    mj8000_setconfig();

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_SETSAOMATOU_EVENT);
    return true;
}

static bool wl_priv_set_voice(int argc, char *argv[])
{
    uint8_t level = str_toint(argv[2]);

    wtn6040_set_voice_store(level);

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_SETVOICE_EVENT);

    return true;
}

static bool wl_priv_set_hot(int argc, char *argv[])
{
    uint8_t status = str_toint(argv[2]);
    hot_ctrl(status);

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_SETHOT_EVENT);
    return true;
}

static bool wl_priv_set_hottimer(int argc, char *argv[])
{
    uint32_t hot_timer = str_toint(argv[2]);

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_SETHOTTIMER_EVENT);
    return true;
}

static bool wl_priv_set_reboot(int argc, char *argv[])
{

    wl.respond_result = WL_OK;
    wl_set_priv_send(WL_PRIVRSEND_REBOOT_EVENT);
    return true;
}


static bool wl_priv_res_register(int argc, char *argv[])
{
    wl.priv_register = true;
    sys_status = SYS_STATUS_SBZC;
    set_draw_update_bit(DRAW_UPDATE_STATUS_BIT);

    return true;
}

static const priv_func_mapping_t priv_func[]={
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

    {WL_PRIV_DREGISTER_RECMD,       wl_priv_res_register}   // 14	设备注册
};

bool wl_rx_priv_parse(int argc, char *argv[])
{
    bool ret = true;

   int cmd = str_toint(argv[0]);
   wl.priv_fnum = str_toint(argv[1]);

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
