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

#if !defined(_SYS_TIME_H_) || !defined(_KERNEL)
#error "must be included via <sys/time.h> in kernel space"
#endif

#include <machine/_timecounter.h>

/* Operations on timespecs */
#ifndef timespecclear
#define	timespecclear(tvp)	((tvp)->tv_sec = (tvp)->tv_nsec = 0)
#endif
#ifndef timespecisset
#define	timespecisset(tvp)	((tvp)->tv_sec || (tvp)->tv_nsec)
#endif
#ifndef timespeccmp
#define	timespeccmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	    ((tvp)->tv_nsec cmp (uvp)->tv_nsec) :			\
	    ((tvp)->tv_sec cmp (uvp)->tv_sec))
#endif

#ifndef timespecadd
#define	timespecadd(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec >= 1000000000L) {			\
			(vsp)->tv_sec++;				\
			(vsp)->tv_nsec -= 1000000000L;			\
		}							\
	} while (0)
#endif
#ifndef timespecsub
#define	timespecsub(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec < 0) {				\
			(vsp)->tv_sec--;				\
			(vsp)->tv_nsec += 1000000000L;			\
		}							\
	} while (0)
#endif

/*
 * Simple macros to convert ticks to milliseconds
 * or microseconds and vice-versa. The answer
 * will always be at least 1. Note the return
 * value is a uint32_t however we step up the
 * operations to 64 bit to avoid any overflow/underflow
 * problems.
 */
#define TICKS_2_MSEC(t) max(1, (uint32_t)(hz == 1000) ? \
	  (t) : (((uint64_t)(t) * (uint64_t)1000)/(uint64_t)hz))
#define TICKS_2_USEC(t) max(1, (uint32_t)(hz == 1000) ? \
	  ((t) * 1000) : (((uint64_t)(t) * (uint64_t)1000000)/(uint64_t)hz))
#define MSEC_2_TICKS(m) max(1, (uint32_t)((hz == 1000) ? \
	  (m) : ((uint64_t)(m) * (uint64_t)hz)/(uint64_t)1000))
#define USEC_2_TICKS(u) max(1, (uint32_t)((hz == 1000) ? \
	 ((u) / 1000) : ((uint64_t)(u) * (uint64_t)hz)/(uint64_t)1000000))

/* Operations on timevals. */

#define	timevalclear(tvp)		((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define	timevalisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timevalcmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	    ((tvp)->tv_usec cmp (uvp)->tv_usec) :			\
	    ((tvp)->tv_sec cmp (uvp)->tv_sec))

/* timevaladd and timevalsub are not inlined */

/*
 * Kernel to clock driver interface.
 */
void	inittodr(time_t base);
void	resettodr(void);

#define	time_second _Timecounter_Time_second
#define	time_uptime _Timecounter_Time_uptime
extern struct timeval boottime;
extern struct bintime tc_tick_bt;
extern sbintime_t tc_tick_sbt;
extern struct bintime tick_bt;
extern sbintime_t tick_sbt;
extern int tc_precexp;
extern int tc_timepercentage;
extern struct bintime bt_timethreshold;
extern struct bintime bt_tickthreshold;
extern sbintime_t sbt_timethreshold;
extern sbintime_t sbt_tickthreshold;

/*
 * Functions for looking at our clock: [get]{bin,nano,micro}[up]time()
 *
 * Functions without the "get" prefix returns the best timestamp
 * we can produce in the given format.
 *
 * "bin"   == struct bintime  == seconds + 64 bit fraction of seconds.
 * "nano"  == struct timespec == seconds + nanoseconds.
 * "micro" == struct timeval  == seconds + microseconds.
 *
 * Functions containing "up" returns time relative to boot and
 * should be used for calculating time intervals.
 *
 * Functions without "up" returns UTC time.
 *
 * Functions with the "get" prefix returns a less precise result
 * much faster than the functions without "get" prefix and should
 * be used where a precision of 1/hz seconds is acceptable or where
 * performance is priority. (NB: "precision", _not_ "resolution" !)
 */

#define	binuptime(_bt) _Timecounter_Binuptime(_bt)
#define	nanouptime(_tsp) _Timecounter_Nanouptime(_tsp)
#define	microuptime(_tvp) _Timecounter_Microuptime(_tvp)

static __inline sbintime_t
sbinuptime(void)
{
	struct bintime _bt;

	binuptime(&_bt);
	return (bttosbt(_bt));
}

#define	bintime(_bt) _Timecounter_Bintime(_bt)
#define	nanotime(_tsp) _Timecounter_Nanotime(_tsp)
#define	microtime(_tvp) _Timecounter_Microtime(_tvp)

#define	getbinuptime(_bt) _Timecounter_Getbinuptime(_bt)
#define	getnanouptime(_tsp) _Timecounter_Getnanouptime(_tsp)
#define	getmicrouptime(_tvp) _Timecounter_Getmicrouptime(_tvp)

static __inline sbintime_t
getsbinuptime(void)
{
	struct bintime _bt;

	getbinuptime(&_bt);
	return (bttosbt(_bt));
}

#define	getbintime(_bt) _Timecounter_Getbintime(_bt)
#define	getnanotime(_tsp) _Timecounter_Getnanotime(_tsp)
#define	getmicrotime(_tvp) _Timecounter_Getmicrotime(_tvp)

#define	getboottime(_tvp) _Timecounter_Getboottime(_tvp)
#define	getboottimebin(_bt) _Timecounter_Getboottimebin(_bt)

/* Other functions */
int	itimerdecr(struct itimerval *itp, int usec);
int	itimerfix(struct timeval *tv);
int	ppsratecheck(struct timeval *, int *, int);
int	ratecheck(struct timeval *, const struct timeval *);
void	timevaladd(struct timeval *t1, const struct timeval *t2);
void	timevalsub(struct timeval *t1, const struct timeval *t2);
int	tvtohz(struct timeval *tv);

#define	TC_DEFAULTPERC		5

#define	BT2FREQ(bt)                                                     \
	(((uint64_t)0x8000000000000000 + ((bt)->frac >> 2)) /           \
	    ((bt)->frac >> 1))

#define	SBT2FREQ(sbt)	((SBT_1S + ((sbt) >> 1)) / (sbt))

#define	FREQ2BT(freq, bt)                                               \
{									\
	(bt)->sec = 0;                                                  \
	(bt)->frac = ((uint64_t)0x8000000000000000  / (freq)) << 1;     \
}

#define	TIMESEL(sbt, sbt2)						\
	(((sbt2) >= sbt_timethreshold) ?				\
	    ((*(sbt) = getsbinuptime()), 1) : ((*(sbt) = sbinuptime()), 0))
