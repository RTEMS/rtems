/*-------------------------------------*/
/* rommon.h                            */
/* Last change : 23. 1.95              */
/*-------------------------------------*/
#ifndef _ROMMON_H_
#define _ROMMON_H_

  /* ROM monitor start point. 
   * Gets control on power on.
   */
extern void romStart(void);
extern void start(void);
  /* ROM monitor start point.
   * Gets control on a fault.
   */ 
extern void romFaultStart(void);
  /* ROM monitor start point.
   * Gets control on a test command.
   */ 
extern void romTestStart(void);

#endif
/*-------------*/
/* End of file */
/*-------------*/

