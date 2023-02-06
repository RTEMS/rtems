/**
 * @file flexbus_glcd.c
 *
 * @ingroup kinetis_flexbus
 *
 * @brief Use freescale kinetis flexbus to drive the GLCD9341.
 */

#include <bsp/glcd_9341.h>

#include <bsp/font.h>
#include <bsp/pinout.h>
#include <bsp/flexbus.h>
 
/************************** Orientation  configuration ************************/

#define HORIZONTAL  0                   /* If vertical = 0, if horizontal = 1 */

/*---------------------- Graphic LCD size definitions ------------------------*/

#if (HORIZONTAL == 1)
#define WIDTH       240                 /* Screen Width (in pixels)           */
#define HEIGHT      320                 /* Screen Hight (in pixels)           */
#else
#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#endif

#define GLCD_BACKCOLOR			White 

/******************************************************************************/
static unsigned short TextColor = Black;
static unsigned short BackColor = White;

static void glcd_draw_char_6x8(unsigned int x, unsigned int y, unsigned char bc, unsigned char c);
static void glcd_draw_char_8x12(unsigned int x, unsigned int y, unsigned char bc, unsigned char c);
static void glcd_draw_char_8x16(unsigned int x, unsigned int y, unsigned char bc, unsigned char c);
static void glcd_draw_char_16x24(unsigned int x, unsigned int y, unsigned char bc, unsigned char c);

static void delay(int count)
{
  while(count != 0) count--;
  return;
}

static inline void write_3spi_cmd(unsigned char cmd)
{
	HX9341_CS_L();
	HX9341_RS_L();
	*(volatile unsigned short *)CS0_BASE  = cmd;
	HX9341_CS_H();
}

static inline void write_3spi_data(unsigned char dat)
{
	unsigned short _dat = 0x100;
	_dat |= dat;
	HX9341_CS_L();
	HX9341_RS_H();
	*(volatile unsigned short *)(CS0_BASE+2)  = dat;
	HX9341_CS_H();
}

static inline void write_3spi_data16(unsigned short dat)
{	
	HX9341_CS_L();
	HX9341_RS_H();
	*(volatile unsigned short *)(CS0_BASE+2)  = dat;
	HX9341_CS_H();
}

static inline void color_data_start(void)
{
	write_3spi_cmd(0x2c);
}

static inline void set_address(unsigned short x,unsigned short y)
{
	write_3spi_cmd(0x20);			//AC Conuter Setting 
	write_3spi_data16(x);
	write_3spi_cmd(0x21);
	write_3spi_data16(y);
	write_3spi_cmd (0x22);	
}

void glcd_init(void) 
{ 
	delay(100000);
	
	write_3spi_cmd(0xCF);  //Power control B
	write_3spi_data(0x00); 
	write_3spi_data(0xC1); 
	write_3spi_data(0X30); 
	 
	write_3spi_cmd(0xED);  //Power on sequence control
	write_3spi_data(0x64); 
	write_3spi_data(0x03); 
	write_3spi_data(0X12); 
	write_3spi_data(0X81); 
	 
	write_3spi_cmd(0xE8);  //Driver timing control A
	write_3spi_data(0x85); 
	write_3spi_data(0x10); 
	write_3spi_data(0x7A); 
	 
	write_3spi_cmd(0xCB);  //Power control A
	write_3spi_data(0x39); 
	write_3spi_data(0x2C); 
	write_3spi_data(0x00); 
	write_3spi_data(0x34); 
	write_3spi_data(0x02); 
	 
	write_3spi_cmd(0xF7);  //Pump ratio control
	write_3spi_data(0x20); 
	 
	write_3spi_cmd(0xEA);  //Driver timing control B
	write_3spi_data(0x00); 
	write_3spi_data(0x00); 
	 
	write_3spi_cmd(0xC0);    //Power control 
	write_3spi_data(0x1B);   //VRH[5:0]  1B
	 
	write_3spi_cmd(0xC1);    //Power control 
	write_3spi_data(0x01);   //SAP[2:0];BT[3:0] 
	 
	write_3spi_cmd(0xC5);    //VCM control 
	write_3spi_data(0x45); 	   //3F
	write_3spi_data(0x25); 	   //3C
	 
	write_3spi_cmd(0xC7);    //VCM control2 
	write_3spi_data(0XB7); 		//b7
	 
	write_3spi_cmd(0x36);    // Memory Access Control 
	write_3spi_data(0x28); 
	 
	write_3spi_cmd(0x3A);  //Pixel Format Set
	write_3spi_data(0x55); 

	write_3spi_cmd(0xB1);   //Frame Rate Control
	write_3spi_data(0x00);   
	write_3spi_data(0x1A); 
	 
	write_3spi_cmd(0xB6);    // Display Function Control 
	write_3spi_data(0x0A); 
	write_3spi_data(0x82); 

	write_3spi_cmd(0xF2);    // 3Gamma Function Disable 
	write_3spi_data(0x00); 
	 
	write_3spi_cmd(0x26);    //Gamma curve selected 
	write_3spi_data(0x01); 
	 
	write_3spi_cmd(0xE0);    //Set Gamma 
	write_3spi_data(0x0F); 
	write_3spi_data(0x2A); 
	write_3spi_data(0x28); 
	write_3spi_data(0x08); 
	write_3spi_data(0x0E); 
	write_3spi_data(0x08); 
	write_3spi_data(0x54); 
	write_3spi_data(0XA9); 
	write_3spi_data(0x43); 
	write_3spi_data(0x0A); 
	write_3spi_data(0x0F); 
	write_3spi_data(0x00); 
	write_3spi_data(0x00); 
	write_3spi_data(0x00); 
	write_3spi_data(0x00); 
	 
	write_3spi_cmd(0XE1);    //Set Gamma 
	write_3spi_data(0x00); 
	write_3spi_data(0x15); 
	write_3spi_data(0x17); 
	write_3spi_data(0x07); 
	write_3spi_data(0x11); 
	write_3spi_data(0x06); 
	write_3spi_data(0x2B); 
	write_3spi_data(0x56); 
	write_3spi_data(0x3C); 
	write_3spi_data(0x05); 
	write_3spi_data(0x10); 
	write_3spi_data(0x0F); 
	write_3spi_data(0x3F); 
	write_3spi_data(0x3F); 
	write_3spi_data(0x0F); 

	write_3spi_cmd(0x2A);
	write_3spi_data(0x00);
	write_3spi_data(0x00);
	write_3spi_data(0x01);
	write_3spi_data(0x3F);	

	write_3spi_cmd(0x2B);
	write_3spi_data(0x00);
	write_3spi_data(0x00);
	write_3spi_data(0x00);
	write_3spi_data(0xEF);

	write_3spi_cmd(0x11); //Exit Sleep
	delay(120);
	write_3spi_cmd(0x29); //display on
	write_3spi_cmd(0x2C);
	
	glcd_clear(GLCD_BACKCOLOR);
}

void glcd_set_window(unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
{
	write_3spi_cmd(0x2A);
	write_3spi_data(x>>8);
	write_3spi_data(x&0xFF);
	write_3spi_data((x+w-1)>>8);
	write_3spi_data((x+w-1)&0xFF);	

	write_3spi_cmd(0x2B);
	write_3spi_data(y>>8);
	write_3spi_data(y&0xFF);
	write_3spi_data((y+h-1)>>8);
	write_3spi_data((y+h-1)&0xFF);
}

void glcd_window_max(void) 
{
#if (HORIZONTAL == 1)
	glcd_set_window (0, 0, HEIGHT, WIDTH);
#else
	glcd_set_window(0, 0, WIDTH,  HEIGHT);
#endif

}

void glcd_draw_pixel(unsigned short x, unsigned short y, unsigned short color) 
{
#if (HORIZONTAL == 1)
	write_3spi_cmd(0x2A);
	write_3spi_data(y>>8);
	write_3spi_data(y&0xFF);;	

	write_3spi_cmd(0x2B);
	write_3spi_data(x>>8);
	write_3spi_data(x&0xFF);
#else
	write_3spi_cmd(0x2A);
	write_3spi_data(x>>8);
	write_3spi_data(x&0xFF);

	write_3spi_cmd(0x2B);
	write_3spi_data(y>>8);
	write_3spi_data(y&0xFF);
#endif
	write_3spi_cmd(0x2c);
	write_3spi_data16(color);

}

void glcd_draw_rectangle(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color)
{
	unsigned short i;

	glcd_set_window (x, y, w, h);
	write_3spi_cmd(0x2c);
	
	for (i=0; i < (w*h); i++){
		write_3spi_data16(color);
	}
	glcd_window_max();

}

void glcd_set_text_color(unsigned short color) 
{
	TextColor = color;
}

unsigned short glcd_get_text_color(void) 
{
	return TextColor;
}

void glcd_set_background_color(unsigned short color) 
{
	BackColor = color;
}

unsigned short glcd_get_background_color(void) 
{
	return BackColor;
}

void glcd_clear(unsigned short color) 
{
	unsigned int i;
		
	write_3spi_cmd(0x2A);
	write_3spi_data(0);
	write_3spi_data(0);;	

	write_3spi_cmd(0x2B);
	write_3spi_data(0);
	write_3spi_data(0);

	write_3spi_cmd(0x2c);
	
	for(i = 0; i < (WIDTH*HEIGHT); i++)
		write_3spi_data16(color);
}

void glcd_draw_char_6x8(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F < c && c < 0x90) {
		idx = (c - 0x20)*8;
		for (i = 0;i < 8;i++) {
			for (j = 0; j < 6; j++) {
				if (Font_6x8_h[idx+i] & 0x01<<(7-j)) {
					glcd_draw_pixel (x+j, y+i, TextColor);
				}
				else {
					if (bc) {
						glcd_draw_pixel (x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void glcd_draw_char_8x12(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x90){
		idx = (c - 0x20)*12;
		for (i=0;i<12;i++){
			for (j=0;j<8;j++){
				if (Font_8x12_h[idx+i] & 0x01<<(7-j)){
					glcd_draw_pixel (x+j, y+i, TextColor);

				}
				else{
					if (bc) {
						glcd_draw_pixel (x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void glcd_draw_char_8x16(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x90){
		idx = (c - 0x20)*16;
		for (i=0;i<16;i++) {
			for (j=0;j<8;j++){
				if (Font_8x16_h[idx+i] & 0x01<<(7-j)){
					glcd_draw_pixel (x+j, y+i, TextColor);
				}
				else{
					if (bc) {
						glcd_draw_pixel (x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void glcd_draw_char_16x24(unsigned int x, unsigned int y, unsigned char bc, unsigned char c) 
{
	int idx = 0, i, j;
	if (0x1F<c && c<0x90) {
		idx = (c-0x20)*24;
		for (i = 0; i < 24; i++) {
			for (j = 0; j < 16; j++) {
				if (Font_16x24_h[idx + i] & (0x0001 << j)) {
					glcd_draw_pixel(x+j, y+i, TextColor);
				}
				else {
					if (bc) {
						glcd_draw_pixel(x+j, y+i, BackColor);
					}
				}
			}
		}
	}
}

void glcd_display_char(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, unsigned char c) 
{
	if (c & 0x80)									//is not ascii
		return;
	switch (fi) {
    case 0:  /* Font 6 x 8 */
		glcd_draw_char_6x8  (x, y, bc, c);
	break;
    case 1:  /* Font 8 x 12 */
		glcd_draw_char_8x12 (x, y, bc, c);
	break;
    case 2:  /* Font 8 x 16 */
		glcd_draw_char_8x16 (x, y, bc, c);
	break;
    case 3:  /* Font 16 x 24 */
		glcd_draw_char_16x24(x, y, bc, c);
	break;
	default:
	break;
	}
}

void glcd_display_string(unsigned int x, unsigned int y, unsigned char fi, unsigned char bc, unsigned char *s) 
{
	glcd_window_max();
	
	while (*s) {
		switch(fi) {
			case 0:
				glcd_display_char(x, y, fi, bc, *s++);
				x += 6;
				break;
			case 3:
				glcd_display_char(x, y, fi, bc, *s++);
				x += 16;
				break;
			default:
				glcd_display_char(x, y, fi, bc, *s++);
				x += 8;
				break;
		}
  }
}

void glcd_display_bitmap(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) 
{
	unsigned int i;
	unsigned short *bitmap_ptr = (unsigned short *)bitmap;
	glcd_set_window (x, y, w, h);
 	color_data_start();

	for(i = 0; i < (w*h); i++)
		write_3spi_data16(*bitmap_ptr++);
	
	glcd_window_max();
}
