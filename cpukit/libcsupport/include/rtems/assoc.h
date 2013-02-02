/**
 * @file rtems/assoc.h
 *
 * @brief RTEMS Associativity Routines
 *
 * RTEMS associativity routines.  Mainly used to convert a value from
 * one space to another (eg: our errno's to host errno's and vice-versa)
 */


#ifndef _RTEMS_RTEMS_ASSOC_H
#define _RTEMS_RTEMS_ASSOC_H

/**
 *  @defgroup Associativity Associativity Routines
 */
/**@{*/

#include <stdint.h> /* uint32_t */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char  *name;
    uint32_t     local_value;
    uint32_t     remote_value;
} rtems_assoc_t;

/*
 * Flag/marker for optional default value in each table
 */

#define RTEMS_ASSOC_DEFAULT_NAME "(default)"

/**
 *  @brief RTEMS Associate Pointer by Name
 */
const rtems_assoc_t *rtems_assoc_ptr_by_name(
  const rtems_assoc_t *,
  const char *
);

/**
 *  @brief RTEMS Associate Pointer by Remote
 */
const rtems_assoc_t *rtems_assoc_ptr_by_remote(
  const rtems_assoc_t *,
  uint32_t
);

uint32_t rtems_assoc_remote_by_local(
  const rtems_assoc_t *,
  uint32_t
);

/**
 *  @brief RTEMS Associate Local by Remote
 */
uint32_t rtems_assoc_local_by_remote(
  const rtems_assoc_t *,
  uint32_t
);

/**
 *  @brief RTEMS Associate Remote by Name
 */
uint32_t rtems_assoc_remote_by_name(
  const rtems_assoc_t *,
  const char *
);

/**
 *  @brief RTEMS Associate Local by Name
 */
uint32_t rtems_assoc_local_by_name(
  const rtems_assoc_t *,
  const char *
);

/**
 *  @brief RTEMS Associate Name by Local
 */
const char *rtems_assoc_name_by_local(
  const rtems_assoc_t *,
  uint32_t
);

/**
 *  @brief RTEMS Associate Name by Remote
 */
const char *rtems_assoc_name_by_remote(
  const rtems_assoc_t *,
  uint32_t
);

/**
 *  @brief RTEMS Assoc Routines
 */
uint32_t rtems_assoc_remote_by_local_bitfield(
  const rtems_assoc_t *,
  uint32_t
);

/**
 *  @brief RTEMS Associate Name by Local Bitfield
 */
char *rtems_assoc_name_by_local_bitfield(
  const rtems_assoc_t *,
  uint32_t  ,
  char *
);

/**
 *  @brief RTEMS Associate Name by Remote Bitfield
 */
char *rtems_assoc_name_by_remote_bitfield(
  const rtems_assoc_t *,
  uint32_t  ,
  char *
);

uint32_t     rtems_assoc_local_by_remote_bitfield(
  const rtems_assoc_t *,
  uint32_t
);

/**
 *  @brief RTEMS Associate Pointer by Local
 */
const rtems_assoc_t *rtems_assoc_ptr_by_local(
  const rtems_assoc_t *ap,
  uint32_t             local_value
);

#if defined(INSIDE_ASSOC)

#define rtems_assoc_is_default(_ap) \
  ((_ap)->name && !strcmp((_ap)->name, RTEMS_ASSOC_DEFAULT_NAME))

/**
 *  @brief RTEMS Associate Bad Name
 *
 *  what to return if a value is not found
 *  this is not reentrant, but it really shouldn't be invoked anyway
 */
const char *rtems_assoc_name_bad(
  uint32_t   bad_value
);
#endif

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* ! _RTEMS_RTEMS_ASSOC_H */
