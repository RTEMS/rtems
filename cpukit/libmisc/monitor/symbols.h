/*
 *  RTEMS monitor symbol table functions
 *
 *  Description:
 *      Entry points for symbol table routines.
 *
 *
 *
 *  TODO:
 *
 *  $Id$
 */

#ifndef _INCLUDE_SYMBOLS_H
#define _INCLUDE_SYMBOLS_H

#include <rtems/monitor.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _rtems_symbol_t {
    rtems_unsigned32 value;
    char            *name;
} ;

#define SYMBOL_STRING_BLOCK_SIZE 4080
typedef struct rtems_symbol_string_block_s {
    struct rtems_symbol_string_block_s *next;
    char   buffer[SYMBOL_STRING_BLOCK_SIZE];
} rtems_symbol_string_block_t;

struct _rtems_symbol_table_t {

    rtems_unsigned32 sorted;          /* are symbols sorted right now? */
    rtems_unsigned32 growth_factor;   /* % to grow by when needed */
    rtems_unsigned32 next;            /* next symbol slot to use when adding */
    rtems_unsigned32 size;            /* max # of symbols */

    /*
     * Symbol list -- sorted by address (when we do a lookup)
     */

    rtems_symbol_t  *addresses;         /* symbol array by address */

    /*
     * String pool, unsorted, a list of blocks of string data
     */

    rtems_symbol_string_block_t *string_buffer_head;
    rtems_symbol_string_block_t *string_buffer_current;
    rtems_unsigned32 strings_next;      /* next byte to use in this block */

} ;

#define rtems_symbol_name(sp)   ((sp)->name)
#define rtems_symbol_value(sp)  ((sp)->value)

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_SYMBOLS_H */
