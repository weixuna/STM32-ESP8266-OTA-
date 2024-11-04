#ifndef _KEY_H_
#define	_KEY_H_

#include "stm32f10x.h"
#include "sys.h"

#define KEY_S2      PAin(0)
#define KEY_S4      PAin(4)
#define KEY_S3      PAin(1)
#define KEY_S5      PAin(5)

#define KEY_S2_PIN      GPIO_Pin_0
#define KEY_S4_PIN      GPIO_Pin_4
#define KEY_S3_PIN      GPIO_Pin_1
#define KEY_S5_PIN      GPIO_Pin_5

typedef enum key_mode
{
    KEY_SINGLE_MODE,//单次模式
    KEY_CONTINUE_MODE//连续模式
}key_mode_t;

typedef enum key_val
{
    NO_KEY_PRESS,//无按键按下
    KEY_S2_PRESS,
    KEY_S3_PRESS,
    KEY_S4_PRESS,
    KEY_S5_PRESS
}key_val_t;


void KEY_Init(void);
key_val_t Get_Key_Value(key_mode_t key_mode);
#endif
