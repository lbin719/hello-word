#include "system_info.h"
#include "main.h"
#include "string.h"
#include "stmflash.h"
#include "hx711.h"
#include "wtn6040.h"
#include "system_info.h"

static sysinfo_store_t sysinfo_store = {0};
//static nvms_store_t nvms_store = {0};

// uint32_t sys_get_fw_version_int(void)
// {
//   return ((MCU_FW_VERSION_MAX << 24) + (MCU_FW_VERSION_MAJOR << 16) + (MCU_FW_VERSION_MINOR << 8) + MCU_FW_VERSION_REVISION);
// }

// char* sys_get_hw_version(char *ver, int len)
// {
//   uint8_t *hw_ver = (uint8_t *)&nvms_store.hw_rev;
//   snprintf(ver, (len - 1), "%d.%d.%d.%d", hw_ver[3], hw_ver[2], hw_ver[1], hw_ver[0]);
//   return ver;
// }

// uint32_t sys_get_hw_version_int(void)
// {
//   return ((nvms_store.hw_rev[3] << 24) + (nvms_store.hw_rev[2] << 16) + (nvms_store.hw_rev[1] << 8) + nvms_store.hw_rev[0]);
// }

// char* sys_get_serial_num(char *sn, int len)
// {
//   memcpy(sn, nvms_store.serial_num, MIN(len, NVMS_SN_LEN));
//   return sn;
// }

// char* sys_get_bsn_num(char *bsn, int len)
// {
//   memcpy(bsn, nvms_store.bsn2_num, MIN(len, NVMS_BSN2_LEN));
//   return bsn;
// }

static void sys_nvms_init(void)
{
//  stmflash_read(FLASH_NVMS_START_ADDR, (uint8_t *)&nvms_store, sizeof(nvms_store));
//
//  if(nvms_store.magic != NVMS_MAGIC)
//  {
//    //set default
//    memset(&nvms_store, 0xFF, sizeof(nvms_store));
//  }
}

//void sysinfo_store_hxparam(hx_param_t *param)
//{
//  stmflash_erase(STMFLASH_SYSINFO_START_ADDR, STMFLASH_SYSINFO_SIZE);
//  stmflash_write(STMFLASH_SYSINFO_START_ADDR, (uint8_t *)&sysinfo_store, sizeof(sysinfo_store));
//}
//hx_param_t* sysinfo_get_hxparam(void)
//{
//  return sysinfo_store.hx_param;
//}

void sysinfo_store_hxzero(uint32_t zero)
{
  sysinfo_store.hxzero = zero;

  stmflash_erase(STMFLASH_SYSINFO_START_ADDR, STMFLASH_SYSINFO_SIZE);
  stmflash_write(STMFLASH_SYSINFO_START_ADDR, (uint32_t *)&sysinfo_store, sizeof(sysinfo_store));
}

uint32_t sysinfo_get_hxzero(void)
{
  return sysinfo_store.hxzero;
}

void sysinfo_store_hxgap(float gap)
{
  sysinfo_store.hxgap = gap;

  stmflash_erase(STMFLASH_SYSINFO_START_ADDR, STMFLASH_SYSINFO_SIZE);
  stmflash_write(STMFLASH_SYSINFO_START_ADDR, (uint32_t *)&sysinfo_store, sizeof(sysinfo_store));
}

float sysinfo_get_hxgap(void)
{
  return sysinfo_store.hxgap;
}

void sysinfo_store_caipin(caiping_data_t *caipin)
{
  memcpy(&sysinfo_store.caiping_store, &caipin, sizeof(caiping_data_t));
  stmflash_erase(STMFLASH_SYSINFO_START_ADDR, STMFLASH_SYSINFO_SIZE);
  stmflash_write(STMFLASH_SYSINFO_START_ADDR, (uint32_t *)&sysinfo_store, sizeof(sysinfo_store));
}

caiping_data_t* sysinfo_get_caipin(void)
{
  return &sysinfo_store.caiping_store;
}

void sysinfo_store_voice(uint8_t voice)
{
  sysinfo_store.wtn_voice = voice;
  stmflash_erase(STMFLASH_SYSINFO_START_ADDR, STMFLASH_SYSINFO_SIZE);
  stmflash_write(STMFLASH_SYSINFO_START_ADDR, (uint32_t *)&sysinfo_store, sizeof(sysinfo_store));
}
uint8_t sysinfo_get_voice(void)
{
  sysinfo_store.wtn_voice;
}

static void sysinfo_init(void)
{
  stmflash_read(STMFLASH_SYSINFO_START_ADDR, (uint8_t *)&sysinfo_store, sizeof(sysinfo_store));

  if(sysinfo_store.magic != SYSINFO_MAGIC)
  {
    //set default
    sysinfo_store.magic = SYSINFO_MAGIC;
    sysinfo_store.hxzero = HX_DEFAULT_ZERO;
    sysinfo_store.hxgap = HX_DEFAULT_GAP;
    memcpy(&sysinfo_store.caiping_store, &default_caiping_data, sizeof(caiping_data_t));
    sysinfo_store.wtn_voice = WTN6040_DEFAULT_VOICE;

    stmflash_erase(STMFLASH_SYSINFO_START_ADDR, STMFLASH_SYSINFO_SIZE);
    stmflash_write(STMFLASH_SYSINFO_START_ADDR, (uint32_t *)&sysinfo_store, sizeof(sysinfo_store));
  }
}

void sys_data_init(void)
{
  sysinfo_init();
  sys_nvms_init();
}
