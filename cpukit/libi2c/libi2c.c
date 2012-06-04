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
/*
 * adaptations to also handle SPI devices
 * by Thomas Doerfler, embedded brains GmbH, Puchheim, Germany
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>

#include <rtems/libi2c.h>

#define DRVNM "libi2c: "

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
		const rtems_driver_address_table *ops = drvs[--drv].drv->ops;	\
		rval = ops->entry ? ops->entry(major,minor,arg) : dflt;	\
	} while (0)


rtems_device_major_number rtems_libi2c_major;

static bool is_initialized = false;

static struct i2cbus
{
  rtems_libi2c_bus_t *bush;
  volatile rtems_id mutex;      /* lock this across start -> stop */
  volatile bool started;
  char *name;
} busses[MAX_NO_BUSSES] = { { NULL, RTEMS_ID_NONE, false, NULL } };

static struct
{
  rtems_libi2c_drv_t *drv;
} drvs[MAX_NO_DRIVERS] = { { NULL } };

static rtems_id libmutex = RTEMS_ID_NONE;

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

/* During early stages of life, stdio is not available */

static void
safe_printf (const char *fmt, ...)
{
va_list ap;

	va_start(ap, fmt);
	if ( _System_state_Is_up( _System_state_Get() ) )
		vfprintf( stderr, fmt, ap );
	else
		vprintk( fmt, ap );
	va_end(ap);
}

static rtems_status_code
mutexCreate (rtems_name nm, rtems_id *pm)
{
  rtems_status_code sc;

  if (RTEMS_SUCCESSFUL !=
      (sc = rtems_semaphore_create (nm, 1, MUTEX_ATTS, 0, pm))) {
	if ( _System_state_Is_up( _System_state_Get() ) )
    	rtems_error (sc, DRVNM "Unable to create mutex\n");
	else
		printk (DRVNM "Unable to create mutex (status code %i)\n", sc);
  }
  return sc;
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

  if (bus->mutex == RTEMS_ID_NONE) {
    /*
     * Nobody is holding the bus mutex - it's not there.  Create it on the fly.
     */
    LIBLOCK ();
    if (bus->mutex == RTEMS_ID_NONE) {
      rtems_id m = RTEMS_ID_NONE;
      rtems_status_code sc = mutexCreate (
        rtems_build_name ('i', '2', 'c', '0' + busno),
        &m
      );
      if (sc != RTEMS_SUCCESSFUL) {
        LIBUNLOCK ();
        rtems_panic (DRVNM "Unable to create bus lock");
        return;
      }
      bus->mutex = m;
    }
    LIBUNLOCK ();
  }

  /* Now lock this bus */
  LOCK (bus->mutex);
}

static void
unlock_bus (int busno)
{
  struct i2cbus *bus = &busses[busno];
  UNLOCK (bus->mutex);
}

/* Note that 'arg' is always passed in as NULL */
rtems_status_code
rtems_i2c_init (rtems_device_major_number major, rtems_device_minor_number minor,
          void *arg)
{
  rtems_status_code rval;
  /* No busses or drivers can be registered at this point;
   * avoid the macro aborting with an error
  DECL_CHECKED_DRV (drv, busno, minor)
   */

  rval = mutexCreate (rtems_build_name ('l', 'I', '2', 'C'), &libmutex);

  if ( RTEMS_SUCCESSFUL == rval ) {
  	is_initialized     = true;
	rtems_libi2c_major = major;
  } else {
  	libmutex = RTEMS_ID_NONE;
  }
  return rval;
}

rtems_status_code
rtems_i2c_open (rtems_device_major_number major, rtems_device_minor_number minor,
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

rtems_status_code
rtems_i2c_close (rtems_device_major_number major, rtems_device_minor_number minor,
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

rtems_status_code
rtems_i2c_read (rtems_device_major_number major, rtems_device_minor_number minor,
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

rtems_status_code
rtems_i2c_write (rtems_device_major_number major, rtems_device_minor_number minor,
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

rtems_status_code
rtems_i2c_ioctl (rtems_device_major_number major, rtems_device_minor_number minor,
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
const rtems_driver_address_table rtems_libi2c_io_ops = {
  .initialization_entry =  rtems_i2c_init,
  .open_entry =            rtems_i2c_open,
  .close_entry =           rtems_i2c_close,
  .read_entry =            rtems_i2c_read,
  .write_entry =           rtems_i2c_write,
  .control_entry =         rtems_i2c_ioctl,
};

int
rtems_libi2c_initialize (void)
{
  rtems_status_code sc;

  if (is_initialized) {
    /*
     * already called before? then skip this step
     */
    return 0;
  }

  /* rtems_io_register_driver does NOT currently check nor report back
   * the return code of the 'init' operation, so we cannot
   * rely on return code since it may seem OK even if the driver 'init;
   * op failed.
   * Let 'init' handle 'is_initialized'...
   */
  sc = rtems_io_register_driver (0, &rtems_libi2c_io_ops, &rtems_libi2c_major);
  if (RTEMS_SUCCESSFUL != sc) {
    safe_printf(
             DRVNM "Claiming driver slot failed (rtems status code %i)\n",
             sc);
    if (libmutex != RTEMS_ID_NONE) {
      rtems_semaphore_delete (libmutex);
    }
    libmutex = RTEMS_ID_NONE;
    is_initialized = false;
    return -1;
  }

  return 0;
}

int
rtems_libi2c_register_bus (const char *name, rtems_libi2c_bus_t * bus)
{
  int i;
  rtems_status_code err;
  size_t length = (name ? strlen (name) + 1 : 20);
  char *nmcpy = malloc(length);
  char tmp, *chpt;
  struct stat sbuf;

  if (nmcpy == NULL) {
    safe_printf ( DRVNM "No memory\n");
    return -RTEMS_NO_MEMORY;
  }

  strncpy (nmcpy, name ? name : "/dev/i2c", length);

  /* check */
  if ('/' != *nmcpy) {
    safe_printf ( DRVNM "Bad name: must be an absolute path starting with '/'\n");
    free( nmcpy );
    return -RTEMS_INVALID_NAME;
  }
  /* file must not exist */
  if (!stat (nmcpy, &sbuf)) {
    safe_printf ( DRVNM "Bad name: file exists already\n");
    free( nmcpy );
    return -RTEMS_INVALID_NAME;
  }

  /* we already verified that there is at least one '/' */
  chpt = strrchr (nmcpy, '/') + 1;
  tmp = *chpt;
  *chpt = 0;
  i = stat (nmcpy, &sbuf);
  *chpt = tmp;
  if (i) {
    safe_printf ( DRVNM "Get %s status failed: %s\n",
             nmcpy, strerror(errno));
    free( nmcpy );
    return -RTEMS_INVALID_NAME;
  }
  /* should be a directory since name terminates in '/' */


  if (libmutex == RTEMS_ID_NONE) {
    safe_printf ( DRVNM "Library not initialized\n");
    free( nmcpy );
    return -RTEMS_NOT_DEFINED;
  }

  if (bus == NULL || bus->size < sizeof (*bus)) {
    safe_printf ( DRVNM "No bus-ops or size too small -- misconfiguration?\n");
    free( nmcpy );
    return -RTEMS_NOT_CONFIGURED;
  }

  LIBLOCK ();
  for (i = 0; i < MAX_NO_BUSSES; i++) {
    if (!busses[i].bush) {
      /* found a free slot */
      busses[i].bush = bus;
      busses[i].mutex = RTEMS_ID_NONE;
      busses[i].started = false;

      if (!name)
        sprintf (nmcpy + strlen (nmcpy), "%i", i);

      if ((err = busses[i].bush->ops->init (busses[i].bush))) {
        /* initialization failed */
        i = -err;
      } else {
        busses[i].name = nmcpy;
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
rtems_libi2c_send_start (rtems_device_minor_number minor)
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
    busses[busno].started = true;
  }
  return rval;
}

rtems_status_code
rtems_libi2c_send_stop (rtems_device_minor_number minor)
{
  rtems_status_code rval;
  DECL_CHECKED_BH (busno, bush, minor, +)

    if (not_started (busno))
    return RTEMS_NOT_OWNER_OF_RESOURCE;

  rval = bush->ops->send_stop (bush);

  busses[busno].started = false;

  unlock_bus (busno);
  return rval;
}

rtems_status_code
rtems_libi2c_send_addr (rtems_device_minor_number minor, int rw)
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
rtems_libi2c_read_bytes (rtems_device_minor_number minor,
			 unsigned char *bytes,
			 int nbytes)
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
rtems_libi2c_write_bytes (rtems_device_minor_number minor,
			  const unsigned char *bytes,
			  int nbytes)
{
  int sc;
  DECL_CHECKED_BH (busno, bush, minor, -)

  if (not_started (busno))
    return -RTEMS_NOT_OWNER_OF_RESOURCE;

  sc = bush->ops->write_bytes (bush, (unsigned char *)bytes, nbytes);
  if (sc < 0)
    rtems_libi2c_send_stop (minor);
  return sc;
}

int
rtems_libi2c_ioctl (rtems_device_minor_number minor,
		    int cmd,
		    ...)
{
  va_list            ap;
  int sc = 0;
  void *args;
  bool is_started = false;
  DECL_CHECKED_BH (busno, bush, minor, -)

  va_start(ap, cmd);
  args = va_arg(ap, void *);

  switch(cmd) {
    /*
     * add ioctls defined for this level here:
     */

  case RTEMS_LIBI2C_IOCTL_GET_DRV_T:
    /*
     * query driver table entry
     */
    *(rtems_libi2c_drv_t **)args = (drvs[MINOR2DRV(minor)-1].drv);
    break;

  case RTEMS_LIBI2C_IOCTL_START_TFM_READ_WRITE:
    if (not_started (busno)) {
      va_end(ap);
      return -RTEMS_NOT_OWNER_OF_RESOURCE;
    }

    /*
     * address device, then set transfer mode and perform read_write transfer
     */
    /*
     * perform start/address
     */
    if (sc == 0) {
      sc = rtems_libi2c_send_start (minor);
      is_started = (sc == 0);
    }
    /*
     * set tfr mode
     */
    if (sc == 0) {
      sc = bush->ops->ioctl
	(bush,
	 RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
	 &((rtems_libi2c_tfm_read_write_t *)args)->tfr_mode);
    }
    /*
     * perform read_write
     */
    if (sc == 0) {
      sc = bush->ops->ioctl
	(bush,
	 RTEMS_LIBI2C_IOCTL_READ_WRITE,
	 &((rtems_libi2c_tfm_read_write_t *)args)->rd_wr);
    }
    if ((sc < 0) && (is_started)) {
      rtems_libi2c_send_stop (minor);
    }
    break;
  default:
    sc = bush->ops->ioctl (bush, cmd, args);
    break;
  }
  va_end(ap);
  return sc;
}

static int
do_s_rw (rtems_device_minor_number minor,
	 unsigned char *bytes,
	 int nbytes,
	 int rw)
{
  rtems_status_code   sc;
  rtems_libi2c_bus_t *bush;
  int                 status;

  if ((sc = rtems_libi2c_send_start (minor)))
    return -sc;

  /* at this point, we hold the bus and are sure the minor number is valid */
  bush = busses[MINOR2BUS (minor)].bush;

  if ((sc = bush->ops->send_addr (bush, MINOR2ADDR (minor), rw))) {
    rtems_libi2c_send_stop (minor);
    return -sc;
  }

  if (rw)
    status = bush->ops->read_bytes (bush, bytes, nbytes);
  else
    status = bush->ops->write_bytes (bush, bytes, nbytes);

  if (status < 0) {
    rtems_libi2c_send_stop (minor);
  }
  return status;
}

int
rtems_libi2c_start_read_bytes (rtems_device_minor_number minor,
			       unsigned char *bytes,
                               int nbytes)
{
  return do_s_rw (minor, bytes, nbytes, 1);
}

int
rtems_libi2c_start_write_bytes (rtems_device_minor_number minor,
				const unsigned char *bytes,
                                int nbytes)
{
  return do_s_rw (minor, (unsigned char *)bytes, nbytes, 0);
}

int
rtems_libi2c_register_drv (const char *name, rtems_libi2c_drv_t * drvtbl,
                           unsigned busno, unsigned i2caddr)
{
  int i;
  rtems_status_code err;
  rtems_device_minor_number minor;

  if (libmutex == RTEMS_ID_NONE) {
    safe_printf ( DRVNM "Library not initialized\n");
    return -RTEMS_NOT_DEFINED;
  }

  if (name && strchr (name, '/')) {
    safe_printf ( DRVNM "Invalid name: '%s' -- must not contain '/'\n", name);
    return -RTEMS_INVALID_NAME;
  }

  if (busno >= MAX_NO_BUSSES || !busses[busno].bush || i2caddr >= 1 << 10) {
    errno = EINVAL;
    return -RTEMS_INVALID_NUMBER;
  }

  if (drvtbl == NULL || drvtbl->size < sizeof (*drvtbl)) {
    safe_printf ( DRVNM "No driver table or size too small -- misconfiguration?\n");
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
        size_t length = strlen (busses[busno].name) + strlen (name) + 2;
        str = malloc (length);
        snprintf (str, length, "%s.%s", busses[busno].name, name);

        dev = rtems_filesystem_make_dev_t (rtems_libi2c_major, minor);

        mode = 0111 | S_IFCHR;
        if (drvtbl->ops->read_entry)
          mode |= 0444;
        if (drvtbl->ops->write_entry)
          mode |= 0222;

        /* note that 'umask' is applied to 'mode' */
        if (mknod (str, mode, dev)) {
          safe_printf( DRVNM
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
