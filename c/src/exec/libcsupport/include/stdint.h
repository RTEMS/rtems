/* 
 * stdint.h
 *
 * ISO C99 integer types
 *
 * $Id$
 */

#ifndef __STDINT_H
#define __STDINT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
typedef long int		int64_t;

typedef unsigned char 		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long int	uint64_t;

#ifdef __cplusplus
}
#endif

#endif
