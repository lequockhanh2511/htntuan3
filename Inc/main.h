#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define ID_LOP      "D23CQDT02-"
#define ID_NHOM     "16"

#define BTN_PIN                 GPIO_PIN_0
#define BTN_GPIO_PORT           GPIOA
#define BTN_EXTI_IRQn           EXTI0_IRQn

#define USARTx                  USART1
#define USARTx_TX_PIN           GPIO_PIN_9
#define USARTx_RX_PIN           GPIO_PIN_10
#define USARTx_GPIO_PORT        GPIOA
#define USARTx_CLK_ENABLE()     __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_GPIO_CLK_ENABLE()__HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_TX_DMA_CHANNEL   DMA1_Channel4
#define USARTx_TX_DMA_IRQn      DMA1_Channel4_IRQn
#define USARTx_TX_DMA_IRQHandler DMA1_Channel4_IRQHandler

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
