/*  threadinfo.c
 *  
 *  Routines to print out information about threads.
 */

#include "system.h"
#include <stdlib.h>

#define PTR unsigned int
#define PTR_MASK "0x%08x"

/*
 *  Return a pointer to a string describing the object type.
 */

const char * _Objects_Type_to_String( Objects_Control * object )
{
  switch( _Objects_Get_class( object->id ) ) {
    case OBJECTS_NO_CLASS:
      return "NO_CLASS";
      
    case OBJECTS_INTERNAL_THREADS:
      return "INTERNAL_THREADS";
      
    case OBJECTS_RTEMS_TASKS:
      return "RTEMS_TASKS";
      
    case OBJECTS_POSIX_THREADS:
      return "POSIX_THREADS";
      
    case OBJECTS_ITRON_TASKS:
      return "ITRON_TASKS";
      
    case OBJECTS_RTEMS_TIMERS:
      return "RTEMS_TIMERS";
      
    case OBJECTS_RTEMS_SEMAPHORES:
      return "RTEMS_SEMAPHORES";
      
    case OBJECTS_RTEMS_MESSAGE_QUEUES:
      return "RTEMS_MESSAGE_QUEUES";
      
    case OBJECTS_RTEMS_PARTITIONS:
      return "RTEMS_PARTITIONS";
      
    case OBJECTS_RTEMS_REGIONS:
      return "RTEMS_REGIONS";
      
    case OBJECTS_RTEMS_PORTS:
      return "RTEMS_PORTS";
      
    case OBJECTS_RTEMS_PERIODS:
      return "RTEMS_PERIODS";
      
    case OBJECTS_RTEMS_EXTENSIONS:
      return "RTEMS_EXTENSIONS";
      
    case OBJECTS_POSIX_KEYS:
      return "POSIX_KEYS";
      
    case OBJECTS_POSIX_INTERRUPTS:
      return "POSIX_INTERRUPTS";
      
    case OBJECTS_POSIX_MESSAGE_QUEUES:
      return "POSIX_MESSAGE_QUEUES";
      
    case OBJECTS_POSIX_MUTEXES:
      return "POSIX_MUTEXES";
      
    case OBJECTS_POSIX_SEMAPHORES:
      return "POSIX_SEMAPHORES";
      
    case OBJECTS_POSIX_CONDITION_VARIABLES:
      return "POSIX_CONDITION_VARIABLES";
      
    case OBJECTS_ITRON_EVENTFLAGS:
      return "ITRON_EVENTFLAGS";
      
    case OBJECTS_ITRON_MAILBOXES:
      return "ITRON_MAILBOXES";
      
    case OBJECTS_ITRON_MESSAGE_BUFFERS:    
       return "ITRON_MESSAGE_BUFFERS";
       
    case OBJECTS_ITRON_PORTS:
      return "ITRON_PORTS";
      
    case OBJECTS_ITRON_SEMAPHORES:
      return "ITRON_SEMAPHORES";
      
    case OBJECTS_ITRON_VARIABLE_MEMORY_POOLS:
      return "ITRON_VARIABLE_MEMORY_POOLS";
      
    case OBJECTS_ITRON_FIXED_MEMORY_POOLS:
      return "ITRON_FIXED_MEMORY_POOLS";
      
    default:
      return "UNKNOWN";
  }
}


/*
 *  Return a pointer to a string describing the thread state
 */

const char * _Thread_State_to_String( States_Control state )
{
  States_Control orig_state = state;
  int i;
  int first_entry = TRUE;
  static char buffer[256];
  static const char *desc[] = {
    "READY",                          /* 0 - 0x00000 */
    "DORMANT",                        /* 1 - 0x00001 */
    "SUSPENDED",                      /* 2 - 0x00002 */
    "TRANSIENT",                      /* 3 - 0x00004 */
    "DELAYING",                       /* 4 - 0x00008 */
    "WAITING_FOR_TIME",               /* 5 - 0x00010 */
    "WAITING_FOR_BUFFER",             /* 6 - 0x00020 */
    "WAITING_FOR_SEGMENT",            /* 7 - 0x00040 */
    "WAITING_FOR_MESSAGE",            /* 8 - 0x00080 */
    "WAITING_FOR_EVENT",              /* 9 - 0x00100 */
    "WAITING_FOR_SEMAPHORE",          /* 10 - 0x00200 */
    "WAITING_FOR_MUTEX",              /* 11 - 0x00400 */
    "WAITING_FOR_CONDITION_VARIABLE", /* 12 - 0x00800 */
    "WAITING_FOR_JOIN_AT_EXIT",       /* 13 - 0x01000 */
    "WAITING_FOR_RPC_REPLY",          /* 14 - 0x02000 */
    "WAITING_FOR_PERIOD",             /* 15 - 0x04000 */
    "WAITING_FOR_SIGNAL",             /* 16 - 0x08000 */
    "INTERRUPTIBLE_BY_SIGNAL",        /* 17 - 0x10000 */
    "UNKNOWN"                         /* 18 - all higher bits */
  };

  if( state == STATES_READY )
    return desc[0];
    
  memset( buffer, '\0', sizeof( buffer ) );
  for ( i = 1; i < 18; i++ ) {
    if ( state & 0x1 ) {
      if ( !first_entry ) {
        strcat ( buffer, " | " );
      }
      strcat ( buffer, desc[i] );
      first_entry = FALSE;
    }
    state = state >> 1;
  }

  if ( state != 0 ) {
    if ( !first_entry ) {
      strcat ( buffer, " | " );
    }
    strcat ( buffer, desc[18] );
  }

  return buffer;
}


/*
 *  Callback function to print out thread Ids, thread priorities, and thread stack
 *  information.
 */

unsigned32 print_thread_info( Thread_Control *thread, void * arg )
{
  printf( "Thread ID 0x%08x at "PTR_MASK"\n", thread->Object.id, (PTR)thread );
  printf( "\ttype = %s\n", _Objects_Type_to_String( &(thread->Object) ) );
  printf( "\tstate = %s\n", _Thread_State_to_String( thread->current_state ) );
  printf( "\treal priority = %d\n", thread->real_priority );
  printf( "\tcurrent priority = %d\n", thread->current_priority );
  printf( "\tstack base = "PTR_MASK"\n", (PTR)thread->Start.Initial_stack.area );
  printf( "\tstack size = 0x%08x\n", thread->Start.Initial_stack.size );
  /* printf( "\tstack pointer = "PTR_MASK"\n", */
  printf( "\n" );
  return 0;
}
