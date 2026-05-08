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


/*******************************************************************************
* 函 数 名         : TIM3_CH1_PWM_Init
* 函数功能           : TIM3通道1 PWM初始化函数
* 输    入         : per:重装载值
                     psc:分频系数
* 输    出         : 无
*******************************************************************************/
void TIM3_CH1_PWM_Init(u16 per,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    /*  配置GPIO的模式和IO口 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    // GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//改变指定管脚的映射

    TIM_TimeBaseInitStructure.TIM_Period=per;   //自动装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //分频系数
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 200;                       // 比较值 (决定占空比)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效
    TIM_OC1Init(TIM3,&TIM_OCInitStructure);                    //输出比较通道1初始化

    TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable); //使能TIMx在 CCR1 上的预装载寄存器
    TIM_ARRPreloadConfig(TIM3,ENABLE);//使能预装载寄存器
    TIM_Cmd(TIM3,ENABLE);             //使能定时器

}

int main(void)
{
    GPIO_InitTypeDef c;
    Delay_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    c.GPIO_Mode = GPIO_Mode_Out_PP;
    c.GPIO_Pin = GPIO_Pin_13;
    c.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &c);

    TIM3_CH1_PWM_Init(500,72-1);
    TIM_SetCompare1(TIM3,200);

    while (1)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500);
    }
    return 0;
}
