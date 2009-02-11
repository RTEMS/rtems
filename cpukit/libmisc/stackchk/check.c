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
static inline bool Stack_check_Frame_pointer_in_range(
  Stack_Control *the_stack
)
{
  void *sp = __builtin_frame_address(0);

  #if defined(__GNUC__)
    if ( sp < the_stack->area ) {
      return false;
    }
    if ( sp > (the_stack->area + the_stack->size) ) {
      return false;
    }
  #else
    #error "How do I check stack bounds on a non-GNU compiler?"
  #endif
  return true;
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
bool rtems_stack_checker_create_extension(
  Thread_Control *running,
  Thread_Control *the_thread
)
{
  Stack_check_Initialize();

  if (the_thread)
    Stack_check_Dope_stack(&the_thread->Start.Initial_stack);

  return true;
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
  bool         pattern_ok
)
{
  Stack_Control *stack = &running->Start.Initial_stack;

  printk(
    "BLOWN STACK!!! Offending task(0x%p): "
        "id=0x%08" PRIx32 "; name=0x%08" PRIx32,
    running,
    running->Object.id,
    running->Object.name.name_u32
  );

  #if defined(RTEMS_MULTIPROCESSING)
    if (rtems_configuration_get_user_multiprocessing_table()) {
      printk(
        "; node=%d",
        rtems_configuration_get_user_multiprocessing_table()->node
      );
    }
  #endif

  printk(
    "\n  stack covers range 0x%p - 0x%p (%d bytes)\n",
    stack->area,
    stack->area + stack->size - 1,
    stack->size
  );

  if ( !pattern_ok ) {
    printk(
      "  Damaged pattern begins at 0x%08lx and is %d bytes long\n",
      (unsigned long) Stack_check_Get_pattern_area(stack),
      PATTERN_SIZE_BYTES);
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
  bool        sp_ok;
  bool        pattern_ok = true;

  pattern = (void *) Stack_check_Get_pattern_area(the_stack)->pattern;

  /*
   *  Check for an out of bounds stack pointer or an overwrite
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
bool rtems_stack_checker_is_blown( void )
{
  Stack_Control *the_stack = &_Thread_Executing->Start.Initial_stack;
  bool           sp_ok;
  bool           pattern_ok = true;

  /*
   *  Check for an out of bounds stack pointer
   */

  sp_ok = Stack_check_Frame_pointer_in_range( the_stack );

  /*
   * The stack checker must be initialized before the pattern is there
   * to check.
   */
  if ( Stack_check_Initialized ) {
    pattern_ok = (!memcmp(
      (void *) Stack_check_Get_pattern_area(the_stack)->pattern,
      (void *) Stack_check_Pattern.pattern,
      PATTERN_SIZE_BYTES
    ));
  }

  /*
   * The Stack Pointer and the Pattern Area are OK so return false.
   */
  if ( sp_ok && pattern_ok )
    return false;

  /*
   * Let's report as much as we can.
   */
  Stack_check_report_blown_task( _Thread_Executing, pattern_ok );
  return true;
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
 *  Stack_check_Dump_threads_usage
 *
 *  Try to print out how much stack was actually used by the task.
 */
static void                   *print_context;
static rtems_printk_plugin_t   print_handler;

void Stack_check_Dump_threads_usage(
  Thread_Control *the_thread
)
{
  uint32_t        size, used;
  void           *low;
  void           *high_water_mark;
  void           *current;
  Stack_Control  *stack;
  char            name[5];

  if ( !the_thread )
    return;

  if ( !print_handler )
    return;

  /*
   *  Obtain interrupt stack information
   */

  if (the_thread == (Thread_Control *) -1) {
    if (Stack_check_Interrupt_stack.area) {
      stack = &Stack_check_Interrupt_stack;
      the_thread = 0;
      current = 0;
    }
    else
      return;
  } else {
    stack  = &the_thread->Start.Initial_stack;
    current = (void *)_CPU_Context_Get_SP( &the_thread->Registers );
  }

  low  = Stack_check_usable_stack_start(stack);
  size = Stack_check_usable_stack_size(stack);

  high_water_mark = Stack_check_find_high_water_mark(low, size);

  if ( high_water_mark )
    used = Stack_check_Calculate_used( low, size, high_water_mark );
  else
    used = 0;

  if ( the_thread ) {
    (*print_handler)(
      print_context,
      "0x%08" PRIx32 "  %4s",
      the_thread->Object.id,
      rtems_object_get_name( the_thread->Object.id, sizeof(name), name )
    );
  } else {
    (*print_handler)( print_context, "0x%08" PRIx32 "  INTR", ~0 );
  }

  (*print_handler)(
    print_context,
    " %010p - %010p %010p  %8" PRId32 "   ",
    stack->area,
    stack->area + stack->size - 1,
    current,
    size
  );

  if (Stack_check_Initialized == 0) {
    (*print_handler)( print_context, "Unavailable\n" );
  } else {
    (*print_handler)( print_context, "%8" PRId32 "\n", used );
  }
    

}

/*
 *  rtems_stack_checker_fatal_extension
 */
#ifndef DONT_USE_FATAL_EXTENSION
  void rtems_stack_checker_fatal_extension(
    Internal_errors_Source  source,
    bool                    is_internal,
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

void rtems_stack_checker_report_usage_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
)
{
  print_context = context;
  print_handler = print;

  (*print)( context, "Stack usage by thread\n");
  (*print)( context, 
"    ID      NAME    LOW          HIGH     CURRENT     AVAILABLE     USED\n"
  );

  /* iterate over all threads and dump the usage */
  rtems_iterate_over_all_threads( Stack_check_Dump_threads_usage );

  /* dump interrupt stack info if any */
  Stack_check_Dump_threads_usage((Thread_Control *) -1);

  print_context = NULL;
  print_handler = NULL;

}

void rtems_stack_checker_report_usage( void )
{
  rtems_stack_checker_report_usage_with_plugin( NULL, printk_plugin );
}
