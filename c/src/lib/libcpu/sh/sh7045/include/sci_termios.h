/*
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _SH_SCI_TERMIOS_H_
#define _SH_SCI_TERMIOS_H_

#ifdef __cplusplus
extern "C"{
#endif


int sh_sci_set_attributes(
  int minor,
  const struct termios *t
);

void sh_sci_initialize_interrupts(int minor);

void sh_sci_init(int minor);

ssize_t sh_sci_write_support_int(
    int         minor,
    const char *buf,
    size_t      len
);

ssize_t sh_sci_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
);

void sh_sci_write_polled(
    int minor,
    char c
);

int sh_sci_inbyte_nonblocking_polled(int minor);


int sh_sci_first_open(
  int major,
  int minor,
  void *arg
);

int sh_sci_last_close(
  int major,
  int minor,
  void *arg
);

#ifdef __cplusplus
}
#endif


#endif /* _SH_SCI_TERMIOS_H_ */
