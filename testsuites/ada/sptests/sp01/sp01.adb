--
--  MAIN / BODY
--
--  DESCRIPTION:
--
--  This is the entry point for Test SP01 of the Single Processor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.org/license/LICENSE.
--

with RTEMS;
with RTEMS.TASKS;
with SPTEST;
with TEST_SUPPORT;
with TEXT_IO;
with System.OS_Interface;

procedure SP01 is
  INIT_ID : RTEMS.ID;
  STATUS  : RTEMS.STATUS_CODES;
  obj_clockid_t : System.OS_Interface.clockid_t;
  pragma Warnings (Off, obj_clockid_t);
  obj_pid_t : System.OS_Interface.pid_t;
  pragma Warnings (Off, obj_pid_t);
  obj_pthread_attr_t : System.OS_Interface.pthread_attr_t;
  pragma Warnings (Off, obj_pthread_attr_t);
  obj_pthread_condattr_t : System.OS_Interface.pthread_condattr_t;
  pragma Warnings (Off, obj_pthread_condattr_t);
  obj_pthread_cond_t : System.OS_Interface.pthread_cond_t;
  pragma Warnings (Off, obj_pthread_cond_t);
  obj_pthread_key_t : System.OS_Interface.pthread_key_t;
  pragma Warnings (Off, obj_pthread_key_t);
  obj_pthread_mutexattr_t : System.OS_Interface.pthread_mutexattr_t;
  pragma Warnings (Off, obj_pthread_mutexattr_t);
  obj_pthread_mutex_t : System.OS_Interface.pthread_mutex_t;
  pragma Warnings (Off, obj_pthread_mutex_t);
  obj_pthread_rwlockattr_t : System.OS_Interface.pthread_rwlockattr_t;
  pragma Warnings (Off, obj_pthread_rwlockattr_t);
  obj_pthread_rwlock_t : System.OS_Interface.pthread_rwlock_t;
  pragma Warnings (Off, obj_pthread_rwlock_t);
  obj_pthread_t : System.OS_Interface.pthread_t;
  pragma Warnings (Off, obj_pthread_t);
  obj_rtems_id : System.OS_Interface.rtems_id;
  pragma Warnings (Off, obj_rtems_id);
  obj_sigset_t : System.OS_Interface.sigset_t;
  pragma Warnings (Off, obj_sigset_t);
  obj_stack_t : System.OS_Interface.stack_t;
  pragma Warnings (Off, obj_stack_t);
  obj_struct_sched_param : System.OS_Interface.struct_sched_param;
  pragma Warnings (Off, obj_struct_sched_param);
  obj_struct_sigaction : System.OS_Interface.struct_sigaction;
  pragma Warnings (Off, obj_struct_sigaction);
  obj_timespec : System.OS_Interface.timespec;
  pragma Warnings (Off, obj_timespec);
begin

   TEXT_IO.NEW_LINE( 2 );
   TEST_SUPPORT.ADA_TEST_BEGIN;

   TEST_SUPPORT.Check_Type(
      0,
      obj_clockid_t'Size,
      obj_clockid_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      1,
      obj_pid_t'Size,
      obj_pid_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      2,
      obj_pthread_attr_t'Size,
      obj_pthread_attr_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      3,
      obj_pthread_condattr_t'Size,
      obj_pthread_condattr_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      4,
      obj_pthread_cond_t'Size,
      obj_pthread_cond_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      5,
      obj_pthread_key_t'Size,
      obj_pthread_key_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      6,
      obj_pthread_mutexattr_t'Size,
      obj_pthread_mutexattr_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      7,
      obj_pthread_mutex_t'Size,
      obj_pthread_mutex_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      8,
      obj_pthread_rwlockattr_t'Size,
      obj_pthread_rwlockattr_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      9,
      obj_pthread_rwlock_t'Size,
      obj_pthread_rwlock_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      10,
      obj_pthread_t'Size,
      obj_pthread_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      11,
      obj_rtems_id'Size,
      obj_rtems_id'Alignment
   );
   TEST_SUPPORT.Check_Type(
      12,
      obj_sigset_t'Size,
      obj_sigset_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      13,
      obj_stack_t'Size,
      obj_stack_t'Alignment
   );
   TEST_SUPPORT.Check_Type(
      14,
      obj_struct_sched_param'Size,
      obj_struct_sched_param'Alignment
   );
   TEST_SUPPORT.Check_Type(
      15,
      obj_struct_sigaction'Size,
      obj_struct_sigaction'Alignment
   );
   TEST_SUPPORT.Check_Type(
      16,
      obj_timespec'Size,
      obj_timespec'Alignment
   );

   RTEMS.TASKS.CREATE(
      RTEMS.BUILD_NAME(  'I', 'N', 'I', 'T' ),
      1,
      RTEMS.MINIMUM_STACK_SIZE,
      RTEMS.NO_PREEMPT,
      RTEMS.DEFAULT_ATTRIBUTES,
      INIT_ID,
      STATUS
   );
   TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF INIT" );


   RTEMS.TASKS.START(
      INIT_ID,
      SPTEST.INIT'ACCESS,
      0,
      STATUS
   );
   TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF INIT" );

   loop
      delay 120.0;
   end loop;

end SP01;

