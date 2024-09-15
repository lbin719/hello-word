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

#if 0

#define ARGC_LIMIT      (12)

#define QUERY_CMD		0x01 	/* 查询命令 */
#define EXECUTE_CMD		0x02	/* 执行命令 */
#define SET_CMD			0x03	/* 设置命令 */

tyepdef struct {
	char *cmd;	/* AT指令 */
    char *respond_cmd;
	int (*deal_func)(int opt, int argc, char *argv[]);	
}at_cmd_t;

int deal_uart_func(int opt, int argc, char *argv[]);
at_cmd_t at_table[] = {
	{"AT",              "OK",       deal_rx_ok_func},
	{"ATE0",            "OK",       NULL},
	{"AT+CGSN=1",       "+CGSN:"    NULL},// +CGSN: "862584073708935"
    {"AT+CPIN?",        "+CPIN:",   NULL},// +CPIN: READY
    {"AT+CMIM",         NULL,       NULL},// 460081925003317
    {"AT+CSQ",          "+CSQ:",    NULL},// +CSQ: 31,0
    {"AT+QICLOSE=0",    "OK",       NULL},// ok
    {"AT+CGREG?",       "+CGREG:",  NULL},// +CGREG: 0,1
    {"AT+CEREG?",       "+CEREG:",  NULL},// +CEREG: 0,1	
	// AT+QICSGP=1,1,"CMIOT","","",1  OK
	// AT+QIACT=1 OK
	// AT+QIACT? +QIACT: 1,1,1,"10.27.82.43" OK
	// AT+QIOPEN=1,0,"TCP","39.106.91.24",10181,0,1  OK
	// AT+QISEND=0 \r\n hello ... SEND OK
};

#define AT_TABLE_SIZE	(sizeof(at_table) / sizeof(at_cmd_t))


/*
 * @brief 字符串拆分解析处理
 * @return 检测归类的参数个数
 **/
int string_split(char *strp, uint32_t strsize, char ch, char *argv[], uint32_t argcM )
{
	int ch_index = 0;
	int argc_index = 0;
	uint8_t spliflag = 0;
	
	if ((!strsize) || (!argcM)) return 0;

	argv[argc_index++] = &strp[ch_index];
	for (ch_index = 0; ch_index < strsize; ch_index++) {
		if (strp[ch_index] == ch) {
			strp[ch_index] = '\0';
#if 0	/* 处理连续出现ch情况 */
			if (1 == splitflag) {
				argv[argc_index++] = &strp[ch_index];
			}
#endif			
			splitflag = 1;
		} else if (splitflag == 1) {
			splitflag = 0;
			argv[argc_index++] = &strp[ch_index];
			if (argc_index >= argcM) break;
		} else {
			splitflag = 0;
		}
	}
	
	return argc_index;
}


int at_cmd_parse(uint8_t *pdata, uint16_t size)
{
	int ret  = -1;
	char *ptr = NULL;
	int argc = ARGC_LIMIT;
	uint16_t offset = 0;
	int index = 0;
	char *argv[ARGC_LIMIT] = { (char *)0 };

	// if (strstr((const char *)pdata, "AT") == NULL) goto at_end;
	for (index = 0; index < AT_TABLE_SIZE; index++) {
		ptr = strstr((const char *)pdata, at_table[index].cmd);
		if (ptr != NULL) {
			ptr += strlen(at_table[index].cmd);
			offset = ptr - (char *)pdata;
			break;
		}
	}
	if (index >= AT_TABLE_SIZE) goto at_end;

	/* 解析查询命令 */
	if ((ptr[0] == '?') && (ptr[1] == '\r') && (ptr[2] == '\n')) {
		if (NULL != at_table[index].deal_func) {
			ret = at_table[index].deal_func(QUERY_CMD, argc, argv);
		}
	} else if ((ptr[0] == '\r') && (ptr[1] == '\n')) { /* 解析执行命令 */
		if (NULL != at_table[index].deal_func) {
			ret = at_table[index].deal_func(EXECUTE_CMD, argc, argv);
		}
	} else if (ptr[0] == '=') { /* 解析设置命令 */
		ptr += 1;
		argc = string_split((char*)ptr, size - offset, ',', argv, argc);
		if (NULL != at_table[index].deal_func) {
			ret = at_table[index].deal_func(SET_CMD, argc, argv);
		}
	} else {
		ret = -1;
	}

at_end:
	if (-1 == ret) printf("ERROR\r\n");
	else printf("OK\r\n");

	return ret;
}

#define AT_RX_TIMEOUT	200	/* ms */
#define AT_RX_BUF_SIZE	512	/* bytes */

static uint8_t cmdbuf[AT_RX_BUF_SIZE];

int at_cmd_recv(uint8_t data)
{
	int ret = -1;
	static uint16_t index = 0;
	static uint32_t tick = 0;
	static uint8_t flag = 0;

	if (((HAL_GetTick() - tick) > AT_RX_TIMEOUT) || (index >= AT_RX_BUF_SIZE	)) {
		index = 0;
		flag = 0;
	}
	tick = HAL_GetTick();
	cmdbuf[index++] = data;
	if ((data == '\n') && flag) {
		ret = at_cmd_parse(cmdbuf, index);
		flag = 0;
		index = 0;
	} else if (data == '\r') {
		flag = 1;
	} else {
		flag = 0;
	}

	return ret;
 }

void at_task(void)
{
	uint8_t data;

	while (uart_recv(&data, 1) > 0) {
		at_cmd_recv(data);
	}
}

#endif

/*
 * @brief 字符串拆分解析处理
 * @return 检测归类的参数个数
 **/
int string_split(char *strp, uint32_t strsize, char ch, char *argv[], uint32_t argcM )
{
	int ch_index = 0;
	int argc_index = 0;
	uint8_t splitflag = 0;

	if ((!strsize) || (!argcM))
		return 0;

	argv[argc_index++] = &strp[ch_index];
	for (ch_index = 0; ch_index < strsize; ch_index++)
	{
		if (strp[ch_index] == ch)
		{
			strp[ch_index] = '\0';
#if 0	/* 处理连续出现ch情况 */
			if (1 == splitflag) {
				argv[argc_index++] = &strp[ch_index];
			}
#endif
			splitflag = 1;
		}
		else if (splitflag == 1)
		{
			splitflag = 0;
			argv[argc_index++] = &strp[ch_index];
			if (argc_index >= argcM)
				break;
		} else
		{
			splitflag = 0;
		}
	}

	return argc_index;
}
