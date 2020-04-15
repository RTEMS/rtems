/**
 *  @file
 *
 *  @brief RTEMS Associate Bad Name
 *  @ingroup SET
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define INSIDE_ASSOC

#include <rtems.h>
#include <rtems/assoc.h>

#include <inttypes.h>
#include <stdio.h>              /* sprintf */

const char *
rtems_assoc_name_bad(
#ifdef RTEMS_DEBUG
    uint32_t   bad_value
#else
    uint32_t   bad_value RTEMS_UNUSED
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
