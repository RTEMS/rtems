/*
 *  File:	monitor.h
 *
 *  Description:
 *    The RTEMS monitor task include file.
 *
 *  TODO:
 *
 *  $Id$
 */

#ifndef __MONITOR_H
#define __MONITOR_H

#include <rtems/symbols.h>
#include <rtems/error.h>		/* rtems_error() */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Monitor types are derived from rtems object classes
 */

typedef enum {
    RTEMS_MONITOR_OBJECT_INVALID   =  OBJECTS_NO_CLASS,
    RTEMS_MONITOR_OBJECT_TASK      =  OBJECTS_RTEMS_TASKS,
    RTEMS_MONITOR_OBJECT_EXTENSION =  OBJECTS_RTEMS_EXTENSIONS,
    RTEMS_MONITOR_OBJECT_QUEUE     =  OBJECTS_RTEMS_MESSAGE_QUEUES,
    RTEMS_MONITOR_OBJECT_SEMAPHORE =  OBJECTS_RTEMS_SEMAPHORES,
    RTEMS_MONITOR_OBJECT_PARTITION =  OBJECTS_RTEMS_PARTITIONS,
    RTEMS_MONITOR_OBJECT_REGION    =  OBJECTS_RTEMS_REGIONS,
    RTEMS_MONITOR_OBJECT_PORT      =  OBJECTS_RTEMS_PORTS,

    /* following monitor objects are not known to RTEMS, but
     * we like to have "types" for them anyway */
    
    RTEMS_MONITOR_OBJECT_DRIVER    =  OBJECTS_RTEMS_CLASSES_LAST+1,
    RTEMS_MONITOR_OBJECT_DNAME,
    RTEMS_MONITOR_OBJECT_CONFIG,
    RTEMS_MONITOR_OBJECT_INIT_TASK,
    RTEMS_MONITOR_OBJECT_MPCI,
    RTEMS_MONITOR_OBJECT_SYMBOL
} rtems_monitor_object_type_t;

/*
 * rtems_monitor_init() flags
 */

#define RTEMS_MONITOR_SUSPEND	0x0001		/* suspend monitor on startup */
#define RTEMS_MONITOR_GLOBAL    0x0002          /* monitor should be global */


/*
 * Public interfaces for RTEMS data structures monitor is aware of.
 * These are only used by the monitor.
 *
 * NOTE:
 *  All the canonical objects that correspond to RTEMS managed "objects"
 *  must have an identical first portion with 'id' and 'name' fields.
 *
 *  Others do not have that restriction, even tho we would like them to.
 *  This is because some of the canonical structures are almost too big
 *  for shared memory driver (eg: mpci) and we are nickel and diming it.
 */

/*
 * Type of a pointer that may be a symbol
 */
    
#define MONITOR_SYMBOL_LEN 20
typedef struct {
    char       name[MONITOR_SYMBOL_LEN];
    unsigned32 value;
    unsigned32 offset;
} rtems_monitor_symbol_t;    

typedef struct {
    rtems_id            id;
    rtems_name          name;
  /* end of common portion */
} rtems_monitor_generic_t;        

/*
 * Task
 */
typedef struct {
    rtems_id            id;
    rtems_name          name;
  /* end of common portion */
    Thread_Entry        entry;
    unsigned32          argument;
    void               *stack;
    unsigned32          stack_size;
    rtems_task_priority priority;
    States_Control      state;
    rtems_event_set     events;
    rtems_mode          modes;
    rtems_attribute     attributes;
    unsigned32          notepad[RTEMS_NUMBER_NOTEPADS];
    rtems_id            wait_id;
    unsigned32          wait_args;
} rtems_monitor_task_t;

/*
 * Init task
 */

typedef struct {
    rtems_id            id;		/* not really an id */
    rtems_name          name;
  /* end of common portion */
    rtems_monitor_symbol_t entry;
    unsigned32             argument;
    unsigned32             stack_size;
    rtems_task_priority    priority;
    rtems_mode             modes;
    rtems_attribute        attributes;
} rtems_monitor_init_task_t;


/*
 * Message queue
 */
typedef struct {
    rtems_id            id;
    rtems_name          name;
  /* end of common portion */
    rtems_attribute     attributes;
    unsigned32          number_of_pending_messages;
    unsigned32          maximum_pending_messages;
    unsigned32          maximum_message_size;
} rtems_monitor_queue_t;

/*
 * Extension
 */
typedef struct {
    rtems_id                 id;
    rtems_name               name;
  /* end of common portion */
    rtems_monitor_symbol_t  e_create;
    rtems_monitor_symbol_t  e_start;
    rtems_monitor_symbol_t  e_restart;
    rtems_monitor_symbol_t  e_delete;
    rtems_monitor_symbol_t  e_tswitch;
    rtems_monitor_symbol_t  e_begin;
    rtems_monitor_symbol_t  e_exitted;
    rtems_monitor_symbol_t  e_fatal;
} rtems_monitor_extension_t;

/*
 * Device driver
 */

typedef struct {
    rtems_id            id;		   /* not really an id (should be tho) */
    rtems_name          name;              /* ditto */
  /* end of common portion */
    rtems_monitor_symbol_t initialization; /* initialization procedure */
    rtems_monitor_symbol_t open;           /* open request procedure */
    rtems_monitor_symbol_t close;          /* close request procedure */
    rtems_monitor_symbol_t read;           /* read request procedure */
    rtems_monitor_symbol_t write;          /* write request procedure */
    rtems_monitor_symbol_t control;        /* special functions procedure */
} rtems_monitor_driver_t;

typedef struct {
    rtems_id            id;		    /* not used for drivers (yet) */
    rtems_name          name;               /* not used for drivers (yet) */
  /* end of common portion */
    unsigned32          major;
    unsigned32          minor;
    char                name_string[64];
} rtems_monitor_dname_t;

/*
 * System config
 */

typedef struct {
    void               *work_space_start;
    unsigned32          work_space_size;
    unsigned32          maximum_tasks;
    unsigned32          maximum_timers;
    unsigned32          maximum_semaphores;
    unsigned32          maximum_message_queues;
    unsigned32          maximum_partitions;
    unsigned32          maximum_regions;
    unsigned32          maximum_ports;
    unsigned32          maximum_periods;
    unsigned32          maximum_extensions;
    unsigned32          microseconds_per_tick;
    unsigned32          ticks_per_timeslice;
    unsigned32          number_of_initialization_tasks;
} rtems_monitor_config_t;

/*
 * MPCI config
 */

#if defined(RTEMS_MULTIPROCESSING)
typedef struct {
    unsigned32  node;                   /* local node number */
    unsigned32  maximum_nodes;          /* maximum # nodes in system */
    unsigned32  maximum_global_objects; /* maximum # global objects */
    unsigned32  maximum_proxies;        /* maximum # proxies */

    unsigned32               default_timeout;        /* in ticks */
    unsigned32               maximum_packet_size;
    rtems_monitor_symbol_t   initialization;
    rtems_monitor_symbol_t   get_packet;
    rtems_monitor_symbol_t   return_packet;
    rtems_monitor_symbol_t   send_packet;
    rtems_monitor_symbol_t   receive_packet;
} rtems_monitor_mpci_t;
#endif

/*
 * The generic canonical information union
 */

typedef union {
    rtems_monitor_generic_t    generic;
    rtems_monitor_task_t       task;
    rtems_monitor_queue_t      queue;
    rtems_monitor_extension_t  extension;
    rtems_monitor_driver_t     driver;
    rtems_monitor_dname_t      dname;
    rtems_monitor_config_t     config;
#if defined(RTEMS_MULTIPROCESSING)
    rtems_monitor_mpci_t       mpci;
#endif
    rtems_monitor_init_task_t  itask;
} rtems_monitor_union_t;

/*
 * Support for talking to other monitors
 */

/*
 * Names of other monitors
 */

#define RTEMS_MONITOR_NAME        (rtems_build_name('R', 'M', 'O', 'N'))
#define RTEMS_MONITOR_SERVER_NAME (rtems_build_name('R', 'M', 'S', 'V'))
#define RTEMS_MONITOR_QUEUE_NAME  (rtems_build_name('R', 'M', 'S', 'Q'))
#define RTEMS_MONITOR_RESPONSE_QUEUE_NAME (rtems_build_name('R', 'M', 'R', 'Q'))

#define RTEMS_MONITOR_SERVER_RESPONSE    0x0001
#define RTEMS_MONITOR_SERVER_CANONICAL   0x0002

typedef struct
{
    unsigned32  command;
    rtems_id    return_id;
    unsigned32  argument0;
    unsigned32  argument1;
    unsigned32  argument2;
    unsigned32  argument3;
    unsigned32  argument4;
    unsigned32  argument5;
} rtems_monitor_server_request_t;

typedef struct
{
    unsigned32  command;
    unsigned32  result0;
    unsigned32  result1;
    rtems_monitor_union_t payload;
} rtems_monitor_server_response_t;

extern rtems_id  rtems_monitor_task_id;

extern unsigned32 rtems_monitor_node;	       /* our node number */
extern unsigned32 rtems_monitor_default_node;  /* current default for commands */

/*
 * Monitor command function and table entry
 */

typedef struct rtems_monitor_command_entry_s rtems_monitor_command_entry_t;

typedef void ( *rtems_monitor_command_function_t )(
                 int         argc,
                 char      **argv,
                 unsigned32  command_arg,
                 boolean     verbose
             );

#if defined(__mips64)
    typedef unsigned64   rtems_monitor_command_arg_t;
#else
    typedef unsigned32   rtems_monitor_command_arg_t;
#endif

struct rtems_monitor_command_entry_s {
    char        *command;      /* command name */
    char        *usage;        /* usage string for the command */
    unsigned32   arguments_required;    /* # of required args */
    rtems_monitor_command_function_t command_function;
                               /* Some argument for the command */
    rtems_monitor_command_arg_t   command_arg;
    struct rtems_monitor_command_entry_s *next;
};

typedef void *(*rtems_monitor_object_next_fn)(void *, void *, rtems_id *);
typedef void (*rtems_monitor_object_canonical_fn)(void *, void *);
typedef void (*rtems_monitor_object_dump_header_fn)(boolean);
typedef void (*rtems_monitor_object_dump_fn)(void *, boolean);

typedef struct {
    rtems_monitor_object_type_t         type;
    void                               *object_information;
    int                                 size;	/* of canonical object */
    rtems_monitor_object_next_fn        next;
    rtems_monitor_object_canonical_fn   canonical;
    rtems_monitor_object_dump_header_fn dump_header;
    rtems_monitor_object_dump_fn        dump;
} rtems_monitor_object_info_t;


/* monitor.c */
void    rtems_monitor_kill(void);
void    rtems_monitor_init(unsigned32);
void    rtems_monitor_wakeup(void);
void    rtems_monitor_pause_cmd(int, char **, unsigned32, boolean);
void    rtems_monitor_fatal_cmd(int, char **, unsigned32, boolean);
void    rtems_monitor_continue_cmd(int, char **, unsigned32, boolean);
void    rtems_monitor_debugger_cmd(int, char **, unsigned32, boolean);
void    rtems_monitor_node_cmd(int, char **, unsigned32, boolean);
void    rtems_monitor_symbols_loadup(void);
int     rtems_monitor_insert_cmd(rtems_monitor_command_entry_t *);
int     rtems_monitor_erase_cmd(rtems_monitor_command_entry_t *);

void    rtems_monitor_task(rtems_task_argument);

/* server.c */
void    rtems_monitor_server_kill(void);
rtems_status_code rtems_monitor_server_request(unsigned32, rtems_monitor_server_request_t *, rtems_monitor_server_response_t *);
void    rtems_monitor_server_task(rtems_task_argument);
void    rtems_monitor_server_init(unsigned32);
  
/* command.c */
int     rtems_monitor_make_argv(char *, int *, char **);
int     rtems_monitor_command_read(char *, int *, char **);
rtems_monitor_command_entry_t *rtems_monitor_command_lookup(
    rtems_monitor_command_entry_t * table, int argc, char **argv);
void    rtems_monitor_command_usage(rtems_monitor_command_entry_t *, char *);
void    rtems_monitor_help_cmd(int, char **, unsigned32, boolean);

/* prmisc.c */
void       rtems_monitor_separator(void);
unsigned32 rtems_monitor_pad(unsigned32 dest_col, unsigned32 curr_col);
unsigned32 rtems_monitor_dump_char(unsigned8 ch);
unsigned32 rtems_monitor_dump_decimal(unsigned32 num);
unsigned32 rtems_monitor_dump_hex(unsigned32 num);
unsigned32 rtems_monitor_dump_id(rtems_id id);
unsigned32 rtems_monitor_dump_name(rtems_name name);
unsigned32 rtems_monitor_dump_priority(rtems_task_priority priority);
unsigned32 rtems_monitor_dump_state(States_Control state);
unsigned32 rtems_monitor_dump_modes(rtems_mode modes);
unsigned32 rtems_monitor_dump_attributes(rtems_attribute attributes);
unsigned32 rtems_monitor_dump_events(rtems_event_set events);
unsigned32 rtems_monitor_dump_notepad(unsigned32 *notepad);

/* object.c */
rtems_id   rtems_monitor_id_fixup(rtems_id, unsigned32, rtems_monitor_object_type_t);
rtems_id   rtems_monitor_object_canonical_get(rtems_monitor_object_type_t, rtems_id, void *, unsigned32 *size_p);
rtems_id   rtems_monitor_object_canonical_next(rtems_monitor_object_info_t *, rtems_id, void *);
void      *rtems_monitor_object_next(void *, void *, rtems_id, rtems_id *);
rtems_id   rtems_monitor_object_canonical(rtems_id, void *);
void       rtems_monitor_object_cmd(int, char **, unsigned32, boolean);

/* manager.c */
void      *rtems_monitor_manager_next(void *, void *, rtems_id *);

/* config.c */
void       rtems_monitor_config_canonical(rtems_monitor_config_t *, void *);
void      *rtems_monitor_config_next(void *, rtems_monitor_config_t *, rtems_id *);
void       rtems_monitor_config_dump_header(boolean);
void       rtems_monitor_config_dump(rtems_monitor_config_t *, boolean verbose);

/* mpci.c */
#if defined(RTEMS_MULTIPROCESSING)
void       rtems_monitor_mpci_canonical(rtems_monitor_mpci_t *, void *);
void      *rtems_monitor_mpci_next(void *, rtems_monitor_mpci_t *, rtems_id *);
void       rtems_monitor_mpci_dump_header(boolean);
void       rtems_monitor_mpci_dump(rtems_monitor_mpci_t *, boolean verbose);
#endif

/* itask.c */
void       rtems_monitor_init_task_canonical(rtems_monitor_init_task_t *, void *);
void      *rtems_monitor_init_task_next(void *, rtems_monitor_init_task_t *, rtems_id *);
void       rtems_monitor_init_task_dump_header(boolean);
void       rtems_monitor_init_task_dump(rtems_monitor_init_task_t *, boolean verbose);

/* extension.c */
void       rtems_monitor_extension_canonical(rtems_monitor_extension_t *, void *);
void       rtems_monitor_extension_dump_header(boolean verbose);
void       rtems_monitor_extension_dump(rtems_monitor_extension_t *, boolean);

/* task.c */
void    rtems_monitor_task_canonical(rtems_monitor_task_t *, void *);
void    rtems_monitor_task_dump_header(boolean verbose);
void    rtems_monitor_task_dump(rtems_monitor_task_t *, boolean);

/* queue.c */
void    rtems_monitor_queue_canonical(rtems_monitor_queue_t *, void *);
void    rtems_monitor_queue_dump_header(boolean verbose);
void    rtems_monitor_queue_dump(rtems_monitor_queue_t *, boolean);

/* driver.c */
void    *rtems_monitor_driver_next(void *, rtems_monitor_driver_t *, rtems_id *);
void     rtems_monitor_driver_canonical(rtems_monitor_driver_t *, void *);
void     rtems_monitor_driver_dump_header(boolean);
void     rtems_monitor_driver_dump(rtems_monitor_driver_t *, boolean);

/* dname.c */
void    *rtems_monitor_dname_next(void *, rtems_monitor_dname_t *, rtems_id *);
void     rtems_monitor_dname_canonical(rtems_monitor_dname_t *, void *);
void     rtems_monitor_dname_dump_header(boolean);
void     rtems_monitor_dname_dump(rtems_monitor_dname_t *, boolean);

/* symbols.c */
rtems_symbol_table_t *rtems_symbol_table_create();
void                  rtems_symbol_table_destroy(rtems_symbol_table_t *table);

rtems_symbol_t *rtems_symbol_create(rtems_symbol_table_t *, char *, unsigned32);
rtems_symbol_t *rtems_symbol_value_lookup(rtems_symbol_table_t *, unsigned32);
const rtems_symbol_t *rtems_symbol_value_lookup_exact(rtems_symbol_table_t *, unsigned32);
rtems_symbol_t *rtems_symbol_name_lookup(rtems_symbol_table_t *, char *);
void   *rtems_monitor_symbol_next(void *object_info, rtems_monitor_symbol_t *, rtems_id *);
void    rtems_monitor_symbol_canonical(rtems_monitor_symbol_t *, rtems_symbol_t *);
void    rtems_monitor_symbol_canonical_by_name(rtems_monitor_symbol_t *, char *);
void    rtems_monitor_symbol_canonical_by_value(rtems_monitor_symbol_t *, void *);
unsigned32 rtems_monitor_symbol_dump(rtems_monitor_symbol_t *, boolean);
void    rtems_monitor_symbol_cmd(int, char **, unsigned32, boolean);


extern rtems_symbol_table_t *rtems_monitor_symbols;

#ifndef MONITOR_PROMPT
#define MONITOR_PROMPT "rtems"		/* will have '> ' appended */
#endif

#define MONITOR_WAKEUP_EVENT   RTEMS_EVENT_0


#define STREQ(a,b)	(strcmp(a,b) == 0)
#define STRNEQ(a,b,n)	(strncmp(a,b,n) == 0)

#ifdef __cplusplus
}
#endif

#endif  /* ! __MONITOR_H */
