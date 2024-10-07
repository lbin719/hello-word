#include <string.h>
#include <stdint.h>
#include "stmencrypt.h"
#include "ulog.h"
#include "hx711.h"
#include "mj8000.h"
#include "hot.h"
#include "sys_task.h"

typedef void (*fct_test_func_t)(uint8_t argc, char **argv);
typedef struct
{
    const char *cmd;
    fct_test_func_t func;
    bool user_supported;
} fct_test_func_mapping_t;

void fct_encrypt_test(uint8_t argc, char **argv);
void fct_power_test(uint8_t argc, char **argv);
void fct_hx711_test(uint8_t argc, char **argv);
void fct_wtn6040_test(uint8_t argc, char **argv);
void fct_mj8000_test(uint8_t argc, char **argv);
void fct_hot_test(uint8_t argc, char **argv);

static const fct_test_func_mapping_t fct_test_func[]={
    {"key",     fct_encrypt_test,   true},
    {"power",   fct_power_test,     true},
    {"hx711",   fct_hx711_test,     true},    
    {"wtn6040", fct_wtn6040_test,   true},
    {"mj8000",  fct_mj8000_test,    true},
    {"hot",     fct_hot_test,       true},    
    // {"led", fct_led_test, false},
    // {"bootfac", fct_bootfac_test, true},
    // {"time", fct_rtc_test, false}
};

#define SHELL_COMMAND_MAX_LENGTH 128         
#define SHELL_COMMAND_MAX_LENGTH 128  /* 定义最大接收字节数 */
#define SHELL_PARAMETER_MAX_NUMBER 16

typedef struct
{
    volatile uint8_t sta;                    /*  接收状态 0x80 接收完成标志；0x40 接收到0x0d（即回车键）*/  
    char buffer[SHELL_COMMAND_MAX_LENGTH];   /**< shell命令缓冲 */
    volatile uint16_t length;                /**< shell命令长度 */
    char *param[SHELL_PARAMETER_MAX_NUMBER]; /**< shell参数 */
} SHELL_TypeDef;

static SHELL_TypeDef shell = {0};

void cmd_factorytest(int argc, char *argv[])
{
#if 1
    LOG_I("argc: %d argv:", argc);
    for (int i = 0; i < argc; i++)
        LOG_I_NOTICK(" %s", argv[i]);
    LOG_I_NOTICK("\r\n");
#endif

    if (argc < 2)
        return;

    /* 产线测试指令格式：
        fct cmd value1 value2 ...
    */
    if (strcmp((const char *)argv[0], "fct"))
    {
        LOG_I("cmd unsupported\r\n");
        return;
    }

    for (uint32_t i = 0; i < (sizeof(fct_test_func) / sizeof(fct_test_func_mapping_t)); i++)
    {
        if (!strcmp(argv[1], fct_test_func[i].cmd))
        {
            LOG_I("found cmd=%s\r\n", fct_test_func[i].cmd);

//            if((fct_test_func[i].user_supported == false) && (sys_get_fctmode() == false))
//            {
//            	LOG_I("user mode nonsupport!\r\n");
//                return;
//            }

            if (fct_test_func[i].func)
            {
                fct_test_func[i].func(argc, argv);
            }
            break;
        }
    }
}

/*
    参数解析：
    当收到 \r\n 时，调用这个函数把数据传入
*/
static void shellEnter(SHELL_TypeDef *shell)
{
    unsigned char paramCount = 0;
    unsigned char quotes = 0;
    unsigned char record = 1;

    if (shell->length == 0)
        return;

    /* 解析命令、参数 */
    *(shell->buffer + shell->length++) = 0;
    for (unsigned short i = 0; i < shell->length; i++)
    {
        if ((quotes != 0 ||
             (*(shell->buffer + i) != ' ' &&
              *(shell->buffer + i) != '\t' &&
              *(shell->buffer + i) != ',')) &&
            *(shell->buffer + i) != 0)
        {
            if (*(shell->buffer + i) == '\"')
            {
                quotes = quotes ? 0 : 1;
                *(shell->buffer + i) = 0;
                continue;
            }
            if (record == 1)
            {
                shell->param[paramCount++] = shell->buffer + i;
                record = 0;
            }
            if (*(shell->buffer + i) == '\\' &&
                *(shell->buffer + i + 1) != 0)
            {
                i++;
            }
        }
        else
        {
            *(shell->buffer + i) = 0;
            record = 1;
        }
    }
    shell->length = 0;
    if (paramCount == 0)
    {
        return;
    }

    cmd_factorytest(paramCount, shell->param);
}


void fct_task_handle(void)
{
    if (shell.sta & 0x80)        /* 串口接收完成？ */
    {
        shell.buffer[shell.length] = '\0';     /* 在末尾加入结束符. */
        LOG_I("%s\r\n", shell.buffer);
        shellEnter(&shell);
        shell.sta = 0;             /* 开启下一次接收 */
    }
}


void usmart_recive_callback(uint8_t *rx_data, uint16_t len)
{
    while(len--)
    {
        if ((shell.sta & 0x80) == 0)                    /* 接收未完成 */
        {
            if (shell.sta & 0x40)                       /* 接收到了0x0d（即回车键） */
            {
                if (*rx_data != '\n')                   /* 接收到的不是0x0a（即不是换行键） */
                {
                    shell.sta = 0;                 /* 接收错误,重新开始 */
                }
                else                                    /* 接收到的是0x0a（即换行键） */
                {
                    shell.sta |= 0x80;                  /* 接收完成了 */
                    sys_ossignal_notify(SYS_NOTIFY_FCT_BIT);
                }
            }
            else                                        /* 还没收到0X0d（即回车键） */
            {
                if (*rx_data == '\r')
                    shell.sta |= 0x40;
                else
                {
                    shell.buffer[shell.length] = *rx_data;
                    shell.length++;

                    if (shell.length > (SHELL_COMMAND_MAX_LENGTH - 1))
                    {
                        shell.sta = 0;
                        shell.length = 0;             /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        rx_data++;
    }
}


void fct_encrypt_test(uint8_t argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[2], "solea48b"))
    {
        uint32_t result = stmencrypt_write_key();
        LOG_I("key write:%s\r\n", result ? "fail" : "success");
    }
    else
    {
        LOG_I("param err!\r\n");
        return ;
    }
}

void fct_power_test(uint8_t argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[2], "reboot"))
    {
        LOG_I("system reboot...\r\n");
        HAL_Delay(100);
        NVIC_SystemReset();
    }
    else
    {
        LOG_I("param err!\r\n");
        return;
    }
}

void fct_hx711_test(uint8_t argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[2], "zero"))
    {
        hx711_set_zero();
    }
    else if (argc == 4 && !strcmp(argv[2], "cala"))
    {
        uint32_t cali_weight = str_toint(argv[3]);
        hx711_set_calibration(cali_weight);
    }
    else
    {
        LOG_I("param err!\r\n");
        return;
    }
}

void fct_wtn6040_test(uint8_t argc, char **argv)
{
    if (argc == 4 && !strcmp(argv[2], "level"))
    {
        uint8_t level = str_toint(argv[3]);
        wtn6040_set_voice_store(level);
    }
    else if (argc == 4 && !strcmp(argv[2], "play"))
    {
        uint8_t index = str_toint(argv[3]);
        wtn6040_play(index);
    }   
    else
    {
        LOG_I("param err!\r\n");
        return;
    }
}

void fct_mj8000_test(uint8_t argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[2], "set"))
    {
        mj8000_setconfig();
        LOG_I("fct mj8000 set ret:0\r\n");
    } 
    else
    {
        LOG_I("param err!\r\n");
        return;
    }
}

void fct_hot_test(uint8_t argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[2], "on"))
    {
        hot_ctrl(HOT_ON);
        LOG_I("fct hot on ret:0\r\n");
    }
    else if (argc == 3 && !strcmp(argv[2], "off"))
    {
        hot_ctrl(HOT_OFF);
        LOG_I("fct hot off ret:0\r\n");
    }  
    else
    {
        LOG_I("param err!\r\n");
        return;
    }
}
