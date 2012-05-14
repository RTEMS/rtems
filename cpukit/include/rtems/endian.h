/**
 * @file rtems/endian.h
 *
 * This include file provides endian information about the target.
 */


#ifndef _RTEMS_ENDIAN_H
#define _RTEMS_ENDIAN_H

#include <rtems.h>

/*
 * BSD-style endian declaration
 */
#define BIG_ENDIAN	4321
#define LITTLE_ENDIAN	1234

#ifndef BYTE_ORDER
#if CPU_BIG_ENDIAN
# define BYTE_ORDER BIG_ENDIAN
#elif CPU_LITTLE_ENDIAN
# define BYTE_ORDER LITTLE_ENDIAN
#else
# error "Can't decide which end is which!"
#endif
#endif

#if ( CPU_BIG_ENDIAN == TRUE )

/*
 *  Very simply on big endian CPUs
 */

static inline uint32_t ntohl( uint32_t _x )
{
  return _x;
}

static inline uint16_t ntohs( uint16_t _x )
{
  return _x;
}

static inline uint32_t htonl( uint32_t _x )
{
  return _x;
}

static inline uint16_t htons( uint16_t _x )
{
  return _x;
}

#define NTOHS(x)
#define HTONS(x)
#define NTOHL(x)
#define HTONL(x)

#elif ( CPU_LITTLE_ENDIAN == TRUE )

/*
 *  A little more complicated on little endian CPUs
 */

static inline uint32_t ntohl( uint32_t _x )
{
  return CPU_swap_u32(_x);
}

static inline uint16_t ntohs( uint16_t _x )
{
  return CPU_swap_u16(_x);
}

static inline uint32_t htonl( uint32_t _x )
{
  return CPU_swap_u32(_x);
}

static inline uint16_t htons( uint16_t _x )
{
  return CPU_swap_u16(_x);
}

#define NTOHS(x) (x) = ntohs(x)
#define HTONS(x) (x) = htons(x)
#define NTOHL(x) (x) = ntohl(x)
#define HTONL(x) (x) = htonl(x)

#else
#error "Unknown endian-ness for this cpu"
#endif

static inline uint16_t rtems_uint16_from_little_endian( const uint8_t *data)
{
  uint16_t value = 0;
  ssize_t i = 0;

  for (i = 1; i >= 0; --i) {
    value = (uint16_t) ((value << 8) + data [i]);
  }

  return value;
}

static inline uint32_t rtems_uint32_from_little_endian( const uint8_t *data)
{
  uint32_t value = 0;
  ssize_t i = 0;

  for (i = 3; i >= 0; --i) {
    value = (value << 8) + data [i];
  }

  return value;
}

static inline uint64_t rtems_uint64_from_little_endian( const uint8_t *data)
{
  uint64_t value = 0;
  ssize_t i = 0;

  for (i = 7; i >= 0; --i) {
    value = (value << 8) + (uint64_t) data [i];
  }

  return value;
}

static inline uint16_t rtems_uint16_from_big_endian( const uint8_t *data)
{
  uint16_t value = 0;
  size_t i = 0;

  for (i = 0; i < 2; ++i) {
    value = (uint16_t) ((value << 8) + data [i]);
  }

  return value;
}

static inline uint32_t rtems_uint32_from_big_endian( const uint8_t *data)
{
  uint32_t value = 0;
  size_t i = 0;

  for (i = 0; i < 4; ++i) {
    value = (value << 8) + (uint32_t) data [i];
  }

  return value;
}

static inline uint64_t rtems_uint64_from_big_endian( const uint8_t *data)
{
  uint64_t value = 0;
  size_t i = 0;

  for (i = 0; i < 8; ++i) {
    value = (value << 8) + (uint64_t) data [i];
  }

  return value;
}

static inline void rtems_uint16_to_little_endian( uint16_t value, uint8_t *data)
{
  size_t i = 0;

  for (i = 0; i < 2; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint32_to_little_endian( uint32_t value, uint8_t *data)
{
  size_t i = 0;

  for (i = 0; i < 4; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint64_to_little_endian( uint64_t value, uint8_t *data)
{
  size_t i = 0;

  for (i = 0; i < 8; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint16_to_big_endian( uint16_t value, uint8_t *data)
{
  ssize_t i = 0;

  for (i = 1; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint32_to_big_endian( uint32_t value, uint8_t *data)
{
  ssize_t i = 0;

  for (i = 3; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint64_to_big_endian( uint64_t value, uint8_t *data)
{
  ssize_t i = 0;

  for (i = 7; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

#endif /* _RTEMS_ENDIAN_H */
