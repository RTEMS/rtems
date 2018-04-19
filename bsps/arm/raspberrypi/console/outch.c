/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief displaying characters on the console
 */

/**
 *
 * Copyright (c) 2015 Yang Qiao
 * based on work by:
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 * Till Straumann <strauman@slac.stanford.edu>, 2003/9:
 *  - added handling of basic escape sequences (cursor movement
 *    and erasing; just enough for the line editor 'libtecla' to
 *    work...)
 *
 */

#include <bsp.h>
#include <bsp/vc.h>
#include <bsp/rpi-fb.h>
#include <rtems/fb.h>

#include <stdlib.h>
#include <string.h>
#include "font_data.h"

static void wr_cursor(
  int           r,
  int           c
)
{
  /* dummy function for now */
}

#define TAB_SPACE 4
#define CONSOLE_BG_COL 0x00
#define CONSOLE_FG_COL 0xa0

static void          *fb_mem = NULL;
static unsigned short maxCol;
static unsigned short maxRow;
static unsigned short bytes_per_pixel;
static unsigned int   bytes_per_line;
static unsigned int   bytes_per_char_line;
static unsigned char  row;
static unsigned char  column;
static unsigned int   nLines;
static uint32_t       fgx, bgx, eorx;
static int            rpi_video_initialized;

static const int video_font_draw_table32[ 16 ][ 4 ] = {
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  { 0x00000000, 0x00000000, 0x00000000, 0x00ffffff },
  { 0x00000000, 0x00000000, 0x00ffffff, 0x00000000 },
  { 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff },
  { 0x00000000, 0x00ffffff, 0x00000000, 0x00000000 },
  { 0x00000000, 0x00ffffff, 0x00000000, 0x00ffffff },
  { 0x00000000, 0x00ffffff, 0x00ffffff, 0x00000000 },
  { 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff },
  { 0x00ffffff, 0x00000000, 0x00000000, 0x00000000 },
  { 0x00ffffff, 0x00000000, 0x00000000, 0x00ffffff },
  { 0x00ffffff, 0x00000000, 0x00ffffff, 0x00000000 },
  { 0x00ffffff, 0x00000000, 0x00ffffff, 0x00ffffff },
  { 0x00ffffff, 0x00ffffff, 0x00000000, 0x00000000 },
  { 0x00ffffff, 0x00ffffff, 0x00000000, 0x00ffffff },
  { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00000000 },
  { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff }
};

static void scroll( void )
{
  int      i, j;      /* Counters */
  uint8_t *pt_scroll, *pt_bitmap;  /* Pointers on the bit-map  */

  pt_bitmap = fb_mem;
  j = 0;
  pt_bitmap = pt_bitmap + j;
  pt_scroll = pt_bitmap + bytes_per_char_line;

  for ( i = j; i < maxRow - 1; i++ ) {
    memcpy( pt_bitmap, pt_scroll, bytes_per_char_line );
    pt_bitmap = pt_bitmap + bytes_per_char_line;
    pt_scroll = pt_bitmap + bytes_per_char_line;
  }

  /*
   * Blank characters are displayed on the last line.
   */
  memset( pt_bitmap, 0, bytes_per_char_line );
}

static void doCRNL(
  int cr,
  int nl
)
{
  if ( nl ) {
    if ( ++row == maxRow ) {
      scroll();   /* Scroll the screen now */
      row = maxRow - 1;
    }

    nLines++;
  }

  if ( cr )
    column = 0;

  /* Move cursor on the next location  */
  if ( cr || nl ) {
    wr_cursor( row, column );
  }
}

static void advanceCursor( void )
{
  if ( ++column == maxCol )
    doCRNL( 1, 1 );
  else
    wr_cursor( row, column );
}

static void gotorc(
  int r,
  int c
)
{
  column = c;
  row = r;
  wr_cursor( row, column );
}

static void video_drawchars(
  int           r,
  int           c,
  unsigned char ch
)
{
  if ( fb_mem == NULL ) {
    return;
  }

  uint8_t *cdat, *dest, *dest0;
  int      rows, offset;

  offset = r * bytes_per_char_line + c * bytes_per_pixel * RPI_FONT_WIDTH;
  dest0 = fb_mem + offset;

  /*
   * only 32-bit per pixel format is supported for now
   */
  cdat = rpi_font + ch * RPI_FONT_HEIGHT;

  for ( rows = RPI_FONT_HEIGHT, dest = dest0;
        rows--; dest += bytes_per_line ) {
    uint8_t bits = *cdat++;

    ( (uint32_t *) dest )[ 0 ] =
      ( video_font_draw_table32
        [ bits >> 4 ][ 0 ] & eorx ) ^ bgx;
    ( (uint32_t *) dest )[ 1 ] =
      ( video_font_draw_table32
        [ bits >> 4 ][ 1 ] & eorx ) ^ bgx;
    ( (uint32_t *) dest )[ 2 ] =
      ( video_font_draw_table32
        [ bits >> 4 ][ 2 ] & eorx ) ^ bgx;
    ( (uint32_t *) dest )[ 3 ] =
      ( video_font_draw_table32
        [ bits >> 4 ][ 3 ] & eorx ) ^ bgx;

    ( (uint32_t *) dest )[ 4 ] =
      ( video_font_draw_table32
        [ bits & 15 ][ 0 ] & eorx ) ^ bgx;
    ( (uint32_t *) dest )[ 5 ] =
      ( video_font_draw_table32
        [ bits & 15 ][ 1 ] & eorx ) ^ bgx;
    ( (uint32_t *) dest )[ 6 ] =
      ( video_font_draw_table32
        [ bits & 15 ][ 2 ] & eorx ) ^ bgx;
    ( (uint32_t *) dest )[ 7 ] =
      ( video_font_draw_table32
        [ bits & 15 ][ 3 ] & eorx ) ^ bgx;
  }
}

#define ESC ( (char) 27 )
/* erase current location without moving the cursor */
#define BLANK ( (char) 0x7f )

static void videoPutChar( char ch )
{
  switch ( ch ) {
    case '\b': {
      if ( column )
        column--;

      /* Move cursor on the previous location  */
      wr_cursor( row, column );
      return;
    }
    case '\t': {
      int i;

      i = TAB_SPACE - ( column & ( TAB_SPACE - 1 ) );

      while ( i-- ) {

        video_drawchars( row, column, ' ' );
        column += 1;

        if ( column >= maxCol ) {
          doCRNL( 1, 1 );
          return;
        }
      }

      wr_cursor( row, column );

      return;
    }
    case '\n': {
      doCRNL( 0, 1 );
      return;
    }
    case 7:   {     /* Bell code must be inserted here */
      return;
    }
    case '\r': {
      doCRNL( 1, 0 );
      return;
    }
    case BLANK: {
      video_drawchars( row, column, ' ' );

      wr_cursor( row, column );

      return;
    }
    default: {
      // *pt_bitmap = (unsigned char)ch | attribute;
      video_drawchars( row, column, ch );
      advanceCursor();
      return;
    }
  }
}

/* trivial state machine to handle escape sequences:
 *
 *                    ---------------------------------
 *                   |                                 |
 *                   |                                 |
 * KEY:        esc   V    [          DCABHKJ       esc |
 * STATE:   0 -----> 27 -----> '[' ----------> -1 -----
 *          ^\        \          \               \
 * KEY:     | \other   \ other    \ other         \ other
 *           <-------------------------------------
 *
 * in state '-1', the DCABHKJ cases are handled
 *
 * (cursor motion and screen clearing)
 */

#define DONE ( -1 )

static int handleEscape(
  int  oldState,
  char ch
)
{
  int rval = 0;
  int ro, co;

  switch ( oldState ) {
    case DONE:  /*  means the previous char terminated an ESC sequence... */
    case 0:

      if ( 27 == ch ) {
        rval = 27;   /* START of an ESC sequence */
      }

      break;

    case 27:

      if ( '[' == ch ) {
        rval = ch;  /* received ESC '[', so far */
      } else {
        /* dump suppressed 'ESC'; outch will append the char */
        videoPutChar( ESC );
      }

      break;

    case '[':
      /* handle 'ESC' '[' sequences here */
      ro = row;
      co = column;
      rval = DONE; /* done */

      switch ( ch ) {
        case 'D': /* left */

          if ( co > 0 )
            co--;

          break;
        case 'C': /* right */

          if ( co < maxCol )
            co++;

          break;
        case 'A': /* up    */

          if ( ro > 0 )
            ro--;

          break;
        case 'B': /* down */

          if ( ro < maxRow )
            ro++;

          break;
        case 'H': /* home */
          ro = co = 0;
          break;
        case 'K': /* clear to end of line */

          while ( column < maxCol - 1 )
            videoPutChar( ' ' );

          videoPutChar( BLANK );
          break;
        case 'J':     /* clear to end of screen */

          while ( ( ( row < maxRow - 1 ) || ( column < maxCol - 1 ) ) )
            videoPutChar( ' ' );

          videoPutChar( BLANK );
          break;
        default:
          videoPutChar( ESC );
          videoPutChar( '[' );
          /* DONT move the cursor */
          ro = -1;
          rval = 0;
          break;
      }

      // /* reset cursor */
      if ( ro >= 0 )
        gotorc( ro, co );

    default:
      break;
  }

  return rval;
}

static void clear_screen( void )
{
  int i, j;

  for ( j = 0; j < maxRow; j++ ) {
    for ( i = 0; i < maxCol; i++ ) {
      videoPutChar( ' ' );
    }
  }

  column = 0;
  row = 0;
}

void rpi_fb_outch( char c )
{
  static int escaped = 0;

  if ( !( escaped = handleEscape( escaped, c ) ) ) {
    if ( '\n' == c )
      videoPutChar( '\r' );

    videoPutChar( c );
  }
}

void rpi_video_init( void )
{
  int ret = rpi_fb_init();

  if ( ( ret != RPI_FB_INIT_OK ) &&
       ( ret != RPI_FB_INIT_ALREADY_INITIALIZED ) ) {
    rpi_video_initialized = 0;
    return;
  }

  struct fb_var_screeninfo fb_var_info;
  struct fb_fix_screeninfo fb_fix_info;
  rpi_get_var_screen_info( &fb_var_info );
  rpi_get_fix_screen_info( &fb_fix_info );
  maxCol = fb_var_info.xres / RPI_FONT_WIDTH;
  maxRow = fb_var_info.yres / RPI_FONT_HEIGHT;
  bytes_per_pixel = fb_var_info.bits_per_pixel / 8;
  bytes_per_line = bytes_per_pixel * fb_var_info.xres;
  bytes_per_char_line = RPI_FONT_HEIGHT * bytes_per_line;
  fb_mem = RTEMS_DEVOLATILE( void *, fb_fix_info.smem_start );
  column = 0;
  row = 0;
  nLines = 0;
  fgx = ( CONSOLE_FG_COL << 24 ) |
        ( CONSOLE_FG_COL << 16 ) |
        ( CONSOLE_FG_COL << 8 ) |
        CONSOLE_FG_COL;
  bgx = ( CONSOLE_BG_COL << 24 ) |
        ( CONSOLE_BG_COL << 16 ) |
        ( CONSOLE_BG_COL << 8 ) |
        CONSOLE_BG_COL;
  eorx = fgx ^ bgx;
  clear_screen();
  rpi_video_initialized = 1;
}

int rpi_video_is_initialized( void )
{
  return rpi_video_initialized;
}

/* for old DOS compatibility n-curses type of applications */
void gotoxy(
  int x,
  int y
);
int whereX( void );
int whereY( void );

void gotoxy(
  int x,
  int y
)
{
  gotorc( y, x );
}

int whereX( void )
{
  return row;
}

int whereY( void )
{
  return column;
}
