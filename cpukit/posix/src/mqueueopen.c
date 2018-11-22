/**
 * @file
 *
 * @brief Creates a new POSIX Message Queue or Opens an Existing Queue 
 * @ingroup POSIXAPI
 */

/*
 *  NOTE:  The structure of the routines is identical to that of POSIX
 *         Message_queues to leave the option of having unnamed message
 *         queues at a future date.  They are currently not part of the
 *         POSIX standard but unnamed message_queues are.  This is also
 *         the reason for the apparently unnecessary tracking of
 *         the process_shared attribute.  [In addition to the fact that
 *         it would be trivial to add pshared to the mq_attr structure
 *         and have process private message queues.]
 *
 *         This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *         time.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/mqueueimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/sysinit.h>

#include <stdarg.h>
#include <fcntl.h>

#define MQ_OPEN_FAILED ((mqd_t) -1)

/*
 *  There is no real basis for the default values.  They will work
 *  but were not compared against any existing implementation for
 *  compatibility.  See README.mqueue for an example program we
 *  think will print out the defaults.  Report anything you find with it.
 */
static const struct mq_attr _POSIX_Message_queue_Default_attributes = {
  .mq_maxmsg = 10,
  .mq_msgsize = 16
};

static mqd_t _POSIX_Message_queue_Create(
  const char           *name_arg,
  size_t                name_len,
  int                   oflag,
  const struct mq_attr *attr
)
{
  POSIX_Message_queue_Control  *the_mq;
  char                         *name;

  /* length of name has already been validated */

  if ( attr->mq_maxmsg <= 0 ){
    rtems_set_errno_and_return_value( EINVAL, MQ_OPEN_FAILED );
  }

  if ( attr->mq_msgsize <= 0 ){
    rtems_set_errno_and_return_value( EINVAL, MQ_OPEN_FAILED );
  }

  the_mq = _POSIX_Message_queue_Allocate_unprotected();
  if ( !the_mq ) {
    rtems_set_errno_and_return_value( ENFILE, MQ_OPEN_FAILED );
  }

  /*
   * Make a copy of the user's string for name just in case it was
   * dynamically constructed.
   */
  name = _Workspace_String_duplicate( name_arg, name_len );
  if ( !name ) {
    _POSIX_Message_queue_Free( the_mq );
    rtems_set_errno_and_return_value( ENOMEM, MQ_OPEN_FAILED );
  }

  the_mq->open_count = 1;
  the_mq->linked = true;
  the_mq->oflag = oflag;

  /*
   *  NOTE: That thread blocking discipline should be based on the
   *  current scheduling policy.
   *
   *  Joel: Cite POSIX or OpenGroup on above statement so we can determine
   *        if it is a real requirement.
   */
  if (
    !_CORE_message_queue_Initialize(
      &the_mq->Message_queue,
      CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO,
      attr->mq_maxmsg,
      attr->mq_msgsize
    )
  ) {
    _POSIX_Message_queue_Free( the_mq );
    _Workspace_Free( name );
    rtems_set_errno_and_return_value( ENOSPC, MQ_OPEN_FAILED );
  }

  _Objects_Open_string(
    &_POSIX_Message_queue_Information,
    &the_mq->Object,
    name
  );
  return the_mq->Object.id;
}

/*
 *  15.2.2 Open a Message Queue, P1003.1b-1993, p. 272
 */
mqd_t mq_open(
  const char *name,
  int         oflag,
  ...
  /* mode_t mode, */
  /* struct mq_attr  attr */
)
{
  POSIX_Message_queue_Control *the_mq;
  size_t                       name_len;
  Objects_Get_by_name_error    error;
  mqd_t                        status;

  _Objects_Allocator_lock();
  the_mq = _POSIX_Message_queue_Get_by_name( name, &name_len, &error );

  /*
   *  If the name to id translation worked, then the message queue exists
   *  and we can just return a pointer to the id.  Otherwise we may
   *  need to check to see if this is a "message queue does not exist"
   *  or some other miscellaneous error on the name.
   */
  if ( the_mq == NULL ) {
    va_list               ap;
    const struct mq_attr *attr;

    /*
     * Unless provided a valid name that did not already exist
     * and we are willing to create then it is an error.
     */

    if ( error != OBJECTS_GET_BY_NAME_NO_OBJECT || ( oflag & O_CREAT ) == 0 ) {
      _Objects_Allocator_unlock();
      rtems_set_errno_and_return_value(
        _POSIX_Get_by_name_error( error ),
        MQ_OPEN_FAILED
      );
    }

    va_start( ap, oflag );
    va_arg( ap, mode_t );
    attr = va_arg( ap, const struct mq_attr * );
    va_end( ap );

    if ( attr == NULL ) {
      attr = &_POSIX_Message_queue_Default_attributes;
    }

    /*
     *  At this point, the message queue does not exist and everything has been
     *  checked. We should go ahead and create a message queue.
     */

    status = _POSIX_Message_queue_Create( name, name_len, oflag, attr );
  } else {

    /*
     * Check for existence with creation.
     */

    if ( ( oflag & ( O_CREAT | O_EXCL ) ) == ( O_CREAT | O_EXCL ) ) {
      _Objects_Allocator_unlock();
      rtems_set_errno_and_return_value( EEXIST, MQ_OPEN_FAILED );
    }

    the_mq->open_count += 1;
    status = the_mq->Object.id;
  }

  _Objects_Allocator_unlock();
  return status;
}

static void _POSIX_Message_queue_Manager_initialization( void )
{
  _Objects_Initialize_information( &_POSIX_Message_queue_Information );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Message_queue_Manager_initialization,
  RTEMS_SYSINIT_POSIX_MESSAGE_QUEUE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
