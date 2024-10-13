
#ifndef __SYS_TASK_H
#define __SYS_TASK_H

#define MJ_STR_MAX_LEN                   (64)


#define SYS_TASK_NOTIFY                  (0xFFFFFFFFUL)
#define SYS_NOTIFY_FCT_BIT               (0x1UL << 0)
#define SYS_NOTIFY_MJRX_BIT              (0x1UL << 1)
#define SYS_NOTIFY_MJIDLE_BIT            (0x1UL << 2)
#define SYS_NOTIFY_MJBUHUO_BIT           (0x1UL << 3)
#define SYS_NOTIFY_MJBPENTER_BIT         (0x1UL << 4)
#define SYS_NOTIFY_MJBPEXIT_BIT          (0x1UL << 5)
#define SYS_NOTIFY_WLBPENTER_BIT         (0x1UL << 6)
#define SYS_NOTIFY_WLLX_BIT              (0x1UL << 7)
#define SYS_NOTIFY_WLREGISTER_BIT        (0x1UL << 8)
#define SYS_NOTIFY_WEIGHTTIME_BIT        (0x1UL << 9)
typedef enum
{
  SYS_STATUS_ZZDL = 0, // 正在登录
  SYS_STATUS_SBLX,     // 设备离线
  SYS_STATUS_SBZC,     // 设备正常
  SYS_STATUS_BHZ,      // 补货中
  SYS_STATUS_QBDCP,    // 请绑定餐盘
  SYS_STATUS_QQC,      // 请取餐
  SYS_STATUS_QXFHCP,   // 请先放好餐盘
}sys_status_e;

extern char mj_str[MJ_STR_MAX_LEN + 1];
extern int upload_cweight;
extern int upload_sweight;

uint8_t get_sys_status(void);
int get_change_weight(void);

int32_t sys_ossignal_notify(int32_t signals);
void sys_task_handle(void);
void sys_init(void);

#endif /* __SYS_TASK_H */
