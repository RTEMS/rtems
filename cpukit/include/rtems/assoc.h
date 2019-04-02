/**
 * @file
 *
 * @ingroup RTEMSAPIAssociativity
 *
 * @brief RTEMS Associativity Routines
 */


#ifndef _RTEMS_RTEMS_ASSOC_H
#define _RTEMS_RTEMS_ASSOC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSAPIAssociativity Associativity Routines
 *
 * @ingroup RTEMSAPI
 *
 * RTEMS associativity routines.  Mainly used to convert a value from
 * one space to another (eg: our errno's to host errno's and vice-versa)
 *
 * @{
 */

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

typedef struct {
  uint32_t    bits;
  const char *name;
} rtems_assoc_32_pair;

/**
 * @brief Converts the specified value into a text representation.
 *
 * @param[in] value The value to convert.
 * @param[in] buffer The buffer for the text representation.
 * @param[in] buffer_size The buffer size in characters.
 * @param[in] pairs Names for particular bits.
 * @param[in] pair_count Count of pairs.
 * @param[in] separator Separator between individual names.
 * @param[in] fallback Fallback value in case no bits contained in the pairs
 *   are set in the value.
 *
 * @retval The length of the text representation.  May be greater than or equal
 * to the buffer size if truncation occurred.
 */
size_t rtems_assoc_32_to_string(
  uint32_t                   value,
  char                      *buffer,
  size_t                     buffer_size,
  const rtems_assoc_32_pair *pairs,
  size_t                     pair_count,
  const char                *separator,
  const char                *fallback
);

/**
 * @brief Converts the specified thread states into a text representation.
 *
 * @param[in] states The thread states to convert.
 * @param[in] buffer The buffer for the text representation.
 * @param[in] buffer_size The buffer size in characters.
 *
 * @retval The length of the text representation.  May be greater than or equal
 * to the buffer size if truncation occurred.
 */
size_t rtems_assoc_thread_states_to_string(
  uint32_t  states,
  char     *buffer,
  size_t    buffer_size
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ! _RTEMS_RTEMS_ASSOC_H */
