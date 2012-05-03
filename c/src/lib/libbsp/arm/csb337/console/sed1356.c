/*
 *  SED1356 Support for KIT637_V6 (CSB637)
 *
 *  Based upon code from MicroMonitor 1.17 from http://www.umonfw.com/
 *  which includes this notice:
 *
 **************************************************************************
 *  General notice:
 *  This code is part of a boot-monitor package developed as a generic base
 *  platform for embedded system designs.  As such, it is likely to be
 *  distributed to various projects beyond the control of the original
 *  author.  Please notify the author of any enhancements made or bugs found
 *  so that all may benefit from the changes.  In addition, notification back
 *  to the author will allow the new user to pick up changes that may have
 *  been made by other users after this version of the code was distributed.
 *
 *  Note1: the majority of this code was edited with 4-space tabs.
 *  Note2: as more and more contributions are accepted, the term "author"
 *         is becoming a mis-representation of credit.
 *
 *  Original author:    Ed Sutter
 *  Email:              esutter@alcatel-lucent.com
 *  Phone:              908-582-2351
 **************************************************************************
 *
 *  Ed Sutter has been informed that this code is being used in RTEMS.
 *
 *  This code was reformatted by Joel Sherrill from OAR Corporation and
 *  Fernando Nicodemos <fgnicodemos@terra.com.br> from NCB - Sistemas
 *  Embarcados Ltda. (Brazil) to be more compliant with RTEMS coding
 *  standards and to eliminate C++ style comments.
 */

#include <bsp.h>

#include <stdlib.h>
#include <stdio.h>
#include "sed1356.h"
#include "font8x16.h"

int mode900lq;
long PIXELS_PER_ROW;
long PIXELS_PER_COL;
long COLS_PER_SCREEN;
long ROWS_PER_SCREEN;
long SED_HOR_PULSE_WIDTH_LCD;
long SED_VER_PULSE_START_LCD;
long SED_HOR_PULSE_START_LCD;
long SED_HOR_NONDISP_LCD;
long SED_VER_NONDISP_LCD;
long SED_VER_PULSE_WIDTH_LCD;

/* globals to keep track of foreground, background colors and x,y position */
int sed_color_depth;    /* 4, 8 or 16 */
int sed_fg_color;      /* 0 to 15, used as lookup into VGA color table */
int sed_bg_color;      /* 0 to 15, used as lookup into VGA color table */
int sed_col;        /* current column, 0 to COLS_PER_SCREEN - 1 */
int sed_row;        /* current row, 0 to (ROWS_PER_SCREEN * 2) - 1 */
int sed_disp_mode_crt;       /* CRT=1, LCD=0 */
int sed135x_ok;
int sed135x_tst;
uint32_t sed_fb_offset;       /* current offset into frame buffer for sed_putchar */

void sed_writechar(uint8_t c);
void sed_scroll(void);

#define SED_REG_BASE      0x30000000  /* *CS2 */
#define SED_MEM_BASE      (SED_REG_BASE + 0x00200000)
#define SED_STEP          1      /* 16-bit port on 16-bit boundry */

#define SED_REG16(_x_)       *(volatile uint16_t *)((uint32_t)SED_REG_BASE + (((uint32_t)_x_ * SED_STEP) ^ 0))  /* Control/Status Registers, 16-bit mode */
#define RD_FB16(_reg_,_val_)   ((_val_) = *((volatile uint16_t *)(((uint32_t)SED_MEM_BASE + ((uint32_t)(_reg_ * SED_STEP) ^ 0)))))
#define WR_FB16(_reg_,_val_)   (*((volatile uint16_t *)(((uint32_t)SED_MEM_BASE + ((uint32_t)(_reg_ * SED_STEP)  ^ 0)))) = (_val_))

#if 0
#define SED1356_REG_LCD_HOR_DISP                 SED_REG16(0x32)
#define SED1356_REG_LCD_HOR_NONDISP_and_START    SED_REG16(0x34)
#define SED1356_REG_LCD_HOR_PULSE               SED_REG16(0x36)
#define SED1356_REG_LCD_VER_DISP_HT_LO_and_HI      SED_REG16(0x38)
#define SED1356_REG_LCD_VER_NONDISP_and_START      SED_REG16(0x3a)
#define SED1356_REG_LCD_VER_PULSE                SED_REG16(0x3c)
#define SED1356_REG_LCD_DISP_MODE_and_MISC      SED_REG16(0x40)
#define SED1356_REG_LCD_DISP_START_LO_and_MID    SED_REG16(0x42)
#define SED1356_REG_LCD_DISP_START_HI           SED_REG16(0x44)
#define SED1356_REG_LCD_ADD_OFFSET_LO_and_HI    SED_REG16(0x46)
#define SED1356_REG_LCD_PIXEL_PAN             SED_REG16(0x48)
#define SED1356_REG_LCD_FIFO_THRESH_LO_and_HI    SED_REG16(0x4a)
#endif


#define H2SED(_x_)        (_x_)

#define FB_SIZE (640 * 480)
#define SED_ROW_SIZE(_depth_)        ((PIXELS_PER_ROW * _depth_) / 8)
#define SED_FB_SIZE(_depth_)         (((PIXELS_PER_COL * PIXELS_PER_ROW) * _depth_) / 8)

#define FONT_WIDTH    8
#define FONT_HEIGHT   16

#define SED_GET_ADD(_row_, _col_, _depth_) \
    (((((_row_ * PIXELS_PER_ROW) * FONT_HEIGHT) \
                                                                                          + (_col_ * FONT_WIDTH)) \
                                                                                          * _depth_) / 8)


#define SED_GET_PHYS_ADD(_reg_) \
  (volatile unsigned long)(SED_MEM_BASE + ((_reg_ * SED_STEP) ^ 0))


#include "sed1356_16bit.h"

/* #define SED_DBG */
int sed135x_tst = 0;

void sed_write_frame_buffer(
  uint32_t i,
  uint16_t wr16
)
{
  WR_FB16(i, wr16);
}

int sed_frame_buffer_size(void)
{
  return SED_FB_SIZE(sed_color_depth);
}

void sed_clr_row(int char_row)
{
  unsigned long sed_mem_add;
  int i;
  uint16_t wr16;

  /* clear the desired row */
  sed_mem_add = SED_GET_ADD(char_row, 0, sed_color_depth);

#ifdef SED_DBG
  sed135x_tst = 1;
  printf("SED Clear Row %d, FB Add 0x%08lx, CPU Add 0x%08lx.\n ", char_row, sed_mem_add, SED_GET_PHYS_ADD(sed_mem_add));
  sed135x_tst = 0;
#endif

  switch (sed_color_depth)
  {
    case 4:
      wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
#ifdef SED_DBG
  sed135x_tst = 1;
  printf("SED Clear Row %d, FB Add 0x%08lx to 0x%08lx.\n ", char_row, sed_mem_add, sed_mem_add + ((PIXELS_PER_ROW * FONT_HEIGHT) / 2));
  sed135x_tst = 0;
#endif
      for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) / 2); i += 2){
        WR_FB16(sed_mem_add, wr16);
        sed_mem_add += 2;
      } /* for font_row */
      break;
    case 8:
      wr16 = ((sed_bg_color << 8) | (sed_bg_color << 0));
      for (i = 0; i < (PIXELS_PER_ROW * FONT_HEIGHT); i += 2){
        WR_FB16(sed_mem_add, wr16);
        sed_mem_add += 2;
      } /* for font_row */
      break;
    case 16:
      wr16 = ((vga_lookup[sed_bg_color]));
      for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) * 2); i += 2){
        WR_FB16(sed_mem_add, wr16);
        sed_mem_add += 2;
      } /* for font_row */
      break;
  } /* switch sed_color_depth */
} /* sed_clr_row */

void sed_init(void)
{
  mode900lq = 0;
  PIXELS_PER_ROW = 640;
  PIXELS_PER_COL = 480;
  COLS_PER_SCREEN = 80;
  ROWS_PER_SCREEN = 30;
  SED_HOR_PULSE_WIDTH_LCD = 0x0b;
  SED_HOR_NONDISP_LCD = 0x13;
  SED_VER_PULSE_WIDTH_LCD = 0x01;
  SED_VER_PULSE_START_LCD = 0x09;
  SED_VER_NONDISP_LCD = 0x2c;

  sed_color_depth = 16;          /* 16 => vga lookup */
  sed_fg_color = 14;             /* Bright Yellow */
  sed_bg_color = 1;              /* Blue */
  sed_disp_mode_crt = 0;         /* default to LCD */
  sed_fb_offset = 0x00;
  sed_row = 0;
  sed_col = 0;

  sed135x_ok = 1;
  sed135x_tst = 0;
  sed_clearscreen();
}

/*
 * sed_putchar()
 *
 * This routine parses the character and calls sed_writechar if it is a
 * printable character
 */
void sed_putchar(char c)
{

  if ((sed135x_ok == 0) || (sed135x_tst == 1)) return;

  /* First parse the character to see if it printable or an
   * acceptable control character.
   */
  switch (c) {
    case '\r':
      sed_col = 0;
      return;
    case '\n':
      sed_col = 0;
      sed_scroll();
      return;
    case '\b':
      sed_col--;
      if (sed_col < 0) {
        sed_row--;
        if (sed_row < 0)
          sed_row = 0;
        sed_col = COLS_PER_SCREEN - 1;
      }
      c = 0;    /* erase the character */
      sed_writechar(c);
      break;
    default:
      if (((uint8_t)c < FIRST_CHAR) || ((uint8_t)c > LAST_CHAR))
        return; /* drop anything we can't print */
      c -= FIRST_CHAR;  /* get aligned to the first printable character */
      sed_writechar(c);
      /* advance to next column */
      sed_col++;
      if (sed_col == COLS_PER_SCREEN) {
        sed_col = 0;
        sed_scroll();
      }
      break;
  }

} /* sed_putchar() */

/*
 * sed_writechar()
 *
 * This routine writes the character to the screen at the current cursor
 * location.
 */
void sed_writechar(uint8_t c)
{
  uint32_t sed_mem_add;
  int font_row, font_col;
  uint8_t font_data8;
  uint16_t wr16;

  /* Convert the current row,col and color depth values
   * into an address
   */
  sed_mem_add = SED_GET_ADD(sed_row, sed_col, sed_color_depth);

#ifdef SED_DBG
  sed135x_tst = 1;
  printf("SED writechar at row %d, col %d, FB Add 0x%08lx, CPU Add 0x%08lx.\n ",    sed_row, sed_col, sed_mem_add, SED_GET_PHYS_ADD(sed_mem_add));
  sed135x_tst = 0;
#endif

  if (FONT_WIDTH == 8) {
    switch (sed_color_depth) {
      case 4:
        /* Now render the font by painting one font row at a time */
        for (font_row = 0; font_row < FONT_HEIGHT; font_row++) {
          /* get the font row of data */
          font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];


          for (font_col = 0; font_col < 8; font_col += 4)
          {
            /* get a words worth of pixels */
            wr16 = (((font_data8 & 0x80) ? sed_fg_color << 12 : sed_bg_color << 12)
                | ((font_data8 & 0x40) ? sed_fg_color << 8 : sed_bg_color << 8)
                | ((font_data8 & 0x20) ? sed_fg_color << 4 : sed_bg_color << 4)
                | ((font_data8 & 0x10) ? sed_fg_color << 0 : sed_bg_color << 0));
            font_data8 = font_data8 << 4;
            WR_FB16(sed_mem_add, wr16);
            /* if we are in the 2nd frame buffer, write to the 1st
             * frame buffer also
             */
            if (sed_row > (ROWS_PER_SCREEN - 1)) {
              WR_FB16((sed_mem_add - SED_FB_SIZE(sed_color_depth)), wr16);
            }
            sed_mem_add += 2;
          } /* for font_col */
          /* go to the next pixel row */
          sed_mem_add += (SED_ROW_SIZE(sed_color_depth) - ((FONT_WIDTH * sed_color_depth) / 8));
        } /* for font_row */
        break;

      case 8:
        /* Now render the font by painting one font row at a time */
        for (font_row = 0; font_row < FONT_HEIGHT; font_row++) {
          /* get the font row of data */
          font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
          for (font_col = 0; font_col < 8; font_col += 2)
          {
            /* get a words worth of pixels */
            wr16 = (((font_data8 & 0x80) ? sed_fg_color << 8 : sed_bg_color << 8)
                | ((font_data8 & 0x40) ? sed_fg_color << 0 : sed_bg_color << 0));
            font_data8 = font_data8 << 2;
            WR_FB16(sed_mem_add, wr16);
            /* if we are in the 2nd frame buffer, write to the 1st
             * frame buffer also
             */
            if (sed_row > (ROWS_PER_SCREEN - 1)) {
              WR_FB16((sed_mem_add - SED_FB_SIZE(sed_color_depth)), wr16);
            }
            sed_mem_add += 2;
          } /* for font_col */
          /* go to the next pixel row */
          sed_mem_add += (SED_ROW_SIZE(sed_color_depth) - ((FONT_WIDTH * sed_color_depth) / 8));
        } /* for font_row */
        break;

      case 16:
        /* Now render the font by painting one font row at a time */
        for (font_row = 0; font_row < FONT_HEIGHT; font_row++) {
          /* get the font row of data */
          font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
          for (font_col = 0; font_col < 8; font_col++)
          {
            /* get a words worth of pixels */
            wr16 = ((font_data8 & 0x80) ?
                     vga_lookup[sed_fg_color] : vga_lookup[sed_bg_color]);
            font_data8 = font_data8 << 1;
            WR_FB16(sed_mem_add, wr16);
            /* if we are in the 2nd frame buffer, write to the 1st
             * frame buffer also.
             */
            if (sed_row > (ROWS_PER_SCREEN - 1)) {
              WR_FB16((sed_mem_add - SED_FB_SIZE(sed_color_depth)), wr16);
            }
            sed_mem_add += 2;
          } /* for font_col */
          /* go to the next pixel row */
          sed_mem_add += (SED_ROW_SIZE(sed_color_depth) - ((FONT_WIDTH * sed_color_depth) / 8));
        } /* for font_row */
        break;

    } /* switch sed_color depth */
  } /* FONT_WIDTH == 8 */
  else
  {
    return;
  }
} /* sed_writechar() */

void sed_update_fb_offset(void)
{
  /* write the new sed_fb_offset value */
  if (sed_disp_mode_crt) {
  /* before we change the address offset, wait for the display to
   * go from active to non-active, unless the display is not enabled
   */
  if (SED1356_REG_DISP_MODE & H2SED(SED1356_DISP_MODE_CRT)) {  /* CRT is on */
    while ((SED1356_REG_CRT_VER_NONDISP_and_START & H2SED(SED1356_VER_NONDISP)) == 0) {}
    while ((SED1356_REG_CRT_VER_NONDISP_and_START & H2SED(SED1356_VER_NONDISP)) == 1) {}
    }
    SED1356_REG_CRT_DISP_START_LO_and_MID  = H2SED(((sed_fb_offset & 0x00ffff) >> 0));
    SED1356_REG_CRT_DISP_START_HI  = H2SED(((sed_fb_offset & 0x070000) >> 16));
  }
  else /* LCD */
  {
    if (SED1356_REG_DISP_MODE & H2SED(SED1356_DISP_MODE_LCD)) {  /* LCD is on */
      while ((SED1356_REG_LCD_VER_NONDISP_and_START & H2SED(SED1356_VER_NONDISP)) == 0) {}
      while ((SED1356_REG_LCD_VER_NONDISP_and_START & H2SED(SED1356_VER_NONDISP)) == 1) {}
    }
    SED1356_REG_LCD_DISP_START_LO_and_MID  = H2SED(((sed_fb_offset & 0x00ffff) >> 0));
    SED1356_REG_LCD_DISP_START_HI  = H2SED(((sed_fb_offset & 0x070000) >> 16));
  }
}

/* sed_scroll()
 *
 * Because we are most likely running out of FLASH and probably also with
 * cache disabled, a brute force memcpy of the whole screen would be very
 * slow, even with reduced color depths.  Instead, we define a frame buffer
 * that is twice the size of our actual display.  This does limit us to a
 * 1Mbyte active display size, but 640 x 480 @ 16-bits/pixel = 614K so it
 * works just fine.  800 x 600 can be had by reducing the color depth to
 * 8-bits/pixel and using the look up tables.
 *
 * With the double buffering, we always write to the first buffer, even
 * when the second buffer is active.  This allows us to scroll by adjusting
 * the starting and ending addresses in the SED135x by one row.  When we
 * reach the end of our virtual buffer, we reset the starting and ending
 * addresses to the first buffer.  Note that we can not adjust the SED135x
 * registers until it is in vertical retrace.  That means we have to wait
 * until it is in active display, then goes to non-display, unless the
 * screen is blanked, in which case we can update immediately.
 */
void sed_scroll(void)
{
  sed_row++;

  /* clear the new row(s) */
  sed_clr_row(sed_row);
  if (sed_row > (ROWS_PER_SCREEN - 1)) {
    sed_clr_row(sed_row - ROWS_PER_SCREEN);
  }
  /* when sed_y_pos is greater than ROWS_PER_SCREEN we just adjust the
   * start and end addresses in the SED135x.  If it is equal to 2 *
   * ROWS_PER_SCREEN, we reset the start and end addresses to SED_MEM_BASE.
   */
  if (sed_row > (ROWS_PER_SCREEN - 1)) {
    if (sed_row > ((ROWS_PER_SCREEN * 2) - 1)) {
      sed_fb_offset = 0x00;
      sed_row = 0;
      sed_clearscreen();
    } else {
      /* calculate the new offset address of the frame buffer in words */
      sed_fb_offset += (SED_GET_ADD(1, 0, sed_color_depth) / 2);
    }
    sed_update_fb_offset();
  } /* if (sed_row > (ROWS_PER_SCREEN - 1)) */
}

void sed_putstring(char *s)
{
  char *p = s;
  while (*p)
    sed_putchar(*p++);
}

void sed_clearscreen(void)
{
  int i;
  uint16_t wr16;
  int bg = sed_bg_color;
  int fbsize = sed_frame_buffer_size();

  /* we double buffer so clear ALL of memory */
  fbsize *= 2;

  /* fill the frame buffer with incrementing color values */
  switch (sed_color_depth){
    case 4:  wr16 = bg | bg << 4 | bg << 8 | bg << 12; break;
    case 8:  wr16 = bg | bg << 8; break;
    /* 16-bits bypasses the lookup table */
    default: wr16 = vga_lookup[bg]; break;
  }
  for (i = 0; i < fbsize; i += 2){
    sed_write_frame_buffer(i, wr16);
  }
}
