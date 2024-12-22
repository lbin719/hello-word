#include "main.h"
#include "ff_gen_drv.h"
#include "norflash_diskio.h"
#include "fs.h"
#include "ulog.h"
#include "version.h"

#if 0
uint16_t get_device_xoruid()
{
    uint32_t xoruid;
    uint32_t uuid32[3];

    uuid32[0] = HAL_GetUIDw0();
    uuid32[1] = HAL_GetUIDw1();
    uuid32[2] = HAL_GetUIDw2();
    xoruid = uuid32[0] ^ uuid32[1] ^ uuid32[2];

    return (uint16_t)((xoruid >> 16) ^ xoruid);
}

void update_details_file(void)
{
    FIL File;
    char read_buff[64];
    char write_buff[64];
    uint32_t readbytes;
    uint32_t writebytes;

    if (f_open(&File, "details.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) != FR_OK)
    {
        Error_Handler();
        LOG_I("open failed!\r\n");
        return;
    }

    if(f_read(&File, read_buff, sizeof(read_buff), (UINT *)&readbytes) != FR_OK)
    {
        Error_Handler();
        LOG_I("read failed!\r\n");
        return;
    }

    snprintf(write_buff, sizeof(write_buff), "Version: %s\r\nBuild: %s %s\r\n", MCU_FW_VERSION, CodeBuildDate, CodeBuildTime);
    if((readbytes == 0) || (strncmp((const char *)write_buff, (const char *)read_buff, strlen(write_buff)) != 0))
    {
        //need to update.
        f_lseek(&File, 0);
        f_write(&File, write_buff, strlen(write_buff), (UINT *)&writebytes);
    }

    f_close(&File);
}
#endif

FATFS FLASHDISKFatFs;
char FLASHDISKPath[4];

void fs_init()
{
    FRESULT result;
    DIR dir = {0};
    FILINFO fno = {0};

    if (FATFS_LinkDriver(&FLASHDISK_Driver, FLASHDISKPath) == 0)
    {
        result = f_mount(&FLASHDISKFatFs, (TCHAR const *)FLASHDISKPath, 1);
        if (result == FR_OK)
        {
            // 文件系统已存在
        }
        else if (result == FR_NO_FILESYSTEM)
        {
            // 无文件系统，可以格式化
           uint8_t buffer[_MAX_SS];
           result = f_mkfs((TCHAR const *)FLASHDISKPath, FM_ANY, 0, buffer, sizeof(buffer));
           if (result != FR_OK)
           {
                LOG_I("f_mkfs error result:%d\r\n", result);
           }
        }
        else
        {
            LOG_I("f_mount error result:%d\r\n", result);
        }
#if 0
        //写入版本信息
        update_details_file();

        /* 设置系统盘名字 */
        char label_set[12] = {0};
        sprintf(label_set, "MSC-%04x", get_device_xoruid());
        char label[12] = {0};
        if(!f_getlabel("", label, NULL) && strcmp(label, label_set))
        {
            f_setlabel(label_set);
        }
#endif
        //printf fat
//        result = f_opendir(&dir, "/");
//        if(result == FR_OK) // 打开当前目录
//        {
//            do {
//                FRESULT result = f_readdir(&dir, &fno);
//                if((result != FR_OK) || !fno.fname[0])
//                {
//                    // LOG_I("f_readdir error result:%d name:0x%02x\r\n", result, fno.fname[0]);
//                    break;
//                }
//                LOG_I("file:%s, size:%d\r\n", fno.fname, fno.fsize); // 打印符合条件的文件名
//            }while(1);
//        }
//        f_closedir(&dir);
    }

    /* 卸载驱动，防止和USB MSC冲突 */
    // FATFS_UnLinkDriver(FLASHDISKPath);
}
