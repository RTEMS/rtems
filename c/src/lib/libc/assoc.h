/*
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
    const char   *name;
    unsigned32   local_value;
    unsigned32   remote_value;
} rtems_assoc_t;

/*
 * Flag/marker for optional default value in each table
 */

#define RTEMS_ASSOC_DEFAULT_NAME "(default)"

const rtems_assoc_t *rtems_assoc_ptr_by_name(const rtems_assoc_t *, const char *);
const rtems_assoc_t *rtems_assoc_ptr_by_value(const rtems_assoc_t *, unsigned32);
const rtems_assoc_t *rtems_assoc_ptr_by_remote(const rtems_assoc_t *, unsigned32);

unsigned32   rtems_assoc_remote_by_local(const rtems_assoc_t *, unsigned32);
unsigned32   rtems_assoc_local_by_remote(const rtems_assoc_t *, unsigned32);
unsigned32   rtems_assoc_remote_by_name(const rtems_assoc_t *, const char *);
unsigned32   rtems_assoc_local_by_name(const rtems_assoc_t *, const char *);
const char  *rtems_assoc_name_by_local(const rtems_assoc_t *, unsigned32);
const char  *rtems_assoc_name_by_remote(const rtems_assoc_t *, unsigned32);

unsigned32   rtems_assoc_remote_by_local_bitfield(const rtems_assoc_t *, unsigned32);
char  *rtems_assoc_name_by_local_bitfield(const rtems_assoc_t *, unsigned32, char *);
char  *rtems_assoc_name_by_remote_bitfield(const rtems_assoc_t *, unsigned32, char *);
unsigned32   rtems_assoc_local_by_remote_bitfield(const rtems_assoc_t *, unsigned32);


#endif /* ! _INCLUDE_ASSOC_H */
