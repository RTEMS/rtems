/*
 * RTEMS Monitor semaphore support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include "monitor.h"
#include <rtems/rtems/attr.inl>
#include <stdio.h>
#include <string.h>    /* memcpy() */

void
rtems_monitor_sema_canonical(
    rtems_monitor_sema_t  *canonical_sema,
    void                  *sema_void
)
{
    Semaphore_Control       *rtems_sema = (Semaphore_Control *) sema_void;

    canonical_sema->attribute = rtems_sema->attribute_set;
    canonical_sema->priority_ceiling =
      rtems_sema->Core_control.mutex.Attributes.priority_ceiling;

    canonical_sema->holder_id        =
      rtems_sema->Core_control.mutex.holder_id;

    if (_Attributes_Is_counting_semaphore(canonical_sema->attribute)) {
      /* we have a counting semaphore */
      canonical_sema->cur_count  =
	rtems_sema->Core_control.semaphore.count;

      canonical_sema->max_count  =
	rtems_sema->Core_control.semaphore.Attributes.maximum_count;
    }
    else {
      /* we have a binary semaphore (mutex) */
      canonical_sema->cur_count        = rtems_sema->Core_control.mutex.lock;
      canonical_sema->max_count        = 1; /* mutex is either 0 or 1 */
    }
}


void
rtems_monitor_sema_dump_header(
    bool verbose __attribute__((unused))
)
{
    printf("\
  ID       NAME   ATTR        PRICEIL CURR_CNT HOLDID \n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 1234
          1         2         3         4         5         6         7    */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_sema_dump(
    rtems_monitor_sema_t *monitor_sema,
    bool  verbose __attribute__((unused))
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_sema->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_sema->id);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_attributes(monitor_sema->attribute);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_priority(monitor_sema->priority_ceiling);
    length += rtems_monitor_pad(38, length);
    length += rtems_monitor_dump_decimal(monitor_sema->cur_count);
    length += rtems_monitor_pad(47, length);
    length += rtems_monitor_dump_id(monitor_sema->holder_id);
    printf("\n");
}
