/*
 *  $Id$
 */

#ifndef _SMC91111_EXP_H_
#define _SMC91111_EXP_H_

typedef struct scmv91111_configuration {
  void                     *baseaddr;
  unsigned int              vector;
  unsigned int              pio;
  unsigned int              ctl_rspeed;
  unsigned int              ctl_rfduplx;
  unsigned int              ctl_autoneg;
#ifndef _OLD_EXCEPTIONS
  /* New arguments for the Interrupt Manager Extension:
   */
  const char *              info;
  rtems_option              options;
  rtems_interrupt_handler   interrupt_wrapper;
  void *                    arg;
#endif
} scmv91111_configuration_t;

#endif  /* _SMC_91111_EXP_H_ */


