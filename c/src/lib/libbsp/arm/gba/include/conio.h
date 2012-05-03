/**
 *  @file conio.h
 *
 *  This file contains the GBA conio I/O package.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _CONIO_H
#define _CONIO_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Includes                                                                  *
 *---------------------------------------------------------------------------*/
#include <gba.h>

/*---------------------------------------------------------------------------*
 * Defines                                                                   *
 *---------------------------------------------------------------------------*/
#define ASCII_BEL       0x07            /**< bell             */
#define ASCII_BS        0x08            /**< backspace        */
#define ASCII_TAB       0x09            /**< horizontal tab   */
#define ASCII_LF        0x0A            /**< line feed        */
#define ASCII_CR        0x0D            /**< carriage return  */
#define ASCII_XON       0x11            /**< control-Q        */
#define ASCII_XOFF      0x13            /**< control-S        */
#define ASCII_ESC       0x1B            /**< escape           */


enum TEXT_MODES
{
    CO60 = 0,                           /**< 60x26 (3x5 font) */
    MAXMODES
};

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
void    gba_textmode(int _mode);
void    gba_clrscr(void);
void    gba_textattr(int _attr);
void    gba_textbackground(int _color);
void    gba_textcolor(int _color);
void    gba_putch(char _c);
void    gba_puts(const char *_str);
int     gba_printf(const char *_format, ...);
void    gba_gotoxy(int _x, int _y);
int     gba_getch(void);


#ifdef __cplusplus
}
#endif

#endif  /* _CONIO_H */
