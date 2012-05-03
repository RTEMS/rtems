/**
 *  @file conio.c
 *
 *  This file contains the GBA conio I/O package.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *  based on MyLib by Rafael Vuijk (aka Dark Fader)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*****************************************************************************
 * This source file is based on work by Rafael Vuijk (aka Dark Fader)
 *****************************************************************************
 *****************************************************************************
 * MyLib by Rafael Vuijk (aka Dark Fader)
 * :
 * This file is released into the public domain for commercial
 * or non-commercial usage with no restrictions placed upon it.
 *****************************************************************************/

/*---------------------------------------------------------------------------*
 *   Includes                                                                *
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <rtems/score/types.h>
#include <gba.h>
#include <conio.h>
#include <stdio.h>
#include <stdarg.h>

/*---------------------------------------------------------------------------*
 * Defines                                                                   *
 *---------------------------------------------------------------------------*/
#define W                   60                        /**< Screen width      */
#define H                   26                        /**< Screen height     */
#define CLRSCR_SIZE         (W*4 * H*6)               /**< Screen size       */
#define DEFAULT_FONT_WIDTH  4                         /**< Font width        */
#define DEFAULT_FONT_HEIGHT 6                         /**< Font height       */

typedef unsigned short      Palette[256];                              /**< 256 colors */
typedef unsigned short      Palettes[16][16];                          /**< 16 palettes with each 16 colors */
typedef unsigned short      Bitmap3[GBA_LCD_HEIGHT][GBA_LCD_WIDTH];    /**< 16 bits, single buffered */
typedef unsigned char       Bitmap4[GBA_LCD_HEIGHT][GBA_LCD_WIDTH];    /**< 8 bits, double buffered  */
typedef unsigned short      Bitmap5[GBA_MODE5_HEIGHT][GBA_MODE5_WIDTH];/**< double buffered          */

#define VRAM                GBA_VRAM_ADDR             /**< VRAM address      */
#define VRAM_END            (VRAM +    0x18000)       /**< VRAM end          */
#define BG_BITMAP0_VRAM     (VRAM +        0x0)       /**< BG Bitmap 0 RAM   */
#define BG_BITMAP1_VRAM     (VRAM +     0xa000)       /**< BG Bitmap 1 RAM   */

#define bg_bitmap0          (*(Bitmap4 *)BG_BITMAP0_VRAM)
#define bg_bitmap1          (*(Bitmap4 *)BG_BITMAP1_VRAM)
#define bg_bitmap3          (*(Bitmap3 *)BG_BITMAP0_VRAM)
#define bg_bitmap4a         (*(Bitmap4 *)BG_BITMAP0_VRAM)
#define bg_bitmap4b         (*(Bitmap4 *)BG_BITMAP1_VRAM)
#define bg_bitmap5a         (*(Bitmap5 *)BG_BITMAP0_VRAM)
#define bg_bitmap5b         (*(Bitmap5 *)BG_BITMAP1_VRAM)

/** Color conversion macro  */
#define RGB(r,g,b)          ( (r)<<0 | (g)<<5 | (b)<<10 )

/** BG Affine Transformation Destination Data Structure */
typedef struct {
    int16_t H_DiffX;        /**< Line Direction X Coordinate Difference     */
    int16_t V_DiffX;        /**< Vertical Direction X Coordinate Difference */
    int16_t H_DiffY;        /**< Line Direction Y Coordinate Difference     */
    int16_t V_DiffY;        /**< Vertical Direction Y Coordinate Difference */
    int32_t StartX;         /**< Start X Coordinate                         */
    int32_t StartY;         /**< Start Y Coordinate                         */
} BgAffineDestData;

typedef volatile BgAffineDestData   vBgAffineDestData;
#define rBg2Affine          (*(vBgAffineDestData *)0x4000020)

/** 256 colors for background(s) */
#define bg_palette          (*(Palette *)(GBA_PAL_RAM_ADDR))


int  _wherex;                /**< Screen X coordinate */
int  _wherey;                /**< Screen Y coordinate */
int  _textattr;              /**< Text attribute      */


/*---------------------------------------------------------------------------*
 *  Defaultfont                                                              *
 *---------------------------------------------------------------------------*/
#include "defaultfont.h"

/**
 *  @brief gba_gotoxy function set screeen xy-coordinates
 *
 *  @param  _x screen x coordinate
 *  @param  _y screen y coordinate
 *  @return None
 */
void gba_gotoxy(int _x, int _y)
{
    _wherex = _x;
    _wherey = _y;
}


/**
 *  @brief gba_putchar function writes char-data to screen memory.
 *
 *  Char code is index to font table.
 *
 *  Input parameters:   char, attribute and cordinates
 *  @param  c character code
 *  @param  textattr text attribute
 *  @param  x screen x coordinate
 *  @param  y screen y coordinate
 *  @return None
 */
void gba_putchar(char c, int textattr, int x, int y)
{
    int       f = textattr & 0x0F;
    int       b = textattr >> 4;
    uint32_t  fmask = f | f<<8 | f<<16 | f<<24;
    uint32_t  bmask = b | b<<8 | b<<16 | b<<24;
    uint32_t  *dest = (uint32_t *)&bg_bitmap4a[((y<<1) + y) << 1][x<<2];
    const uint32_t  *src = (uint32_t *)&(font3x5[(int)c]);
    uint32_t s;
    s = *src++; *dest = (fmask&s) | (bmask&~s); dest += GBA_LCD_WIDTH/sizeof(uint32_t);
    s = *src++; *dest = (fmask&s) | (bmask&~s); dest += GBA_LCD_WIDTH/sizeof(uint32_t);
    s = *src++; *dest = (fmask&s) | (bmask&~s); dest += GBA_LCD_WIDTH/sizeof(uint32_t);
    s = *src++; *dest = (fmask&s) | (bmask&~s); dest += GBA_LCD_WIDTH/sizeof(uint32_t);
    s = *src++; *dest = (fmask&s) | (bmask&~s); dest += GBA_LCD_WIDTH/sizeof(uint32_t);
    s = *src++; *dest = (fmask&s) | (bmask&~s); dest += GBA_LCD_WIDTH/sizeof(uint32_t);
}


/**
 *  @brief gba_textattr function set textattribute
 *
 *  @param  _attr text attribute
 *  @return None
 */
void gba_textattr(int _attr)
{
    _textattr = _attr;
}

/**
 *  @brief gba_textbackground function set text background color
 *
 *  @param  _color backround color
 *  @return None

 */
void gba_textbackground(int _color)
{
    _textattr = (_textattr & 0x0F) | (_color << 4);
}

/**
 *  @brief gba_textcolor function set text color
 *
 *  @param  _colour text color
 *  @return None
 */
void gba_textcolor(int _color)
{
    _textattr = (_textattr & 0xF0) | (_color);
}


/**
 *  @brief gba_clearline function clear line nro y
 *
 *  Line is filled with spaces
 *
 *  @param  y line number
 *  @return None
 */
void gba_clearline(int y)
{
    int x;

    for (x=0 ; x<=W ; x++) {
       gba_putchar(0, _textattr, x, y);
    }
}

/**
 *  @brief gba_nextline function moves cursor to next line and clears it
 *
 *  @param  None
 *  @return None
 */
void gba_nextline(void)
{
    _wherex = 0;
    if (++_wherey >= H) {
       _wherey = 0;
    }
    gba_clearline(_wherey);
}

/**
 *  @brief gba_clrscr function clear screen
 *
 *  @param  None
 *  @return None
 */
void gba_clrscr(void)
{
    int y;

    for (y=0 ; y<=H ; y++) {
       gba_clearline(y);
    }
    gba_gotoxy(0,0);
}

/**
 *  @brief gba_put function convert ascii char to font index and
 *  write char to screen memory
 *
 *  @param  _c character code
 *  @return None
 */
void gba_put(char _c)
{
  /* We have save some memory with reduced fonts */
  _c = _c & 0x7F;   /* no extened chars */
  _c = _c - 0x20;   /* no cntr chars    */
  gba_putchar(_c, _textattr, _wherex, _wherey);
}


/**
 *  @brief gba_putch function write ascii chars to screen
 *
 *  @param  _c character code
 *  @return None
 */
void gba_putch(char _c)
{
    switch (_c) {
        case ASCII_LF:
            gba_nextline();
            break;
        case ASCII_CR:
            gba_gotoxy(0, _wherey);
            break;
        default:
            gba_put(_c);
            if (++_wherex >= W)
            {
               gba_nextline();
            }
            break;
    }
    return;
}

/**
 *  @brief gba_puts function write ascii string to screen
 *
 *  @param  _str ASCII string
 *  @return None
 */
void gba_puts(const char *_str)
{
    while (*_str) {
       gba_putch(*_str++);
    }
    return;
}

/**
 *  @brief gba_printf function do formated printf
 *
 *  @param  _format printf format string
 *  @param  ... parameters specified in format string
 *  @return None
 */
int gba_printf(const char *_format, ...)
{
    char s[256];
    va_list marker;
    va_start(marker, _format);
    int r = vsprintf(s, _format, marker);
    va_end(marker);
    gba_puts(s);
    return r;
}

/**
 *  @brief gba_initconio function initialize console
 *
 *  @param  None
 *  @return None
 */
void gba_initconio(void)
{
    GBA_REG_DISPCNT = GBA_DISP_MODE_4 | GBA_DISP_BG2_ON;/*  256 color bitmapped mode */
    const BgAffineDestData bgAffineReset = {256,0,0,256,0,-256*2};
    rBg2Affine = bgAffineReset;
    bg_palette[BLACK       ] =  RGB( 0, 0, 0);   /*  BLACK */
    bg_palette[BLUE        ] =  RGB( 0, 0,16);   /*  BLUE */
    bg_palette[GREEN       ] =  RGB( 0,16, 0);   /*  GREEN */
    bg_palette[CYAN        ] =  RGB( 0,16,16);   /*  CYAN */
    bg_palette[RED         ] =  RGB(16, 0, 0);   /*  RED */
    bg_palette[MAGENTA     ] =  RGB(16, 0,16);   /*  MAGENTA */
    bg_palette[BROWN       ] =  RGB(16,16, 0);   /*  BROWN */
    bg_palette[LIGHTGRAY   ] =  RGB(24,24,24);   /*  LIGHTGRAY */
    bg_palette[DARKGRAY    ] =  RGB(16,16,16);   /*  DARKGRAY */
    bg_palette[LIGHTBLUE   ] =  RGB( 0, 0,31);   /*  LIGHTBLUE */
    bg_palette[LIGHTGREEN  ] =  RGB( 0,31, 0);   /*  LIGHTGREEN */
    bg_palette[LIGHTCYAN   ] =  RGB( 0,31,31);   /*  LIGHTCYAN */
    bg_palette[LIGHTRED    ] =  RGB(31, 0, 0);   /*  LIGHTRED */
    bg_palette[LIGHTMAGENTA] =  RGB(31, 0,31);   /*  LIGHTMAGENTA */
    bg_palette[YELLOW      ] =  RGB(31,31, 0);   /*  YELLOW */
    bg_palette[WHITE       ] =  RGB(31,31,31);   /*  WHITE */
    gba_textattr(0);
    gba_textcolor(DEF_TEXTCOLOR);
    gba_textbackground(DEF_TEXTBACKGROUND);
    gba_clrscr();
}

/**
 *  @brief gba_textmode function set console mode
 *
 *  @param  _mode console mode code
 *  @return None
 */
void gba_textmode(int _mode)
{
    switch (_mode) {
        case CO60:
        {
            gba_initconio();
            break;
        }
    }
}


/**
 *  @brief delay_loop function is simple delay loop
 *
 *  @param  count loop counter
 *  @return None
 */
void delay_loop(unsigned int count)
{
    int i;
    for (i = 0; i<count; i++) i = i;
}

static unsigned char inputch = ASCII_CR;    /**< input character value */
/**
 *  @brief gba_getch function read char from game pad keys
 *
 *  Character input is done with GBA buttons, up-down-left-right/A/B/R/L/Select/Start
 *  - Select-key accept selected character
 *  - Start-key read CR (Enter)
 *  - A-key select 'A' character
 *  - B-key select 'Z' character
 *  - R-key select '1' character
 *  - L-key select '9' character
 *  - up-key increment character ('A'->'B')
 *  - down-key decrement character ('B'-'A')
 *  - left-key change set of character ('!'->'A'->'a')
 *  - right-key change set of character ('a'->'A'->'!')
 *
 *  @param  None
 *  @return Selected char code
 */
int gba_getch(void)
{
  int  keyx, key  = 0;

  while(1) {
      key = GBA_KEY();
      while ( (keyx=GBA_KEY())==key );
      switch (key)
      {
        case GBA_KEY_SELECT:
            gba_put(inputch);
            return inputch;
            break;
        case GBA_KEY_START:
            gba_put(' ');
            inputch = ASCII_CR;
            return inputch;
            break;
        case GBA_KEY_A:
            inputch = 'A';
            break;
        case GBA_KEY_B:
            inputch = 'Z';
            break;
        case GBA_KEY_UP:
            if ((inputch-1) >= 0x20) inputch--;
            break;
        case GBA_KEY_DOWN:
            if ((inputch+1) <=  0x7E) inputch++;
            break;
        case GBA_KEY_LEFT:
            if ((inputch - 0x20) >= 0x20) inputch -= 0x20;
            break;
        case GBA_KEY_RIGHT:
            if ((inputch + 0x20) <= 0x7E) inputch += 0x20;
            break;
        case GBA_KEY_R:
            inputch = '1';
            break;
        case GBA_KEY_L:
            inputch = '9';
            break;
        default:
            break;
      }
      gba_put(inputch);
      delay_loop(1000);
  }
}
