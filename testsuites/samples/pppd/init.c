
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/rtems_bsdnet.h>
#include <rtemspppd.h>

#define CONFIGURE_INIT
#include "system.h"
#include "netconfig.h"


extern int pppdapp_initialize(void);


rtems_task Init(rtems_task_argument argument)
{
  /* initialize network */
  rtems_bsdnet_initialize_network();
  rtems_pppd_initialize();
  pppdapp_initialize();

  rtems_task_delete(RTEMS_SELF);
}
