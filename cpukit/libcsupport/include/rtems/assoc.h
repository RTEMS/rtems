/*
 *	@(#)assoc.h	1.2 - 95/06/28
 *	
 *
 *  Rtems associativity routines.  Mainly used to convert a value from
 *  one space to another (eg: our errno's to host errno's and v.v)
 *
 *
 *  $Id$
 */

#ifndef _INCLUDE_ASSOC_H
#define _INCLUDE_ASSOC_H

typedef struct {
    char *name;
    unsigned32   local_value;
    unsigned32   remote_value;
} rtems_assoc_t;

/*
 * Flag/marker for optional default value in each table
 */

#define RTEMS_ASSOC_DEFAULT_NAME "(default)"

rtems_assoc_t *rtems_assoc_ptr_by_name(rtems_assoc_t *, char *);
rtems_assoc_t *rtems_assoc_ptr_by_value(rtems_assoc_t *, unsigned32);
rtems_assoc_t *rtems_assoc_ptr_by_remote(rtems_assoc_t *, unsigned32);

unsigned32   rtems_assoc_remote_by_local(rtems_assoc_t *, unsigned32);
unsigned32   rtems_assoc_local_by_remote(rtems_assoc_t *, unsigned32);
unsigned32   rtems_assoc_remote_by_name(rtems_assoc_t *, char *);
unsigned32   rtems_assoc_local_by_name(rtems_assoc_t *, char *);
char        *rtems_assoc_name_by_local(rtems_assoc_t *, unsigned32);
char        *rtems_assoc_name_by_remote(rtems_assoc_t *, unsigned32);

unsigned32   rtems_assoc_remote_by_local_bitfield(rtems_assoc_t *, unsigned32);
char        *rtems_assoc_name_by_local_bitfield(rtems_assoc_t *, unsigned32, char *);
char        *rtems_assoc_name_by_remote_bitfield(rtems_assoc_t *, unsigned32, char *);
unsigned32   rtems_assoc_local_by_remote_bitfield(rtems_assoc_t *ap, unsigned32);


#endif /* ! _INCLUDE_ASSOC_H */
