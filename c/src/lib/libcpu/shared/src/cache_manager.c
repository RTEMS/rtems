/*
 *  Cache Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *
 *  The functions in this file implement the API to the RTEMS Cache Manager and
 *  are divided into data cache and instruction cache functions. Data cache
 *  functions only have bodies if a data cache is supported. Instruction
 *  cache functions only have bodies if an instruction cache is supported.
 *  Support for a particular cache exists only if CPU_x_CACHE_ALIGNMENT is
 *  defined, where x E {DATA, INSTRUCTION}. These definitions are found in
 *  the Cache Manager Wrapper header files, often
 *
 *  rtems/c/src/lib/libcpu/CPU/cache_.h
 *
 *  The cache implementation header file can define
 *  CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS
 *  if it provides cache maintenance functions which operate on multiple lines.
 *  Otherwise a generic loop with single line operations will be used.
 *
 *  The functions below are implemented with CPU dependent inline routines
 *  found in the cache.c files for each CPU. In the event that a CPU does
 *  not support a specific function for a cache it has, the CPU dependent
 *  routine does nothing (but does exist).
 *
 *  At this point, the Cache Manager makes no considerations, and provides no
 *  support for BSP specific issues such as a secondary cache. In such a system,
 *  the CPU dependent routines would have to be modified, or a BSP layer added
 *  to this Manager.
 */

#include <rtems.h>
#include "cache_.h"
#include <rtems/score/smpimpl.h>
#include <rtems/score/smplock.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/sysstate.h>

#if defined( RTEMS_SMP )

typedef void (*Cache_manager_Function_ptr)(const void *d_addr, size_t n_bytes);

typedef struct {
  Chain_Node Node;
  Cache_manager_Function_ptr func;
  const void *addr;
  size_t size;
  cpu_set_t *recipients;
  size_t setsize;
  Atomic_Ulong done;
} Cache_manager_SMP_node;

typedef struct {
  SMP_lock_Control Lock;
  Chain_Control List;
} Cache_manager_SMP_control;

static Cache_manager_SMP_control _Cache_manager_SMP_control = {
  .Lock = SMP_LOCK_INITIALIZER("cachemgr"),
  .List = CHAIN_INITIALIZER_EMPTY(_Cache_manager_SMP_control.List)
};

void
_SMP_Cache_manager_message_handler(void)
{
  SMP_lock_Context lock_context;
  Cache_manager_SMP_node *node;
  Cache_manager_SMP_node *next;
  uint32_t cpu_self_idx;

  _SMP_lock_ISR_disable_and_acquire( &_Cache_manager_SMP_control.Lock,
      &lock_context );
  cpu_self_idx = _SMP_Get_current_processor();

  node = (Cache_manager_SMP_node*)_Chain_First(
      &_Cache_manager_SMP_control.List );
  while ( !_Chain_Is_tail( &_Cache_manager_SMP_control.List, &node->Node ) ) {
    next = (Cache_manager_SMP_node*)_Chain_Next( &node->Node );
    if ( CPU_ISSET_S ( cpu_self_idx, node->setsize, node->recipients ) ) {
      CPU_CLR_S ( cpu_self_idx, node->setsize, node->recipients );

      node->func( node->addr, node->size );

      if ( CPU_COUNT_S( node->setsize, node->recipients ) == 0 ) {
        _Chain_Extract_unprotected( &node->Node );
        _Atomic_Store_ulong( &node->done, 1, ATOMIC_ORDER_RELEASE );
      }
    }
    node = next;
  }

  _SMP_lock_Release_and_ISR_enable( &_Cache_manager_SMP_control.Lock,
      &lock_context );
}

#if defined(CPU_DATA_CACHE_ALIGNMENT) || \
    (defined(CPU_INSTRUCTION_CACHE_ALIGNMENT) && \
    defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING))

static void
_Cache_manager_Process_cache_messages( void )
{
  unsigned long message;
  Per_CPU_Control *cpu_self;
  ISR_Level isr_level;

  _ISR_Disable_without_giant( isr_level );

  cpu_self = _Per_CPU_Get();

  message = _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED );

  if ( message & SMP_MESSAGE_CACHE_MANAGER ) {
    if ( _Atomic_Compare_exchange_ulong( &cpu_self->message, &message,
        message & ~SMP_MESSAGE_CACHE_MANAGER, ATOMIC_ORDER_RELAXED,
        ATOMIC_ORDER_RELAXED ) ) {
      _SMP_Cache_manager_message_handler();
    }
  }

  _ISR_Enable_without_giant( isr_level );
}

/*
 * We can not make this function static as we need to access it
 * from the test program.
 */
void
_Cache_manager_Send_smp_msg(
    const size_t setsize,
    const cpu_set_t *set,
    Cache_manager_Function_ptr func,
    const void * addr,
    size_t size
  );

void
_Cache_manager_Send_smp_msg(
    const size_t setsize,
    const cpu_set_t *set,
    Cache_manager_Function_ptr func,
    const void * addr,
    size_t size
  )
{
  uint32_t i;
  Cache_manager_SMP_node node;
  size_t set_size = CPU_ALLOC_SIZE( _SMP_Get_processor_count() );
  char cpu_set_copy[set_size];
  SMP_lock_Context lock_context;

  if ( ! _System_state_Is_up( _System_state_Get() ) ) {
    func( addr, size );
    return;
  }

  memset( cpu_set_copy, 0, set_size );
  if( set == NULL ) {
    for( i=0; i<_SMP_Get_processor_count(); ++i )
      CPU_SET_S( i, set_size, (cpu_set_t *)cpu_set_copy );
  } else {
    for( i=0; i<_SMP_Get_processor_count(); ++i )
      if( CPU_ISSET_S( i, set_size, set ) )
        CPU_SET_S( i, set_size, (cpu_set_t *)cpu_set_copy );
  }

  node.func = func;
  node.addr = addr;
  node.size = size;
  node.setsize = set_size;
  node.recipients = (cpu_set_t *)cpu_set_copy;
  _Atomic_Store_ulong( &node.done, 0, ATOMIC_ORDER_RELAXED );


  _SMP_lock_ISR_disable_and_acquire( &_Cache_manager_SMP_control.Lock,
      &lock_context );
  _Chain_Prepend_unprotected( &_Cache_manager_SMP_control.List, &node.Node );
  _SMP_lock_Release_and_ISR_enable( &_Cache_manager_SMP_control.Lock,
      &lock_context );

  _SMP_Send_message_multicast( set_size, node.recipients,
      SMP_MESSAGE_CACHE_MANAGER );

  _Cache_manager_Process_cache_messages();

  while ( !_Atomic_Load_uint( &node.done, ATOMIC_ORDER_ACQUIRE ) );
}
#endif

void
rtems_cache_flush_multiple_data_lines_processor_set(
  const void *addr,
  size_t size,
  const size_t setsize,
  const cpu_set_t *set
)
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _Cache_manager_Send_smp_msg( setsize, set,
      rtems_cache_flush_multiple_data_lines, addr, size );
#endif
}

void
rtems_cache_invalidate_multiple_data_lines_processor_set(
  const void *addr,
  size_t size,
  const size_t setsize,
  const cpu_set_t *set
)
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _Cache_manager_Send_smp_msg( setsize, set,
      rtems_cache_invalidate_multiple_data_lines, addr, size );
#endif
}

void
rtems_cache_flush_entire_data_processor_set(
  const size_t setsize,
  const cpu_set_t *set
)
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _Cache_manager_Send_smp_msg( setsize, set,
      (Cache_manager_Function_ptr)rtems_cache_flush_entire_data, 0, 0 );
#endif
}

void
rtems_cache_invalidate_entire_data_processor_set(
  const size_t setsize,
  const cpu_set_t *set
)
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _Cache_manager_Send_smp_msg( setsize, set,
      (Cache_manager_Function_ptr)rtems_cache_invalidate_entire_data, 0, 0 );
#endif
}
#endif

/*
 * THESE FUNCTIONS ONLY HAVE BODIES IF WE HAVE A DATA CACHE
 */

/*
 * This function is called to flush the data cache by performing cache
 * copybacks. It must determine how many cache lines need to be copied
 * back and then perform the copybacks.
 */
void
rtems_cache_flush_multiple_data_lines( const void * d_addr, size_t n_bytes )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
#if defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)
  _CPU_cache_flush_data_range( d_addr, n_bytes );
#else
  const void * final_address;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be pushed. Increment d_addr and push
  * the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to flush is zero */
    return;

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));
  while( d_addr <= final_address )  {
    _CPU_cache_flush_1_data_line( d_addr );
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }
#endif
#endif
}


/*
 * This function is responsible for performing a data cache invalidate.
 * It must determine how many cache lines need to be invalidated and then
 * perform the invalidations.
 */

void
rtems_cache_invalidate_multiple_data_lines( const void * d_addr, size_t n_bytes )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
#if defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)
  _CPU_cache_invalidate_data_range( d_addr, n_bytes );
#else
  const void * final_address;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be invalidated. Increment d_addr and
  * invalidate the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to invalidate is zero */
    return;

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));
  while( final_address >= d_addr ) {
    _CPU_cache_invalidate_1_data_line( d_addr );
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }
#endif
#endif
}


/*
 * This function is responsible for performing a data cache flush.
 * It flushes the entire cache.
 */
void
rtems_cache_flush_entire_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
   /*
    * Call the CPU-specific routine
    */
   _CPU_cache_flush_entire_data();
#endif
}


/*
 * This function is responsible for performing a data cache
 * invalidate. It invalidates the entire cache.
 */
void
rtems_cache_invalidate_entire_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
 /*
  * Call the CPU-specific routine
  */

 _CPU_cache_invalidate_entire_data();
#endif
}


/*
 * This function returns the data cache granularity.
 */
size_t
rtems_cache_get_data_line_size( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  return CPU_DATA_CACHE_ALIGNMENT;
#else
  return 0;
#endif
}


size_t
rtems_cache_get_data_cache_size( uint32_t level )
{
#if defined(CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS)
  return _CPU_cache_get_data_cache_size( level );
#else
  return 0;
#endif
}

/*
 * This function freezes the data cache; cache lines
 * are not replaced.
 */
void
rtems_cache_freeze_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_freeze_data();
#endif
}


/*
 * This function unfreezes the instruction cache.
 */
void rtems_cache_unfreeze_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_unfreeze_data();
#endif
}


/* Turn on the data cache. */
void
rtems_cache_enable_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_enable_data();
#endif
}


/* Turn off the data cache. */
void
rtems_cache_disable_data( void )
{
#if defined(CPU_DATA_CACHE_ALIGNMENT)
  _CPU_cache_disable_data();
#endif
}



/*
 * THESE FUNCTIONS ONLY HAVE BODIES IF WE HAVE AN INSTRUCTION CACHE
 */



/*
 * This function is responsible for performing an instruction cache
 * invalidate. It must determine how many cache lines need to be invalidated
 * and then perform the invalidations.
 */

#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
#if !defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)
static void
_invalidate_multiple_instruction_lines_no_range_functions(
  const void * i_addr,
  size_t n_bytes
)
{
  const void * final_address;

 /*
  * Set i_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be invalidated. Increment i_addr and
  * invalidate the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to invalidate is zero */
    return;

  final_address = (void *)((size_t)i_addr + n_bytes - 1);
  i_addr = (void *)((size_t)i_addr & ~(CPU_INSTRUCTION_CACHE_ALIGNMENT - 1));
  while( final_address >= i_addr ) {
    _CPU_cache_invalidate_1_instruction_line( i_addr );
    i_addr = (void *)((size_t)i_addr + CPU_INSTRUCTION_CACHE_ALIGNMENT);
  }
}
#endif
#endif

void
rtems_cache_invalidate_multiple_instruction_lines(
  const void * i_addr,
  size_t n_bytes
)
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
#if defined(CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS)

#if defined(RTEMS_SMP) && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)
  _Cache_manager_Send_smp_msg( 0, 0, _CPU_cache_invalidate_instruction_range,
      i_addr, n_bytes );
#else
  _CPU_cache_invalidate_instruction_range( i_addr, n_bytes );
#endif

#else

#if defined(RTEMS_SMP) && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)
  _Cache_manager_Send_smp_msg( 0, 0,
      _invalidate_multiple_instruction_lines_no_range_functions, i_addr,
      n_bytes );
#else
  _invalidate_multiple_instruction_lines_no_range_functions( i_addr, n_bytes );
#endif

#endif
#endif
}


/*
 * This function is responsible for performing an instruction cache
 * invalidate. It invalidates the entire cache.
 */
void
rtems_cache_invalidate_entire_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
 /*
  * Call the CPU-specific routine
  */

#if defined(RTEMS_SMP) && defined(CPU_CACHE_NO_INSTRUCTION_CACHE_SNOOPING)
  _Cache_manager_Send_smp_msg( 0, 0,
      (Cache_manager_Function_ptr)_CPU_cache_invalidate_entire_instruction,
      0, 0 );
#else
 _CPU_cache_invalidate_entire_instruction();
#endif
#endif
}


/*
 * This function returns the instruction cache granularity.
 */
size_t
rtems_cache_get_instruction_line_size( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  return CPU_INSTRUCTION_CACHE_ALIGNMENT;
#else
  return 0;
#endif
}


size_t
rtems_cache_get_instruction_cache_size( uint32_t level )
{
#if defined(CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS)
  return _CPU_cache_get_instruction_cache_size( level );
#else
  return 0;
#endif
}


/*
 * This function freezes the instruction cache; cache lines
 * are not replaced.
 */
void
rtems_cache_freeze_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_freeze_instruction();
#endif
}


/*
 * This function unfreezes the instruction cache.
 */
void rtems_cache_unfreeze_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_unfreeze_instruction();
#endif
}


/* Turn on the instruction cache. */
void
rtems_cache_enable_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_enable_instruction();
#endif
}


/* Turn off the instruction cache. */
void
rtems_cache_disable_instruction( void )
{
#if defined(CPU_INSTRUCTION_CACHE_ALIGNMENT)
  _CPU_cache_disable_instruction();
#endif
}
