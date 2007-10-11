/* $Id$ */

/* libi2c Implementation */

/* 
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/libio.h>

#include <rtems/libi2c.h>

#define DRVNM "libi2c:"

#define MAX_NO_BUSSES	8       /* Also limited by the macro building minor numbers */
#define MAX_NO_DRIVERS	16      /* Number of high level drivers we support          */

#define MINOR2ADDR(minor)	((minor)&((1<<10)-1))
#define MINOR2BUS(minor)	(((minor)>>10)&7)
#define MINOR2DRV(minor)	((minor)>>13)

/* Check the 'minor' argument, i.e., verify that
 * we have a driver connected
 */
#define DECL_CHECKED_BH(b, bh, m, s)\
	unsigned b = MINOR2BUS(m);		\
	rtems_libi2c_bus_t	*bh;		\
	if ( b >= MAX_NO_BUSSES || 0 == (bh=busses[b].bush) ) {	\
		return s RTEMS_INVALID_NUMBER;	\
	}

#define DECL_CHECKED_DRV(d, b, m)	\
	unsigned d = MINOR2DRV(m);		\
	unsigned b = MINOR2BUS(m);		\
	if (   b >= MAX_NO_BUSSES  || 0 == busses[b].bush 	\
		|| d >  MAX_NO_DRIVERS || (d && 0 == drvs[d-1].drv )) {\
		return RTEMS_INVALID_NUMBER;	\
	}

#define DISPATCH(rval, entry, dflt)	\
	do {				\
		rtems_driver_address_table *ops = drvs[--drv].drv->ops;	\
		rval = ops->entry ? ops->entry(major,minor,arg) : dflt;	\
	} while (0)


rtems_device_major_number rtems_libi2c_major;

static struct i2cbus
{
  rtems_libi2c_bus_t *bush;
  volatile rtems_id mutex;      /* lock this across start -> stop */
  volatile short waiting;
  volatile char started;
  char *name;
} busses[MAX_NO_BUSSES] = { { 0 } };

static struct
{
  rtems_libi2c_drv_t *drv;
} drvs[MAX_NO_DRIVERS] = { { 0} };

static rtems_id libmutex = 0;

#define LOCK(m)		assert(!rtems_semaphore_obtain((m), RTEMS_WAIT, RTEMS_NO_TIMEOUT))
#define UNLOCK(m)	rtems_semaphore_release((m))

#define LIBLOCK()	LOCK(libmutex)
#define LIBUNLOCK()	UNLOCK(libmutex)

#define MUTEX_ATTS 	\
	(  RTEMS_PRIORITY			\
	 | RTEMS_BINARY_SEMAPHORE	\
	 |RTEMS_INHERIT_PRIORITY	\
	 |RTEMS_NO_PRIORITY_CEILING	\
	 |RTEMS_LOCAL )

static rtems_id
mutexCreate (rtems_name nm)
{
  rtems_status_code sc;
  rtems_id rval;
  if (RTEMS_SUCCESSFUL !=
      (sc = rtems_semaphore_create (nm, 1, MUTEX_ATTS, 0, &rval))) {
    rtems_error (sc, DRVNM " unable to create mutex\n");
    return 0;
  }
  return rval;
}

/* Lock a bus avoiding to have a mutex, which is mostly
 * unused, hanging around all the time. We just create
 * and delete it on the fly...
 *
 * ASSUMES: argument checked by caller
 */

static void
lock_bus (int busno)
{
  struct i2cbus *bus = &busses[busno];
  LIBLOCK ();
  if (!bus->waiting) {
    /* nobody is holding the bus mutex - it's not there. Create it on the fly */
    if (!
        (bus->mutex =
         mutexCreate (rtems_build_name ('i', '2', 'c', '0' + busno)))) {
      LIBUNLOCK ();
      rtems_panic (DRVNM " unable to create bus lock");
    }
  }
  /* count number of people waiting on this bus; only the last one deletes the mutex */
  bus->waiting++;
  LIBUNLOCK ();
  /* Now lock this bus */
  LOCK (bus->mutex);
}

static void
unlock_bus (int busno)
{
  struct i2cbus *bus = &busses[busno];
  LIBLOCK ();
  UNLOCK (bus->mutex);
  if (!--bus->waiting) {
    rtems_semaphore_delete (bus->mutex);
  }
  LIBUNLOCK ();
}

/* Note that 'arg' is always passed in as NULL */
static rtems_status_code
i2c_init (rtems_device_major_number major, rtems_device_minor_number minor,
          void *arg)
{
  rtems_status_code rval;
  DECL_CHECKED_DRV (drv, busno, minor)

    if (0 == drv) {
    rval = 0;
  } else {
    /* this is probably never called */
    DISPATCH (rval, initialization_entry, RTEMS_SUCCESSFUL);
  }
  return rval;
}

static rtems_status_code
i2c_open (rtems_device_major_number major, rtems_device_minor_number minor,
          void *arg)
{
  rtems_status_code rval;
  DECL_CHECKED_DRV (drv, busno, minor)

    if (0 == drv) {
    rval = RTEMS_SUCCESSFUL;
  } else {
    DISPATCH (rval, open_entry, RTEMS_SUCCESSFUL);
  }
  return rval;
}

static rtems_status_code
i2c_close (rtems_device_major_number major, rtems_device_minor_number minor,
           void *arg)
{
  rtems_status_code rval;
  DECL_CHECKED_DRV (drv, busno, minor)

    if (0 == drv) {
    rval = RTEMS_SUCCESSFUL;
  } else {
    DISPATCH (rval, close_entry, RTEMS_SUCCESSFUL);
  }
  return rval;
}

static rtems_status_code
i2c_read (rtems_device_major_number major, rtems_device_minor_number minor,
          void *arg)
{
  int rval;                     /* int so we can check for negative value */
  rtems_libio_rw_args_t *rwargs = arg;
  DECL_CHECKED_DRV (drv, busno, minor)

    if (0 == rwargs->count) {
    rwargs->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  if (0 == drv) {
    rval =
      rtems_libi2c_start_read_bytes (minor, (unsigned char *) rwargs->buffer,
                                     rwargs->count);
    if (rval >= 0) {
      rwargs->bytes_moved = rval;
      rtems_libi2c_send_stop (minor);
      rval = RTEMS_SUCCESSFUL;
    } else {
      rval = -rval;
    }
  } else {
    DISPATCH (rval, read_entry, RTEMS_NOT_IMPLEMENTED);
  }
  return rval;
}

static rtems_status_code
i2c_write (rtems_device_major_number major, rtems_device_minor_number minor,
           void *arg)
{
  int rval;                     /* int so we can check for negative value */
  rtems_libio_rw_args_t *rwargs = arg;
  DECL_CHECKED_DRV (drv, busno, minor)

    if (0 == rwargs->count) {
    rwargs->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  if (0 == drv) {
    rval =
      rtems_libi2c_start_write_bytes (minor, (unsigned char *) rwargs->buffer,
                                      rwargs->count);
    if (rval >= 0) {
      rwargs->bytes_moved = rval;
      rtems_libi2c_send_stop (minor);
      rval = RTEMS_SUCCESSFUL;
    } else {
      rval = -rval;
    }
  } else {
    DISPATCH (rval, write_entry, RTEMS_NOT_IMPLEMENTED);
  }
  return rval;
}

static rtems_status_code
i2c_ioctl (rtems_device_major_number major, rtems_device_minor_number minor,
           void *arg)
{
  rtems_status_code rval;
  DECL_CHECKED_DRV (drv, busno, minor)

    if (0 == drv) {
    rval = RTEMS_NOT_IMPLEMENTED;
  } else {
    DISPATCH (rval, control_entry, RTEMS_NOT_IMPLEMENTED);
  }
  return rval;
}


/* Our ops just dispatch to the registered drivers */
static rtems_driver_address_table libi2c_io_ops = {
  initialization_entry:  i2c_init,
  open_entry:            i2c_open,
  close_entry:           i2c_close,
  read_entry:            i2c_read,
  write_entry:           i2c_write,
  control_entry:         i2c_ioctl,
};


int
rtems_libi2c_initialize ()
{
  rtems_status_code sc;

  if (!(libmutex = mutexCreate (rtems_build_name ('l', 'I', '2', 'C'))))
    return -1;

  sc = rtems_io_register_driver (0, &libi2c_io_ops, &rtems_libi2c_major);
  if (RTEMS_SUCCESSFUL != sc) {
    fprintf (stderr,
             DRVNM " Claiming driver slot failed (rtems status code %i)\n",
             sc);
    rtems_semaphore_delete (libmutex);
    libmutex = 0;
    return -1;
  }

  return 0;
}

int
rtems_libi2c_register_bus (char *name, rtems_libi2c_bus_t * bus)
{
  int i;
  rtems_status_code err;
  char *nmcpy = malloc (name ? strlen (name) + 1 : 20);
  char tmp, *chpt;
  struct stat sbuf;

  strcpy (nmcpy, name ? name : "/dev/i2c");


  /* check */
  if ('/' != *nmcpy) {
    fprintf (stderr,
             "Bad name; must be an absolute path starting with '/'\n");
    return -RTEMS_INVALID_NAME;
  }
  /* file must not exist */
  if (!stat (nmcpy, &sbuf)) {
    fprintf (stderr, "Bad name; file exists already\n");
    return -RTEMS_INVALID_NAME;
  }

  /* we already verified that there is at least one '/' */
  chpt = strrchr (nmcpy, '/') + 1;
  tmp = *chpt;
  *chpt = 0;
  i = stat (nmcpy, &sbuf);
  *chpt = tmp;
  if (i) {
    fprintf (stderr, "Get %s status failed: %s\n",
             nmcpy,strerror(errno));
    return -RTEMS_INVALID_NAME;
  }
  /* should be a directory since name terminates in '/' */


  if (!libmutex) {
    fprintf (stderr, DRVNM " library not initialized\n");
    return -RTEMS_NOT_DEFINED;
  }

  if (bus->size < sizeof (*bus)) {
    fprintf (stderr, DRVNM " bus-ops size too small -- misconfiguration?\n");
    return -RTEMS_NOT_CONFIGURED;
  }

  LIBLOCK ();
  for (i = 0; i < MAX_NO_BUSSES; i++) {
    if (!busses[i].bush) {
      /* found a free slot */
      busses[i].bush = bus;
      busses[i].mutex = 0;
      busses[i].waiting = 0;
      busses[i].started = 0;

      if (!name)
        sprintf (nmcpy + strlen (nmcpy), "%i", i);

      if ((err = busses[i].bush->ops->init (busses[i].bush))) {
        /* initialization failed */
        i = -err;
      } else {
        busses[i].name = nmcpy;;
        nmcpy = 0;
      }

      break;
    }
  }
  LIBUNLOCK ();

  if (i >= MAX_NO_BUSSES) {
    i = -RTEMS_TOO_MANY;
  }

  free (nmcpy);

  return i;
}

static int
not_started (int busno)
{
  int rval;
  lock_bus (busno);
  rval = !busses[busno].started;
  unlock_bus (busno);
  return rval;
}

rtems_status_code
rtems_libi2c_send_start (uint32_t minor)
{
  int rval;
  DECL_CHECKED_BH (busno, bush, minor, +)

    lock_bus (busno);
  rval = bush->ops->send_start (bush);

  /* if this failed or is not the first start, unlock */
  if (rval || busses[busno].started) {
    /* HMM - what to do if the 1st start failed ?
     * try to reset...
     */
    if (!busses[busno].started) {
      /* just in case the bus driver fiddles with errno */
      int errno_saved = errno;
      bush->ops->init (bush);
      errno = errno_saved;
    } else if (rval) {
      /* failed restart */
      rtems_libi2c_send_stop (minor);
    }
    unlock_bus (busno);
  } else {
    /* successful 1st start; keep bus locked until stop is sent */
    busses[busno].started = 1;
  }
  return rval;
}

rtems_status_code
rtems_libi2c_send_stop (uint32_t minor)
{
  rtems_status_code rval;
  DECL_CHECKED_BH (busno, bush, minor, +)

    if (not_started (busno))
    return RTEMS_NOT_OWNER_OF_RESOURCE;

  rval = bush->ops->send_stop (bush);

  busses[busno].started = 0;

  unlock_bus (busno);
  return rval;
}

rtems_status_code
rtems_libi2c_send_addr (uint32_t minor, int rw)
{
  rtems_status_code sc;
  DECL_CHECKED_BH (busno, bush, minor, +)

    if (not_started (busno))
    return RTEMS_NOT_OWNER_OF_RESOURCE;

  sc = bush->ops->send_addr (bush, MINOR2ADDR (minor), rw);
  if (RTEMS_SUCCESSFUL != sc)
    rtems_libi2c_send_stop (minor);
  return sc;
}

int
rtems_libi2c_read_bytes (uint32_t minor, unsigned char *bytes, int nbytes)
{
  int sc;
  DECL_CHECKED_BH (busno, bush, minor, -)

    if (not_started (busno))
    return -RTEMS_NOT_OWNER_OF_RESOURCE;

  sc = bush->ops->read_bytes (bush, bytes, nbytes);
  if (sc < 0)
    rtems_libi2c_send_stop (minor);
  return sc;
}

int
rtems_libi2c_write_bytes (uint32_t minor, unsigned char *bytes, int nbytes)
{
  int sc;
  DECL_CHECKED_BH (busno, bush, minor, -)

    if (not_started (busno))
    return -RTEMS_NOT_OWNER_OF_RESOURCE;

  sc = bush->ops->write_bytes (bush, bytes, nbytes);
  if (sc < 0)
    rtems_libi2c_send_stop (minor);
  return sc;
}

static int
do_s_rw (uint32_t minor, unsigned char *bytes, int nbytes, int rw)
{
  rtems_status_code sc;
  rtems_libi2c_bus_t *bush;

  if ((sc = rtems_libi2c_send_start (minor)))
    return -sc;

  /* at this point, we hold the bus and are sure the minor number is valid */
  bush = busses[MINOR2BUS (minor)].bush;

  if ((sc = bush->ops->send_addr (bush, MINOR2ADDR (minor), rw))) {
    rtems_libi2c_send_stop (minor);
    return -sc;
  }

  if (rw)
    sc = bush->ops->read_bytes (bush, bytes, nbytes);
  else
    sc = bush->ops->write_bytes (bush, bytes, nbytes);

  if (sc < 0) {
    rtems_libi2c_send_stop (minor);
  }
  return sc;
}

int
rtems_libi2c_start_read_bytes (uint32_t minor, unsigned char *bytes,
                               int nbytes)
{
  return do_s_rw (minor, bytes, nbytes, 1);
}

int
rtems_libi2c_start_write_bytes (uint32_t minor, unsigned char *bytes,
                                int nbytes)
{
  return do_s_rw (minor, bytes, nbytes, 0);
}

int
rtems_libi2c_register_drv (char *name, rtems_libi2c_drv_t * drvtbl,
                           unsigned busno, unsigned i2caddr)
{
  int i;
  rtems_status_code err;
  rtems_device_minor_number minor;

  if (!libmutex) {
    fprintf (stderr, DRVNM " library not initialized\n");
    return -RTEMS_NOT_DEFINED;
  }

  if (name && strchr (name, '/')) {
    fprintf (stderr, "Invalid name: '%s' -- must not contain '/'\n", name);
    return -RTEMS_INVALID_NAME;
  }

  if (busno >= MAX_NO_BUSSES || !busses[busno].bush || i2caddr >= 1 << 10) {
    errno = EINVAL;
    return -RTEMS_INVALID_NUMBER;
  }

  if (drvtbl->size < sizeof (*drvtbl)) {
    fprintf (stderr, DRVNM " drv-ops size too small -- misconfiguration?\n");
    return -RTEMS_NOT_CONFIGURED;
  }

  /* allocate slot */
  LIBLOCK ();
  for (i = 0; i < MAX_NO_DRIVERS; i++) {
    /* driver # 0 is special, it is the built-in raw driver */
    if (!drvs[i].drv) {
      char *str;
      dev_t dev;
      uint32_t mode;

      /* found a free slot; encode slot + 1 ! */
      minor = ((i + 1) << 13) | RTEMS_LIBI2C_MAKE_MINOR (busno, i2caddr);

      if (name) {
        str = malloc (strlen (busses[busno].name) + strlen (name) + 2);
        sprintf (str, "%s.%s", busses[busno].name, name);

        dev = rtems_filesystem_make_dev_t (rtems_libi2c_major, minor);

        mode = 0111 | S_IFCHR;
        if (drvtbl->ops->read_entry)
          mode |= 0444;
        if (drvtbl->ops->write_entry)
          mode |= 0222;

        /* note that 'umask' is applied to 'mode' */
        if (mknod (str, mode, dev)) {
          fprintf (stderr,
                   "Creating device node failed: %s; you can try to do it manually...\n",
                   strerror (errno));
        }

        free (str);
      }

      drvs[i].drv = drvtbl;

      if (drvtbl->ops->initialization_entry)
        err =
          drvs[i].drv->ops->initialization_entry (rtems_libi2c_major, minor,
                                                  0);
      else
        err = RTEMS_SUCCESSFUL;

      LIBUNLOCK ();
      return err ? -err : minor;
    }
  }
  LIBUNLOCK ();
  return -RTEMS_TOO_MANY;
}
