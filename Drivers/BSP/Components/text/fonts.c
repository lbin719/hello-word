#include "stdint.h"
#include "ff.h"
#include "ulog.h"
#include "fonts.h"


/* 字库存放在磁盘中的路径 */
char *const FONT_GBK_PATH[] =
{
    // "UNIGBK.BIN",    
    // "GBK12.FON",       
    // "GBK16.FON",       
    // "GBK24.FON",      
    "GBK32.FON",     
    "GBK40.FON",   
};

void fonts_init(void)
{
	FIL File;
    FRESULT result;

    for(uint8_t i = 0; i < sizeof(FONT_GBK_PATH)/sizeof(char *); i++)
    {
        result = f_open(&File, FONT_GBK_PATH[i], FA_READ);
        LOG_I("f_open:%s result: %s\r\n", FONT_GBK_PATH[i], result ? "fail" : "success");
    }

}












