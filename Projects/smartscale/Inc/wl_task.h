#ifndef _WL_TASK_H
#define _WL_TASK_H

#include "stdint.h"
#include "stdbool.h"

#define WL_HEART_PERIOD_MS                     (60*1000)


#define WL_TASK_NOTIFY                         (0xFFFFFFFFUL)
#define WL_NOTIFY_RECEIVE_BIT                  (0x1UL << 0)
#define WL_NOTIFY_PRIVSEND_RIGISTER_BIT        (0x1UL << 1)
#define WL_NOTIFY_PRIVSEND_HEART_BIT           (0x1UL << 2)
#define WL_NOTIFY_PRIVSEND_BUHUO_BIT           (0x1UL << 3)
#define WL_NOTIFY_PRIVSEND_BUHUOEND_BIT        (0x1UL << 4)
#define WL_NOTIFY_PRIVSEND_BANGPAN_BIT         (0x1UL << 5)
#define WL_NOTIFY_PRIVSEND_BANGPANEND_BIT      (0x1UL << 6)
#define WL_NOTIFY_PRIVSEND_IWEIGHT_BIT         (0x1UL << 7) // 空闲状态下重量变化上报

#define WL_NOTIFY_PRIVSEND_HEART_BIT           (0x1UL << 31)
// 


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

extern wl_t wl;

int32_t wl_ossignal_notify(int32_t signals);
extern void wl_init(void);

#endif /* _WL_TASK_H */
