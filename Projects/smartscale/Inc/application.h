
#ifndef __APPLICATION_H
#define __APPLICATION_H

#if 0
tyepdef struct {
    char *cmd;	/* AT指令 */
    char *r_cmd;
    int (*deal_func)(int opt, int argc, char *argv[]);
}at_cmd_t;

// int deal_uart_func(int opt, int argc, char *argv[]);

at_cmd_t at_table[] = {
    {"AT",              "OK",       NULL},
    {"ATE0",            "OK",       NULL},
    {"AT+CGSN=1",       "+CGSN:"    NULL},// +CGSN: "862584073708935"
    {"AT+CPIN?",        "+CPIN:",   NULL},// +CPIN: READY
    {"AT+CMIM",         "0",       NULL},// 460081925003317
    {"AT+CSQ",          "+CSQ:",    NULL},// +CSQ: 31,0 信号强度
    {"AT+QICLOSE=0",    "OK",       NULL},// ok
    {"AT+CGREG?",       "+CGREG:",  NULL},// +CGREG: 0,1
    {"AT+CEREG?",       "+CEREG:",  NULL},// +CEREG: 0,1
    {"AT+QLTS=2",       "+QLTS:", NULL},// +QLTS:
	// AT+QICSGP=1,1,"CMIOT","","",1  OK
	// AT+QIACT=1 OK
	// AT+QIACT? +QIACT: 1,1,1,"10.27.82.43" OK
	// AT+QIOPEN=1,0,"TCP","39.106.91.24",10181,0,1  OK
	// AT+QISEND=0 \r\n hello ... SEND OK
};
#endif

#define ARGC_LIMIT      (12)

typedef enum
{
    WL_STATE_INIT,
    WL_STATE_WAIT_UART_CONNECT,

    WL_STATE_CLOSE_DIS,    
    WL_STATE_WAIT_CLOSE_DIS,

    WL_STATE_CGSN,    
    WL_STATE_WAIT_CGSN,

    WL_STATE_CHECK_SIM,    
    WL_STATE_WAIT_CHECK_SIM,

    WL_STATE_CHECK_CSQ,    
    WL_STATE_WAIT_CHECK_CSQ,

    WL_STATE_CIMI,    
    WL_STATE_WAIT_CIMI,

    WL_STATE_QCCID,    
    WL_STATE_WAIT_QCCID,

    WL_STATE_QICLOSE,    
    WL_STATE_WAIT_QICLOSE,

    WL_STATE_CGREG,    
    WL_STATE_WAIT_CGREG, 

    WL_STATE_CEREG,    
    WL_STATE_WAIT_CEREG, 

    WL_STATE_QLTS,    
    WL_STATE_WAIT_QLTS,    
                            //AT+QICSGP=1,1,"CMIOT","","",1//配置 TCP/IP 场景参数

    WL_STATE_QIACT,         // AT+QIACT=1 
    WL_STATE_WAIT_QIACT, 
                        
    WL_STATE_GET_QIACT,     // AT+QIACT?
    WL_STATE_WAIT_GET_QIACT,  

    WL_STATE_GET_QIOPEN,     
    WL_STATE_WAIT_QIOPEN,  


    WL_STATE_PRIV_SEND, 
    WL_STATE_PRIV_WAIT_SEND, 
    WL_STATE_TXRX,            
} wlstate_e;

typedef enum
{
  SYS_STATUS_ZZDL = 0, // 正在登录
  SYS_STATUS_SBLX,     // 设备离线
  SYS_STATUS_SBZC,     // 设备正常
  SYS_STATUS_BHZ,      // 补货中
  SYS_STATUS_QBDCP,    // 请绑定餐盘
  SYS_STATUS_QQC,      // 请取餐
}sys_status_e;

typedef enum
{
  WL_OK = 0, 
  WL_ERROR,     
  WL_TIMEOUT,     
}wl_result_e;

#define WL_SN_LEN                       (15)
#define WL_IMSI_LEN                     (15)
#define WL_ICCID_LEN                    (20)
#define WL_IP_LEN                       (16)   

typedef struct{
    wlstate_e      state;
    uint32_t       respond_timer;
    uint32_t       heart_timer;
    bool           connect;
    bool           sim_status; 
    uint8_t        rssi; 
    char           sn[WL_SN_LEN + 1];
    char           imsi[WL_IMSI_LEN + 1];
    char           iccid[WL_ICCID_LEN + 1];
    char           ip[WL_IP_LEN + 1];

    uint32_t       priv_dnum;
    uint32_t       priv_fnum;
    bool           priv_register;
    bool           send_status;
    bool           wait_send_status;
    uint8_t        device_status;

    uint8_t        respond_result; //回复结果
} wl_t;


// Cmd ID	命令名称
#define WL_PRIV_DBUHUO_CMD              (1) // 1	设备发起补货
#define WL_PRIV_DWEIGHT_CMD             (2) // 2	设备传感器重量变化上报
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

#define WL_PRIV_DBUHUO_RECMD            (1 + 128) // 1	设备发起补货
#define WL_PRIV_DWEIGHT_RECMD           (2 + 128) // 2	设备传感器重量变化上报
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


typedef enum
{
  WL_PRIVSEND_RIGISTER_EVENT = 1, 
  WL_PRIVSEND_HEART_EVENT,    
  WL_PRIVSEND_BUHUO_EVENT,  
  WL_PRIVSEND_BANGPAN_EVENT, 
  WL_PRIVRSEND_SETCAIPING_EVENT,
      
}wl_privsend_e;


void weight_task_handle(void);

uint8_t get_sys_status(void);
void wl_task_handle(void);

#endif /* __APPLICATION_H */
