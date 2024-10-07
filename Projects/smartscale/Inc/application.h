
#ifndef __APPLICATION_H
#define __APPLICATION_H

typedef enum
{
  SYS_STATUS_ZZDL = 0, // 正在登录
  SYS_STATUS_SBLX,     // 设备离线
  SYS_STATUS_SBZC,     // 设备正常
  SYS_STATUS_BHZ,      // 补货中
  SYS_STATUS_QBDCP,    // 请绑定餐盘
  SYS_STATUS_QQC,      // 请取餐
  SYS_STATUS_QQFHCP,   // 请先放好餐盘
}sys_status_e;


extern sys_status_e sys_status;

int get_change_weight(void);
void weight_task_handle(void);

void mj8000_task_handle(void);

void sys_task_handle(void);

uint8_t get_sys_status(void);

#endif /* __APPLICATION_H */
