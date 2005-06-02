/*
 * conio.c :  ARM GBA BSP
 *
 * This file contains the GBA conio I/O package.
 *
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  This source file is based on work by Rafael Vuijk (aka Dark Fader)
 *  MyLib by Rafael Vuijk (aka Dark Fader)
 * 
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
*/


/*---------------------------------------------------------------------------*
 *   Includes                                                                *
 *---------------------------------------------------------------------------*/
#include <rtems/score/types.h>
#include <s3c2400.h>
#include <bsp.h>
#include <conio.h>
#include <stdio.h>
#include <stdarg.h>

#define gp_initButtons() {rPBCON=0x0;}
#define gp_getButtons() ((((~rPEDAT >> 6) & 0x3) << 8) | \
                         ((~rPBDAT >> 8) & 0xFF))

/*---------------------------------------------------------------------------*
 * Defines                                                                   *
 *---------------------------------------------------------------------------*/
#define DEFAULT_FONT_WIDTH  4
#define DEFAULT_FONT_HEIGHT 6
#define W                   (LCD_WIDTH/DEFAULT_FONT_WIDTH)
#define H                   (LCD_HEIGHT/DEFAULT_FONT_HEIGHT)

typedef unsigned char       Bitmap[LCD_HEIGHT][LCD_WIDTH];
#define GP32_CONIO_VRAM 0x0c7ed000
#define bg_bitmap          (*(Bitmap *)GP32_CONIO_VRAM)

/* color conversion */
#define RGB(r,g,b)              ( (r)<<11 | (g)<<6 | (b)<<1 )

int  _wherex;
int  _wherey;
int  _textattr;


/*---------------------------------------------------------------------------*
 *  Defaultfont                                                              *
 *---------------------------------------------------------------------------*/
#include "defaultfont.c"

/*---------------------------------------------------------------------------*
 *  gp32_gotoxy                                                                *
 *---------------------------------------------------------------------------*/
void gpconio_gotoxy(int _x, int _y)
{
    _wherex = _x;
    _wherey = _y;
}


/*---------------------------------------------------------------------------*
 *  PutChar                                                                   *
 *---------------------------------------------------------------------------*/
void PutChar(char c, int textattr, int x, int y)
{
    int        f = textattr & 0x0F;
    int        b = textattr >> 4;
    uint32_t fmask = f | (f << 8) | (f << 16) | (f << 24);
    uint32_t bmask = b | (b << 8) | (b << 16) | (b << 24);
    uint32_t *dest = (uint32_t *)&bg_bitmap[((y << 1) + y) << 1][x << 2];
    const uint32_t *src = (uint32_t *)&(font3x5[(int)c]);    
    uint32_t s;

    s = *src++; 
    *dest = (fmask&s) | (bmask&~s); 
    dest += LCD_WIDTH / sizeof(uint32_t);

    s = *src++; 
    *dest = (fmask&s) | (bmask&~s); 
    dest += LCD_WIDTH / sizeof(uint32_t);

    s = *src++;
    *dest = (fmask&s) | (bmask&~s);
    dest += LCD_WIDTH / sizeof(uint32_t);

    s = *src++;
    *dest = (fmask&s) | (bmask&~s);
    dest += LCD_WIDTH / sizeof(uint32_t);

    s = *src++;
    *dest = (fmask&s) | (bmask&~s);
    dest += LCD_WIDTH / sizeof(uint32_t);

    s = *src++;
    *dest = (fmask&s) | (bmask&~s);
    dest += LCD_WIDTH / sizeof(uint32_t);
}


/*---------------------------------------------------------------------------*
 *  gp32_textattr                                                             *
 *---------------------------------------------------------------------------*/
void gpconio_textattr(int _attr)
{
    _textattr = _attr;
}

/*---------------------------------------------------------------------------*
 *  gp32_textbackground                                                       *
 *---------------------------------------------------------------------------*/
void gpconio_textbackground(int _color)
{
    _textattr = (_textattr & 0x0F) | (_color << 4);
}

/*---------------------------------------------------------------------------*
 *  gp32_textcolor                                                            *
 *---------------------------------------------------------------------------*/
void gpconio_textcolor(int _color)
{
    _textattr = (_textattr & 0xF0) | (_color);
}


/*---------------------------------------------------------------------------*
 *  ClrLine                                                                  *
 *---------------------------------------------------------------------------*/
void ClrLine(int y)
{
    int x;

    for(x=0 ; x<=W ; x++)  PutChar(0, _textattr, x, y);
}

/*---------------------------------------------------------------------------*
 *  NextLine                                                                 *
 *---------------------------------------------------------------------------*/
void NextLine()
{
    _wherex = 0;

    if (++_wherey >= H) {
        _wherey = 0;
    }

    ClrLine(_wherey);
}

/*---------------------------------------------------------------------------*
 *  clrscr                                                                   *
 *---------------------------------------------------------------------------*/
void gpconio_clrscr()
{
    int y;

    for(y=0 ; y<=H ; y++) {
        ClrLine(y);
    }

    gpconio_gotoxy(0,0);
}

/*---------------------------------------------------------------------------*
 *  gp32_put                                                                  *
 *---------------------------------------------------------------------------*/
void gpconio_put(char _c)
{
  /* We have save some memory with fonts */
  _c = _c & 0x7F;   /* no extened chars */
  _c = _c - 0x20;   /* no cntr chars    */
  PutChar(_c, _textattr, _wherex, _wherey);
}

/*---------------------------------------------------------------------------*
 *  gp32_putch                                                                *
 *---------------------------------------------------------------------------*/
void gpconio_putch(char _c)
{
    switch (_c) {
    case ASCII_LF:
        NextLine();
        break;
    case ASCII_CR:
        gpconio_gotoxy(0, _wherey);
        break;
    default:
        gpconio_put(_c);
        if (++_wherex >= W) {
            NextLine();
        }
        break;
    }
    return;
}

/*---------------------------------------------------------------------------*
 *  gp32_puts                                                                 *
 *---------------------------------------------------------------------------*/
void gpconio_puts(const char *_str)
{
    while (*_str) {
        gpconio_putch(*_str++);
    }
    return;
}

/*---------------------------------------------------------------------------*
 *  gp32_printf                                                               *
 *---------------------------------------------------------------------------*/
int gpconio_printf(const char *_format, ...)
{
    char s[256];
    va_list marker;
    va_start(marker, _format);
    int r = vsprintf(s, _format, marker);

    va_end(marker);
    gpconio_puts(s);
    return r;
}

void ShowConIO()
{
    unsigned short BPPMODE = 11;
    unsigned short CLKVAL = (get_HCLK()/(83385*2*60))-1;
    uint32_t LCDBANK  =  GP32_CONIO_VRAM >> 22;
    uint32_t LCDBASEU = (GP32_CONIO_VRAM & 0x3FFFFF) >> 1;
    uint32_t LCDBASEL;
    unsigned short OFFSIZE = 0;
    unsigned short PAGEWIDTH;

    rLCDCON1 = (CLKVAL<<8) | (0<<7) | (3<<5) | (BPPMODE<<1) | (1<<0) ;
    rLCDCON2 = 0;
    rLCDCON2 = (1<<24) | (319<<14) | (2<<6) | (1<<0) ;
    rLCDCON3 = 0;
    rLCDCON3 = (6<<19) | (239<<8) | (2<<0) ;
    rLCDCON4 = 0;
    rLCDCON4 = (0<<24) | (0<<16) | (0<<8) | (4<<0) ;
    rLCDCON5 = 0;
    rLCDCON5 = ((1<<10) | (1<<9) | (1<<8) | (0<<7) | 
                (0<<6) | (0<<4) | (0<<2) | (1<<1) | (0<<0)) ;

    LCDBASEL  = LCDBASEU + 320*120;
    PAGEWIDTH = 120;

    rLCDSADDR1 = (LCDBANK<<21) | (LCDBASEU<<0) ;
    rLCDSADDR2 = (LCDBASEL<<0) ;
    rLCDSADDR3 = (OFFSIZE<<11) | (PAGEWIDTH<<0) ;
}


/*---------------------------------------------------------------------------*
 *  InitConIO                                                                *
 *---------------------------------------------------------------------------*/
void InitConIO()
{
    ShowConIO();
    
    gp_initButtons();
    gpconio_textattr(0);
    gpconio_textcolor(DEF_TEXTCOLOR);
    gpconio_textbackground(DEF_TEXTBACKGROUND);
    gpconio_clrscr();
}


static void delay_loop(unsigned int count)
{
    int i;

    for(i = 0; i < count; i++) {
        i = i;
    }

}

/*---------------------------------------------------------------------------*
 *  gp32_getch                                                                *
 *---------------------------------------------------------------------------*/
static unsigned char inputch = ASCII_CR;
char gpconio_getch(void)
{
    int  keyx;
    int key  = 0;

    while(1) {
        key = gp_getButtons();

        do {
            keyx = gp_getButtons();
        } while (keyx == key);

        switch (key) {
        case GP32_KEY_SELECT:
            gpconio_put(inputch);
            return inputch;
            break;
        
        case GP32_KEY_START:
            gpconio_put(' ');
            inputch = ASCII_CR;
            return inputch;
            break;

        case GP32_KEY_A:
            inputch = 'A';
            break;

        case GP32_KEY_B:
            inputch = 'Z';
            break;

        case GP32_KEY_UP:
            if ((inputch-1) >= 0x20) {
                inputch--;
            }
            break;

        case GP32_KEY_DOWN:
            if ((inputch+1) <=  0x7E) {
                inputch++;
            }
            break;

        case GP32_KEY_LEFT:
            if ((inputch - 0x20) >= 0x20) {
                inputch -= 0x20;
            }
            break;

        case GP32_KEY_RIGHT:
            if ((inputch + 0x20) <= 0x7E) {
                inputch += 0x20;
            }
            break;

        case GP32_KEY_R:
            inputch = '1';
            break;

        case GP32_KEY_L:
            inputch = '9';
            break;

        default:
            break;
        }

        gpconio_put(inputch);
        delay_loop(10000);
    }
}
