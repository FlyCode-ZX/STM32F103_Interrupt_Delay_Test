#include <delay.h>
#include <stm32f10x.h>

#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"
#include "usart.h"

void _init(void) {
    int i;
    i = 0;
}

void * _sbrk(intptr_t incr) {
    (void)incr;
    return (void *)-1;
}

int main(void)
{
    GPIO_InitTypeDef c;
    Delay_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    c.GPIO_Mode = GPIO_Mode_Out_PP;
    c.GPIO_Pin = GPIO_Pin_13;
    c.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &c);

    while (1)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500);
    }
    return 0;
}