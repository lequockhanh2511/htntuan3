#include <stdint.h>
#include "i2c.h"
#include "ds1307.h"

/* RCC */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)

/* GPIOA */
#define GPIOA_CRH     (*(volatile uint32_t *)0x40010804)

/* USART1 */
#define USART1_SR     (*(volatile uint32_t *)0x40013800)
#define USART1_DR     (*(volatile uint32_t *)0x40013804)
#define USART1_BRR    (*(volatile uint32_t *)0x40013808)
#define USART1_CR1    (*(volatile uint32_t *)0x4001380C)

void delay(void)
{
    volatile uint32_t i;

    for (i = 0; i < 800000; i++)
    {
    }
}

void UART_Init(void)
{
    /* GPIOA clock */
    RCC_APB2ENR |= (1 << 2);

    /* USART1 clock */
    RCC_APB2ENR |= (1 << 14);

    /*
     * PA9 = TX
     * PA10 = RX
     */

    /* PA9 AF Push-Pull */
    GPIOA_CRH &= ~(0xF << 4);
    GPIOA_CRH |=  (0xB << 4);

    /* PA10 input floating */
    GPIOA_CRH &= ~(0xF << 8);
    GPIOA_CRH |=  (0x4 << 8);

    /* 9600 baud, 8 MHz */
    USART1_BRR = 0x341;

    /* USART enable */
    USART1_CR1 |= (1 << 13);

    /* Transmitter enable */
    USART1_CR1 |= (1 << 3);

    /* Receiver enable */
    USART1_CR1 |= (1 << 2);
}

void UART_SendChar(char c)
{
    while (!(USART1_SR & (1 << 7)))
    {
    }

    USART1_DR = c;
}

void UART_SendString(const char *str)
{
    while (*str)
    {
        UART_SendChar(*str);
        str++;
    }
}

char UART_ReadChar(void)
{
    while (!(USART1_SR & (1 << 5)))
    {
    }

    return (char)USART1_DR;
}

void UART_SendNumber(uint8_t number)
{
    UART_SendChar('0' + number / 10);
    UART_SendChar('0' + number % 10);
}

void UART_PrintTime(RTC_Time *time)
{
    UART_SendString("RTC: ");

    UART_SendNumber(time->hour);
    UART_SendChar(':');

    UART_SendNumber(time->minute);
    UART_SendChar(':');

    UART_SendNumber(time->second);

    UART_SendString("  ");

    UART_SendNumber(time->date);
    UART_SendChar('/');

    UART_SendNumber(time->month);
    UART_SendString("/20");

    UART_SendNumber(time->year);

    UART_SendString("\r\n");
}

uint8_t is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

uint8_t get_number(char a, char b)
{
    return (uint8_t)((a - '0') * 10 + (b - '0'));
}

void UART_SetTime(void)
{
    char buffer[24];
    uint8_t i = 0;

    UART_SendString("\r\nNhap: SET HH:MM:SS DD/MM/20YY\r\n");
    UART_SendString("> ");

    while (i < 23)
    {
        char c = UART_ReadChar();

        UART_SendChar(c);

        if (c == '\r' || c == '\n')
            break;

        buffer[i++] = c;
    }

    buffer[i] = '\0';

    /*
     * Expected:
     * SET 12:34:56 22/09/2026
     */

    if (i < 23)
    {
        UART_SendString("\r\nSai dinh dang!\r\n");
        return;
    }

    if (buffer[0] != 'S' ||
        buffer[1] != 'E' ||
        buffer[2] != 'T' ||
        buffer[3] != ' ')
    {
        UART_SendString("\r\nSai dinh dang!\r\n");
        return;
    }

    if (!is_digit(buffer[4]) ||
        !is_digit(buffer[5]) ||
        buffer[6] != ':' ||
        !is_digit(buffer[7]) ||
        !is_digit(buffer[8]) ||
        buffer[9] != ':' ||
        !is_digit(buffer[10]) ||
        !is_digit(buffer[11]) ||
        buffer[12] != ' ' ||
        !is_digit(buffer[13]) ||
        !is_digit(buffer[14]) ||
        buffer[15] != '/' ||
        !is_digit(buffer[16]) ||
        !is_digit(buffer[17]) ||
        buffer[18] != '/' ||
        !is_digit(buffer[19]) ||
        !is_digit(buffer[20]) ||
        !is_digit(buffer[21]) ||
        !is_digit(buffer[22]))
    {
        UART_SendString("\r\nSai dinh dang!\r\n");
        return;
    }

    RTC_Time time;

    time.hour   = get_number(buffer[4], buffer[5]);
    time.minute = get_number(buffer[7], buffer[8]);
    time.second = get_number(buffer[10], buffer[11]);

    time.date   = get_number(buffer[13], buffer[14]);
    time.month  = get_number(buffer[16], buffer[17]);

    /*
     * DS1307 stores only 2 digits of year.
     * 2026 -> 26
     */
    time.year = get_number(buffer[21], buffer[22]);

    /*
     * Sunday = 1
     * We use 1 as default day-of-week.
     */
    time.day = 1;

    if (time.hour > 23 ||
        time.minute > 59 ||
        time.second > 59 ||
        time.date < 1 ||
        time.date > 31 ||
        time.month < 1 ||
        time.month > 12)
    {
        UART_SendString("\r\nGia tri thoi gian khong hop le!\r\n");
        return;
    }

    if (DS1307_SetTime(&time))
    {
        UART_SendString("\r\nDa cai dat thoi gian!\r\n");
    }
    else
    {
        UART_SendString("\r\nLoi ghi DS1307!\r\n");
    }
}

int main(void)
{
    RTC_Time time;

    UART_Init();
    I2C1_Init();

    UART_SendString("\r\n========================\r\n");
    UART_SendString("STM32F103 + DS1307\r\n");
    UART_SendString("========================\r\n");

    while (1)
    {
        /*
         * If user sends 's', configure time
         */
        if (USART1_SR & (1 << 5))
        {
            char c = (char)USART1_DR;

            if (c == 's' || c == 'S')
            {
                UART_SetTime();
            }
        }

        if (DS1307_ReadTime(&time))
        {
            UART_PrintTime(&time);
        }
        else
        {
            UART_SendString("Loi: khong doc duoc DS1307!\r\n");
        }

        delay();
    }
}
