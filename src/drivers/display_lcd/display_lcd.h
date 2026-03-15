#ifndef __DISPLAY_LCD_H__
#define __DISPLAY_LCD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void display_lcd_init(void);
void display_lcd_update(uint8_t lcd[4][20]);

#ifdef __cplusplus
}
#endif

#endif // __DISPLAY_LCD_H__
