/*
 *  Stack Overflow Check User Extension Set
 *
 *  NOTE:  This extension set automatically determines at
 *         initialization time whether the stack for this
 *         CPU grows up or down and installs the correct
 *         extension routines for that direction.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <inttypes.h>

/*
 * The stack dump information may be printed by a "fatal" extension.
 * Fatal extensions only get called via rtems_fatal_error_occurred()
 * and not when rtems_shutdown_executive() is called.
 * When that happens, this #define should be deleted and all the code
 * it marks.
 */
#define DONT_USE_FATAL_EXTENSION

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <rtems/bspIo.h>
#include <rtems/stackchk.h>
#include "internal.h"

/*
 *  Variable to indicate when the stack checker has been initialized.
 */
static int   Stack_check_Initialized = 0;

/*
 *  The "magic pattern" used to mark the end of the stack.
 */
Stack_check_Control Stack_check_Pattern;

/*
 * Helper function to report if the actual stack pointer is in range.
 *
 * NOTE: This uses a GCC specific method.
 */
static inline boolean Stack_check_Frame_pointer_in_range(
  Stack_Control *the_stack
)
{
  void *sp = __builtin_frame_address(0);

  #if defined(__GNUC__)
    if ( sp < the_stack->area ) {
      printk( "Stack Pointer Too Low!\n" );
      return FALSE;
    }
    if ( sp > (the_stack->area + the_stack->size) ) {
      printk( "Stack Pointer Too High!\n" );
      return FALSE;
    }
  #else
    #error "How do I check stack bounds on a non-GNU compiler?"
  #endif
  return TRUE;
}

/*
 *  Where the pattern goes in the stack area is dependent upon
 *  whether the stack grow to the high or low area of the memory.
 */
#if (CPU_STACK_GROWS_UP == TRUE)
  #define Stack_check_Get_pattern_area( _the_stack ) \
    ((Stack_check_Control *) ((char *)(_the_stack)->area + \
         (_the_stack)->size - sizeof( Stack_check_Control ) ))

  #define Stack_check_Calculate_used( _low, _size, _high_water ) \
      ((char *)(_high_water) - (char *)(_low))

  #define Stack_check_usable_stack_start(_the_stack) \
    ((_the_stack)->area)

#else
  #define Stack_check_Get_pattern_area( _the_stack ) \
    ((Stack_check_Control *) ((char *)(_the_stack)->area + HEAP_OVERHEAD))

  #define Stack_check_Calculate_used( _low, _size, _high_water) \
      ( ((char *)(_low) + (_size)) - (char *)(_high_water) )

  #define Stack_check_usable_stack_start(_the_stack) \
      ((char *)(_the_stack)->area + sizeof(Stack_check_Control))

#endif

/*
 *  The assumption is that if the pattern gets overwritten, the task
 *  is too close.  This defines the usable stack memory.
 */
#define Stack_check_usable_stack_size(_the_stack) \
    ((_the_stack)->size - sizeof(Stack_check_Control))

/*
 * Do we have an interrupt stack?
 * XXX it would sure be nice if the interrupt stack were also
 *     stored in a "stack" structure!
 */
Stack_Control Stack_check_Interrupt_stack;

/*
 *  Fill an entire stack area with BYTE_PATTERN.  This will be used
 *  to check for amount of actual stack used.
 */
#define Stack_check_Dope_stack(_stack) \
  memset((_stack)->area, BYTE_PATTERN, (_stack)->size)

/*
 *  Stack_check_Initialize
 */
void Stack_check_Initialize( void )
{
  uint32_t *p;

  if (Stack_check_Initialized)
    return;

  /*
   * Dope the pattern and fill areas
   */

  for ( p = Stack_check_Pattern.pattern;
        p < &Stack_check_Pattern.pattern[PATTERN_SIZE_WORDS];
        p += 4
      ) {
      p[0] = 0xFEEDF00D;          /* FEED FOOD to BAD DOG */
      p[1] = 0x0BAD0D06;
      p[2] = 0xDEADF00D;          /* DEAD FOOD GOOD DOG */
      p[3] = 0x600D0D06;
  }
  
  /*
   * If appropriate, setup the interrupt stack for high water testing
   * also.
   */
  #if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE)
    if (_CPU_Interrupt_stack_low && _CPU_Interrupt_stack_high) {
      Stack_check_Interrupt_stack.area = _CPU_Interrupt_stack_low;
      Stack_check_Interrupt_stack.size = (char *) _CPU_Interrupt_stack_high -
                                  (char *) _CPU_Interrupt_stack_low;
      Stack_check_Dope_stack(&Stack_check_Interrupt_stack);
  }
  #endif

  Stack_check_Initialized = 1;
}

/*
 *  rtems_stack_checker_create_extension
 */
boolean rtems_stack_checker_create_extension(
  Thread_Control *running,
  Thread_Control *the_thread
)
{
  Stack_check_Initialize();

  if (the_thread)
    Stack_check_Dope_stack(&the_thread->Start.Initial_stack);

  return TRUE;
}

/*
 *  rtems_stack_checker_Begin_extension
 */
void rtems_stack_checker_begin_extension(
  Thread_Control *the_thread
)
{
  Stack_check_Control  *the_pattern;

  if ( the_thread->Object.id == 0 )        /* skip system tasks */
    return;

  the_pattern = Stack_check_Get_pattern_area(&the_thread->Start.Initial_stack);

  *the_pattern = Stack_check_Pattern;
}

/*
 *  Stack_check_report_blown_task
 *
 *  Report a blown stack.  Needs to be a separate routine
 *  so that interrupt handlers can use this too.
 *
 *  NOTE: The system is in a questionable state... we may not get
 *        the following message out.
 */
void Stack_check_report_blown_task(
  Thread_Control *running,
  boolean         pattern_ok
)
{
  Stack_Control *stack = &running->Start.Initial_stack;

  printk(
    "BLOWN STACK!!! Offending task(%p): id=0x%08" PRIx32 "; name=0x%08" PRIx32,
    running,
    running->Object.id,
    (uint32_t) running->Object.name
  );

  #if defined(RTEMS_MULTIPROCESSING)
    if (rtems_configuration_get_user_multiprocessing_table()) {
      printk(
        "; node=%d\n",
        rtems_configuration_get_user_multiprocessing_table()->node
      );
    }
  #else
      printk( "\n" );
  #endif

  printk(
    "  stack covers range %p - %p (%d bytes)\n",
    stack->area,
    stack->area + stack->size - 1,
    stack->size
  );

  if ( !pattern_ok ) {
    printk(
      "  Damaged pattern begins at 0x%08lx and is %ld bytes long\n",
      (unsigned long) Stack_check_Get_pattern_area(stack),
      (long) PATTERN_SIZE_BYTES);
  }

  rtems_fatal_error_occurred( 0x81 );
}

/*
 *  rtems_stack_checker_switch_extension
 */
void rtems_stack_checker_switch_extension(
  Thread_Control *running,
  Thread_Control *heir
)
{
  Stack_Control *the_stack = &running->Start.Initial_stack;
  void          *pattern;
  boolean        sp_ok;
  boolean        pattern_ok = TRUE;

  pattern = (void *) Stack_check_Get_pattern_area(the_stack)->pattern;

  /*
   *  Check for an out of bounds stack pointer and then an overwrite
   */

  sp_ok = Stack_check_Frame_pointer_in_range( the_stack );
  pattern_ok = (!memcmp( pattern,
            (void *) Stack_check_Pattern.pattern, PATTERN_SIZE_BYTES));

  if ( !sp_ok || !pattern_ok ) {
    Stack_check_report_blown_task( running, pattern_ok );
  }
}

/*
 *  Check if blown
 */
boolean rtems_stack_checker_is_blown( void )
{
  Stack_Control *the_stack = &_Thread_Executing->Start.Initial_stack;
  void          *pattern;
  boolean        sp_ok;
  boolean        pattern_ok = TRUE;

  pattern = (void *) Stack_check_Get_pattern_area(the_stack)->pattern;

  /*
   *  Check for an out of bounds stack pointer and then an overwrite
   */

  sp_ok = Stack_check_Frame_pointer_in_range( the_stack );
  pattern_ok = (!memcmp( pattern,
            (void *) Stack_check_Pattern.pattern, PATTERN_SIZE_BYTES));

  if ( !sp_ok || !pattern_ok ) {
    return TRUE;
  }
  return FALSE;
}

/*
 * Stack_check_find_high_water_mark
 */
void *Stack_check_find_high_water_mark(
  const void *s,
  size_t      n
)
{
  const uint32_t   *base, *ebase;
  uint32_t   length;

  base = s;
  length = n/4;

  #if ( CPU_STACK_GROWS_UP == TRUE )
    /*
     * start at higher memory and find first word that does not
     * match pattern
     */

    base += length - 1;
    for (ebase = s; base > ebase; base--)
	if (*base != U32_PATTERN)
	    return (void *) base;
  #else
    /*
     * start at lower memory and find first word that does not
     * match pattern
     */

    base += PATTERN_SIZE_WORDS;
    for (ebase = base + length; base < ebase; base++)
	if (*base != U32_PATTERN)
	    return (void *) base;
  #endif

  return (void *)0;
}

/*
 *  Report Name
 */

char *Stack_check_Get_object_name(
  Objects_Control  *the_object,
  char            **name
)
{
  Objects_Information *info;

  info = _Objects_Get_information(the_object->id);
  if ( info->is_string ) {
    *name = (char *) the_object->name;
  } else {
    uint32_t  u32_name = (uint32_t) the_object->name;
    (*name)[ 0 ] = (u32_name >> 24) & 0xff;
    (*name)[ 1 ] = (u32_name >> 16) & 0xff;
    (*name)[ 2 ] = (u32_name >>  8) & 0xff;
    (*name)[ 3 ] = (u32_name >>  0) & 0xff;
    (*name)[ 4 ] = '\0';
  }
  return *name;
}

/*
 *  Stack_check_Dump_threads_usage(
 *
 *  Try to print out how much stack was actually used by the task.
 */
void Stack_check_Dump_threads_usage(
  Thread_Control *the_thread
)
{
  uint32_t        size, used;
  void           *low;
  void           *high_water_mark;
  Stack_Control  *stack;
  uint32_t        u32_name;
  char            name_str[5];
  char           *name;

  if ( !the_thread )
    return;

  /*
   * XXX HACK to get to interrupt stack
   */

  if (the_thread == (Thread_Control *) -1) {
    if (Stack_check_Interrupt_stack.area) {
      stack = &Stack_check_Interrupt_stack;
      the_thread = 0;
    }
    else
      return;
  } else
    stack = &the_thread->Start.Initial_stack;

  low  = Stack_check_usable_stack_start(stack);
  size = Stack_check_usable_stack_size(stack);

  high_water_mark = Stack_check_find_high_water_mark(low, size);

  if ( high_water_mark )
    used = Stack_check_Calculate_used( low, size, high_water_mark );
  else
    used = 0;

  name = name_str;
  if ( the_thread ) {
    name = Stack_check_Get_object_name( &the_thread->Object, &name );
  } else {
    u32_name = rtems_build_name('I', 'N', 'T', 'R');
    name[ 0 ] = (u32_name >> 24) & 0xff;
    name[ 1 ] = (u32_name >> 16) & 0xff;
    name[ 2 ] = (u32_name >>  8) & 0xff;
    name[ 3 ] = (u32_name >>  0) & 0xff;
    name[ 4 ] = '\0';
  }

  printk("0x%08" PRIx32 "  %4s  %p - %p   %8" PRId32 "   %8" PRId32 "\n",
    the_thread ? the_thread->Object.id : ~0,
    name,
    stack->area,
    stack->area + stack->size - 1,
    size,
    used
  );
}

/*
 *  rtems_stack_checker_fatal_extension
 */
#ifndef DONT_USE_FATAL_EXTENSION
  void rtems_stack_checker_fatal_extension(
    Internal_errors_Source  source,
    boolean                 is_internal,
    uint32_t                status
  )
  {
    if (status == 0)
      rtems_stack_checker_report_usage();
  }
#endif

/*PAGE
 *
 *  rtems_stack_checker_report_usage
 */

void rtems_stack_checker_report_usage( void )
{
  if (Stack_check_Initialized == 0)
      return;

  printk("Stack usage by thread\n");
  printk(
    "    ID      NAME       LOW        HIGH     AVAILABLE      USED\n"
  );

  /* iterate over all threads and dump the usage */
  rtems_iterate_over_all_threads( Stack_check_Dump_threads_usage );

  /* dump interrupt stack info if any */
  Stack_check_Dump_threads_usage((Thread_Control *) -1);
}
