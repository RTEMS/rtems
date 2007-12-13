/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <aio.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>

/*PAGE
 *
 *  6.7.2 Asynchronous Read, P1003.1b-1993, p. 154
 */

int aio_read(
  struct aiocb  *aiocbp
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.3 Asynchronous Write, P1003.1b-1993, p. 155
 */

int aio_write(
  struct aiocb  *aiocbp
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.4 List Directed I/O, P1003.1b-1993, p. 158
 */

int lio_listio(
  int                    mode,
  struct aiocb  * const  list[],
  int                    nent,
  struct sigevent       *sig
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.5 Retrieve Error of Asynchronous I/O Operation, P1003.1b-1993, p. 161
 */

int aio_error(
  const struct aiocb  *aiocbp
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.6 Retrieve Return Status of Asynchronous I/O Operation,
 *        P1003.1b-1993, p. 162
 */

int aio_return(
  const struct aiocb  *aiocbp
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.7 Cancel Asynchronous I/O Operation, P1003.1b-1993, p. 163
 */

int aio_cancel(
  int            filedes,
  struct aiocb  *aiocbp
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.7 Wait for Asynchronous I/O Request, P1003.1b-1993, p. 164
 */

int aio_suspend(
  struct aiocb  * const   list[],
  int                     nent,
  const struct timespec  *timeout
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  6.7.9 Asynchronous File Synchronization, P1003.1b-1993, p. 166
 */

int aio_fsync(
  int            op,
  struct aiocb  *aiocbp
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
