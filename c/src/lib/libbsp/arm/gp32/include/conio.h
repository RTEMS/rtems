/*
 * conio.h :  ARM GP32 BSP
 *
 * This file contains the GP32 conio I/O package.
 *
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _CONIO_H
#define _CONIO_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Defines                                                                   *
 *---------------------------------------------------------------------------*/
#define ASCII_BEL       0x07            /* bell            */
#define ASCII_BS        0x08            /* backspace       */
#define ASCII_TAB       0x09            /* horizontal tab  */
#define ASCII_LF        0x0A            /* line feed       */
#define ASCII_CR        0x0D            /* carriage return */
#define ASCII_XON       0x11            /* control-Q       */
#define ASCII_XOFF      0x13            /* control-S       */
#define ASCII_ESC       0x1B            /* escape          */

enum COLORS
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE,
    MAXCOLORS
};

#define  DEF_TEXTCOLOR       BLACK
#define  DEF_TEXTBACKGROUND  WHITE

/*---------------------------------------------------------------------------*
 * Prototypes                                                                *
 *---------------------------------------------------------------------------*/
void    gpconio_clrscr();
void    gpconio_textattr(int _attr);
void    gpconio_textbackground(int _color);
void    gpconio_textcolor(int _color);
void    gpconio_putch(char _c);
void    gpconio_puts(const char *_str);
int     gpconio_printf(const char *_format, ...);
void    gpconio_gotoxy(int _x, int _y);

char    gpconio_getch(void);
char    gpconio_getch_noblock(void);

void	InitConIO();
void	ShowConIO();


#ifdef __cplusplus
}
#endif

#endif  // _CONIO_H
