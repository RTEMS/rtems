/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsTaskMode class.

  This class allows the user to query or change the mode of an RTEMS
  task.

  This object only operates on the currently executing task.

  The standard flags defined in RTEMS are used.

  Methods are provided to operate on a group of modes which are
  required to be changed in a single operation. The mode and mask is
  specified by ORing the required flags.

  Methods are provided for accessing and controlling a specific
  mode. The returned value will only contain the requested mode's flags,
  and only the that mode will be changed when setting a mode.

  ------------------------------------------------------------------------ */

#if !defined(_rtemsTaskMode_h_)
#define _rtemsTaskMode_h_

#include <rtems++/rtemsStatusCode.h>

/* ----
    rtemsTaskMode
*/

class rtemsTaskMode
  : public rtemsStatusCode
{
public:

  rtemsTaskMode() {};

  // group mode control, OR the values together
  inline const rtems_status_code get_mode(rtems_mode& mode);
  inline const rtems_status_code set_mode(const rtems_mode mode,
                                          const rtems_mode mask);
  inline const rtems_status_code set_mode(const rtems_mode mode,
                                          const rtems_mode mask,
                                          rtems_mode& old_mode);

  // preemption control
  inline const rtems_status_code get_preemption_state(rtems_mode& preemption);
  inline const rtems_status_code set_preemption_state(const rtems_mode preemption);
  inline const rtems_status_code set_preemption_state(const rtems_mode preemption,
                                                      rtems_mode& old_preemption);
  inline const bool preemption_set(const rtems_mode preemption);

  // timeslice control
  inline const rtems_status_code get_timeslice_state(rtems_mode& timeslice);
  inline const rtems_status_code set_timeslice_state(const rtems_mode timeslice);
  inline const rtems_status_code set_timeslice_state(const rtems_mode timeslice,
                                                     rtems_mode& old_timeslice);
  inline const bool timeslice_set(const rtems_mode preemption);

  // async-sub-routine control
  inline const rtems_status_code get_asr_state(rtems_mode& asr);
  inline const rtems_status_code set_asr_state(const rtems_mode asr);
  inline const rtems_status_code set_asr_state(const rtems_mode asr,
                                               rtems_mode& old_asr);
  inline const bool asr_set(const rtems_mode preemption);

  // interrupt mask control
  inline const rtems_status_code get_interrupt_level(rtems_interrupt_level& level);
  inline const rtems_status_code set_interrupt_level(const rtems_interrupt_level level);
  inline const rtems_status_code set_interrupt_level(const rtems_interrupt_level level,
                                                     rtems_interrupt_level& old_level);
};

const rtems_status_code rtemsTaskMode::get_mode(rtems_mode& mode)
{
  return set_status_code(rtems_task_mode(0, RTEMS_CURRENT_MODE, &mode));
}

const rtems_status_code rtemsTaskMode::set_mode(const rtems_mode mode,
                                                const rtems_mode mask)
{
  rtems_mode old_mode;
  return set_status_code(rtems_task_mode(mode, mask, &old_mode));
}

const rtems_status_code rtemsTaskMode::set_mode(const rtems_mode mode,
                                                const rtems_mode mask,
                                                rtems_mode& old_mode)
{
  return set_status_code(rtems_task_mode(mode, mask, &old_mode));
}

const rtems_status_code rtemsTaskMode::get_preemption_state(rtems_mode& preemption)
{
  set_status_code(rtems_task_mode(0, RTEMS_CURRENT_MODE, &preemption));
  preemption &= RTEMS_PREEMPT_MASK;
  return last_status_code();
}

const rtems_status_code rtemsTaskMode::set_preemption_state(const rtems_mode preemption)
{
  rtems_mode old_mode;
  return set_status_code(rtems_task_mode(preemption, RTEMS_PREEMPT_MASK, &old_mode));
}

const rtems_status_code rtemsTaskMode::set_preemption_state(const rtems_mode preemption,
                                                       rtems_mode& old_preemption)
{
  set_status_code(rtems_task_mode(preemption, RTEMS_PREEMPT_MASK, &old_preemption));
  old_preemption &= RTEMS_PREEMPT_MASK;
  return last_status_code();
}

const bool rtemsTaskMode::preemption_set(const rtems_mode preemption)
{
  return (preemption & RTEMS_PREEMPT_MASK) ? false : true;
}

const rtems_status_code rtemsTaskMode::get_timeslice_state(rtems_mode& timeslice)
{
  set_status_code(rtems_task_mode(0, RTEMS_CURRENT_MODE, &timeslice));
  timeslice &= RTEMS_TIMESLICE_MASK;
  return last_status_code();
}

const rtems_status_code rtemsTaskMode::set_timeslice_state(const rtems_mode timeslice)
{
  rtems_mode old_mode;
  return set_status_code(rtems_task_mode(timeslice, RTEMS_TIMESLICE_MASK, &old_mode));
}

const rtems_status_code rtemsTaskMode::set_timeslice_state(const rtems_mode timeslice,
                                                           rtems_mode& old_timeslice)
{
  set_status_code(rtems_task_mode(timeslice, RTEMS_TIMESLICE_MASK, &old_timeslice));
  old_timeslice &= RTEMS_TIMESLICE_MASK;
  return last_status_code();
}

const bool rtemsTaskMode::timeslice_set(const rtems_mode timeslice)
{
  return (timeslice & RTEMS_TIMESLICE_MASK) ? true : false;
}

const rtems_status_code rtemsTaskMode::get_asr_state(rtems_mode& asr)
{
  set_status_code(rtems_task_mode(0, RTEMS_CURRENT_MODE, &asr));
  asr &= RTEMS_ASR_MASK;
  return last_status_code();
}

const rtems_status_code rtemsTaskMode::set_asr_state(const rtems_mode asr)
{
  rtems_mode old_mode;
  return set_status_code(rtems_task_mode(asr, RTEMS_ASR_MASK, &old_mode));
}

const rtems_status_code rtemsTaskMode::set_asr_state(const rtems_mode asr,
                                                     rtems_mode& old_asr)
{
  set_status_code(rtems_task_mode(asr, RTEMS_ASR_MASK, &old_asr));
  old_asr &= RTEMS_ASR_MASK;
  return last_status_code();
}

const bool rtemsTaskMode::asr_set(const rtems_mode asr)
{
  return (asr & RTEMS_ASR_MASK) ? true : false;
}

const rtems_status_code rtemsTaskMode::get_interrupt_level(rtems_interrupt_level& level)
{
  rtems_mode mode;
  set_status_code(rtems_task_mode(0, RTEMS_CURRENT_MODE, &mode));
  level = mode & RTEMS_INTERRUPT_MASK;
  return last_status_code();
}

const rtems_status_code rtemsTaskMode::set_interrupt_level(const rtems_interrupt_level level)
{
  rtems_mode old_mode;
  return set_status_code(rtems_task_mode(level, RTEMS_INTERRUPT_MASK, &old_mode));
}

const rtems_status_code rtemsTaskMode::set_interrupt_level(rtems_interrupt_level level,
                                                           rtems_interrupt_level& old_level)
{
  set_status_code(rtems_task_mode(level, RTEMS_INTERRUPT_MASK, &old_level));
  old_level = old_level & RTEMS_INTERRUPT_MASK;
  return last_status_code();
}

#endif  // _rtemsTaskMode_h_
