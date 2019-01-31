#include <t.h>

#include <errno.h>

void T_check_psx_error(int a, const T_check_context *t, int eeno)
{
	int aeno;

	aeno = errno;
	T_check_true(a == -1 && aeno == eeno, t, "%i == -1, %s == %s", a,
	    T_strerror(aeno), T_strerror(eeno));
}

void T_check_psx_success(int a, const T_check_context *t)
{
	T_check_true(a == 0, t, "%i == 0, %s", a, T_strerror(errno));
}
