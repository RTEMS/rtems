/* mqueue.h
 *
 *  $Id$
 */

#ifndef __POSIX_MESSAGE_QUEUE_h
#define __POSIX_MESSAGE_QUEUE_h

#include <rtems/posix/features.h>

#if defined(_POSIX_MESSAGE_PASSING)

#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>

/*
 *  15.1.1 Data Structures, P1003.1b-1993, p. 271
 */

typedef Objects_Id  mqd_t;

struct mq_attr {
  long  mq_flags;     /* Message queue flags */
  long  mq_maxmsg;    /* Maximum number of messages */
  long  mq_msgsize;   /* Maximum message size */
  long  mq_curmsgs;   /* Number of messages currently queued */
};

/*
 *  15.2.2 Open a Message Queue, P1003.1b-1993, p. 272
 */

mqd_t mq_open(
  const char *name,
  int         oflag,
  ...
);

/*
 *  15.2.2 Close a Message Queue, P1003.1b-1993, p. 275
 */

int mq_close(
  mqd_t  mqdes
);

/*
 *  15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 */

int mq_unlink(
  const char *name
);

/*
 *  15.2.4 Send a Message to a Message Queue, P1003.1b-1993, p. 277
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedsend().
 */

int mq_send(
  mqd_t         mqdes,
  const char   *msg_ptr,
  size_t        msg_len,
  unsigned int  msg_prio 
);

#if defined(_POSIX_TIMEOUTS)

#include <time.h>

int mq_timedsend(
  mqd_t                  mqdes,
  const char            *msg_ptr,
  size_t                 msg_len,
  unsigned int           msg_prio,
  const struct timespec *timeout
);

#endif /* _POSIX_TIMEOUTS */

/*
 *  15.2.5 Receive a Message From a Message Queue, P1003.1b-1993, p. 279
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedreceive().
 */

ssize_t mq_receive(
  mqd_t         mqdes,
  char         *msg_ptr,
  size_t        msg_len,
  unsigned int *msg_prio
);

#if defined(_POSIX_TIMEOUTS)

int mq_timedreceive(                  /* XXX: should this be ssize_t */
  mqd_t                  mqdes,
  char                  *msg_ptr,
  size_t                 msg_len,
  unsigned int          *msg_prio,
  const struct timespec *timeout
);

#endif /* _POSIX_TIMEOUTS */

#if defined(_POSIX_REALTIME_SIGNALS)

/*
 *  15.2.6 Notify Process that a Message is Available on a Queue, 
 *         P1003.1b-1993, p. 280
 */

int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
);

#endif /* _POSIX_REALTIME_SIGNALS */

/*
 *  15.2.7 Set Message Queue Attributes, P1003.1b-1993, p. 281
 */

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *mqstat,
  struct mq_attr       *omqstat
);

/*
 *  15.2.8 Get Message Queue Attributes, P1003.1b-1993, p. 283
 */

int mq_getattr(
  mqd_t           mqdes,
  struct mq_attr *mqstat
);

#endif /* _POSIX_MESSAGE_PASSING */

#endif
/* end of include file */
