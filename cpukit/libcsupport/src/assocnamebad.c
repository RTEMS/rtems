/*
 * assoc.c
 *      rtems assoc routines
 *
 *  $Id$
 */

#include <rtems.h>
#include "assoc.h"

#include <stdio.h>              /* sprintf */
#include <string.h>             /* strcat, strcmp */

#define STREQ(a,b)      (strcmp((a), (b)) == 0)
#define rtems_assoc_is_default(ap)  ((ap)->name && STREQ(ap->name, RTEMS_ASSOC_DEFAULT_NAME))

/*
 * what to return if a value is not found
 * this is not reentrant, but it really shouldn't be invoked anyway
 */

const char *
rtems_assoc_name_bad(
    unsigned32 bad_value
)
{
#ifdef RTEMS_DEBUG
    static char bad_buffer[32];

    sprintf(bad_buffer, "< %d [0x%x] >", bad_value, bad_value);
#else
    static char bad_buffer[32] = "<assoc.c: BAD NAME>";
#endif
    return bad_buffer;
}


