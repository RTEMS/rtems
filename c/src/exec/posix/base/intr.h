/* intr.h
 *
 *  XXX: It is unclear if the type "intr_t" should be defined when
 *       _POSIX_INTERRUPT_CONTROL is not.
 */

#ifndef __POSIX_INTERRUPTS_h
#define __POSIX_INTERRUPTS_h

#include <rtems/posix/features.h>
#include <sys/types.h>
#include <sys/time.h>

#if defined(_POSIX_INTERRUPT_CONTROL)

/*
 *  22.2 Concepts, P1003.4b/D8, p. 73
 */

typedef int intr_t;

/*
 *  22.3.1 Associate a User-Written ISR with an Interrupt, P1003.4b/D8, p. 74
 */

/*
 *  Return codes from an interrupt handler
 */

#define INTR_HANDLED_NOTIFY         0 /* ISR handled this interrupt, notify */
                                      /*   the thread that registered the */
                                      /*   ISR that the interrupt occurred. */
#define INTR_HANDLED_DO_NOT_NOTIFY  1 /* ISR handled this interrupt, but */
                                      /*   do NOT perform notification. */
#define INTR_NOT_HANDLED            2 /* ISR did not handle this interrupt, */
                                      /*   let the next handler try. */

int intr_capture(
  intr_t          intr,
  int           (*intr_handler)( void *area ),
  volatile void   *area,
  size_t           areasize
);

int intr_release(
  intr_t    intr,
  int     (*intr_handler)( void *area )
);

int intr_lock(
  intr_t  intr
);
  
int intr_unlock(
  intr_t  intr
);
  
/*
 *  22.3.2 Await Interrupt Notification, P1003.4b/D8, p. 76
 */

int intr_timed_wait(
  int                     flags,
  const struct timespec  *timeout
);

#endif

#endif
/* end of include file */
