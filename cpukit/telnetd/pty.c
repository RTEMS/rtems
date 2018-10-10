/*
 * /dev/ptyXX  (Support for pseudo-terminals)
 *
 *  Original Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Till Straumann <strauman@slac.stanford.edu>
 *
 *   - converted into a loadable module
 *   - NAWS support / ioctls for querying/setting the window
 *     size added.
 *   - don't delete the running task when the connection
 *     is closed. Rather let 'read()' return a 0 count so
 *     they may cleanup. Some magic hack works around termios
 *     limitation.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define DEBUG_WH    (1<<0)
#define DEBUG_DETAIL  (1<<1)

/* #define DEBUG DEBUG_WH */

/*-----------------------------------------*/
#include <sys/ttycom.h>
#include <rtems/pty.h>
#include <rtems/seterr.h>
#include <errno.h>
#include <sys/socket.h>
/*-----------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
/*-----------------------------------------*/
#define IAC_ESC    255
#define IAC_DONT   254
#define IAC_DO     253
#define IAC_WONT   252
#define IAC_WILL   251
#define IAC_SB     250
#define IAC_GA     249
#define IAC_EL     248
#define IAC_EC     247
#define IAC_AYT    246
#define IAC_AO     245
#define IAC_IP     244
#define IAC_BRK    243
#define IAC_DMARK  242
#define IAC_NOP    241
#define IAC_SE     240
#define IAC_EOR    239

#define SB_MAX     RTEMS_PTY_SB_MAX

static bool ptyPollInitialize(rtems_termios_tty *,
  rtems_termios_device_context *, struct termios *,
  rtems_libio_open_close_args_t *);
static void ptyShutdown(rtems_termios_tty *,
  rtems_termios_device_context *, rtems_libio_open_close_args_t *);
static void ptyPollWrite(rtems_termios_device_context *, const char *, size_t);
static int ptyPollRead(rtems_termios_device_context *);
static bool ptySetAttributes(rtems_termios_device_context *,
  const struct termios *);
static int my_pty_control(rtems_termios_device_context *,
  ioctl_command_t, void *);

static const rtems_termios_device_handler pty_handler = {
  .first_open = ptyPollInitialize,
  .last_close = ptyShutdown,
  .poll_read = ptyPollRead,
  .write = ptyPollWrite,
  .set_attributes = ptySetAttributes,
  .ioctl = my_pty_control
};

static
int send_iac(rtems_pty_context *pty, unsigned char mode, unsigned char option)
{
  unsigned char buf[3];

  buf[0]=IAC_ESC;
  buf[1]=mode;
  buf[2]=option;
  return write(pty->socket, buf, sizeof(buf));
}

const char *rtems_pty_initialize(rtems_pty_context *pty, uintptr_t unique)
{
  rtems_status_code sc;

  memset(pty, 0, sizeof(*pty));
  (void)snprintf(pty->name, sizeof(pty->name), "/dev/pty%" PRIuPTR, unique);
  rtems_termios_device_context_initialize(&pty->base, "pty");
  pty->socket = -1;
  sc = rtems_termios_device_install(pty->name, &pty_handler, NULL, &pty->base);
  if (sc != RTEMS_SUCCESSFUL) {
    return NULL;
  }

  return pty->name;
}

void rtems_pty_close_socket(rtems_pty_context *pty)
{
  if (pty->socket >= 0) {
    (void)close(pty->socket);
    pty->socket = -1;
  }
}

void rtems_pty_set_socket(rtems_pty_context *pty, int socket)
{
  struct timeval t;

  rtems_pty_close_socket(pty);
  pty->socket = socket;

  /* set a long polling interval to save CPU time */
  t.tv_sec=2;
  t.tv_usec=00000;
  (void)setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));

  /* inform the client that we will echo */
  send_iac(pty, IAC_WILL, 1);
}

/*-----------------------------------------------------------*/
/*
 * The NVT terminal is negociated in PollRead and PollWrite
 * with every BYTE sendded or received.
 * A litle status machine in the pty_read_byte(int minor)
 *
 */
static const char IAC_AYT_RSP[]="\r\nAYT? Yes, RTEMS-SHELL is here\r\n";
static const char IAC_BRK_RSP[]="<*Break*>";
static const char IAC_IP_RSP []="<*Interrupt*>";

static int
handleSB(rtems_pty_context *pty)
{
  switch (pty->sb_buf[0]) {
    case 31:  /* NAWS */
      pty->width  = (pty->sb_buf[1]<<8) + pty->sb_buf[2];
      pty->height = (pty->sb_buf[3]<<8) + pty->sb_buf[4];
#if DEBUG & DEBUG_WH
      fprintf(stderr,
          "Setting width/height to %ix%i\n",
          pty->width,
          pty->height);
#endif
      break;
    default:
      break;
  }
  return 0;
}

static int ptyPollRead(rtems_termios_device_context *base)
{ /* Characters written to the client side*/
   rtems_pty_context *pty = (rtems_pty_context *)base;
   unsigned char  value;
   unsigned int  omod;
   int      count;
   int      result;

   count=read(pty->socket,&value,sizeof(value));
   if (count<0)
    return -1;

   if (count<1) {
      /* Unfortunately, there is no way of passing an EOF
       * condition through the termios driver. Hence, we
       * resort to an ugly hack. Setting cindex>ccount
       * causes the termios driver to return a read count
       * of '0' which is what we want here. We leave
       * 'errno' untouched.
       */
      pty->ttyp->cindex=pty->ttyp->ccount+1;
      return pty->ttyp->termios.c_cc[VEOF];
   };

   omod=pty->iac_mode;
   pty->iac_mode=0;
   switch(omod & 0xff) {
       case IAC_ESC:
           switch(value) {
               case IAC_ESC :
                   /* in case this is an ESC ESC sequence in SB mode */
                   pty->iac_mode = omod>>8;
                   return IAC_ESC;
               case IAC_DONT:
               case IAC_DO  :
               case IAC_WONT:
               case IAC_WILL:
                   pty->iac_mode=value;
                   return -1;
               case IAC_SB  :
#if DEBUG & DEBUG_DETAIL
                   printk("SB\n");
#endif
                   pty->iac_mode=value;
                   pty->sb_ind=0;
                   return -100;
               case IAC_GA  :
                   return -1;
               case IAC_EL  :
                   return 0x03; /* Ctrl-C*/
               case IAC_EC  :
                   return '\b';
               case IAC_AYT :
                   write(pty->socket,IAC_AYT_RSP,strlen(IAC_AYT_RSP));
                   return -1;
               case IAC_AO  :
                   return -1;
               case IAC_IP  :
                   write(pty->socket,IAC_IP_RSP,strlen(IAC_IP_RSP));
                   return -1;
               case IAC_BRK :
                   write(pty->socket,IAC_BRK_RSP,strlen(IAC_BRK_RSP));
                   return -1;
               case IAC_DMARK:
                   return -2;
               case IAC_NOP :
                   return -1;
               case IAC_SE  :
#if DEBUG & DEBUG_DETAIL
                  {
                  int i;
                  printk("SE");
                  for (i=0; i<pty->sb_ind; i++)
                    printk(" %02x",pty->sb_buf[i]);
                  printk("\n");
                  }
#endif
                  handleSB(pty);
               return -101;
               case IAC_EOR :
                   return -102;
               default      :
                   return -1;
           };
           break;

       case IAC_SB:
           pty->iac_mode=omod;
           if (IAC_ESC==value) {
             pty->iac_mode=(omod<<8)|value;
           } else {
             if (pty->sb_ind < SB_MAX)
               pty->sb_buf[pty->sb_ind++]=value;
           }
           return -1;

       case IAC_WILL:
           if (value==34){
              send_iac(pty,IAC_DONT,   34);  /*LINEMODE*/
              send_iac(pty,IAC_DO  ,    1);  /*ECHO    */
           } else if (value==31) {
              send_iac(pty,IAC_DO  ,   31);  /*NAWS    */
#if DEBUG & DEBUG_DETAIL
              printk("replied DO NAWS\n");
#endif
           } else {
              send_iac(pty,IAC_DONT,value);
           }
           return -1;
       case IAC_DONT:
           return -1;
       case IAC_DO  :
           if (value==3) {
              send_iac(pty,IAC_WILL,    3);  /* GO AHEAD*/
           } else  if (value==1) {
              send_iac(pty,IAC_WILL,    1);  /* ECHO */
           } else {
              send_iac(pty,IAC_WONT,value);
           };
           return -1;
       case IAC_WONT:
           if (value==1) {
             send_iac(pty,IAC_WILL,    1);
           } else { /* ECHO */
             send_iac(pty,IAC_WONT,value);
           }
           return -1;
       default:
           if (value==IAC_ESC) {
              pty->iac_mode=value;
              return -1;
           } else {
              result=value;
              if ( 0
                /* map CRLF to CR for symmetry */
                 || ((value=='\n') && pty->last_cr)
                /* map telnet CRNUL to CR down here */
                 || ((value==0) && pty->last_cr)
                ) result=-1;
               pty->last_cr=(value=='\r');
               return result;
           };
   };
  /* should never get here but keep compiler happy */
  return -1;
}

/*-----------------------------------------------------------*/
/* Set the 'Hardware'                                        */
/*-----------------------------------------------------------*/
static bool
ptySetAttributes(rtems_termios_device_context *base, const struct termios *t)
{
  rtems_pty_context *pty = (rtems_pty_context *)base;
  pty->c_cflag = t->c_cflag;
  return true;
}
/*-----------------------------------------------------------*/
static bool
ptyPollInitialize(rtems_termios_tty *ttyp,
  rtems_termios_device_context *base, struct termios *t,
  rtems_libio_open_close_args_t *args)
{
  rtems_pty_context *pty = (rtems_pty_context *)base;
  pty->ttyp = ttyp;
  pty->iac_mode = 0;
  pty->sb_ind = 0;
  pty->width = 0;
  pty->height = 0;
  return ptySetAttributes(&pty->base, t);
}
/*-----------------------------------------------------------*/
static void
ptyShutdown(rtems_termios_tty *ttyp,
  rtems_termios_device_context *base, rtems_libio_open_close_args_t *arg)
{
  rtems_pty_context *pty = (rtems_pty_context *)base;
  close(pty->socket);
}
/*-----------------------------------------------------------*/
/* Write Characters into pty device                          */
/*-----------------------------------------------------------*/
static void
ptyPollWrite(rtems_termios_device_context *base, const char *buf, size_t len)
{
  rtems_pty_context *pty = (rtems_pty_context *)base;

  while (len > 0) {
    ssize_t n = write(pty->socket, buf, len);
    if (n <= 0) {
      break;
    }

    buf += (size_t)n;
    len -= (size_t)n;
  }
}

static int
my_pty_control(rtems_termios_device_context *base,
  ioctl_command_t request, void *buffer)
{
  rtems_pty_context *p = (rtems_pty_context *)base;
  struct winsize *wp = buffer;

  switch (request) {
    case TIOCGWINSZ:
      wp->ws_row = p->height;
      wp->ws_col = p->width;
#if DEBUG & DEBUG_WH
      fprintf(stderr,
          "ioctl(TIOCGWINSZ), returning %ix%i\n",
          wp->ws_col,
          wp->ws_row);
#endif
      break;
    case TIOCSWINSZ:
#if DEBUG & DEBUG_WH
      fprintf(stderr,
          "ioctl(TIOCGWINSZ), setting %ix%i\n",
          wp->ws_col,
          wp->ws_row);
#endif

      p->height = wp->ws_row;
      p->width  = wp->ws_col;
      break;
    default:
      rtems_set_errno_and_return_minus_one(EINVAL);
      break;
  }

  return 0;
}
