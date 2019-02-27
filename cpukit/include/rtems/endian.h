/**
 * @file
 *
 * This include file provides endian information about the target.
 */


#ifndef _RTEMS_ENDIAN_H
#define _RTEMS_ENDIAN_H

#include <stdint.h>

#include <machine/endian.h>

#ifndef _BYTEORDER_FUNC_DEFINED
#define	_BYTEORDER_FUNC_DEFINED
#define	htonl(x)	__htonl(x)
#define	htons(x)	__htons(x)
#define	ntohl(x)	__ntohl(x)
#define	ntohs(x)	__ntohs(x)
#endif

#define NTOHS(x) (x) = ntohs(x)
#define HTONS(x) (x) = htons(x)
#define NTOHL(x) (x) = ntohl(x)
#define HTONL(x) (x) = htonl(x)

static inline uint16_t rtems_uint16_from_little_endian( const uint8_t *data)
{
  uint16_t value = 0;
  int i;

  for (i = 1; i >= 0; --i) {
    value = (uint16_t) ((value << 8) + data [i]);
  }

  return value;
}

static inline uint32_t rtems_uint32_from_little_endian( const uint8_t *data)
{
  uint32_t value = 0;
  int i;

  for (i = 3; i >= 0; --i) {
    value = (value << 8) + data [i];
  }

  return value;
}

static inline uint64_t rtems_uint64_from_little_endian( const uint8_t *data)
{
  uint64_t value = 0;
  int i;

  for (i = 7; i >= 0; --i) {
    value = (value << 8) + (uint64_t) data [i];
  }

  return value;
}

static inline uint16_t rtems_uint16_from_big_endian( const uint8_t *data)
{
  uint16_t value = 0;
  int i;

  for (i = 0; i < 2; ++i) {
    value = (uint16_t) ((value << 8) + data [i]);
  }

  return value;
}

static inline uint32_t rtems_uint32_from_big_endian( const uint8_t *data)
{
  uint32_t value = 0;
  int i;

  for (i = 0; i < 4; ++i) {
    value = (value << 8) + (uint32_t) data [i];
  }

  return value;
}

static inline uint64_t rtems_uint64_from_big_endian( const uint8_t *data)
{
  uint64_t value = 0;
  int i;

  for (i = 0; i < 8; ++i) {
    value = (value << 8) + (uint64_t) data [i];
  }

  return value;
}

static inline void rtems_uint16_to_little_endian( uint16_t value, uint8_t *data)
{
  int i;

  for (i = 0; i < 2; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint32_to_little_endian( uint32_t value, uint8_t *data)
{
  int i;

  for (i = 0; i < 4; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint64_to_little_endian( uint64_t value, uint8_t *data)
{
  int i;

  for (i = 0; i < 8; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint16_to_big_endian( uint16_t value, uint8_t *data)
{
  int i;

  for (i = 1; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint32_to_big_endian( uint32_t value, uint8_t *data)
{
  int i;

  for (i = 3; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint64_to_big_endian( uint64_t value, uint8_t *data)
{
  int i;

  for (i = 7; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

#endif /* _RTEMS_ENDIAN_H */
