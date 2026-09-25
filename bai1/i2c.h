#ifndef I2C_H
#define I2C_H

int I2C1_Init(void);
int I2C1_Write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t length);
int I2C1_Read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t length);

#endif
