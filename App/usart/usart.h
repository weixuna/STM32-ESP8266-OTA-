#ifndef	_USART_H_
#define	_USART_H_
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "timer.h"

#define USART_RX_MAXLEN 4096

enum
{
    RX_BUFF_EMPTY, //接收缓冲区空
    RX_BUFF_NO_EMPTY//接收缓冲区非空
};
typedef struct usart
{
    u8 flag;//接收完成标志
    u8 rx_buff[USART_RX_MAXLEN+1];//接收缓存
    u32 cnt;//接收缓冲计数器
    
}usart_t;

extern usart_t usart1;
extern usart_t usart3;
void Usarts_Init(void);
void USARTx_Init(USART_TypeDef* USARTx,u32 Baud);
void USARTx_Send_Char(USART_TypeDef* USARTx,u8 ch);
void USARTx_Send_String(USART_TypeDef* USARTx,char *str);
void USARTx_Send_Buff(USART_TypeDef* USARTx,u8 *data_buff,u16 data_len);

#endif
