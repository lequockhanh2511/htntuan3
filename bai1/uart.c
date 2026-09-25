#include "uart.h"
#include "stm32f103xb.h"

void UART1_Init(void)
{
    /*
     * PA9  = USART1_TX
     * PA10 = USART1_RX
     */

    /* Clock GPIOA */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* Clock USART1 */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /*
     * PA9:
     * Alternate Function Push-Pull
     * 50 MHz
     */

    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_0 |
                  GPIO_CRH_MODE9_1 |
                  GPIO_CRH_CNF9_1;

    /*
     * PA10:
     * Input floating
     */

    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    /*
     * APB2 clock = 8 MHz
     * Baudrate = 115200
     *
     * USARTDIV = 8000000 / (16 * 115200)
     *          ≈ 4.34
     *
     * BRR ≈ 0x45
     */

    USART1->BRR = 0x45;

    /*
     * Enable:
     * TX
     * RX
     * USART
     */

    USART1->CR1 =
        USART_CR1_TE |
        USART_CR1_RE |
        USART_CR1_UE;
}

void UART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE))
    {
    }

    USART1->DR = c;
}

void UART1_SendString(const char *str)
{
    while (*str)
    {
        UART1_SendChar(*str++);
    }
}

int UART1_ReadChar(char *c)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        *c = USART1->DR;
        return 1;
    }

    return 0;
}
