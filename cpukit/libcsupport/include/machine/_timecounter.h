/*-
 * Copyright (c) 2016 embedded brains GmbH
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SYS_TIME_H_
#error "<sys/time.h> must be included first"
#endif /* !_SYS_TIME_H_ */

#ifndef _MACHINE__TIMECOUNTER_H_
#define _MACHINE__TIMECOUNTER_H_

__BEGIN_DECLS
extern volatile time_t _Timecounter_Time_second;
extern volatile int32_t _Timecounter_Time_uptime;
extern struct bintime _Timecounter_Boottimebin;

void   _Timecounter_Binuptime(struct bintime *);
void   _Timecounter_Nanouptime(struct timespec *);
void   _Timecounter_Microuptime(struct timeval *);
void   _Timecounter_Bintime(struct bintime *);
void   _Timecounter_Nanotime(struct timespec *);
void   _Timecounter_Microtime(struct timeval *);
void   _Timecounter_Getbinuptime(struct bintime *);
void   _Timecounter_Getnanouptime(struct timespec *);
void   _Timecounter_Getmicrouptime(struct timeval *);
void   _Timecounter_Getbintime(struct bintime *);
void   _Timecounter_Getnanotime(struct timespec *);
void   _Timecounter_Getmicrotime(struct timeval *);
__END_DECLS

#endif /* _MACHINE__TIMECOUNTER_H_ */
