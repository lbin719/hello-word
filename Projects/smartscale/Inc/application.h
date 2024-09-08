
#ifndef __APPLICATION_H
#define __APPLICATION_H

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

                            //AT+QICSGP=1,1,"CMIOT","","",1//配置 TCP/IP 场景参数

    WL_STATE_QIACT,         // AT+QIACT=1 
    WL_STATE_WAIT_QIACT, 
                        
    WL_STATE_GET_QIACT,     // AT+QIACT?
    WL_STATE_WAIT_GET_QIACT,  

    WL_STATE_GET_QIOPEN,     
    WL_STATE_WAIT_QIOPEN,  

    WL_STATE_CONNECT_FINISH,  

    WL_STATE_TXRX,            
} wlstate_e;


#define WL_SN_LEN                       (16)
#define WL_IMSI_LEN                     (16)
#define WL_ICCID_LEN                    (20)
#define WL_IP_LEN                       (16)   

typedef struct{
    wlstate_e      state;
    uint32_t       respond_timer;

    bool           sim_status; 
    uint8_t        rssi; 
    char           sn[WL_SN_LEN + 1];
    char           imsi[WL_IMSI_LEN + 1];
    char           iccid[WL_ICCID_LEN + 1];
    char           ip[WL_IP_LEN + 1];
} wl_t;

void weight_task_handle(void);
void wireless_task_handle(void);

#endif /* __APPLICATION_H */
