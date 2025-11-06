#include "DisplayManager.h"
static uint8_t cursor_col = 0;
static uint8_t cursor_row = 0;
static const struct i2c_dt_spec pcf8574 =
    I2C_DT_SPEC_GET(DT_NODELABEL(mydisplay));
LOG_MODULE_REGISTER(Display_Debug, 0);
void LCD_init() {

  if (!i2c_is_ready_dt(&pcf8574)) {
    LOG_ERR("LCD is not ready!");

    uint8_t data = 0xFF;
    i2c_write_dt(&pcf8574, &data, 1);
  }
  LOG_INF("LCD is ready");

  LCD_begin();
}
void LCD_begin() {

  LOG_INF("LCD begin");

  k_msleep(50); // Wait for LCD to power up
  LCD_SendByte(0x28, 0);
  k_msleep(5);
  // Send reset sequence
  LCD_SendNibble(0x30, 0);
  k_msleep(5);
  LCD_SendNibble(0x30, 0);
  k_msleep(1);
  LCD_SendNibble(0x30, 0);
  k_msleep(10);
  LCD_SendNibble(0x20, 0); // 4-bit mode

  // Function set: 4-bit, 2 line, 5x8 font
  LCD_SendByte(0x28, 0);
  // Display ON
  LCD_SendByte(0x0C, 0);
  // Clear display
  LCD_SendByte(0x01, 0);
  k_msleep(2);
  // Entry mode
  LCD_SendByte(0x06, 0);
}
void LCD_Clear() {
  LOG_INF("Clearing LCD");

  LCD_SendByte(0x01, 0); // Clear display command
  k_msleep(2);
}
void lcd_send(uint8_t value, uint8_t mode) {}
void hal_transmit(uint8_t data, uint8_t length) {}
void set_4_bit_mode() {
  uint8_t val = 0x03;
  lcd_send(val, sizeof(val));
}
void LCD_SendNibble(uint8_t nibble, uint8_t rs) {
  // uint8_t data = (nibble & 0xF0) | LCD_BACKLIGHT | rs;

  // // EN = 1
  // i2c_write_dt(&pcf8574, (uint8_t){data | LCD_ENABLE}, 1);
  // k_msleep(1);
  // // EN = 0
  // i2c_write_dt(&pcf8574, (uint8_t){data & ~LCD_ENABLE}, 1);

  // k_msleep(1);

  uint8_t data = (nibble & 0xF0) | LCD_BACKLIGHT | rs;

  uint8_t buf = data | LCD_ENABLE; // EN = 1
  i2c_write_dt(&pcf8574, &buf, 1);
  k_busy_wait(1);

  buf = data & ~LCD_ENABLE; // EN = 0
  i2c_write_dt(&pcf8574, &buf, 1);
  k_busy_wait(100);
}
void LCD_SendByte(uint8_t byte, uint8_t rs) {
  LCD_SendNibble(byte & 0xF0, rs);        // Send high nibble
  LCD_SendNibble((byte << 4) & 0xF0, rs); // Send low nibble
}
void LCD_SetCursor(uint8_t row, uint8_t col) {
  uint8_t address;

  switch (row) {
  case 0:
    address = 0x00 + col; // First line
    break;
  case 1:
    LOG_DBG("address = 0x40 + col; ");

    address = 0x40 + col; // Second line
    break;
  default:
    address = 0x00 + col; // Default to first line
    break;
  }

  LCD_SendByte(0x80 | address, 0); // 0x80 sets DDRAM address
}

void LCD_SendChar(char c) {
  if (c == '\n') {
    LOG_DBG("End reached");
    cursor_row = 0;
    cursor_col = 0;
    LCD_SetCursor(cursor_row, cursor_col);
    return;
  }
  // LOG_INF("The character is %c", c);
  // LOG_INF("The cursor row is %d", cursor_row);
  // LOG_INF("The cursor column is %d", cursor_col);

  if (cursor_col >= 16) {
    LOG_DBG("cursor column greater than 16 ");
    LOG_INF("The character is %c", c);

    cursor_col = 0;
    cursor_row++;
    if (cursor_row >= 2) {
      LOG_INF("wrap to first line");
      cursor_row = 0; // wrap to first line
    }
    LCD_SetCursor(cursor_row, cursor_col);
  }
  LCD_SendByte((uint8_t)c, LCD_RS);
  cursor_col++;
}
void LCD_SendString(const char *str) {
  LCD_Clear();
  // LCD_SetCursor(0, 0);

  while (*str) {
    LCD_SendChar(*str++);
  }
  cursor_row = 0;
  cursor_col = 0;
  LCD_SetCursor(0, 0);
}