#include "time_impl.h"
#include <errno.h>

struct tm *__gmtime_r(const time_t *restrict t, struct tm *restrict tm)
{
	if (__secs_to_tm(*t, tm) < 0) {
		errno = EOVERFLOW;
		return 0;
	}
	tm->tm_isdst = 0;
#ifdef __TM_GMTOFF
	tm->__TM_GMTOFF = 0;
#endif
#ifdef __TM_ZONE
	tm->__TM_ZONE = __utc;
#endif
	return tm;
}

weak_alias(__gmtime_r, gmtime_r);
