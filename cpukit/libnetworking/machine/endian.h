/*
 *  $Id$
 */

#ifndef _MACHINE_ENDIAN_H_
#define _MACHINE_ENDIAN_H_

#include <rtems.h>

/*
 * BSD-style endian declaration
 */
#define BIG_ENDIAN	4321
#define LITTLE_ENDIAN	1234

#if CPU_BIG_ENDIAN
# define BYTE_ORDER BIG_ENDIAN
#elif CPU_LITTLE_ENDIAN
# define BYTE_ORDER LITTLE_ENDIAN
#else
# error "Can't decide which end is which!"
#endif

#if ( CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES == FALSE )

#if ( CPU_BIG_ENDIAN == TRUE )

/*
 *  Very simply on big endian CPUs
 */

#define       ntohl(_x)        (_x)
#define       ntohs(_x)        (_x)
#define       htonl(_x)        (_x)
#define       htons(_x)        (_x)

#define NTOHS(x)
#define HTONS(x)
#define NTOHL(x)
#define HTONL(x)

#elif ( CPU_LITTLE_ENDIAN == TRUE )

/*
 *  A little more complicated on little endian CPUs
 */

#define       ntohl(_x)        ((long)  CPU_swap_u32((unsigned32)_x))
#define       ntohs(_x)        ((short) CPU_swap_u16((unsigned16)_x))
#define       htonl(_x)        ((long)  CPU_swap_u32((unsigned32)_x))
#define       htons(_x)        ((short) CPU_swap_u16((unsigned16)_x))

#define NTOHS(x) (x) = ntohs(x)
#define HTONS(x) (x) = htons(x)
#define NTOHL(x) (x) = ntohl(x)
#define HTONL(x) (x) = htonl(x)

#else
#error "Unknown endian-ness for this cpu"
#endif

#endif  /* CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES */

#endif /* _MACHINE_ENDIAN_H_ */
