@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter ATA Driver

@section Terms

ATA device - physical device attached to an IDE controller 

@section Introduction

ATA driver provides generic interface to an ATA device. ATA driver is
hardware independent implementation of ATA standard defined in working draft
"AT Attachment Interface with Extensions (ATA-2)" X3T10/0948D revision 4c, 
March 18, 1996. ATA Driver based on IDE Controller Driver and may be used for 
computer systems with single IDE controller and with multiple as well. Although
current implementation has several restrictions detailed below ATA driver
architecture allows easily extend the driver. Current restrictions are:

@itemize @bullet
@item Only mandatory (see draft p.29) and two optional (READ/WRITE MULTIPLE)
commands are implemented
@item Only PIO mode is supported but both poll and interrupt driven
@end itemize

The reference implementation for ATA driver can be found in 
@code{cpukit/libblock/src/ata.c}.  

@section Initialization

The @code{ata_initialize} routine is responsible for ATA driver
initialization. The main goal of the initialization is to detect and 
register in the system all ATA devices attached to IDE controllers 
successfully initialized by the IDE Controller driver. 

In the implementation of the driver, the following actions are performed:

@example
@group
rtems_device_driver ata_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
   initialize internal ATA driver data structure 

   for each IDE controller successfully initialized by the IDE Controller 
       driver
     if the controller is interrupt driven
       set up interrupt handler 

     obtain information about ATA devices attached to the controller
     with help of EXECUTE DEVICE DIAGNOSTIC command

     for each ATA device detected on the controller
       obtain device parameters with help of DEVICE IDENTIFY command

       register new ATA device as new block device in the system
@}
@end group
@end example

Special processing of ATA commands is required because of absence of 
multitasking environment during the driver initialization.

Detected ATA devices are registered in the system as physical block devices
(see libblock library description). Device names are formed based on IDE
controller minor number device is attached to and device number on the 
controller (0 - Master, 1 - Slave). In current implementation 64 minor 
numbers are reserved for each ATA device which allows to support up to 63
logical partitions per device.

@example
@group
controller minor    device number    device name    ata device minor
      0                  0             hda                0
      0                  1             hdb               64
      1                  0             hdc              128
      1                  1             hdd              172
     ...                ...            ...
@end group
@end example

@section ATA Driver Architecture

@subsection ATA Driver Main Internal Data Structures

ATA driver works with ATA requests. ATA request is described by the
following structure:

@example
@group
/* ATA request */
typedef struct ata_req_s @{
    Chain_Node        link;   /* link in requests chain */
    char              type;   /* request type */
    ata_registers_t   regs;   /* ATA command */
    uint32_t          cnt;    /* Number of sectors to be exchanged */
    uint32_t          cbuf;   /* number of current buffer from breq in use */
    uint32_t          pos;    /* current position in 'cbuf' */
    blkdev_request   *breq;   /* blkdev_request which corresponds to the
                               * ata request
                               */
    rtems_id          sema;   /* semaphore which is used if synchronous
                               * processing of the ata request is required
                               */
    rtems_status_code status; /* status of ata request processing */
    int               error;  /* error code */
@} ata_req_t;
@end group
@end example

ATA driver supports separate ATA requests queues for each IDE
controller (one queue per controller). The following structure contains 
information about controller's queue and devices attached to the controller:

@example
@group
/*
 * This structure describes controller state, devices configuration on the
 * controller and chain of ATA requests to the controller.
 */
typedef struct ata_ide_ctrl_s @{
    bool          present;   /* controller state */
    ata_dev_t     device[2]; /* ata devices description */
    Chain_Control reqs;      /* requests chain */
@} ata_ide_ctrl_t;
@end group
@end example

Driver uses array of the structures indexed by the controllers minor
number.

The following structure allows to map an ATA device to the pair (IDE 
controller minor number device is attached to, device number
on the controller): 

@example
@group
/*
 * Mapping of rtems ATA devices to the following pairs:
 * (IDE controller number served the device, device number on the controller)
 */
typedef struct ata_ide_dev_s @{
    int ctrl_minor;/* minor number of IDE controller serves rtems ATA device */
    int device;    /* device number on IDE controller (0 or 1) */
@} ata_ide_dev_t;
@end group
@end example

Driver uses array of the structures indexed by the ATA devices minor
number.

ATA driver defines the following internal events:

@example
@group
/* ATA driver events */
typedef enum ata_msg_type_s @{
    ATA_MSG_GEN_EVT = 1,     /* general event */
    ATA_MSG_SUCCESS_EVT,     /* success event */
    ATA_MSG_ERROR_EVT,       /* error event */ 
    ATA_MSG_PROCESS_NEXT_EVT /* process next ata request event */
@} ata_msg_type_t;
@end group
@end example

@subsection Brief ATA Driver Core Overview

All ATA driver functionality is available via ATA driver ioctl. Current
implementation supports only two ioctls: BLKIO_REQUEST and
ATAIO_SET_MULTIPLE_MODE. Each ATA driver ioctl() call generates an 
ATA request which is appended to the appropriate controller queue depending
on ATA device the request belongs to. If appended request is single request in
the controller's queue then ATA driver event is generated.

ATA driver task which manages queue of ATA driver events is core of ATA
driver. In current driver version queue of ATA driver events implemented
as RTEMS message queue. Each message contains event type, IDE controller
minor number on which event happened and error if an error occurred. Events
may be generated either by ATA driver ioctl call or by ATA driver task itself.
Each time ATA driver task receives an event it gets controller minor number
from event, takes first ATA request from controller queue and processes it 
depending on request and event types. An ATA request processing may also
includes sending of several events. If ATA request processing is finished
the ATA request is removed from the controller queue. Note, that in current
implementation maximum one event per controller may be queued at any moment
of the time.

(This part seems not very clear, hope I rewrite it soon) 



