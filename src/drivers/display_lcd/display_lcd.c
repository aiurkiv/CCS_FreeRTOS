#include "display_lcd.h"
#include "utils/utils.h"

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#define LCD_RS_PORT_BASE GPIO_PORTC_BASE
#define LCD_RS_PIN       GPIO_PIN_4
#define LCD_E_PORT_BASE  GPIO_PORTC_BASE
#define LCD_E_PIN        GPIO_PIN_5
#define LCD_D4_PORT_BASE GPIO_PORTC_BASE
#define LCD_D4_PIN       GPIO_PIN_6
#define LCD_D5_PORT_BASE GPIO_PORTE_BASE
#define LCD_D5_PIN       GPIO_PIN_5
#define LCD_D6_PORT_BASE GPIO_PORTD_BASE
#define LCD_D6_PIN       GPIO_PIN_3
#define LCD_D7_PORT_BASE GPIO_PORTC_BASE
#define LCD_D7_PIN       GPIO_PIN_7

#define LCD_COLS               20U
#define LCD_ROWS               4U
#define LCD_CMD_CLEAR_DISPLAY  0x01U
#define LCD_CMD_RETURN_HOME    0x02U

static void lcd_write_enable_pulse(void)
{
    MAP_GPIOPinWrite(LCD_E_PORT_BASE, LCD_E_PIN, LCD_E_PIN);
    delay_us(2U);
    MAP_GPIOPinWrite(LCD_E_PORT_BASE, LCD_E_PIN, 0U);
    delay_us(50U);
}

static void lcd_write_nibble(uint8_t nibble)
{
    MAP_GPIOPinWrite(LCD_D4_PORT_BASE, LCD_D4_PIN, (nibble & 0x01U) ? LCD_D4_PIN : 0U);
    MAP_GPIOPinWrite(LCD_D5_PORT_BASE, LCD_D5_PIN, (nibble & 0x02U) ? LCD_D5_PIN : 0U);
    MAP_GPIOPinWrite(LCD_D6_PORT_BASE, LCD_D6_PIN, (nibble & 0x04U) ? LCD_D6_PIN : 0U);
    MAP_GPIOPinWrite(LCD_D7_PORT_BASE, LCD_D7_PIN, (nibble & 0x08U) ? LCD_D7_PIN : 0U);
    lcd_write_enable_pulse();
    delay_us(2U);
}

static void lcd_write_byte(bool rs, uint8_t value)
{
    MAP_GPIOPinWrite(LCD_RS_PORT_BASE, LCD_RS_PIN, rs ? LCD_RS_PIN : 0U);
    lcd_write_nibble((uint8_t)((value >> 4) & 0x0FU));
    lcd_write_nibble((uint8_t)(value & 0x0FU));
}

static void lcd_write_command(uint8_t command)
{
    lcd_write_byte(false, command);

    if((command == LCD_CMD_CLEAR_DISPLAY) || (command == LCD_CMD_RETURN_HOME))
    {
        delay_ms(2U);
    }
}

static void lcd_write_data(uint8_t data)
{
    lcd_write_byte(true, data);
}

static void lcd_set_cursor(uint8_t row, uint8_t col)
{
    static const uint8_t row_address[LCD_ROWS] = { 0x00U, 0x40U, 0x14U, 0x54U };

    if((row >= LCD_ROWS) || (col >= LCD_COLS))
    {
        return;
    }

    lcd_write_command((uint8_t)(0x80U | (row_address[row] + col)));
}

void display_lcd_init()
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)) {}
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, LCD_RS_PIN | LCD_E_PIN | LCD_D4_PIN | LCD_D7_PIN);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, LCD_D5_PIN);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, LCD_D6_PIN);

    MAP_GPIOPinWrite(GPIO_PORTC_BASE, LCD_RS_PIN | LCD_E_PIN | LCD_D4_PIN | LCD_D7_PIN, 0U);
    MAP_GPIOPinWrite(GPIO_PORTE_BASE, LCD_D5_PIN, 0U);
    MAP_GPIOPinWrite(GPIO_PORTD_BASE, LCD_D6_PIN, 0U);

    delay_ms(2U);

    MAP_GPIOPinWrite(LCD_RS_PORT_BASE, LCD_RS_PIN, 0U);
    lcd_write_nibble(0x03U);
    delay_ms(5U);
    lcd_write_nibble(0x03U);
    delay_us(200U);
    lcd_write_nibble(0x03U);
    delay_us(200U);
    lcd_write_nibble(0x02U);
    delay_us(100U);

    lcd_write_command(0x28U);
    lcd_write_command(0x08U);
    lcd_write_command(LCD_CMD_CLEAR_DISPLAY);
    lcd_write_command(0x06U);
    lcd_write_command(0x0CU);
    delay_ms(2U);
}

void display_lcd_update(uint8_t lcd[4][20])
{
    uint8_t row;
    uint8_t col;

    if(lcd == 0)
    {
        return;
    }

    for(row = 0U; row < LCD_ROWS; row++)
    {
        lcd_set_cursor(row, 0U);

        for(col = 0U; col < LCD_COLS; col++)
        {
            lcd_write_data(lcd[row][col]);
        }
    }
}
