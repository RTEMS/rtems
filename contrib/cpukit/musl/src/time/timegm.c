#define _GNU_SOURCE
#include "time_impl.h"
#include <errno.h>

time_t timegm(struct tm *tm)
{
	struct tm new;
	long long t = __tm_to_secs(tm);
	if (__secs_to_tm(t, &new) < 0) {
		errno = EOVERFLOW;
		return -1;
	}
	*tm = new;
	tm->tm_isdst = 0;
#ifdef __TM_GMTOFF
	tm->__TM_GMTOFF = 0;
#endif
#ifdef __TM_ZONE
	tm->__TM_ZONE = __utc;
#endif
	return t;
}
