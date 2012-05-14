/*
 * assoc.c
 *      rtems assoc routines
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define INSIDE_ASSOC

#include <rtems.h>
#include <rtems/assoc.h>

#include <inttypes.h>
#include <stdio.h>              /* sprintf */

/*
 * what to return if a value is not found
 * this is not reentrant, but it really shouldn't be invoked anyway
 */

const char *
rtems_assoc_name_bad(
#ifdef RTEMS_DEBUG
    uint32_t   bad_value
#else
    uint32_t   bad_value __attribute((unused))
#endif
)
{
#ifdef RTEMS_DEBUG
    static char bad_buffer[32];

    sprintf(bad_buffer, "< %" PRId32 "[0x%" PRIx32 " ] >", bad_value, bad_value);
#else
    static char bad_buffer[40] = "<assocnamebad.c: : BAD NAME>";
#endif
    return bad_buffer;
}
