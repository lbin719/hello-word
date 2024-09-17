#include "stdio.h"
#include "str.h"

int str_toint(const char* str) 
{
    int num = 0;
    int sign = 1;
    int i = 0;

    // 处理字符串中的空格
    while (str[i] == ' ') {
        i++;
    }

    // 处理正负号
    if (str[i] == '+' || str[i] == '-') {
        sign = (str[i++] == '+') ? 1 : -1;
    }

    // 将字符串转换为整数
    while (str[i] >= '0' && str[i] <= '9') {
        num = num * 10 + str[i++] - '0';
    }

    return sign * num;
}

int str_length(char *buf)
{
    int i = 0;
    do{
        if((buf[i] == '\r') || (buf[i] == '\n') || (buf[i] == '\0'))
        	break;
        else
            i++;
    } while(1);

    return i;
}

// char* str_find_number(char *str, uint16_t len, char *num_buf, uint16_t *num_len)
// {
//     int i = 0;
//     // 处理字符串中的空格
//     while (str[i] < '0' || str[i] > '9') {
//         i++;
//         if(i >= len)
//         {
//             num_buf = NULL;
//             return NULL;
//         }
//     }
//     num_buf = &str[i];

//     *num_len = 0;
//     while (str[i] >= '0' || str[i] <= '9') {
//         num_len++;
//     }

//     return num_buf;
// }

int str_char_offest(char *buf, char data)
{
    int i;
    for(i = 0; buf[i] != data; i++)
    {
        if(buf[i] == '\0')
            return -1;
    }

    return i;
}

int str_num_offest(char *str)
{
    int i = 0;
    // 处理字符串中的空格
    while (str[i] < '0' || str[i] > '9') {
        i++;
        if(str[i] == '\0')
            return -1;
    }

    return i;
}

uint32_t str_split(char *str, uint32_t strsize, char *argv[], uint32_t argcm)
{
    unsigned char quotes = 0;
    unsigned char record = 1;

    if (str == NULL || strsize == 0)
        return 0;

    /* 解析命令、参数 */
    // *(str + strsize++) = 0;
    for (unsigned short i = 0; i < strsize; i++)
    {
        if ((quotes != 0 ||
             (*(str + i) != ' ' &&
              *(str + i) != '\t' &&
			  *(str + i) != '\/' &&
			  *(str + i) != ':' &&
              *(str + i) != ',')) &&
            *(str + i) != 0)
        {
            if (*(str + i) == '\"')
            {
                quotes = quotes ? 0 : 1;
                *(str + i) = 0;
                continue;
            }
            if (record == 1)
            {
                argv[argcm++] = str + i;
                record = 0;
            }
            if (*(str + i) == '\\' &&
                *(str + i + 1) != 0)
            {
                i++;
            }
        }
        else
        {
            *(str + i) = 0;
            record = 1;
        }
    }
    strsize = 0;

	return argcm;
}