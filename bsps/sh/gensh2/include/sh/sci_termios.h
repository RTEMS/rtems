/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
