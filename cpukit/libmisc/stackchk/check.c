/**
 * @file
 *
 * @brief Stack Overflow Check User Extension Set
 * @ingroup libmisc_stackchk Stack Checker Mechanism
 *
 * NOTE:  This extension set automatically determines at
 *         initialization time whether the stack for this
 *         CPU grows up or down and installs the correct
 *         extension routines for that direction.
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <inttypes.h>

#include <string.h>
#include <stdlib.h>

#include <rtems/bspIo.h>
#include <rtems/printer.h>
#include <rtems/stackchk.h>
#include <rtems/sysinit.h>
#include <rtems/score/address.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smp.h>
#include <rtems/score/threadimpl.h>

/*
 *  This structure is used to fill in and compare the "end of stack"
 *  marker pattern.
 *  pattern area must be a multiple of 4 words.
 */

#if !defined(CPU_STACK_CHECK_PATTERN_INITIALIZER)
#define CPU_STACK_CHECK_PATTERN_INITIALIZER \
  { \
    0xFEEDF00D, 0x0BAD0D06, /* FEED FOOD to  BAD DOG */ \
    0xDEADF00D, 0x600D0D06  /* DEAD FOOD but GOOD DOG */ \
  }
#endif

/*
 *  The pattern used to fill the entire stack.
 */

#define BYTE_PATTERN 0xA5
#define U32_PATTERN 0xA5A5A5A5

/*
 *  Variable to indicate when the stack checker has been initialized.
 */
static bool Stack_check_Initialized;

/*
 *  The "magic pattern" used to mark the end of the stack.
 */
static const uint32_t Stack_check_Sanity_pattern[] =
  CPU_STACK_CHECK_PATTERN_INITIALIZER;

#define SANITY_PATTERN_SIZE_BYTES sizeof(Stack_check_Sanity_pattern)

#define SANITY_PATTERN_SIZE_WORDS RTEMS_ARRAY_SIZE(Stack_check_Sanity_pattern)

/*
 * Helper function to report if the actual stack pointer is in range.
 *
 * NOTE: This uses a GCC specific method.
 */
static inline bool Stack_check_Frame_pointer_in_range(
  const Thread_Control *the_thread
)
{
  #if defined(__GNUC__)
    void *sp = __builtin_frame_address(0);
    const Stack_Control *the_stack = &the_thread->Start.Initial_stack;

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
  #define Stack_check_Get_pattern( _the_stack ) \
    ((char *)(_the_stack)->area + \
         (_the_stack)->size - SANITY_PATTERN_SIZE_BYTES )

  #define Stack_check_Calculate_used( _low, _size, _high_water ) \
      ((char *)(_high_water) - (char *)(_low))

  #define Stack_check_Usable_stack_start(_the_stack) \
    ((_the_stack)->area)

#else
  #define Stack_check_Get_pattern( _the_stack ) \
    ((char *)(_the_stack)->area)

  #define Stack_check_Calculate_used( _low, _size, _high_water) \
      ( ((char *)(_low) + (_size)) - (char *)(_high_water) )

  #define Stack_check_Usable_stack_start(_the_stack) \
      ((char *)(_the_stack)->area + SANITY_PATTERN_SIZE_BYTES)

#endif

/*
 *  The assumption is that if the pattern gets overwritten, the task
 *  is too close.  This defines the usable stack memory.
 */
#define Stack_check_Usable_stack_size(_the_stack) \
    ((_the_stack)->size - SANITY_PATTERN_SIZE_BYTES)

#if defined(RTEMS_SMP)
static Stack_Control Stack_check_Interrupt_stack[ CPU_MAXIMUM_PROCESSORS ];
#else
static Stack_Control Stack_check_Interrupt_stack[ 1 ];
#endif

/*
 *  Fill an entire stack area with BYTE_PATTERN.  This will be used
 *  to check for amount of actual stack used.
 */
static void Stack_check_Dope_stack( Stack_Control *stack )
{
  memset(
    Stack_check_Usable_stack_start( stack ),
    BYTE_PATTERN,
    Stack_check_Usable_stack_size( stack )
  );
}

static void Stack_check_Add_sanity_pattern( Stack_Control *stack )
{
  memcpy(
    Stack_check_Get_pattern( stack ),
    Stack_check_Sanity_pattern,
    SANITY_PATTERN_SIZE_BYTES
  );
}

static bool Stack_check_Is_sanity_pattern_valid( const Stack_Control *stack )
{
  return memcmp(
    Stack_check_Get_pattern( stack ),
    Stack_check_Sanity_pattern,
    SANITY_PATTERN_SIZE_BYTES
  ) == 0;
}

/*
 *  rtems_stack_checker_create_extension
 */
bool rtems_stack_checker_create_extension(
  Thread_Control *running RTEMS_UNUSED,
  Thread_Control *the_thread
)
{
  Stack_check_Initialized = true;

  Stack_check_Dope_stack( &the_thread->Start.Initial_stack );
  Stack_check_Add_sanity_pattern( &the_thread->Start.Initial_stack );

  return true;
}

void rtems_stack_checker_begin_extension( Thread_Control *executing )
{
  Per_CPU_Control *cpu_self;
  uint32_t         cpu_self_index;
  Stack_Control   *stack;

  /*
   * If appropriate, set up the interrupt stack of the current processor for
   * high water testing also.  This must be done after multi-threading started,
   * since the initialization stacks may reuse the interrupt stacks.  Disable
   * thread dispatching in SMP configurations to prevent thread migration.
   * Writing to the interrupt stack is only safe if done from the corresponding
   * processor in thread context.
   */

#if defined(RTEMS_SMP)
  cpu_self = _Thread_Dispatch_disable();
#else
  cpu_self = _Per_CPU_Get();
#endif

  cpu_self_index = _Per_CPU_Get_index( cpu_self );
  stack = &Stack_check_Interrupt_stack[ cpu_self_index ];

  if ( stack->area == NULL ) {
    stack->area = cpu_self->interrupt_stack_low;
    stack->size = (size_t) ( (char *) cpu_self->interrupt_stack_high -
      (char *) cpu_self->interrupt_stack_low );

    /*
     * Sanity pattern has been added by Stack_check_Prepare_interrupt_stack()
     */
    if ( !Stack_check_Is_sanity_pattern_valid( stack ) ) {
      rtems_fatal(
        RTEMS_FATAL_SOURCE_STACK_CHECKER,
        rtems_build_name( 'I', 'N', 'T', 'R' )
      );
    }

    Stack_check_Dope_stack( stack );
  }

#if defined(RTEMS_SMP)
  _Thread_Dispatch_enable( cpu_self );
#endif
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
static void Stack_check_report_blown_task(
  const Thread_Control *running,
  bool pattern_ok
)
{
  const Stack_Control *stack = &running->Start.Initial_stack;
  void                *pattern_area = Stack_check_Get_pattern(stack);
  char                 name[2 * THREAD_DEFAULT_MAXIMUM_NAME_SIZE];

  printk("BLOWN STACK!!!\n");
  printk("task control block: 0x%08" PRIxPTR "\n", (intptr_t) running);
  printk("task ID: 0x%08lx\n", (unsigned long) running->Object.id);
  printk(
    "task name: 0x%08" PRIx32 "\n",
    running->Object.name.name_u32
  );
  _Thread_Get_name(running, name, sizeof(name));
  printk("task name string: %s\n", name);
  printk(
    "task stack area (%lu Bytes): 0x%08" PRIxPTR " .. 0x%08" PRIxPTR "\n",
    (unsigned long) stack->size,
    (intptr_t) stack->area,
    (intptr_t) ((char *) stack->area + stack->size)
  );
  if (!pattern_ok) {
    printk(
      "damaged pattern area (%lu Bytes): 0x%08" PRIxPTR " .. 0x%08" PRIxPTR "\n",
      (unsigned long) SANITY_PATTERN_SIZE_BYTES,
      (intptr_t) pattern_area,
      (intptr_t) (pattern_area + SANITY_PATTERN_SIZE_BYTES)
    );
  }

  #if defined(RTEMS_MULTIPROCESSING)
    if (rtems_configuration_get_user_multiprocessing_table()) {
      printk(
        "node: 0x%08" PRIxPTR "\n",
          (intptr_t) rtems_configuration_get_user_multiprocessing_table()->node
      );
    }
  #endif

  rtems_fatal(
    RTEMS_FATAL_SOURCE_STACK_CHECKER,
    running->Object.name.name_u32
  );
}

/*
 *  rtems_stack_checker_switch_extension
 */
void rtems_stack_checker_switch_extension(
  Thread_Control *running RTEMS_UNUSED,
  Thread_Control *heir RTEMS_UNUSED
)
{
  bool sp_ok;
  bool pattern_ok;
  const Stack_Control *stack;

  /*
   *  Check for an out of bounds stack pointer or an overwrite
   */
  sp_ok = Stack_check_Frame_pointer_in_range( running );

  pattern_ok = Stack_check_Is_sanity_pattern_valid( &running->Start.Initial_stack );

  if ( !sp_ok || !pattern_ok ) {
    Stack_check_report_blown_task( running, pattern_ok );
  }

  stack = &Stack_check_Interrupt_stack[ _SMP_Get_current_processor() ];

  if ( stack->area != NULL && !Stack_check_Is_sanity_pattern_valid( stack ) ) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_STACK_CHECKER,
      rtems_build_name( 'I', 'N', 'T', 'R' )
    );
  }
}

/*
 *  Check if blown
 */
bool rtems_stack_checker_is_blown( void )
{
  rtems_stack_checker_switch_extension( _Thread_Get_executing(), NULL );

  /*
   * The Stack Pointer and the Pattern Area are OK so return false.
   */
  return false;
}

/*
 * Stack_check_find_high_water_mark
 */
static inline void *Stack_check_Find_high_water_mark(
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

    base += SANITY_PATTERN_SIZE_WORDS;
    for (ebase = base + length; base < ebase; base++)
      if (*base != U32_PATTERN)
        return (void *) base;
  #endif

  return (void *)0;
}

static bool Stack_check_Dump_stack_usage(
  const Stack_Control *stack,
  const void          *current,
  const char          *name,
  uint32_t             id,
  const rtems_printer *printer
)
{
  uint32_t  size;
  uint32_t  used;
  void     *low;
  void     *high_water_mark;

  low  = Stack_check_Usable_stack_start(stack);
  size = Stack_check_Usable_stack_size(stack);

  high_water_mark = Stack_check_Find_high_water_mark(low, size);

  if ( high_water_mark )
    used = Stack_check_Calculate_used( low, size, high_water_mark );
  else
    used = 0;

  rtems_printf(
    printer,
    "0x%08" PRIx32 " %-21s 0x%08" PRIxPTR " 0x%08" PRIxPTR " 0x%08" PRIxPTR " %6" PRId32 " ",
    id,
    name,
    (uintptr_t) stack->area,
    (uintptr_t) stack->area + (uintptr_t) stack->size - 1,
    (uintptr_t) current,
    size
  );

  if (Stack_check_Initialized) {
    rtems_printf( printer, "%6" PRId32 "\n", used );
  } else {
    rtems_printf( printer, "N/A\n" );
  }

  return false;
}

static bool Stack_check_Dump_threads_usage(
  Thread_Control *the_thread,
  void           *arg
)
{
  char                 name[ 22 ];
  const rtems_printer *printer;

  printer = arg;
  _Thread_Get_name( the_thread, name, sizeof( name ) );
  Stack_check_Dump_stack_usage(
    &the_thread->Start.Initial_stack,
    (void *) _CPU_Context_Get_SP( &the_thread->Registers ),
    name,
    the_thread->Object.id,
    printer
  );
  return false;
}

static void Stack_check_Dump_interrupt_stack_usage(
  const Stack_Control *stack,
  uint32_t             id,
  const rtems_printer *printer
)
{
  Stack_check_Dump_stack_usage(
    stack,
    NULL,
    "Interrupt Stack",
    id,
    printer
  );
}

/*
 *  rtems_stack_checker_report_usage
 */

void rtems_stack_checker_report_usage_with_plugin(
  const rtems_printer* printer
)
{
  uint32_t cpu_max;
  uint32_t cpu_index;

  rtems_printf(
     printer,
     "                             STACK USAGE BY THREAD\n"
     "ID         NAME                  LOW        HIGH       CURRENT     AVAIL   USED\n"
  );

  /* iterate over all threads and dump the usage */
  rtems_task_iterate(
    Stack_check_Dump_threads_usage,
    RTEMS_DECONST( rtems_printer *, printer )
  );

  cpu_max = rtems_scheduler_get_processor_maximum();

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Stack_check_Dump_interrupt_stack_usage(
      &Stack_check_Interrupt_stack[ cpu_index ],
      cpu_index,
      printer
    );
  }
}

void rtems_stack_checker_report_usage( void )
{
  rtems_printer printer;
  rtems_print_printer_printk(&printer);
  rtems_stack_checker_report_usage_with_plugin( &printer );
}

static void Stack_check_Prepare_interrupt_stacks( void )
{
  Stack_Control stack;
  uint32_t      cpu_index;
  uint32_t      cpu_max;

  stack.size = rtems_configuration_get_interrupt_stack_size();
  stack.area = _ISR_Stack_area_begin;
  cpu_max = rtems_configuration_get_maximum_processors();

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Stack_check_Add_sanity_pattern( &stack );
    stack.area = _Addresses_Add_offset( stack.area, stack.size );
  }
}

RTEMS_SYSINIT_ITEM(
  Stack_check_Prepare_interrupt_stacks,
  RTEMS_SYSINIT_BSP_WORK_AREAS,
  RTEMS_SYSINIT_ORDER_SECOND
);
