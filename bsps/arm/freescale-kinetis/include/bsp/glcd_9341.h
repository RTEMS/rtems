#ifndef __GLCD_9341_H
#define __GLCD_9341_H

#define RS_MODE				0				//1:IO software RS 0:ADDR1 hardware RS

#if RS_MODE == 1
	#define HX9341_RS_H()		gpio_set_bit(PTC, IO_16)
	#define HX9341_RS_L()		gpio_clear_bit(PTC, IO_16)
#else
	#define HX9341_RS_H()
	#define HX9341_RS_L()
#endif

#define HX9341_CS_H()
#define HX9341_CS_L()

/* GLCD RGB color definitions                                                 */
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */

void glcd_init(void);
void glcd_set_window(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void glcd_window_max(void);
void glcd_draw_pixel(unsigned short x, unsigned short y, unsigned short color);
void glcd_set_text_color(unsigned short color);
unsigned short glcd_get_text_color (void);
void glcd_set_background_color(unsigned short color);
unsigned short glcd_get_background_color (void);
void glcd_draw_rectangle(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color);
void glcd_clear(unsigned short color);
void glcd_display_char(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, unsigned char  c);
void glcd_display_string(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, unsigned char *s);
void glcd_display_bitmap(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap);

#endif
