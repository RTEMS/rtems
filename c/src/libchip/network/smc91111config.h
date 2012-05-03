#ifndef _SMC91111_CONFIG_H_
#define _SMC91111_CONFIG_H_

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT	RTEMS_EVENT_2

 /* event to send when tx buffers become available */
#define SMC91111_TX_WAIT_EVENT  RTEMS_EVENT_3


/* Number of OCs supported by this driver*/
#define NOCDRIVER	1

/* Receive buffer size -- Allow for a full ethernet packet including CRC */
#define RBUF_SIZE	1536

#define	ET_MINLEN 64		/* minimum message length */

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/* ----------------- cygdriver params ----------------- */

#define LAN91CXX_32BIT_RX
#define LAN91CXX_IS_LAN91C111

/* ----------------- compat layer ----------------- */

#include <stdint.h>

typedef uint32_t  CYG_WORD;
typedef uint8_t   CYG_BYTE;
typedef uint16_t  CYG_WORD16;
typedef uint32_t  CYG_WORD32;

#ifndef CYG_SWAP16
# define CYG_SWAP16(_x_)                                        \
    ({ uint16_t _x = (_x_); ((_x << 8) | (_x >> 8)); })
#endif

#ifndef CYG_SWAP32
# define CYG_SWAP32(_x_)                        \
    ({ uint32_t _x = (_x_);                   \
       ((_x << 24) |                            \
       ((0x0000FF00UL & _x) <<  8) |            \
       ((0x00FF0000UL & _x) >>  8) |            \
       (_x  >> 24)); })
#endif

# define CYG_CPU_TO_BE16(_x_) (_x_)
# define CYG_CPU_TO_BE32(_x_) (_x_)
# define CYG_BE16_TO_CPU(_x_) (_x_)
# define CYG_BE32_TO_CPU(_x_) (_x_)

# define CYG_CPU_TO_LE16(_x_) CYG_SWAP16((_x_))
# define CYG_CPU_TO_LE32(_x_) CYG_SWAP32((_x_))
# define CYG_LE16_TO_CPU(_x_) CYG_SWAP16((_x_))
# define CYG_LE32_TO_CPU(_x_) CYG_SWAP32((_x_))

#define CYG_MACRO_START do {
#define CYG_MACRO_END   } while (0)
#define HAL_IO_BARRIER()                        \
    __asm__ volatile ( "" : : : "memory" )

#define HAL_READ_UINT8( _register_, _value_ )           \
    CYG_MACRO_START                                     \
    ((_value_) = *((volatile CYG_BYTE *)(_register_))); \
    HAL_IO_BARRIER ();                                  \
    CYG_MACRO_END

#define HAL_WRITE_UINT8( _register_, _value_ )          \
    CYG_MACRO_START                                     \
    (*((volatile CYG_BYTE *)(_register_)) = (_value_)); \
    HAL_IO_BARRIER ();                                  \
    CYG_MACRO_END

#define HAL_READ_UINT16( _register_, _value_ )                  \
    CYG_MACRO_START                                             \
    ((_value_) = *((volatile CYG_WORD16 *)(_register_)));       \
    HAL_IO_BARRIER ();                                          \
    CYG_MACRO_END

#define HAL_WRITE_UINT16( _register_, _value_ )                 \
    CYG_MACRO_START                                             \
    (*((volatile CYG_WORD16 *)(_register_)) = (_value_));       \
    HAL_IO_BARRIER ();                                          \
    CYG_MACRO_END

#define HAL_READ_UINT32( _register_, _value_ )                  \
    CYG_MACRO_START                                             \
    ((_value_) = *((volatile CYG_WORD32 *)(_register_)));       \
    HAL_IO_BARRIER ();                                          \
    CYG_MACRO_END

#define HAL_READ_UINT16( _register_, _value_ )                  \
    CYG_MACRO_START                                             \
    ((_value_) = *((volatile CYG_WORD16 *)(_register_)));       \
    HAL_IO_BARRIER ();                                          \
    CYG_MACRO_END

#define CYG_ASSERT(c,p) do { if (!(c)) { while(1) { printf(p);} }; } while(0)

#define HAL_DELAY_US(p) rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (p))


#endif  /* _SMC_91111_CONFIG_H_ */


