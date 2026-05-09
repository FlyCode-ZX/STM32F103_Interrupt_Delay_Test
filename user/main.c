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
void TIM3_CH1_PWM_Init()
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

    TIM_TimeBaseInitStructure.TIM_Period    = 1000;            //自动装载值, PWM T=1000us
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;            // 分频系数, 72分频， 定时器输入频率 1MHZ, 1us
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 400;                       // 比较值 (决定占空比)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效
    TIM_OC1Init(TIM3,&TIM_OCInitStructure);                    //输出比较通道1初始化

    TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);           //使能TIMx在 CCR1 上的预装载寄存器
    TIM_ARRPreloadConfig(TIM3,ENABLE);                         //使能预装载寄存器
    TIM_Cmd(TIM3,ENABLE);                                      //使能定时器

}

unsigned char gpio_pa4_s;
#define IRQ_F0

#if defined(IRQ_F0)
// 660ns
void EXTI9_5_IRQHandler(void)
{
    EXTI->PR = EXTI_Line5;
    if(gpio_pa4_s){
        GPIOA->BSRR = GPIO_Pin_4;
    } else {
        GPIOA->BRR = GPIO_Pin_4;
    }
    gpio_pa4_s = !gpio_pa4_s;
}

#elif defined(IRQ_F1)
// 1us
void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR & EXTI_Line5)
    {
        EXTI->PR = EXTI_Line5;
        if(gpio_pa4_s){
            GPIOA->BSRR = GPIO_Pin_4;
        } else {
            GPIOA->BRR = GPIO_Pin_4;
        }
        gpio_pa4_s = !gpio_pa4_s;
    }
}

#elif  defined(IRQ_F2)
// 2.2us
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line5);
        if(gpio_pa4_s){
            GPIO_SetBits(GPIOA, GPIO_Pin_4);
        } else {
            GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        }
        gpio_pa4_s = !gpio_pa4_s;
    }
}
#endif

int main(void)
{
    GPIO_InitTypeDef gpio;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    Delay_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio);

    // GPIO-A5 中断输入
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &gpio);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);

    // 配置 EXTI 中断线
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;              // 选择 EXTI 线 Line5
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     // 中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发[reference:4]
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               // 使能中断线
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // 配置外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;        // 选择中断通道（Line5-9）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
    NVIC_Init(&NVIC_InitStructure);

    // GPIO-A4 GPIO反转输出
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = GPIO_Pin_4;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    TIM3_CH1_PWM_Init();
    // TIM_SetCompare1(TIM3,200);

    while (1)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500);
    }
    return 0;
}
