#ifndef DisplayManager_H
#define DisplayManager_H
#include "common.h"

#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

void LCD_init();
void LCD_begin();
void LCD_Clear();
void send(uint8_t value, uint8_t mode);
void hal_transmit(uint8_t data, uint8_t length);
void set_4_bit_mode();
void LCD_SendNibble(uint8_t nibble, uint8_t rs);
void LCD_SendByte(uint8_t byte, uint8_t rs);
void LCD_SendChar(char c);
void LCD_SendString(const char *str);
#endif