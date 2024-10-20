#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <stdint.h>
#include <stdbool.h>
#include "ui_task.h"

#define SYS_INFO_OK                 (0)
#define SYS_INFO_FAIL               (-1)


#define SYSINFO_MAGIC               (0xF5F5F504)
#define NVMS_MAGIC                  (0x55AA55AA)

#define NVMS_INIT_VERSION_0         (0X00000000)
#define NVMS_INIT_VERSION_1         (0XFFFFFFFF)

#define NVMS_SN_LEN                 (20)
#define NVMS_HW_VER_LEN             (4)
#define NVMS_ADDR_LEN               (6)
#define NVMS_PUBKEY_LEN             (64)

typedef struct {
    uint32_t magic;                         // magic for key 是否有效
    uint8_t hw_rev[NVMS_HW_VER_LEN];        // hardware revision
    uint8_t public_key[NVMS_PUBKEY_LEN];    // public key
    uint8_t serial_num[NVMS_SN_LEN];        // for serial sn
    uint32_t boot_magic;                    //启动模式 USER_FW, FCTEST
    uint32_t boot_mode;                     //启动标志
} nvms_store_t;

typedef struct {
    uint32_t magic;                       // sysinfo for key 
    uint32_t hxzero;
    float hxgap;

    caiping_data_t caiping_store;
    uint8_t wtn_voice;
    uint8_t hot_mode;
    uint8_t hot_time;
    uint8_t resvered[1];

} sysinfo_store_t;

void sysinfo_store_hxzero(uint32_t zero);
uint32_t sysinfo_get_hxzero(void);

void sysinfo_store_hxgap(float cali);
float sysinfo_get_hxgap(void);

void sysinfo_store_caipin(caiping_data_t *caipin);
caiping_data_t* sysinfo_get_caipin(void);


void sysinfo_store_voice(uint8_t voice);
uint8_t sysinfo_get_voice(void);

void sysinfo_store_hot(uint8_t mode, uint8_t time);
void sysinfo_get_hot(uint8_t *mode, uint8_t *time);

// int sys_get_fw_version(char *ver, int len);
// uint32_t sys_get_fw_version_int(void);
// char* sys_get_hw_version(char *ver, int len);
// uint32_t sys_get_hw_version_int(void);
// char* sys_get_serial_num(char *sn, int len);
// void sys_set_fctmode(bool mode);
// bool sys_get_fctmode(void);

void sys_data_init(void);

#endif /* SYSTEM_INFO_H */