/*****  P A C K H E X . C  ************************************************
 *
 *   Packhex is a hex-file compaction utility.  It attempts to concatenate
 *   hex records to produce more size-efficient packaging.
 *
 *   Limitations: Input files must be correctly formatted.  This utility
 *                is not robust enough to detect hex-record formatting
 *                errors.
 *
 *   Published:   May 1993 Embedded Systems Programming magazine
 *                "Creating Faster Hex Files"
 *
 *   URL:         ESP magazine: http://www.embedded.com
 *                Source Code:  ftp://ftp.mfi.com/pub/espmag/1993/pakhex.zip
 *
 *   Author:      Mark Gringrich
 *
 *   Compiler:    Microsoft C 6.0
 *                cl /F 1000 packhex.c
 *
 **************************************************************************/


/* #define SMALLER_RECORDS */
#ifdef SMALLER_RECORDS
#define MAX_LEN_S1_RECS 128
#define MAX_LEN_S2_RECS 128
#define MAX_LEN_S3_RECS 128
#else
#define MAX_LEN_S1_RECS 252
#define MAX_LEN_S2_RECS 251
#define MAX_LEN_S3_RECS 250
#endif


/*--------------------------------- includes ---------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#ifndef VMS
#ifndef HAVE_STRERROR
extern int sys_nerr;
extern char *sys_errlist[];

#define strerror( _err ) \
  ((_err) < sys_nerr) ? sys_errlist [(_err)] : "unknown error"

#else   /* HAVE_STRERROR */
char *strerror ();
#endif
#else   /* VMS */
char *strerror (int,...);
#endif

#if defined(__unix__) && !defined(EXIT_FAILURE)
#define EXIT_FAILURE -1
#define EXIT_SUCCESS  0
#endif

/*--------------------------------- defines ----------------------------------*/

#define YES                   1
#define MAX_LINE_SIZE       600
#define EOS                 '\0'


/*---------------------------------- macros ----------------------------------*/

/* Convert ASCII hexadecimal digit to value. */

#define HEX_DIGIT( C )   ( ( ( ( C ) > '9' ) ? ( C ) + 25 : ( C ) ) & 0xF )


/*--------------------------------- typedefs ---------------------------------*/

typedef unsigned char   Boolean;
typedef unsigned char   Uchar;
typedef unsigned int    Uint;
typedef unsigned long   Ulong;

typedef struct   /* Functions and constant returning Hex-record vital stats. */
{
    Boolean    ( *is_data_record  )( char *              );
    Ulong      ( *get_address     )( char *              );
    Uint       ( *get_data_count  )( char *              );
    const Uint    max_data_count;
    char      *( *get_data_start  )( char *              );
    void       ( *put_data_record )( Uint, Ulong, char * );
} Rec_vitals;


/*--------------------------- function prototypes ----------------------------*/

Rec_vitals  * identify_first_data_record( char *, int );
Ulong         get_ndigit_hex( char *, int );


/*----------------------------- Intel Hex format -----------------------------*/

/*
 *    Intel Hex data-record layout
 *
 *    :aabbbbccd...dee
 *
 *    :      - header character
 *    aa     - record data byte count, a 2-digit hex value
 *    bbbb   - record address, a 4-digit hex value
 *    cc     - record type, a 2-digit hex value:
 *              "00" is a data record
 *              "01" is an end-of-data record
 *              "02" is an extended-address record
 *              "03" is a start record
 *    d...d  - data (always an even number of chars)
 *    ee     - record checksum, a 2-digit hex value
 *             checksum = 2's complement
 *                 [ (sum of bytes: aabbbbccd...d) modulo 256 ]
 */


Boolean is_intel_data_rec( char * rec_str )
{
    return( ( rec_str[ 0 ] == ':' )  &&  ( rec_str[ 8 ] == '0' ) );
}

Uint get_intel_rec_data_count( char * rec_str )
{
    return( ( Uint ) get_ndigit_hex( rec_str + 1, 2 ) );
}

Ulong get_intel_rec_address( char * rec_str )
{
    return( get_ndigit_hex( rec_str + 3, 4 ) );
}

char * get_intel_rec_data_start( char * rec_str )
{
    return( rec_str + 9 );
}

void put_intel_data_rec( Uint count, Ulong address, char * data_str )
{
    char    *ptr;
    Uint    sum = count + ( address >> 8 & 0xff ) + ( address & 0xff );

    for ( ptr = data_str ; *ptr != EOS ; ptr += 2 )
        sum += ( Uint ) get_ndigit_hex( ptr, 2 );

    printf(
      ":%02X%04lX00%s%02X\n", count, address, data_str, (~sum + 1) & 0xff
    );
}


Rec_vitals  intel_hex =
{
    is_intel_data_rec,
    get_intel_rec_address,
    get_intel_rec_data_count,
    255,                        /* Maximum data bytes in a record. */
    get_intel_rec_data_start,
    put_intel_data_rec
};


/*------------------------- Motorola S1-record format ------------------------*/

/*
 *    Motorola S-record data-record layout
 *
 *    Sabbc...cd...dee
 *
 *    S      - header character
 *    a      - record type, a 1-digit value:
 *               "0" is a header record
 *               "1" is a 2-byte-address data record
 *               "2" is a 3-byte-address data record
 *               "3" is a 4-byte-address data record
 *               "7" is a 4-byte-address end-of-data record
 *               "8" is a 3-byte-address end-of-data record
 *               "9" is a 2-byte-address end-of-data record
 *    bb     - record length in bytes, a 2-digit hex value
 *             (record length doesn't count the header/type
 *              chars and checksum byte)
 *    c...c  - record address, a 4-, 6-, or 8-digit value,
 *               depending on record type
 *    d...d  - data (always an even number of chars)
 *    ee     - record checksum, a 2-digit hex value
 *             checksum = 1's complement
 *             [ (sum of all bytes: bbc..cd...d) modulo 256 ]
 */

#define S1_COUNT_OFFSET         3


Boolean is_moto_s1_data_rec( char * rec_str )
{
    return ( ( rec_str[ 0 ] == 'S' )  &&  ( rec_str[ 1 ] == '1' ) );
}

Uint get_moto_s1_rec_data_count( char * rec_str )
{
    return( ( Uint ) get_ndigit_hex( rec_str + 2, 2 ) - S1_COUNT_OFFSET );
}

Ulong get_moto_s1_rec_address( char * rec_str )
{
    return( get_ndigit_hex( rec_str + 4, 4 ) );
}

char * get_moto_s1_rec_data_start( char * rec_str )
{
    return( rec_str + 8 );
}

void put_moto_s1_data_rec( Uint count, Ulong address, char * data_str )
{
    char   *ptr;
    Uint    sum = S1_COUNT_OFFSET + count +
                    ( address >> 8 & 0xff ) + ( address & 0xff );

    for ( ptr = data_str ; *ptr != EOS ; ptr += 2 )
        sum += ( Uint ) get_ndigit_hex( ptr, 2 );

    printf(
      "S1%02X%04lX%s%02X\n",
      count + S1_COUNT_OFFSET, address, data_str, ~sum & 0xff
    );
}


Rec_vitals  motorola_s1_rec =
{
    is_moto_s1_data_rec,
    get_moto_s1_rec_address,
    get_moto_s1_rec_data_count,
    MAX_LEN_S1_RECS,            /* Maximum data bytes in a record. */
    get_moto_s1_rec_data_start,
    put_moto_s1_data_rec
};


/*------------------------- Motorola S2-record format ------------------------*/

#define S2_COUNT_OFFSET         4

Boolean is_moto_s2_data_rec( char * rec_str )
{
    return ( ( rec_str[ 0 ] == 'S' )  &&  ( rec_str[ 1 ] == '2' ) );
}

Uint get_moto_s2_rec_data_count( char * rec_str )
{
    return( ( Uint ) get_ndigit_hex( rec_str + 2, 2 ) - S2_COUNT_OFFSET );
}

Ulong get_moto_s2_rec_address( char * rec_str )
{
    return( get_ndigit_hex( rec_str + 4, 6 ) );
}

char * get_moto_s2_rec_data_start( char * rec_str )
{
    return( rec_str + 10 );
}

void put_moto_s2_data_rec( Uint count, Ulong address, char * data_str )
{
    char    *ptr;
    Uint    sum = S2_COUNT_OFFSET + count + ( address >> 16 & 0xff ) +
                                            ( address >>  8 & 0xff ) +
                                            ( address       & 0xff );

    for ( ptr = data_str ; *ptr != EOS ; ptr += 2 )
        sum += ( Uint ) get_ndigit_hex( ptr, 2 );

    printf(
      "S2%02X%06lX%s%02X\n",
      count + S2_COUNT_OFFSET, address, data_str, ~sum & 0xff
    );
}


Rec_vitals  motorola_s2_rec =
{
    is_moto_s2_data_rec,
    get_moto_s2_rec_address,
    get_moto_s2_rec_data_count,
    MAX_LEN_S2_RECS,            /* Maximum data bytes in a record. */
    get_moto_s2_rec_data_start,
    put_moto_s2_data_rec
};


/*------------------------- Motorola S3-record format ------------------------*/

#define S3_COUNT_OFFSET         5

Boolean is_moto_s3_data_rec( char * rec_str )
{
    return ( ( rec_str[ 0 ] == 'S' )  &&  ( rec_str[ 1 ] == '3' ) );
}

Uint get_moto_s3_rec_data_count( char * rec_str )
{
    return( ( Uint ) get_ndigit_hex( rec_str + 2, 2 ) - S3_COUNT_OFFSET );
}

Ulong get_moto_s3_rec_address( char * rec_str )
{
    return( get_ndigit_hex( rec_str + 4, 8 ) );
}

char * get_moto_s3_rec_data_start( char * rec_str )
{
    return( rec_str + 12 );
}

void put_moto_s3_data_rec( Uint count, Ulong address, char * data_str )
{
    char    *ptr;
    Uint     sum = S3_COUNT_OFFSET + count + ( address >> 24 & 0xff ) +
                                             ( address >> 16 & 0xff ) +
                                             ( address >>  8 & 0xff ) +
                                             ( address       & 0xff );

    for ( ptr = data_str ; *ptr != EOS ; ptr += 2 )
        sum += ( Uint ) get_ndigit_hex( ptr, 2 );

    printf(
      "S3%02X%08lX%s%02X\n",
      count + S3_COUNT_OFFSET, address, data_str, ~sum & 0xff
   );
}


Rec_vitals  motorola_s3_rec =
{
    is_moto_s3_data_rec,
    get_moto_s3_rec_address,
    get_moto_s3_rec_data_count,
    MAX_LEN_S3_RECS,            /* Maximum data bytes in a record. */
    get_moto_s3_rec_data_start,
    put_moto_s3_data_rec
};


/*-------------------- Put your favorite hex format here ---------------------*/

/*
 *    * * *  The following is a template for an additional hex format:  * * *
 *
 *
 *    Boolean is_X_data_rec( char * rec_str ) {}
 *
 *    Uint get_X_rec_data_count( char * rec_str ) {}
 *
 *    Ulong get_X_rec_address( char * rec_str ) {}
 *
 *    char * get_X_rec_data_start( char * rec_str ) {}
 *
 *    void put_X_data_rec( Uint count, Ulong address, char * data_str ) {}
 *
 *    Rec_vitals  X_rec =
 *    {
 *        is_X_data_rec,
 *        get_X_rec_address,
 *        get_X_rec_data_count,
 *        MAXIMUM DATA BYTES IN A RECORD,
 *        get_X_rec_data_start,
 *        put_X_data_rec
 *    };
 *
 */

/*----------------------------------------------------------------------------*/


/*
 *   Put address of additional Rec_vitals structures
 *   in this array, before the NULL entry.
 */

Rec_vitals  *formats[] =
{
    &intel_hex,
    &motorola_s1_rec,
    &motorola_s2_rec,
    &motorola_s3_rec,
    ( Rec_vitals * ) NULL
};


/****   main   *****************************************************************
*
*
*       Expects: Nothing (no command-line parameters).
*
*       Returns: Exit status (EXIT_SUCCESS or EXIT_FAILURE).
*
*       Reads hex records on the standard input and attempts to
*       splice adjacent data fields together.  Results appear on
*       the standard output.
*
*******************************************************************************/

int main(
  int argc,
  char **argv
)
{

    char       inbuff[ MAX_LINE_SIZE ], outbuff[ MAX_LINE_SIZE ];
    char       *in_dptr, *out_dptr;
    int        d_total, d_count, d_excess, n;
    int        length;
    Ulong      in_rec_addr, out_rec_addr = 0;
    Rec_vitals *rptr;


    /* Sift through file until first hex record is identified.    */

    rptr = identify_first_data_record( inbuff, MAX_LINE_SIZE );
    if ( rptr == NULL )
    {
        fputs( "No hex records found.\n", stderr );
        exit( EXIT_FAILURE );
    }


    /* Attempt data-record splicing until end-of-file is reached. */
    d_total = 0;
    for (;;) {
        if ( rptr->is_data_record( inbuff ) == YES )
        { /* Input record is a data record. */
            d_count     = rptr->get_data_count( inbuff );
            in_rec_addr = rptr->get_address( inbuff );
            in_dptr     = rptr->get_data_start( inbuff );

            if ( d_total == 0  ||  in_rec_addr != out_rec_addr + d_total )
            { /* Begin a new output record. */
                if ( d_total != 0 )
                    rptr->put_data_record( d_total, out_rec_addr, outbuff );
                out_dptr     = outbuff;
                n = d_total  = d_count;
                out_rec_addr = in_rec_addr;
            }
            else if
              ( ( d_excess = d_total + d_count - rptr->max_data_count ) > 0 )
            { /* Output a maximum-length record, then start a new record. */
                strncat( outbuff, in_dptr, 2 * ( d_count - d_excess ) );
                rptr->put_data_record(
                  rptr->max_data_count, out_rec_addr, outbuff
                );
                in_dptr      += 2 * ( d_count - d_excess );
                out_dptr      = outbuff;
                n = d_total   = d_excess;
                out_rec_addr += rptr->max_data_count;
            }
            else
            { /* Append input record's data field with accumulated data. */
                out_dptr = outbuff + ( 2 * d_total );
                d_total += n = d_count;
            }
            strncpy( out_dptr, in_dptr, 2 * n );
            out_dptr[ 2 * n ] = EOS;
        }
        else
        { /* Not a data record;
           * flush accumulated data then echo non-data record.
           */
            if ( d_total != 0 )
            {
                rptr->put_data_record( d_total, out_rec_addr, outbuff );
                d_total = 0;
            }
            puts( inbuff );
        }

        inbuff[ MAX_LINE_SIZE - 1 ] = '\0';
        if ( !fgets( inbuff, MAX_LINE_SIZE, stdin ) )
           break;
        if ( inbuff[ MAX_LINE_SIZE - 1 ] ) {
           fprintf( stderr, "Input line too long" );
           exit( 1 );
        }
        length = strlen(inbuff);
        inbuff[length - 1] = '\0';

    }


    return ( EXIT_SUCCESS );

}


/****   identify_first_data_record   *******************************************
*
*       Expects: Pointer to hex-record line buffer.
*
*       Returns: Pointer to hex-record structure (NULL if no match found).
*
*       Reads the standard input, line by line, searching for a valid
*       record header character.  If a valid header is found, a pointer
*       to the hex-record's type structure is returned, otherwise NULL.
*
*       The input-stream pointer is left pointing to the first valid hex record.
*
*******************************************************************************/

Rec_vitals * identify_first_data_record( char * buff_ptr, int max_length )
{
    Rec_vitals  ** ptr;
    int            length;



    for ( ;; ) {

        buff_ptr[ max_length - 1 ] = '\0';
        if ( !fgets( buff_ptr, max_length, stdin ) )
           break;
        if ( buff_ptr[ max_length - 1 ] ) {
           fprintf( stderr, "Input line too long" );
           exit( 1 );
        }
        length = strlen(buff_ptr);
        buff_ptr[length - 1] = '\0';

        for ( ptr = formats ; *ptr != ( Rec_vitals * ) NULL ; ptr++ )
            if ( ( *ptr )->is_data_record( buff_ptr ) == YES )
                return( *ptr );        /* Successful return.        */

        puts( buff_ptr );              /* Echo non-hex-record line. */
    }

    return( ( Rec_vitals * ) NULL );   /* Unsuccessful return.      */
}


/****   get_ndigit_hex   *******************************************************
*
*       Expects: Pointer to first ASCII hexadecimal digit, number of digits.
*
*       Returns: Value of hexadecimal string as an unsigned long.
*
*******************************************************************************/

Ulong get_ndigit_hex( char * cptr, int digits )
{
    Ulong    value;

    for ( value = 0 ; --digits >= 0 ; cptr++ )
        value = ( value * 16L ) + HEX_DIGIT( *cptr );

    return( value );
}
