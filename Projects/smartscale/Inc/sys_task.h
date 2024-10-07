
#ifndef __SYS_TASK_H
#define __SYS_TASK_H

#define SYS_TASK_NOTIFY                  (0xFFFFFFFFUL)
#define SYS_NOTIFY_FCT_BIT               (0x1UL << 0)
#define SYS_NOTIFY_MJ_BIT                (0x1UL << 1)
#define SYS_NOTIFY_WLLX_BIT              (0x1UL << 2)
#define SYS_NOTIFY_WLREGISTER_BIT        (0x1UL << 3)

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

int32_t sys_ossignal_notify(int32_t signals);
void sys_init(void);

#endif /* __SYS_TASK_H */