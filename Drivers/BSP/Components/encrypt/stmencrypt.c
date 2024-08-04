#include "stmencrypt.h"
#include "stmflash.h"
#include "ulog.h"

const char encrypt_key[32] = {
0X9C, 0X18, 0XFF, 0XFF, 0XFF, 0X18, 0X29, 0XFF, 0XFF, 0XFF, 0X29, 0X39, 0XD6, 0XFF, 0XFF, 0X42,
0X45, 0X29, 0XB6, 0XB5, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
};

#define STM32_ID_D  852468   //�����һ���� 

//stm32оƬ��ID��ַ���ѵ�ַ��ȥһ����������������ֱ�ӳ���ID�ĵ�ַ����Ȼ�����ױ�¶������ID���й�
volatile uint32_t stm32_id_addr[]={
    UID_BASE - STM32_ID_D, 
    UID_BASE + 4 + STM32_ID_D,
    UID_BASE + 8 - STM32_ID_D
};
 

volatile void stm32_readid(volatile uint32_t *p)
{
   volatile uint32_t Addr;
// ��Ϊ�����ó����ڷ�����ֱ���ҵ������ַ�����������ַ����������ģ�
// ��stm32_id_addr�����㣬��Ȼ��Ҳ�����ø߼����㷨��ע�ⲻ���ñ������Ż������ַ
   Addr = stm32_id_addr[0] + STM32_ID_D;
   p[0] = *(uint32_t*)(Addr);
   Addr = stm32_id_addr[1] - STM32_ID_D;
   p[1] = *(uint32_t*)(Addr);
   Addr = stm32_id_addr[2] + STM32_ID_D;
   p[2] = *(uint32_t*)(Addr);
}

uint32_t stm32_generate_key(void)
{
   uint32_t stm32ID[4], dat;   
   stm32_readid(stm32ID);
   stm32ID[3] = STM32_ID_D;  

   //�������������һЩ�߼����㷨������ͼ�Ҫһ��
   //��ID�Ŵ����һ��32λ����Ҳ�������Լ����㷨��������������ݣ�����λ����
   dat = stm32ID[0] ^ stm32ID[1] - (stm32ID[2] / stm32ID[3]); //�����һ��32λ��

   return dat;
}

/********************************************************************
�������ܣ�����ID������
��ڲ�����
��    �أ�
��    ע��
********************************************************************/
void stm_encrypted_id(void)
{
    // uint32_t flash_key = *(uint32_t*)(STM32_FLASH_EN_ID_START_ADDR);  //��������ʱ��������flash�е���
    // if(flash_key == 0xFFFFFFFF)//δд��
    // {
        uint32_t key = stm32_generate_key();
        stmflash_erase(STM32_FLASH_EN_ID_START_ADDR, STM32_FLASH_EN_ID_SIZE);
        stmflash_write(STM32_FLASH_EN_ID_START_ADDR, key, sizeof(key)); //�����������д��32λ 
    // }
}
 
/********************************************************************
�������ܣ��Ƚϼ���ID����ȷ����0
��ڲ�����
��    �أ�1������ȷ��0����ȷ
��    ע��
********************************************************************/
bool stm_cmp_encryped_id(void)
{
    uint32_t flash_key = *(uint32_t*)(STM32_FLASH_EN_ID_START_ADDR);  //��������ʱ��������flash�е���
    uint32_t key = stm32_generate_key();

    if(flash_key == key)
        return true;
    else
        return false;
}
 
void stm_encrypt_init(void)
{
    if(stm_cmp_encryped_id() == false)
    {
        LOG_I("stm encrypt no write\r\n");     // ID��ȷ����������
        //����ʱ��һЩ��������������Ͷ�ID���ܣ�Ȼ��Ѽ��ܽ�����浽flash�У��Ѹó�����оƬ��ID��Ψһ��Ӧ���������������Ҳ���������Թ���	  
        if(true)
        { 
            stm_encrypted_id();   //����ID
            //  �Թ�                    //���Ѽ�����δ����flash���������ֱ������ȥ����ִ��
        }
        else
        {
            while(1);
        }
    }
    else
    {
        LOG_I("stm encrypt ok\r\n");     // ID��ȷ����������
    }
}

