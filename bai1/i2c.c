#include <stdint.h>
#include "i2c.h"

/* RCC */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)
#define RCC_APB1ENR   (*(volatile uint32_t *)0x4002101C)

/* GPIOB */
#define GPIOB_CRL     (*(volatile uint32_t *)0x40010C00)

/* I2C1 */
#define I2C1_CR1      (*(volatile uint32_t *)0x40005400)
#define I2C1_CR2      (*(volatile uint32_t *)0x40005404)
#define I2C1_OAR1     (*(volatile uint32_t *)0x40005408)
#define I2C1_DR       (*(volatile uint32_t *)0x40005410)
#define I2C1_SR1      (*(volatile uint32_t *)0x40005414)
#define I2C1_SR2      (*(volatile uint32_t *)0x40005418)
#define I2C1_CCR      (*(volatile uint32_t *)0x4000541C)
#define I2C1_TRISE    (*(volatile uint32_t *)0x40005420)

#define I2C_TIMEOUT   1000000

static int wait_flag(uint32_t flag)
{
    uint32_t timeout = I2C_TIMEOUT;

    while (!(I2C1_SR1 & flag))
    {
        if (--timeout == 0)
            return 0;
    }

    return 1;
}

int I2C1_Init(void)
{
    /* Clock GPIOB */
    RCC_APB2ENR |= (1 << 3);

    /* Clock I2C1 */
    RCC_APB1ENR |= (1 << 21);

    /*
     * PB6 = I2C1_SCL
     * PB7 = I2C1_SDA
     * Alternate function open-drain 50 MHz
     */
    GPIOB_CRL &= ~((0xF << 24) | (0xF << 28));
    GPIOB_CRL |=  ((0xF << 24) | (0xF << 28));

    /* Reset I2C */
    I2C1_CR1 |= (1 << 15);
    I2C1_CR1 &= ~(1 << 15);

    /*
     * APB1 = 8 MHz
     * I2C = 100 kHz
     */
    I2C1_CR2 = 8;
    I2C1_CCR = 40;
    I2C1_TRISE = 9;

    I2C1_OAR1 = 0x4000;

    /* Enable I2C */
    I2C1_CR1 = (1 << 0);

    return 1;
}

static int I2C1_Start(void)
{
    uint32_t timeout = I2C_TIMEOUT;

    I2C1_CR1 |= (1 << 8);

    while (!(I2C1_SR1 & (1 << 0)))
    {
        if (--timeout == 0)
            return 0;
    }

    return 1;
}

static void I2C1_Stop(void)
{
    I2C1_CR1 |= (1 << 9);
}

static int I2C1_SendAddress(uint8_t address)
{
    I2C1_DR = address;

    if (!wait_flag(1 << 1))
        return 0;

    /* Clear ADDR */
    (void)I2C1_SR1;
    (void)I2C1_SR2;

    return 1;
}

int I2C1_Write(uint8_t address, uint8_t reg,
               uint8_t *data, uint8_t length)
{
    uint8_t i;

    if (!I2C1_Start())
        return 0;

    /* Write address */
    if (!I2C1_SendAddress((address << 1) | 0))
    {
        I2C1_Stop();
        return 0;
    }

    /* Register address */
    I2C1_DR = reg;

    if (!wait_flag(1 << 7))
    {
        I2C1_Stop();
        return 0;
    }

    /* Data */
    for (i = 0; i < length; i++)
    {
        I2C1_DR = data[i];

        if (!wait_flag(1 << 7))
        {
            I2C1_Stop();
            return 0;
        }
    }

    I2C1_Stop();

    return 1;
}

int I2C1_Read(uint8_t address, uint8_t reg,
              uint8_t *data, uint8_t length)
{
    uint8_t i;

    /* First: write register address */
    if (!I2C1_Start())
        return 0;

    if (!I2C1_SendAddress((address << 1) | 0))
    {
        I2C1_Stop();
        return 0;
    }

    I2C1_DR = reg;

    if (!wait_flag(1 << 7))
    {
        I2C1_Stop();
        return 0;
    }

    /* Repeated START */
    if (!I2C1_Start())
        return 0;

    if (!I2C1_SendAddress((address << 1) | 1))
    {
        I2C1_Stop();
        return 0;
    }

    /* Enable ACK */
    I2C1_CR1 |= (1 << 10);

    for (i = 0; i < length; i++)
    {
        if (i == length - 1)
        {
            /* Last byte: NACK + STOP */
            I2C1_CR1 &= ~(1 << 10);
            I2C1_Stop();
        }

        if (!wait_flag(1 << 6))
        {
            return 0;
        }

        data[i] = I2C1_DR;
    }

    I2C1_CR1 |= (1 << 10);

    return 1;
}
