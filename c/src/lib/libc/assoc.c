/*
 * assoc.c
 *      rtems assoc routines
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/assoc.h>

#include <string.h>             /* strcat, strcmp */

#define STREQ(a,b)      (strcmp((a), (b)) == 0)
#define rtems_assoc_is_default(ap)  ((ap)->name && STREQ(ap->name, RTEMS_ASSOC_DEFAULT_NAME))

const rtems_assoc_t *
rtems_assoc_ptr_by_name(
    const rtems_assoc_t *ap,
    const char *name
  )
{
    const rtems_assoc_t *default_ap = 0;

    if (rtems_assoc_is_default(ap))
        default_ap = ap++;
    
    for ( ; ap->name; ap++)
        if (strcmp(ap->name, name) == 0)
            return ap;

    return default_ap;
}

const rtems_assoc_t *
rtems_assoc_ptr_by_local(
    const rtems_assoc_t *ap,
    unsigned32     local_value
  )
{
    const rtems_assoc_t *default_ap = 0;

    if (rtems_assoc_is_default(ap))
        default_ap = ap++;
    
    for ( ; ap->name; ap++)
        if (ap->local_value == local_value)
            return ap;

    return default_ap;
}


const rtems_assoc_t *
rtems_assoc_ptr_by_remote(
    const rtems_assoc_t *ap,
    unsigned32     remote_value
  )
{
    const rtems_assoc_t *default_ap = 0;

    if (rtems_assoc_is_default(ap))
        default_ap = ap++;
    
    for ( ; ap->name; ap++)
        if (ap->remote_value == remote_value)
            return ap;

    return default_ap;
}


/*
 * Get values
 */

unsigned32
rtems_assoc_remote_by_local(
    const rtems_assoc_t *ap,
    unsigned32     local_value
  )
{
    const rtems_assoc_t *nap;
    nap = rtems_assoc_ptr_by_local(ap, local_value);
    if (nap)
        return nap->remote_value;
    
    return 0;
}

unsigned32
rtems_assoc_local_by_remote(
    const rtems_assoc_t *ap,
    unsigned32     remote_value
  )
{
    const rtems_assoc_t *nap;
    nap = rtems_assoc_ptr_by_remote(ap, remote_value);
    if (nap)
        return nap->local_value;
    
    return 0;
}

unsigned32
rtems_assoc_remote_by_name(
    const rtems_assoc_t *ap,
    const char          *name
  )
{
    const rtems_assoc_t *nap;
    nap = rtems_assoc_ptr_by_name(ap, name);
    if (nap)
        return nap->remote_value;
    
    return 0;
}

unsigned32
rtems_assoc_local_by_name(
    const rtems_assoc_t *ap,
    const char          *name
  )
{
    const rtems_assoc_t *nap;
    nap = rtems_assoc_ptr_by_name(ap, name);
    if (nap)
        return nap->local_value;
    
    return 0;
}

/*
 * what to return if a value is not found
 * this is not reentrant, but it really shouldn't be invoked anyway
 */

const char *rtems_assoc_name_bad(
    unsigned32 bad_value
);

/* body in separate file to reduce dependency on printf */


const char *
rtems_assoc_name_by_local(
    const rtems_assoc_t *ap,
    unsigned32     local_value
  )
{
    const rtems_assoc_t *nap;
    nap = rtems_assoc_ptr_by_local(ap, local_value);
    if (nap)
        return nap->name;
    
    return rtems_assoc_name_bad(local_value);
}

const char *
rtems_assoc_name_by_remote(
    const rtems_assoc_t *ap,
    unsigned32     remote_value
  )
{
    const rtems_assoc_t *nap;
    nap = rtems_assoc_ptr_by_remote(ap, remote_value);
    if (nap)
        return nap->name;
    
    return rtems_assoc_name_bad(remote_value);
}

/*
 * Bitfield functions assume just 1 bit set in each of remote and local
 *      entries; they do not check for this.
 */

unsigned32 rtems_assoc_remote_by_local_bitfield(
    const rtems_assoc_t *ap,
    unsigned32     local_value
  )
{  
    unsigned32 b;
    unsigned32 remote_value = 0;

    for (b = 1; b; b <<= 1)
        if (b & local_value)
            remote_value |= rtems_assoc_remote_by_local(ap, b);
        
    return remote_value;
}


unsigned32 rtems_assoc_local_by_remote_bitfield(
    const rtems_assoc_t *ap,
    unsigned32     remote_value
  )
{  
    unsigned32 b;
    unsigned32 local_value = 0;

    for (b = 1; b; b <<= 1)
        if (b & remote_value)
            local_value |= rtems_assoc_local_by_remote(ap, b);
        
    return local_value;
}

char *
rtems_assoc_name_by_remote_bitfield(
    const rtems_assoc_t *ap,
    unsigned32     value,
    char          *buffer
  )
{  
    unsigned32 b;

    *buffer = 0;

    for (b = 1; b; b <<= 1)
        if (b & value)
        {
            if (*buffer)
                strcat(buffer, " ");
            strcat(buffer, rtems_assoc_name_by_remote(ap, b));
        }
        
    return buffer;
}

char *
rtems_assoc_name_by_local_bitfield(
    const rtems_assoc_t *ap,
    unsigned32     value,
    char          *buffer
  )
{  
    unsigned32 b;

    *buffer = 0;

    for (b = 1; b; b <<= 1)
        if (b & value)
        {
            if (*buffer)
                strcat(buffer, " ");
            strcat(buffer, rtems_assoc_name_by_local(ap, b));
        }
        
    return buffer;
}
