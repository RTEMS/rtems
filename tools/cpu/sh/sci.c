/*
 * Copyright (c) 1998 Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 * See the file COPYING for copyright notice.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sci.h"

/*
   n .. baudrate generator source 0,1,2,3

   N .. BRR setting (0..255)

   Phi .. processor baud rate

   B .. bitrate
 */

typedef struct sci_tab {
  unsigned int  B ;
  unsigned int  n ;
  int 		N ;
  double        err ;
  } sci_tab_t ;

static unsigned int bitrate [] = {
  50,
  75,
  110,
  134,
  150,
  200,
  300,
  600,
  1200,
  1800,
  2400,
  4800,
  9600,
  19200,
  38400,
  57600,
  115200,
  230400,
  460800
};

static sci_tab_t test_array[4] ;

static void Compute(
  unsigned int n,
  unsigned int B,
  double   Phi,
  struct sci_tab *entry )
{
  int    a = ( 32 << ( 2 * n ) ) * B ;

  entry->n = n ;
  entry->B = B ;
  entry->N = rint( ( Phi / a ) - 1.0 ) ;

  if ( ( entry->N > 0 ) && ( entry->N < 256 ) )
    entry->err =
      ( ( Phi / ( (entry->N + 1) * a ) - 1.0 ) * 100.0 );
  else
  {
    entry->err = 100.0 ;
    entry->n = 255 ;
    entry->N = 0 ;
  }
}

static sci_tab_t *SelectN(
  unsigned int 	B,
  double 	Phi )
{
  unsigned int i ;
  struct sci_tab* best = NULL ;

  for ( i = 0 ; i < 4 ; i++ )
  {
    double err ;

    Compute( i, B, Phi, &test_array[i] );
    err = fabs( test_array[i].err );

    if ( best )
    {
      if ( err < fabs( best->err ) )
        best = &test_array[i] ;
    }
    else
      best = &test_array[i] ;
  }

  return best ;
}

int shgen_gensci(
  FILE		*file,
  double	Phi ) /* Processor frequency [Hz] */
{
  unsigned int i ;

  fprintf( file,
    "/*\n * Bitrate table for the serial devices (sci) of the SH at %.3f MHz\n"
    " */\n\n", Phi / 1000000.0 );
  fprintf( file,
    "/*\n"
    " * n     .. SMR bits 0,1 : baud rate generator clock source\n"
    " * N     .. BRR bits 0..7: setting for baud rate generator\n"
    " * error .. percentual error to nominal bitrate\n"
    " *   Hitachi's HW manual recommends bitrates with an error less than 1%%\n"
    " *   We experienced values less than 2%% to be stable\n"
    " */\n\n" );
  fprintf( file, "#include <termios.h>\n\n" );
  fprintf( file,
    "static struct sci_bitrate_t {\n"
    "  unsigned char n ;\n"
    "  unsigned char N ;\n"
    "} _sci_bitrates[] = {\n"
    "/*  n    N      error */\n" );

  for ( i = 0 ; i < sizeof(bitrate)/sizeof(int) ; i++ )
  {
    struct sci_tab* best = SelectN( bitrate[i], Phi );

    if ( i > 0 )
      fprintf( file, ",\n" );
    fprintf( file, "  { %1d, %3d } /* %+7.2f%% ; B%d ",
      best->n,
      best->N,
      best->err,
      best->B );
    if ( best->n > 3 )
      fprintf( file, "(unusable) " );
    fprintf( file, "*/" );
  }

  fprintf( file, "\n};\n\n" );

  fprintf( file,
    "int _sci_get_brparms( \n"
    "  tcflag_t      cflag,\n"
    "  unsigned char *smr,\n"
    "  unsigned char *brr )\n"
    "{\n"
    "  unsigned int offset ;\n\n"
    "  offset = ( cflag & ( CBAUD & ~CBAUDEX ) )\n"
    "    + ( ( cflag & CBAUDEX ) ? B38400 : 0 );\n"
    "  if ( offset == 0 ) return -1 ;\n"
    "  offset-- ;\n\n"
    "  if ( _sci_bitrates[offset].n > 3 )  return -1;\n\n"
    "  *smr &= ~0x03;\n"
    "  *smr |= _sci_bitrates[offset].n;\n"
    "  *brr =  _sci_bitrates[offset].N;\n\n"
    "  return 0;\n"
    "}\n" );

  return 0 ;
}
