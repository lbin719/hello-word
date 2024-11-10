#ifndef _WL_TASK_H
#define _WL_TASK_H

#include "stdint.h"
#include "stdbool.h"

/* 
+CME ERROR 
0 phone failure 拨打电话失败
1 no connection to phone 电话未连接
2 phone-adaptor link reserved 保留电话适配器连接
3 operation not allowed 操作不允许
4 operation not supported 操作不支持
5 PH-SIM PIN required 需要 PH-SIM 卡 PIN 码
6 PH-FSIM PIN required 需要 PH-FSIM 卡 PIN 码
7 PH-FSIM PUK required 需要 PH-FSIM 卡 PUK 码
10 SIM not inserted 未插入 SIM 卡
11 SIM PIN required 需要 SIM 卡 PIN 码
12 SIM PUK required 需要 SIM 卡 PUK 码
13 SIM failure SIM 卡失败
14 SIM busy SIM 卡忙
15 SIM wrong SIM 卡错误
16 incorrect password 密码不正确
17 SIM PIN2 required 需要 SIM 卡 PIN2 码
18 SIM PUK2 required 需要 SIM 卡 PUK2 码
20 memory full 内存已满
21 invalid index 无效索引
22 not found 未发现
23 memory failure 内存不足
24 text string too long 文本字符过长
25 invalid characters in text string 无效文本字符
26 dial string too long 拨号字符过长
27 invalid characters in dial string 无效拨号字符
30 no network service 无网络服务
31 network timeout 网络超时
32 network not allowed - emergency calls only 网络不允许-仅支持紧急通话
40 network personalization PIN required 需要网络个性化 PIN 码
41 network personalization PUK required 需要网络个性化 PUK 码
42 network subset personalization PIN required 需要网络子集个性化 PIN 码
43 network subset personalization PUK required 需要网络子集个性化 PUK 码
44 service provider personalization PIN required 需要网络服务商个性化 PIN 码
45 service provider personalization PUK required 需要网络服务商个性化 PUK 码
46 corporate personalization PIN required 需要企业个性化 PIN 码
47 corporate personalization PUK required 需要企业个性化 PUK 码

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
*/


#define WL_HEART_PERIOD_MS                     (60*1000)
#define WL_WAIT_RECEIVE_TIMEOUT                (500)

#define WL_TASK_NOTIFY                         (0xFFFFFFFFUL)
#define WL_NOTIFY_RECEIVE_BIT                  (0x1UL << 0)
#define WL_NOTIFY_PRIVSEND_RIGISTER_BIT        (0x1UL << 1)
#define WL_NOTIFY_PRIVSEND_HEART_BIT           (0x1UL << 2)
#define WL_NOTIFY_PRIVSEND_BUHUO_BIT           (0x1UL << 3)
#define WL_NOTIFY_PRIVSEND_BUHUOEND_BIT        (0x1UL << 4)
#define WL_NOTIFY_PRIVSEND_BANGPAN_BIT         (0x1UL << 5)
#define WL_NOTIFY_PRIVSEND_BANGPANEND_BIT      (0x1UL << 6)
#define WL_NOTIFY_PRIVSEND_IWEIGHT_BIT         (0x1UL << 7) // 空闲状态下重量变化上报


// #define WL_STATUS_AT_BIT                    (0x1UL << 0)
// #define WL_STATUS_ATE0_BIT                  (0x1UL << 1)
#define WL_STATUS_CGSN_BIT                  (0x1UL << 2)
#define WL_STATUS_CPIN_BIT                  (0x1UL << 3)
#define WL_STATUS_CIMI_BIT                  (0x1UL << 4)
#define WL_STATUS_CSQ_BIT                   (0x1UL << 5)
// #define WL_STATUS_QICLOSE_BIT               (0x1UL << 6)
#define WL_STATUS_CGREG_BIT                 (0x1UL << 7)
#define WL_STATUS_CEREG_BIT                 (0x1UL << 8)
#define WL_STATUS_QLTS_BIT                  (0x1UL << 9)
#define WL_STATUS_QCCID_BIT                 (0x1UL << 10)
#define WL_STATUS_QIACT_BIT                 (0x1UL << 11)
#define WL_STATUS_QIOPEN_BIT                (0x1UL << 12)
#define WL_STATUS_RECV_BIT                  (0x1UL << 13)
#define WL_STATUS_ENTERSEND_BIT             (0x1UL << 14)
#define WL_STATUS_SENDFINSH_BIT             (0x1UL << 15)

#define WL_STATUS_PRIVREGISTER_BIT          (0x1UL << 16)


#define ARGC_LIMIT      (12)

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
    uint32_t       status;
    uint8_t        cme_error;
    uint8_t        device_status;
    uint8_t        rssi; 
    char           sn[WL_SN_LEN + 1];
    char           imsi[WL_IMSI_LEN + 1];
    char           iccid[WL_ICCID_LEN + 1];
    char           ip[WL_IP_LEN + 1];

    uint32_t       priv_dnum;
    uint32_t       priv_fnum;
    uint8_t        priv_res_result; //回复结果
} wl_t;

extern wl_t wl;

void wl_set_status_bit(uint32_t status);
bool wl_get_status_bit(uint32_t status);
void wl_clear_status_bit(uint32_t status);

int32_t wl_ossignal_notify(int32_t signals);
void wl_init(void);

#endif /* _WL_TASK_H */
