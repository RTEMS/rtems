/**
 * @file rtems/assoc.h
 */

/*
 *
 *  Rtems associativity routines.  Mainly used to convert a value from
 *  one space to another (eg: our errno's to host errno's and v.v)
 *
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_ASSOC_H
#define _RTEMS_RTEMS_ASSOC_H

typedef struct {
    const char   *name;
    uint32_t     local_value;
    uint32_t     remote_value;
} rtems_assoc_t;

/*
 * Flag/marker for optional default value in each table
 */

#define RTEMS_ASSOC_DEFAULT_NAME "(default)"

const rtems_assoc_t *rtems_assoc_ptr_by_name(const rtems_assoc_t *, const char *);
const rtems_assoc_t *rtems_assoc_ptr_by_local(const rtems_assoc_t *, uint32_t);
const rtems_assoc_t *rtems_assoc_ptr_by_remote(const rtems_assoc_t *, uint32_t);

uint32_t     rtems_assoc_remote_by_local(const rtems_assoc_t *, uint32_t);
uint32_t     rtems_assoc_local_by_remote(const rtems_assoc_t *, uint32_t);
uint32_t     rtems_assoc_remote_by_name(const rtems_assoc_t *, const char *);
uint32_t     rtems_assoc_local_by_name(const rtems_assoc_t *, const char *);
const char  *rtems_assoc_name_by_local(const rtems_assoc_t *, uint32_t);
const char  *rtems_assoc_name_by_remote(const rtems_assoc_t *, uint32_t);

uint32_t     rtems_assoc_remote_by_local_bitfield(const rtems_assoc_t *, uint32_t);
char  *rtems_assoc_name_by_local_bitfield(const rtems_assoc_t *, uint32_t  , char *);
char  *rtems_assoc_name_by_remote_bitfield(const rtems_assoc_t *, uint32_t  , char *);
uint32_t     rtems_assoc_local_by_remote_bitfield(const rtems_assoc_t *, uint32_t  );


#endif /* ! _INCLUDE_ASSOC_H */
