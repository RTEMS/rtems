/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/itron/time.h>

/*
 *  set_tim - Set System Clock
 */

void converTime(SYSTIME ms, TOD_Control *time)
{
  int millisecond;

/* XXX need to dereference structure */
#if 0
  milisecond = ms % 1000;
  tempSecond = ms / 1000;
#else
   millisecond = 0;
#endif
}
   

/*
 *  set_tim - Set System Clock
 */

ER set_tim(
  SYSTIME *pk_tim
)
{
 Watchdog_Interval temp; /* an integer in seconds after conversion from SYSTIME */
 TOD_Control the_tod;
 
/* convert *pk_tim which is 48 bits integer in binary into an ordinary
 integer in milliseconds */
 
/* XXX */ temp = 0;
  if(temp > 0) {
    _Thread_Disable_dispatch();
      _TOD_Set(&the_tod, temp);
    _Thread_Enable_dispatch();
      if(_TOD_Validate(&the_tod)){
      return E_OK;
      }
  }
  return E_SYS;
}

/*
 *  get_tim - Get System Clock
 */

ER get_tim(
  SYSTIME *pk_tim
)
{
  return E_OK;
}

/*
 *  dly_tsk - Delay Task
 */

ER dly_tsk(
  DLYTIME dlytim
)
{
  Watchdog_Interval ticks;

  ticks = TOD_MILLISECONDS_TO_TICKS(dlytim);

  _Thread_Disable_dispatch();
    if ( ticks == 0 ) {
      _Thread_Yield_processor();
    } else {
      _Thread_Set_state( _Thread_Executing, STATES_DELAYING );
      _Watchdog_Initialize(
        &_Thread_Executing->Timer,
        _Thread_Delay_ended,
        _Thread_Executing->Object.id,
        NULL
      );
      _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
    }
  _Thread_Enable_dispatch();
  return E_OK;
}

/*
 *  def_cyc - Define Cyclic Handler
 */

ER def_cyc(
  HNO     cycno,
  T_DCYC *pk_dcyc
)
{
  Watchdog_Control *object;
#if 0
  Watchdog_Interval timeInterval;
#endif
  
/* XXX */ object = 0;
  if ( pk_dcyc->cycatr != TA_HLNG)
     return E_RSATR;
#if 0
  timeInterval = pk_dcyc->cyctim / 1000;
  object->delta_interval = timeInterval;
#endif
  _Watchdog_Initialize(
         object,
         pk_dcyc->cychdr,
         cycno,
         pk_dcyc->exinf);
  _Watchdog_Deactivate(object);
               
  return E_OK;
}

/*
 *  act_cyc - Activate Cyclic Handler
 */

ER act_cyc(
  HNO   cycno,
  UINT  cycact
)
{
  Watchdog_Control *object;
   
  if(cycact != TCY_OFF || cycact != TCY_ON)
     return E_PAR;

#if 0
  if( object->Object_ID != cycno)
     return E_NOEXS;
#endif
  
  _Watchdog_Activate(object);
         
  return E_OK;
}

/*
 *  ref_cyc - Reference Cyclic Handler Status
 */

ER ref_cyc(
  T_RCYC *pk_rcyc,
  HNO     cycno
)
{
#if 0
  int timeElapse_since_actCyclic;
#endif
  T_DCYC *pk_dcyc;
#if 0
  Watchdog_Control *object;
#endif

/* XXX */ pk_dcyc = 0; 
  
/* XXX will to use a "get" routine to map from id to object pointer */
/* XXX and the object pointer should be of type specific to this manager */
#if 0
  if( object->Object_ID == cycno))
#else
  if ( 1 )
#endif
  {
     pk_rcyc->exinf = pk_dcyc->exinf;
     pk_rcyc->cycact = pk_dcyc->cycact;
#if 0
     pk_rcyc->lfttim = pk_dcyc->cyctim - timeElapse_since_actCyclic;
#endif
     
     return E_OK;
  }
  else
     return E_NOEXS;   
}

/*
 *  def_alm - Define Alarm Handler
 */

ER def_alm(
  HNO     almno,
  T_DALM *pk_dalm
)
{  
#if 0
  Objects_Control *objectAlrm;
  Watchdog_Interval timeInterval;
  
  timeInterval = pk_dalm->almtim / 1000;
  (void) _Watchdog_Remove(&objectAlrm);
  _Watchdog_Initialize(
           objectAlrm,
           pk_dalm->almhdr,
           almno,
           pk_dalm->exinf);
  _Watchdong_Insert_seconds(objectAlrm,timeInterval);
  _Thread_Enable_dispatch();
#endif
              
  return E_OK;
}

/*
 *  ref_alm - Reference Alarm Handler Status
 */

ER ref_alm(
  T_RALM *pk_ralm,
  HNO     almno
)
{
   
  return E_OK;
}

/*
 *  ret_tmr - Return from Timer Handler
 */

void ret_tmr( void )
{
}

