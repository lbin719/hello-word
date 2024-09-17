
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

typedef struct
{
	const unsigned char *content;
	uint16_t length;
	uint16_t offset;
} parse_buffer;

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

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

    // WL_STATE_PRIV_SEND_REGISTER,
    // WL_STATE_PRIV_WAIT_REGISTER, 
    // WL_STATE_PRIV_DATA,
    // WL_STATE_CONNECT_FINISH,  
    WL_STATE_TXRX,            
} wlstate_e;


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

#define WL_PRIV_DBUHUO_RECMD            (1 + 129) // 1	设备发起补货
#define WL_PRIV_DWEIGHT_RECMD           (2 + 129) // 2	设备传感器重量变化上报
#define WL_PRIV_DUSER_RECMD             (3 + 129) // 3	设备发起用户绑盘称重
#define WL_PRIV_FCAIPING_RECMD          (4 + 129) // 4	服务器设置菜品
#define WL_PRIV_FQUPI_RECMD             (5 + 129) // 5	传感器操作（去皮）
#define WL_PRIV_FJIAOZHUN_RECMD         (6 + 129) // 6	传感器操作（校准）
#define WL_PRIV_FWEIGHT_RECMD           (7 + 129) // 7	传感器操作（获取重量）
#define WL_PRIV_FGETSTATUS_RECMD        (8 + 129) // 8	获取传感器稳定状态
#define WL_PRIV_FSAOMATUO_RECMD         (9 + 129) // 9	扫码头默认参数设置
#define WL_PRIV_FSETVOICE_RECMD         (10 + 129) // 10	设置音量大小
#define WL_PRIV_FHOT_RECMD              (11 + 129) // 11	设置加热状态
#define WL_PRIV_FHOTTIMER_RECMD         (12 + 129) // 12	设置加热等级
#define WL_PRIV_FREBOOT_RECMD           (13 + 129) // 13	设备重启
#define WL_PRIV_DREGISTER_RECMD         (14 + 129) // 14	设备注册
#define WL_PRIV_DXINTIAOBAO_RECMD       (15 + 129) // 15	设备发送心跳包


#define WL_PRIVSEND_RIGISTER_BIT        (0x1UL << 0)       
#define WL_PRIVSEND_HEART_BIT          (0x1UL << 1)    




void weight_task_handle(void);
void wireless_task_handle(void);

#endif /* __APPLICATION_H */
