#include <rtems/test.h>

#include <errno.h>

void T_check_psx_error(const T_check_context *t, int a, int eeno)
{
	int aeno;

	aeno = errno;
	T_check(t, a == -1 && aeno == eeno, "%i == -1, %s == %s", a,
	    T_strerror(aeno), T_strerror(eeno));
}

void T_check_psx_success(const T_check_context *t, int a)
{
	T_check(t, a == 0, "%i == 0, %s", a, T_strerror(errno));
}
