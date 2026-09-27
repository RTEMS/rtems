#include <time.h>

#ifdef __rtems__
#define hidden __attribute__((__visibility__("hidden")))
/* Newlib provides a gmtime_r() of its own, so the alias is strong. */
#define weak_alias(old, new) \
	extern __typeof(old) new __attribute__((__alias__(#old)))
hidden struct tm *__gmtime_r(const time_t *restrict, struct tm *restrict);
#endif

hidden int __days_in_month(int, int);
hidden int __month_to_secs(int, int);
hidden long long __year_to_secs(long long, int *);
hidden long long __tm_to_secs(const struct tm *);
hidden const char *__tm_to_tzname(const struct tm *);
hidden int __tzname_to_isdst(const char *restrict *);
hidden int __secs_to_tm(long long, struct tm *);
hidden void __secs_to_zone(long long, int, int *, long *, long *, const char **);
hidden const char *__strftime_fmt_1(char (*)[100], size_t *, int, const struct tm *, locale_t, int);
extern hidden const char __utc[];
