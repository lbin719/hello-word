#ifndef __WL_PRIV_DATA_H
#define __WL_PRIV_DATA_H

#include "stdbool.h"
#include "stdint.h"

#define WL_HEART_PERIOD_MS                     (30*1000)
#define WLPRIV_WAIT_TIMEOUT                    (5000)


// Cmd ID	命令名称
#define WL_PRIV_DBUHUO_CMD              (1) // 1	设备发起补货
#define WL_PRIV_DBHWEIGHT_CMD           (2) // 2	设备补货完成传感器重量变化上报
#define WL_PRIV_DUSER_CMD               (3) // 3	设备发起用户绑盘称重
#define WL_PRIV_FCAIPING_CMD            (4) // 4	服务器设置菜品
#define WL_PRIV_FQUPI_CMD               (5) // 5	传感器操作（去皮）
#define WL_PRIV_FJIAOZHUN_CMD           (6) // 6	传感器操作（校准）
#define WL_PRIV_FWEIGHT_CMD             (7) // 7	传感器操作（获取重量）
#define WL_PRIV_FGETSTATUS_CMD          (8) // 8	获取传感器稳定状态
#define WL_PRIV_FSAOMATUO_CMD           (9) // 9	扫码头默认参数设置
#define WL_PRIV_FSETVOICE_CMD           (10) // 10	设置音量大小
#define WL_PRIV_FHOT_CMD                (11) // 11	设置加热状态
#define WL_PRIV_FHOTTIMER_CMD           (12) // 12	设置加热等级
#define WL_PRIV_FREBOOT_CMD             (13) // 13	设备重启
#define WL_PRIV_DREGISTER_CMD           (14) // 14	设备注册
#define WL_PRIV_DXINTIAOBAO_CMD         (15) // 15	设备发送心跳包
#define WL_PRIV_DBPWEIGHT_CMD           (16) // 16	设备绑盘传感器重量变化上报
#define WL_PRIV_DIWEIGHT_CMD            (17) // 17	设备空闲状态传感器重量变化上报

#define WL_PRIV_DBUHUO_RECMD            (1 + 128) // 1	设备发起补货
#define WL_PRIV_DBHWEIGHT_RECMD         (2 + 128) // 2	设备补货完成传感器重量变化上报
#define WL_PRIV_DUSER_RECMD             (3 + 128) // 3	设备发起用户绑盘称重
#define WL_PRIV_FCAIPING_RECMD          (4 + 128) // 4	服务器设置菜品
#define WL_PRIV_FQUPI_RECMD             (5 + 128) // 5	传感器操作（去皮）
#define WL_PRIV_FJIAOZHUN_RECMD         (6 + 128) // 6	传感器操作（校准）
#define WL_PRIV_FWEIGHT_RECMD           (7 + 128) // 7	传感器操作（获取重量）
#define WL_PRIV_FGETSTATUS_RECMD        (8 + 128) // 8	获取传感器稳定状态
#define WL_PRIV_FSAOMATUO_RECMD         (9 + 128) // 9	扫码头默认参数设置
#define WL_PRIV_FSETVOICE_RECMD         (10 + 128) // 10	设置音量大小
#define WL_PRIV_FHOT_RECMD              (11 + 128) // 11	设置加热状态
#define WL_PRIV_FHOTTIMER_RECMD         (12 + 128) // 12	设置加热等级
#define WL_PRIV_FREBOOT_RECMD           (13 + 128) // 13	设备重启
#define WL_PRIV_DREGISTER_RECMD         (14 + 128) // 14	设备注册
#define WL_PRIV_DXINTIAOBAO_RECMD       (15 + 128) // 15	设备发送心跳包
#define WL_PRIV_FBPWEIGHT_RECMD         (16 + 128) // 16	
#define WL_PRIV_FIWEIGHT_RECMD          (17 + 128) // 17	

typedef enum
{
  WL_PRIVSEND_RIGISTER_EVENT = 1, 
  WL_PRIVSEND_HEART_EVENT,    
  WL_PRIVSEND_BUHUO_EVENT,
  WL_PRIVSEND_BHWEIGHT_EVENT,    
  WL_PRIVSEND_BANGPAN_EVENT,
  WL_PRIVSEND_BPWEIGHT_EVENT,
  WL_PRIVSEND_IWEIGHT_EVENT,

  WL_PRIVRSEND_SETCAIPING_EVENT,
  WL_PRIVRSEND_QUPI_EVENT,
  WL_PRIVRSEND_JIAOZHUN_EVENT,
  WL_PRIVRSEND_GETWEIGHT_EVENT,
  WL_PRIVRSEND_GETSTATUS_EVENT,
  WL_PRIVRSEND_SETSAOMATOU_EVENT,
  WL_PRIVRSEND_SETVOICE_EVENT,
  WL_PRIVRSEND_SETHOT_EVENT,
  WL_PRIVRSEND_SETHOTTIMER_EVENT,
  WL_PRIVRSEND_REBOOT_EVENT,

}wl_privsend_e;

typedef enum
{
  WL_OK = 0, 
  WL_ERROR,     
  WL_TIMEOUT,     
}wl_result_e;

typedef bool (*priv_func_t)(int argc, char *argv[]);

typedef struct
{
    int cmd;
    priv_func_t func;
} priv_func_mapping_t;

typedef struct{
  uint32_t tx_pnum;
  uint32_t rx_pnum;

  uint32_t service_num;
  uint32_t device_num;
  uint8_t res_result;

  float user_sumprice;

} wlpriv_t;


extern wlpriv_t wlpriv;
extern bool wlpriv_banpan_result;
extern bool wlpriv_buhuo_result;

extern uint32_t hx711_cali_value;

bool wl_priv_rx_parse(int argc, char *argv[]);
void wl_priv_tx(uint8_t event);

void wlpriv_init(void);
#endif