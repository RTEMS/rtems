/*
 *  Description:
 *      HPPA fast spinlock semaphores based on LDCWX instruction.
 *      These semaphores are not known to RTEMS.
 *
 *  TODO:
 *      Put node number in high 16 bits of flag??
 *      XXX: Need h_s_deallocate
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>

#include <hppa1.1/semaphore.h>

/*
 * Report fatal semaphore error
 */

#define SEM_FATAL_ERROR(sp)  rtems_fatal_error_occurred((rtems_unsigned32) sp)

#define SEM_CHECK(sp)   do { \
                              if (((sp) == 0) || (int) (sp) & 0xf) \
                              { \
                                  SEM_FATAL_ERROR(sp); \
                              } \
                         } while (0)

/*
 * Init a semaphore to be free
 */

#define SEM_FREE_INIT(sp) \
    do { \
        (sp)->lock = 1; \
        (sp)->flags = 0; \
        (sp)->owner_tcb  = 0; \
    } while (0)

/*
 * Grab a semaphore recording its owner.
 */

#define SEM_MARK_GRABBED(sp) \
    do { \
        (sp)->owner_tcb  = _Thread_Executing; \
    } while (0)

/*
 * Mark the semaphore busy
 */

#define SEM_MARK_BUSY(sp)    ((sp)->flags |= HPPA_SEM_IN_USE)

/*
 * Is a semaphore available?
 */

#define SEM_IS_AVAILABLE(sp)  ((sp)->owner_tcb == 0)

/*
 * The pool control semaphore is the first in the pool
 */

#define SEM_CONTROL  (&hppa_semaphore_pool[0])
#define SEM_FIRST    (&hppa_semaphore_pool[1])

#define SEM_PRIVATE(cookie)  rtems_interrupt_disable(cookie)

#define SEM_PUBLIC(cookie)   rtems_interrupt_enable(cookie)


/*
 * Control variables for the pool
 */

hppa_semaphore_t *hppa_semaphore_pool;        /* ptr to first */
int               hppa_semaphores;
int               hppa_semaphores_available;

void
hppa_semaphore_pool_initialize(void *pool_base,
                               int   pool_size)
{
    hppa_semaphore_t *sp;
    int align_factor;
    rtems_unsigned32 isr_level;

    /*
     * round pool_base up to be a multiple of SEM_ALIGN
     */

    align_factor = SEM_ALIGN - (((int) pool_base) & (SEM_ALIGN-1));
    if (align_factor != SEM_ALIGN)
    {
        pool_base += align_factor;
        pool_size -= align_factor;
    }

    /*
     * How many can the pool hold?
     * Assumes the semaphores are SEM_ALIGN bytes each
     */

    if (sizeof(hppa_semaphore_t) != SEM_ALIGN)
        rtems_fatal_error_occurred(RTEMS_INVALID_SIZE);

    pool_size &= ~(SEM_ALIGN - 1);

    SEM_PRIVATE(isr_level);

    hppa_semaphore_pool = pool_base;
    hppa_semaphores = pool_size / SEM_ALIGN;

    /*
     * If we are node0, then init all in the pool
     */

#if 0
    if (cpu_number == 0)
#else
    if (_Configuration_Table->User_multiprocessing_table->node == 1)
#endif
    {
        /*
         * Tell other cpus we are not done, jic
         */
        SEM_CONTROL->user = rtems_build_name('!', 'D', 'N', 'E');

        for (sp=SEM_FIRST; sp < &hppa_semaphore_pool[hppa_semaphores]; sp++)
            SEM_FREE_INIT(sp);
        SEM_FREE_INIT(SEM_CONTROL);
    }

    /*
     * Tell other cpus we are done, or wait for it to be done if on another cpu
     */

#if 0
    if (cpu_number == 0)
#else
    if (_Configuration_Table->User_multiprocessing_table->node == 1)
#endif
        SEM_CONTROL->user = rtems_build_name('D', 'O', 'N', 'E');
    else
        while (SEM_CONTROL->user != rtems_build_name('D', 'O', 'N', 'E'))
            ;

    hppa_semaphores_available = hppa_semaphores;

    SEM_PUBLIC(isr_level);
}

/*
 *  Function:   hppa_semaphore_acquire
 *  Created:    94/11/29
 *  RespEngr:   tony bennett
 *
 *  Description:
 *      Acquire a semaphore.   Will spin on the semaphore unless
 *      'flag' says not to.
 *
 *  Parameters:
 *
 *
 *  Returns:
 *      0        -- if did not acquire
 *      non-zero -- if acquired semaphore
 *                      (actually this is the spin count)
 *
 *  Notes:
 *      There is no requirement that the semaphore be within the pool
 *
 *  Deficiencies/ToDo:
 *
 */


rtems_unsigned32
hppa_semaphore_acquire(hppa_semaphore_t *sp,
                       int               flag)
{
    rtems_unsigned32 lock_value;
    rtems_unsigned32 spin_count = 1;

    SEM_CHECK(sp);

    for (;;)
    {
        HPPA_ASM_LDCWS(0, 0, sp, lock_value);

        if (lock_value)         /* we now own the lock */
        {
            SEM_MARK_GRABBED(sp);
            return spin_count ? spin_count : ~0;        /* jic */
        }

        if (flag & HPPA_SEM_NO_SPIN)
            return 0;

        spin_count++;
    }
}

void
hppa_semaphore_release(hppa_semaphore_t *sp)
{
    SEM_CHECK(sp);

    if (sp->owner_tcb != _Thread_Executing)
        SEM_FATAL_ERROR("owner mismatch");

    sp->lock = 1;
}


/*
 *  Function:   hppa_semaphore_allocate
 *  Created:    94/11/29
 *  RespEngr:   tony bennett
 *
 *  Description:
 *      Get a pointer to a semaphore.
 *
 *  Parameters:
 *      which -- if 0, then allocate a free semaphore from the pool
 *               if non-zero, then return pointer to that one, even
 *                  if it is already busy.
 *
 *  Returns:
 *      successful -- pointer to semaphore
 *                    NULL otherwise
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *
 *
 */

hppa_semaphore_t *
hppa_semaphore_allocate(rtems_unsigned32 which,
                        int              flag)
{
    hppa_semaphore_t *sp = 0;

    /*
     * grab the control semaphore
     */

    if (hppa_semaphore_acquire(SEM_CONTROL, 0) == 0)
        SEM_FATAL_ERROR("could not grab control semaphore");

    /*
     * Find a free one and init it
     */

    if (which)
    {
        if (which >= hppa_semaphores)
            SEM_FATAL_ERROR("requested non-existent semaphore");
        sp = &hppa_semaphore_pool[which];

        /*
         * if it is "free", then mark it claimed now.
         * If it is not free then we are done.
         */

        if (SEM_IS_AVAILABLE(sp))
            goto allmine;
    }
    else for (sp = SEM_FIRST;
              sp < &hppa_semaphore_pool[hppa_semaphores];
              sp++)
    {
        if (SEM_IS_AVAILABLE(sp))
        {
allmine:    SEM_FREE_INIT(sp);
            SEM_MARK_BUSY(sp);
            if ( ! (flag & HPPA_SEM_INITIALLY_FREE))
                SEM_MARK_GRABBED(sp);
            break;
        }
    }

    /*
     * Free up the control semaphore
     */

    hppa_semaphore_release(SEM_CONTROL);

    return sp;
}

