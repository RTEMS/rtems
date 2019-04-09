#include <t.h>

#include <sys/types.h>

T_TEST_CASE(step_assert_true)
{
	T_plan(2);
	T_step_assert_true(0, true, "nix");
	T_step_assert_true(1, false, "a");
}

T_TEST_CASE(check_true)
{
	T_true(true, "nix");
	T_true(false, "a");
	T_quiet_true(true, "nix");
	T_quiet_true(false, "ab %i", 0);
	T_step_true(2, true, "nix");
	T_step_true(3, false, "abc %i", 0, 1);
	T_assert_true(true, "nix");
	T_assert_true(false, "abcd %i %i %i", 0, 1, 2);
}

T_TEST_CASE(step_assert_false)
{
	T_plan(2);
	T_step_assert_false(0, false, "nix");
	T_step_assert_false(1, true, "a");
}

T_TEST_CASE(check_false)
{
	T_false(false, "nix");
	T_false(true, "a");
	T_quiet_false(false, "nix");
	T_quiet_false(true, "ab %i", 0);
	T_step_false(2, false, "nix");
	T_step_false(3, true, "abc %i", 0, 1);
	T_assert_false(false, "nix");
	T_assert_false(true, "abcd %i %i %i", 0, 1, 2);
}

T_TEST_CASE(step_assert_eq)
{
	T_plan(2);
	T_step_assert_eq(0, 0, 0, "nix");
	T_step_assert_eq(1, 1, 2, "1 == 2");
}

T_TEST_CASE(check_eq)
{
	T_eq(0, 0, "nix");
	T_eq(1, 2, "1 == 2");
	T_quiet_eq(3, 3, "nix");
	T_quiet_eq(4, 5, "4 == 5");
	T_step_eq(2, 6, 6, "nix");
	T_step_eq(3, 7, 8, "7 == 8");
	T_assert_eq(9, 9, "nix");
	T_assert_eq(10, 11, "10 == 11");
}

T_TEST_CASE(step_assert_ne)
{
	T_plan(2);
	T_step_assert_ne(0, 0, 1, "nix");
	T_step_assert_ne(1, 2, 2, "2 != 2");
}

T_TEST_CASE(check_ne)
{
	T_ne(0, 1, "nix");
	T_ne(2, 2, "2 != 2");
	T_quiet_ne(3, 4, "nix");
	T_quiet_ne(5, 5, "5 != 5");
	T_step_ne(2, 6, 7, "nix");
	T_step_ne(3, 8, 8, "5 != 5");
	T_assert_ne(9, 10, "nix");
	T_assert_ne(11, 11, "11 != 11");
}

T_TEST_CASE(step_assert_eq_ptr)
{
	int a;
	int b;

	T_plan(2);
	T_step_eq_ptr(0, &a, &a);
	T_step_eq_ptr(1, &a, &b);
}

T_TEST_CASE(check_eq_ptr)
{
	int a;
	int b;

	T_eq_ptr(&a, &a);
	T_eq_ptr(&a, &b);
	T_quiet_eq_ptr(&a, &a);
	T_quiet_eq_ptr(&a, &b);
	T_step_eq_ptr(2, &a, &a);
	T_step_eq_ptr(3, &a, &b);
	T_assert_eq_ptr(&a, &a);
	T_assert_eq_ptr(&a, &b);
}

T_TEST_CASE(step_assert_ne_ptr)
{
	int a;
	int b;

	T_plan(2);
	T_step_assert_ne_ptr(0, &a, &b);
	T_step_assert_ne_ptr(1, &a, &a);
}

T_TEST_CASE(check_ne_ptr)
{
	int a;
	int b;

	T_ne_ptr(&a, &b);
	T_ne_ptr(&a, &a);
	T_quiet_ne_ptr(&a, &b);
	T_quiet_ne_ptr(&a, &a);
	T_step_ne_ptr(2, &a, &b);
	T_step_ne_ptr(3, &a, &a);
	T_assert_ne_ptr(&a, &b);
	T_assert_ne_ptr(&a, &a);
}

T_TEST_CASE(step_assert_null)
{
	int a;

	T_plan(2);
	T_step_assert_null(0, NULL);
	T_step_assert_null(1, &a);
}

T_TEST_CASE(check_null)
{
	int a;

	T_null(NULL);
	T_null(&a);
	T_quiet_null(NULL);
	T_quiet_null(&a);
	T_step_null(2, NULL);
	T_step_null(3, &a);
	T_assert_null(NULL);
	T_assert_null(&a);
}

T_TEST_CASE(step_assert_not_null)
{
	int a;

	T_plan(2);
	T_step_assert_not_null(0, &a);
	T_step_assert_not_null(1, NULL);
}

T_TEST_CASE(check_not_null)
{
	int a;

	T_not_null(&a);
	T_not_null(NULL);
	T_quiet_not_null(&a);
	T_quiet_not_null(NULL);
	T_step_not_null(2, &a);
	T_step_not_null(3, NULL);
	T_assert_not_null(&a);
	T_assert_not_null(NULL);
}

T_TEST_CASE(step_assert_eq_mem)
{
	static const int a = 0;
	static const int b = 1;

	T_plan(2);
	T_step_assert_eq_mem(0, &a, &a, sizeof(a));
	T_step_assert_eq_mem(1, &a, &b, sizeof(a));
}

T_TEST_CASE(check_eq_mem)
{
	static const int a = 0;
	static const int b = 1;

	T_eq_mem(&a, &a, sizeof(a));
	T_eq_mem(&a, &b, sizeof(a));
	T_quiet_eq_mem(&a, &a, sizeof(a));
	T_quiet_eq_mem(&a, &b, sizeof(a));
	T_step_eq_mem(2, &a, &a, sizeof(a));
	T_step_eq_mem(3, &a, &b, sizeof(a));
	T_assert_eq_mem(&a, &a, sizeof(a));
	T_assert_eq_mem(&a, &b, sizeof(a));
}

T_TEST_CASE(step_assert_ne_mem)
{
	static const int a = 0;
	static const int b = 1;

	T_plan(2);
	T_step_assert_ne_mem(0, &a, &b, sizeof(a));
	T_step_assert_ne_mem(1, &a, &a, sizeof(a));
}

T_TEST_CASE(check_ne_mem)
{
	static const int a = 0;
	static const int b = 1;

	T_ne_mem(&a, &b, sizeof(a));
	T_ne_mem(&a, &a, sizeof(a));
	T_quiet_ne_mem(&a, &b, sizeof(a));
	T_quiet_ne_mem(&a, &a, sizeof(a));
	T_step_ne_mem(2, &a, &b, sizeof(a));
	T_step_ne_mem(3, &a, &a, sizeof(a));
	T_assert_ne_mem(&a, &b, sizeof(a));
	T_assert_ne_mem(&a, &a, sizeof(a));
}

T_TEST_CASE(step_assert_eq_str)
{
	static const char a[] = "a";
	static const char b[] = "b";

	T_plan(2);
	T_step_assert_eq_str(0, a, a);
	T_step_assert_eq_str(1, a, b);
}

T_TEST_CASE(check_eq_str)
{
	static const char a[] = "a";
	static const char b[] = "b";

	T_eq_str(a, a);
	T_eq_str(a, b);
	T_quiet_eq_str(a, a);
	T_quiet_eq_str(a, b);
	T_step_eq_str(2, a, a);
	T_step_eq_str(3, a, b);
	T_assert_eq_str(a, a);
	T_assert_eq_str(a, b);
}

T_TEST_CASE(step_assert_ne_str)
{
	static const char a[] = "a";
	static const char b[] = "b";

	T_plan(2);
	T_step_assert_ne_str(0, a, b);
	T_step_assert_ne_str(1, a, a);
}

T_TEST_CASE(check_ne_str)
{
	static const char a[] = "a";
	static const char b[] = "b";

	T_ne_str(a, b);
	T_ne_str(a, a);
	T_quiet_ne_str(a, b);
	T_quiet_ne_str(a, a);
	T_step_ne_str(2, a, b);
	T_step_ne_str(3, a, a);
	T_assert_ne_str(a, b);
	T_assert_ne_str(a, a);
}

T_TEST_CASE(step_assert_eq_nstr)
{
	static const char a[] = "aaa";
	static const char b[] = "aaab";

	T_plan(3);
	T_step_assert_eq_nstr(0, a, a, 3);
	T_step_assert_eq_nstr(1, a, b, 3);
	T_step_assert_eq_nstr(2, a, b, 4);
}

T_TEST_CASE(check_eq_nstr)
{
	static const char a[] = "aaa";
	static const char b[] = "aaab";

	T_eq_nstr(a, a, 3);
	T_eq_nstr(a, b, 3);
	T_eq_nstr(a, b, 4);
	T_quiet_eq_nstr(a, a, 3);
	T_quiet_eq_nstr(a, b, 3);
	T_quiet_eq_nstr(a, b, 4);
	T_step_eq_nstr(3, a, a, 3);
	T_step_eq_nstr(4, a, b, 3);
	T_step_eq_nstr(5, a, b, 4);
	T_assert_eq_nstr(a, a, 3);
	T_assert_eq_nstr(a, b, 3);
	T_assert_eq_nstr(a, b, 4);
}

T_TEST_CASE(step_assert_ne_nstr)
{
	static const char a[] = "aaa";
	static const char b[] = "aaab";

	T_plan(3);
	T_step_assert_ne_nstr(0, a, b, 4);
	T_step_assert_ne_nstr(1, a, a, 3);
	T_step_assert_ne_nstr(2, a, b, 3);
}

T_TEST_CASE(check_ne_nstr)
{
	static const char a[] = "aaa";
	static const char b[] = "aaab";

	T_ne_nstr(a, b, 4);
	T_ne_nstr(a, a, 3);
	T_ne_nstr(a, b, 3);
	T_quiet_ne_nstr(a, b, 4);
	T_quiet_ne_nstr(a, a, 3);
	T_quiet_ne_nstr(a, b, 3);
	T_step_ne_nstr(3, a, b, 4);
	T_step_ne_nstr(4, a, a, 3);
	T_step_ne_nstr(5, a, b, 3);
	T_assert_ne_nstr(a, b, 4);
	T_assert_ne_nstr(a, a, 3);
	T_assert_ne_nstr(a, b, 3);
}

T_TEST_CASE(step_assert_eq_char)
{
	T_plan(2);
	T_step_assert_eq_char(0, '1', '1');
	T_step_assert_eq_char(1, '2', '3');
}

T_TEST_CASE(check_eq_char)
{
	T_eq_char('1', '1');
	T_eq_char('2', '3');
	T_quiet_eq_char('4', '4');
	T_quiet_eq_char('5', '6');
	T_step_eq_char(2, '7', '7');
	T_step_eq_char(3, '8', '9');
	T_assert_eq_char('A', 'A');
	T_assert_eq_char('B', 'C');
}

T_TEST_CASE(step_assert_ne_char)
{
	T_plan(2);
	T_step_assert_ne_char(0, '2', '3');
	T_step_assert_ne_char(1, '1', '1');
}

T_TEST_CASE(check_ne_char)
{
	T_ne_char('2', '3');
	T_ne_char('1', '1');
	T_quiet_ne_char('5', '6');
	T_quiet_ne_char('4', '4');
	T_step_ne_char(2, '8', '9');
	T_step_ne_char(3, '7', '7');
	T_assert_ne_char('B', 'C');
	T_assert_ne_char('A', 'A');
}

T_TEST_CASE(step_assert_eq_schar)
{
	T_plan(2);
	T_step_assert_eq_schar(0, (signed char)1, (signed char)1);
	T_step_assert_eq_schar(1, (signed char)2, (signed char)3);
}

T_TEST_CASE(check_eq_schar)
{
	T_eq_schar((signed char)1, (signed char)1);
	T_eq_schar((signed char)2, (signed char)3);
	T_quiet_eq_schar((signed char)4, (signed char)4);
	T_quiet_eq_schar((signed char)5, (signed char)6);
	T_step_eq_schar(2, (signed char)7, (signed char)7);
	T_step_eq_schar(3, (signed char)8, (signed char)9);
	T_assert_eq_schar((signed char)10, (signed char)10);
	T_assert_eq_schar((signed char)11, (signed char)12);
}

T_TEST_CASE(step_assert_ne_schar)
{
	T_plan(2);
	T_step_assert_ne_schar(0, (signed char)2, (signed char)3);
	T_step_assert_ne_schar(1, (signed char)1, (signed char)1);
}

T_TEST_CASE(check_ne_schar)
{
	T_ne_schar((signed char)2, (signed char)3);
	T_ne_schar((signed char)1, (signed char)1);
	T_quiet_ne_schar((signed char)5, (signed char)6);
	T_quiet_ne_schar((signed char)4, (signed char)4);
	T_step_ne_schar(2, (signed char)8, (signed char)9);
	T_step_ne_schar(3, (signed char)7, (signed char)7);
	T_assert_ne_schar((signed char)11, (signed char)12);
	T_assert_ne_schar((signed char)10, (signed char)10);
}

T_TEST_CASE(step_assert_ge_schar)
{
	T_plan(2);
	T_step_assert_ge_schar(0, (signed char)1, (signed char)1);
	T_step_assert_ge_schar(1, (signed char)2, (signed char)3);
}

T_TEST_CASE(check_ge_schar)
{
	T_ge_schar((signed char)1, (signed char)1);
	T_ge_schar((signed char)2, (signed char)3);
	T_quiet_ge_schar((signed char)4, (signed char)4);
	T_quiet_ge_schar((signed char)5, (signed char)6);
	T_step_ge_schar(2, (signed char)7, (signed char)7);
	T_step_ge_schar(3, (signed char)8, (signed char)9);
	T_assert_ge_schar((signed char)10, (signed char)10);
	T_assert_ge_schar((signed char)11, (signed char)12);
}

T_TEST_CASE(step_assert_gt_schar)
{
	T_plan(2);
	T_step_assert_gt_schar(0, (signed char)2, (signed char)1);
	T_step_assert_gt_schar(1, (signed char)3, (signed char)3);
}

T_TEST_CASE(check_gt_schar)
{
	T_gt_schar((signed char)2, (signed char)1);
	T_gt_schar((signed char)3, (signed char)3);
	T_quiet_gt_schar((signed char)5, (signed char)4);
	T_quiet_gt_schar((signed char)6, (signed char)6);
	T_step_gt_schar(2, (signed char)8, (signed char)7);
	T_step_gt_schar(3, (signed char)9, (signed char)9);
	T_assert_gt_schar((signed char)10, (signed char)11);
	T_assert_gt_schar((signed char)12, (signed char)12);
}

T_TEST_CASE(step_assert_le_schar)
{
	T_plan(2);
	T_step_assert_le_schar(0, (signed char)1, (signed char)1);
	T_step_assert_le_schar(1, (signed char)3, (signed char)2);
}

T_TEST_CASE(check_le_schar)
{
	T_le_schar((signed char)1, (signed char)1);
	T_le_schar((signed char)3, (signed char)2);
	T_quiet_le_schar((signed char)4, (signed char)4);
	T_quiet_le_schar((signed char)6, (signed char)5);
	T_step_le_schar(2, (signed char)7, (signed char)7);
	T_step_le_schar(3, (signed char)9, (signed char)8);
	T_assert_le_schar((signed char)10, (signed char)10);
	T_assert_le_schar((signed char)12, (signed char)11);
}

T_TEST_CASE(step_assert_lt_schar)
{
	T_plan(2);
	T_step_assert_lt_schar(0, (signed char)1, (signed char)2);
	T_step_assert_lt_schar(1, (signed char)3, (signed char)3);
}

T_TEST_CASE(check_lt_schar)
{
	T_lt_schar((signed char)1, (signed char)2);
	T_lt_schar((signed char)3, (signed char)3);
	T_quiet_lt_schar((signed char)4, (signed char)5);
	T_quiet_lt_schar((signed char)6, (signed char)6);
	T_step_lt_schar(2, (signed char)7, (signed char)8);
	T_step_lt_schar(3, (signed char)9, (signed char)9);
	T_assert_lt_schar((signed char)10, (signed char)11);
	T_assert_lt_schar((signed char)12, (signed char)12);
}

T_TEST_CASE(step_assert_eq_uchar)
{
	T_plan(2);
	T_step_assert_eq_uchar(0, (unsigned char)1, (unsigned char)1);
	T_step_assert_eq_uchar(1, (unsigned char)2, (unsigned char)3);
}

T_TEST_CASE(check_eq_uchar)
{
	T_eq_uchar((unsigned char)1, (unsigned char)1);
	T_eq_uchar((unsigned char)2, (unsigned char)3);
	T_quiet_eq_uchar((unsigned char)4, (unsigned char)4);
	T_quiet_eq_uchar((unsigned char)5, (unsigned char)6);
	T_step_eq_uchar(2, (unsigned char)7, (unsigned char)7);
	T_step_eq_uchar(3, (unsigned char)8, (unsigned char)9);
	T_assert_eq_uchar((unsigned char)10, (unsigned char)10);
	T_assert_eq_uchar((unsigned char)11, (unsigned char)12);
}

T_TEST_CASE(step_assert_ne_uchar)
{
	T_plan(2);
	T_step_assert_ne_uchar(0, (unsigned char)2, (unsigned char)3);
	T_step_assert_ne_uchar(1, (unsigned char)1, (unsigned char)1);
}

T_TEST_CASE(check_ne_uchar)
{
	T_ne_uchar((unsigned char)2, (unsigned char)3);
	T_ne_uchar((unsigned char)1, (unsigned char)1);
	T_quiet_ne_uchar((unsigned char)5, (unsigned char)6);
	T_quiet_ne_uchar((unsigned char)4, (unsigned char)4);
	T_step_ne_uchar(2, (unsigned char)8, (unsigned char)9);
	T_step_ne_uchar(3, (unsigned char)7, (unsigned char)7);
	T_assert_ne_uchar((unsigned char)11, (unsigned char)12);
	T_assert_ne_uchar((unsigned char)10, (unsigned char)10);
}

T_TEST_CASE(step_assert_ge_uchar)
{
	T_plan(2);
	T_step_assert_ge_uchar(0, (unsigned char)1, (unsigned char)1);
	T_step_assert_ge_uchar(1, (unsigned char)2, (unsigned char)3);
}

T_TEST_CASE(check_ge_uchar)
{
	T_ge_uchar((unsigned char)1, (unsigned char)1);
	T_ge_uchar((unsigned char)2, (unsigned char)3);
	T_quiet_ge_uchar((unsigned char)4, (unsigned char)4);
	T_quiet_ge_uchar((unsigned char)5, (unsigned char)6);
	T_step_ge_uchar(2, (unsigned char)7, (unsigned char)7);
	T_step_ge_uchar(3, (unsigned char)8, (unsigned char)9);
	T_assert_ge_uchar((unsigned char)10, (unsigned char)10);
	T_assert_ge_uchar((unsigned char)11, (unsigned char)12);
}

T_TEST_CASE(step_assert_gt_uchar)
{
	T_plan(2);
	T_step_assert_gt_uchar(0, (unsigned char)2, (unsigned char)1);
	T_step_assert_gt_uchar(1, (unsigned char)3, (unsigned char)3);
}

T_TEST_CASE(check_gt_uchar)
{
	T_gt_uchar((unsigned char)2, (unsigned char)1);
	T_gt_uchar((unsigned char)3, (unsigned char)3);
	T_quiet_gt_uchar((unsigned char)5, (unsigned char)4);
	T_quiet_gt_uchar((unsigned char)6, (unsigned char)6);
	T_step_gt_uchar(2, (unsigned char)8, (unsigned char)7);
	T_step_gt_uchar(3, (unsigned char)9, (unsigned char)9);
	T_assert_gt_uchar((unsigned char)10, (unsigned char)11);
	T_assert_gt_uchar((unsigned char)12, (unsigned char)12);
}

T_TEST_CASE(step_assert_le_uchar)
{
	T_plan(2);
	T_step_assert_le_uchar(0, (unsigned char)1, (unsigned char)1);
	T_step_assert_le_uchar(1, (unsigned char)3, (unsigned char)2);
}

T_TEST_CASE(check_le_uchar)
{
	T_le_uchar((unsigned char)1, (unsigned char)1);
	T_le_uchar((unsigned char)3, (unsigned char)2);
	T_quiet_le_uchar((unsigned char)4, (unsigned char)4);
	T_quiet_le_uchar((unsigned char)6, (unsigned char)5);
	T_step_le_uchar(2, (unsigned char)7, (unsigned char)7);
	T_step_le_uchar(3, (unsigned char)9, (unsigned char)8);
	T_assert_le_uchar((unsigned char)10, (unsigned char)10);
	T_assert_le_uchar((unsigned char)12, (unsigned char)11);
}

T_TEST_CASE(step_assert_lt_uchar)
{
	T_plan(2);
	T_step_assert_lt_uchar(0, (unsigned char)1, (unsigned char)2);
	T_step_assert_lt_uchar(1, (unsigned char)3, (unsigned char)3);
}

T_TEST_CASE(check_lt_uchar)
{
	T_lt_uchar((unsigned char)1, (unsigned char)2);
	T_lt_uchar((unsigned char)3, (unsigned char)3);
	T_quiet_lt_uchar((unsigned char)4, (unsigned char)5);
	T_quiet_lt_uchar((unsigned char)6, (unsigned char)6);
	T_step_lt_uchar(2, (unsigned char)7, (unsigned char)8);
	T_step_lt_uchar(3, (unsigned char)9, (unsigned char)9);
	T_assert_lt_uchar((unsigned char)10, (unsigned char)11);
	T_assert_lt_uchar((unsigned char)12, (unsigned char)12);
}

T_TEST_CASE(step_assert_eq_short)
{
	T_plan(2);
	T_step_assert_eq_short(0, (short)1, (short)1);
	T_step_assert_eq_short(1, (short)2, (short)3);
}

T_TEST_CASE(check_eq_short)
{
	T_eq_short((short)1, (short)1);
	T_eq_short((short)2, (short)3);
	T_quiet_eq_short((short)4, (short)4);
	T_quiet_eq_short((short)5, (short)6);
	T_step_eq_short(2, (short)7, (short)7);
	T_step_eq_short(3, (short)8, (short)9);
	T_assert_eq_short((short)10, (short)10);
	T_assert_eq_short((short)11, (short)12);
}

T_TEST_CASE(step_assert_ne_short)
{
	T_plan(2);
	T_step_assert_ne_short(0, (short)2, (short)3);
	T_step_assert_ne_short(1, (short)1, (short)1);
}

T_TEST_CASE(check_ne_short)
{
	T_ne_short((short)2, (short)3);
	T_ne_short((short)1, (short)1);
	T_quiet_ne_short((short)5, (short)6);
	T_quiet_ne_short((short)4, (short)4);
	T_step_ne_short(2, (short)8, (short)9);
	T_step_ne_short(3, (short)7, (short)7);
	T_assert_ne_short((short)11, (short)12);
	T_assert_ne_short((short)10, (short)10);
}

T_TEST_CASE(step_assert_ge_short)
{
	T_plan(2);
	T_step_assert_ge_short(0, (short)1, (short)1);
	T_step_assert_ge_short(1, (short)2, (short)3);
}

T_TEST_CASE(check_ge_short)
{
	T_ge_short((short)1, (short)1);
	T_ge_short((short)2, (short)3);
	T_quiet_ge_short((short)4, (short)4);
	T_quiet_ge_short((short)5, (short)6);
	T_step_ge_short(2, (short)7, (short)7);
	T_step_ge_short(3, (short)8, (short)9);
	T_assert_ge_short((short)10, (short)10);
	T_assert_ge_short((short)11, (short)12);
}

T_TEST_CASE(step_assert_gt_short)
{
	T_plan(2);
	T_step_assert_gt_short(0, (short)2, (short)1);
	T_step_assert_gt_short(1, (short)3, (short)3);
}

T_TEST_CASE(check_gt_short)
{
	T_gt_short((short)2, (short)1);
	T_gt_short((short)3, (short)3);
	T_quiet_gt_short((short)5, (short)4);
	T_quiet_gt_short((short)6, (short)6);
	T_step_gt_short(2, (short)8, (short)7);
	T_step_gt_short(3, (short)9, (short)9);
	T_assert_gt_short((short)10, (short)11);
	T_assert_gt_short((short)12, (short)12);
}

T_TEST_CASE(step_assert_le_short)
{
	T_plan(2);
	T_step_assert_le_short(0, (short)1, (short)1);
	T_step_assert_le_short(1, (short)3, (short)2);
}

T_TEST_CASE(check_le_short)
{
	T_le_short((short)1, (short)1);
	T_le_short((short)3, (short)2);
	T_quiet_le_short((short)4, (short)4);
	T_quiet_le_short((short)6, (short)5);
	T_step_le_short(2, (short)7, (short)7);
	T_step_le_short(3, (short)9, (short)8);
	T_assert_le_short((short)10, (short)10);
	T_assert_le_short((short)12, (short)11);
}

T_TEST_CASE(step_assert_lt_short)
{
	T_plan(2);
	T_step_assert_lt_short(0, (short)1, (short)2);
	T_step_assert_lt_short(1, (short)3, (short)3);
}

T_TEST_CASE(check_lt_short)
{
	T_lt_short((short)1, (short)2);
	T_lt_short((short)3, (short)3);
	T_quiet_lt_short((short)4, (short)5);
	T_quiet_lt_short((short)6, (short)6);
	T_step_lt_short(2, (short)7, (short)8);
	T_step_lt_short(3, (short)9, (short)9);
	T_assert_lt_short((short)10, (short)11);
	T_assert_lt_short((short)12, (short)12);
}

T_TEST_CASE(step_assert_eq_ushort)
{
	T_plan(2);
	T_step_assert_eq_ushort(0, (unsigned short)1, (unsigned short)1);
	T_step_assert_eq_ushort(1, (unsigned short)2, (unsigned short)3);
}

T_TEST_CASE(check_eq_ushort)
{
	T_eq_ushort((unsigned short)1, (unsigned short)1);
	T_eq_ushort((unsigned short)2, (unsigned short)3);
	T_quiet_eq_ushort((unsigned short)4, (unsigned short)4);
	T_quiet_eq_ushort((unsigned short)5, (unsigned short)6);
	T_step_eq_ushort(2, (unsigned short)7, (unsigned short)7);
	T_step_eq_ushort(3, (unsigned short)8, (unsigned short)9);
	T_assert_eq_ushort((unsigned short)10, (unsigned short)10);
	T_assert_eq_ushort((unsigned short)11, (unsigned short)12);
}

T_TEST_CASE(step_assert_ne_ushort)
{
	T_plan(2);
	T_step_assert_ne_ushort(0, (unsigned short)2, (unsigned short)3);
	T_step_assert_ne_ushort(1, (unsigned short)1, (unsigned short)1);
}

T_TEST_CASE(check_ne_ushort)
{
	T_ne_ushort((unsigned short)2, (unsigned short)3);
	T_ne_ushort((unsigned short)1, (unsigned short)1);
	T_quiet_ne_ushort((unsigned short)5, (unsigned short)6);
	T_quiet_ne_ushort((unsigned short)4, (unsigned short)4);
	T_step_ne_ushort(2, (unsigned short)8, (unsigned short)9);
	T_step_ne_ushort(3, (unsigned short)7, (unsigned short)7);
	T_assert_ne_ushort((unsigned short)11, (unsigned short)12);
	T_assert_ne_ushort((unsigned short)10, (unsigned short)10);
}

T_TEST_CASE(step_assert_ge_ushort)
{
	T_plan(2);
	T_step_assert_ge_ushort(0, (unsigned short)1, (unsigned short)1);
	T_step_assert_ge_ushort(1, (unsigned short)2, (unsigned short)3);
}

T_TEST_CASE(check_ge_ushort)
{
	T_ge_ushort((unsigned short)1, (unsigned short)1);
	T_ge_ushort((unsigned short)2, (unsigned short)3);
	T_quiet_ge_ushort((unsigned short)4, (unsigned short)4);
	T_quiet_ge_ushort((unsigned short)5, (unsigned short)6);
	T_step_ge_ushort(2, (unsigned short)7, (unsigned short)7);
	T_step_ge_ushort(3, (unsigned short)8, (unsigned short)9);
	T_assert_ge_ushort((unsigned short)10, (unsigned short)10);
	T_assert_ge_ushort((unsigned short)11, (unsigned short)12);
}

T_TEST_CASE(step_assert_gt_ushort)
{
	T_plan(2);
	T_step_assert_gt_ushort(0, (unsigned short)2, (unsigned short)1);
	T_step_assert_gt_ushort(1, (unsigned short)3, (unsigned short)3);
}

T_TEST_CASE(check_gt_ushort)
{
	T_gt_ushort((unsigned short)2, (unsigned short)1);
	T_gt_ushort((unsigned short)3, (unsigned short)3);
	T_quiet_gt_ushort((unsigned short)5, (unsigned short)4);
	T_quiet_gt_ushort((unsigned short)6, (unsigned short)6);
	T_step_gt_ushort(2, (unsigned short)8, (unsigned short)7);
	T_step_gt_ushort(3, (unsigned short)9, (unsigned short)9);
	T_assert_gt_ushort((unsigned short)10, (unsigned short)11);
	T_assert_gt_ushort((unsigned short)12, (unsigned short)12);
}

T_TEST_CASE(step_assert_le_ushort)
{
	T_plan(2);
	T_step_assert_le_ushort(0, (unsigned short)1, (unsigned short)1);
	T_step_assert_le_ushort(1, (unsigned short)3, (unsigned short)2);
}

T_TEST_CASE(check_le_ushort)
{
	T_le_ushort((unsigned short)1, (unsigned short)1);
	T_le_ushort((unsigned short)3, (unsigned short)2);
	T_quiet_le_ushort((unsigned short)4, (unsigned short)4);
	T_quiet_le_ushort((unsigned short)6, (unsigned short)5);
	T_step_le_ushort(2, (unsigned short)7, (unsigned short)7);
	T_step_le_ushort(3, (unsigned short)9, (unsigned short)8);
	T_assert_le_ushort((unsigned short)10, (unsigned short)10);
	T_assert_le_ushort((unsigned short)12, (unsigned short)11);
}

T_TEST_CASE(step_assert_lt_ushort)
{
	T_plan(2);
	T_step_assert_lt_ushort(0, (unsigned short)1, (unsigned short)2);
	T_step_assert_lt_ushort(1, (unsigned short)3, (unsigned short)3);
}

T_TEST_CASE(check_lt_ushort)
{
	T_lt_ushort((unsigned short)1, (unsigned short)2);
	T_lt_ushort((unsigned short)3, (unsigned short)3);
	T_quiet_lt_ushort((unsigned short)4, (unsigned short)5);
	T_quiet_lt_ushort((unsigned short)6, (unsigned short)6);
	T_step_lt_ushort(2, (unsigned short)7, (unsigned short)8);
	T_step_lt_ushort(3, (unsigned short)9, (unsigned short)9);
	T_assert_lt_ushort((unsigned short)10, (unsigned short)11);
	T_assert_lt_ushort((unsigned short)12, (unsigned short)12);
}

T_TEST_CASE(step_assert_eq_int)
{
	T_plan(2);
	T_step_assert_eq_int(0, 1, 1);
	T_step_assert_eq_int(1, 2, 3);
}

T_TEST_CASE(check_eq_int)
{
	T_eq_int(1, 1);
	T_eq_int(2, 3);
	T_quiet_eq_int(4, 4);
	T_quiet_eq_int(5, 6);
	T_step_eq_int(2, 7, 7);
	T_step_eq_int(3, 8, 9);
	T_assert_eq_int(10, 10);
	T_assert_eq_int(11, 12);
}

T_TEST_CASE(step_assert_ne_int)
{
	T_plan(2);
	T_step_assert_ne_int(0, 2, 3);
	T_step_assert_ne_int(1, 1, 1);
}

T_TEST_CASE(check_ne_int)
{
	T_ne_int(2, 3);
	T_ne_int(1, 1);
	T_quiet_ne_int(5, 6);
	T_quiet_ne_int(4, 4);
	T_step_ne_int(2, 8, 9);
	T_step_ne_int(3, 7, 7);
	T_assert_ne_int(11, 12);
	T_assert_ne_int(10, 10);
}

T_TEST_CASE(step_assert_ge_int)
{
	T_plan(2);
	T_step_assert_ge_int(0, 1, 1);
	T_step_assert_ge_int(1, 2, 3);
}

T_TEST_CASE(check_ge_int)
{
	T_ge_int(1, 1);
	T_ge_int(2, 3);
	T_quiet_ge_int(4, 4);
	T_quiet_ge_int(5, 6);
	T_step_ge_int(2, 7, 7);
	T_step_ge_int(3, 8, 9);
	T_assert_ge_int(10, 10);
	T_assert_ge_int(11, 12);
}

T_TEST_CASE(step_assert_gt_int)
{
	T_plan(2);
	T_step_assert_gt_int(0, 2, 1);
	T_step_assert_gt_int(1, 3, 3);
}

T_TEST_CASE(check_gt_int)
{
	T_gt_int(2, 1);
	T_gt_int(3, 3);
	T_quiet_gt_int(5, 4);
	T_quiet_gt_int(6, 6);
	T_step_gt_int(2, 8, 7);
	T_step_gt_int(3, 9, 9);
	T_assert_gt_int(10, 11);
	T_assert_gt_int(12, 12);
}

T_TEST_CASE(step_assert_le_int)
{
	T_plan(2);
	T_step_assert_le_int(0, 1, 1);
	T_step_assert_le_int(1, 3, 2);
}

T_TEST_CASE(check_le_int)
{
	T_le_int(1, 1);
	T_le_int(3, 2);
	T_quiet_le_int(4, 4);
	T_quiet_le_int(6, 5);
	T_step_le_int(2, 7, 7);
	T_step_le_int(3, 9, 8);
	T_assert_le_int(10, 10);
	T_assert_le_int(12, 11);
}

T_TEST_CASE(step_assert_lt_int)
{
	T_plan(2);
	T_step_assert_lt_int(0, 1, 2);
	T_step_assert_lt_int(1, 3, 3);
}

T_TEST_CASE(check_lt_int)
{
	T_lt_int(1, 2);
	T_lt_int(3, 3);
	T_quiet_lt_int(4, 5);
	T_quiet_lt_int(6, 6);
	T_step_lt_int(2, 7, 8);
	T_step_lt_int(3, 9, 9);
	T_assert_lt_int(10, 11);
	T_assert_lt_int(12, 12);
}

T_TEST_CASE(step_assert_eq_uint)
{
	T_plan(2);
	T_step_assert_eq_uint(0, 1U, 1U);
	T_step_assert_eq_uint(1, 2U, 3U);
}

T_TEST_CASE(check_eq_uint)
{
	T_eq_uint(1U, 1U);
	T_eq_uint(2U, 3U);
	T_quiet_eq_uint(4U, 4U);
	T_quiet_eq_uint(5U, 6U);
	T_step_eq_uint(2, 7U, 7U);
	T_step_eq_uint(3, 8U, 9U);
	T_assert_eq_uint(10U, 10U);
	T_assert_eq_uint(11U, 12U);
}

T_TEST_CASE(step_assert_ne_uint)
{
	T_plan(2);
	T_step_assert_ne_uint(0, 2U, 3U);
	T_step_assert_ne_uint(1, 1U, 1U);
}

T_TEST_CASE(check_ne_uint)
{
	T_ne_uint(2U, 3U);
	T_ne_uint(1U, 1U);
	T_quiet_ne_uint(5U, 6U);
	T_quiet_ne_uint(4U, 4U);
	T_step_ne_uint(2, 8U, 9U);
	T_step_ne_uint(3, 7U, 7U);
	T_assert_ne_uint(11U, 12U);
	T_assert_ne_uint(10U, 10U);
}

T_TEST_CASE(step_assert_ge_uint)
{
	T_plan(2);
	T_step_assert_ge_uint(0, 1U, 1U);
	T_step_assert_ge_uint(1, 2U, 3U);
}

T_TEST_CASE(check_ge_uint)
{
	T_ge_uint(1U, 1U);
	T_ge_uint(2U, 3U);
	T_quiet_ge_uint(4U, 4U);
	T_quiet_ge_uint(5U, 6U);
	T_step_ge_uint(2, 7U, 7U);
	T_step_ge_uint(3, 8U, 9U);
	T_assert_ge_uint(10U, 10U);
	T_assert_ge_uint(11U, 12U);
}

T_TEST_CASE(step_assert_gt_uint)
{
	T_plan(2);
	T_step_assert_gt_uint(0, 2U, 1U);
	T_step_assert_gt_uint(1, 3U, 3U);
}

T_TEST_CASE(check_gt_uint)
{
	T_gt_uint(2U, 1U);
	T_gt_uint(3U, 3U);
	T_quiet_gt_uint(5U, 4U);
	T_quiet_gt_uint(6U, 6U);
	T_step_gt_uint(2, 8U, 7U);
	T_step_gt_uint(3, 9U, 9U);
	T_assert_gt_uint(10U, 11U);
	T_assert_gt_uint(12U, 12U);
}

T_TEST_CASE(step_assert_le_uint)
{
	T_plan(2);
	T_step_assert_le_uint(0, 1U, 1U);
	T_step_assert_le_uint(1, 3U, 2U);
}

T_TEST_CASE(check_le_uint)
{
	T_le_uint(1U, 1U);
	T_le_uint(3U, 2U);
	T_quiet_le_uint(4U, 4U);
	T_quiet_le_uint(6U, 5U);
	T_step_le_uint(2, 7U, 7U);
	T_step_le_uint(3, 9U, 8U);
	T_assert_le_uint(10U, 10U);
	T_assert_le_uint(12U, 11U);
}

T_TEST_CASE(step_assert_lt_uint)
{
	T_plan(2);
	T_step_assert_lt_uint(0, 1U, 2U);
	T_step_assert_lt_uint(1, 3U, 3U);
}

T_TEST_CASE(check_lt_uint)
{
	T_lt_uint(1U, 2U);
	T_lt_uint(3U, 3U);
	T_quiet_lt_uint(4U, 5U);
	T_quiet_lt_uint(6U, 6U);
	T_step_lt_uint(2, 7U, 8U);
	T_step_lt_uint(3, 9U, 9U);
	T_assert_lt_uint(10U, 11U);
	T_assert_lt_uint(12U, 12U);
}

T_TEST_CASE(step_assert_eq_long)
{
	T_plan(2);
	T_step_assert_eq_long(0, 1L, 1L);
	T_step_assert_eq_long(1, 2L, 3L);
}

T_TEST_CASE(check_eq_long)
{
	T_eq_long(1L, 1L);
	T_eq_long(2L, 3L);
	T_quiet_eq_long(4L, 4L);
	T_quiet_eq_long(5L, 6L);
	T_step_eq_long(2, 7L, 7L);
	T_step_eq_long(3, 8L, 9L);
	T_assert_eq_long(10L, 10L);
	T_assert_eq_long(11L, 12L);
}

T_TEST_CASE(step_assert_ne_long)
{
	T_plan(2);
	T_step_assert_ne_long(0, 2L, 3L);
	T_step_assert_ne_long(1, 1L, 1L);
}

T_TEST_CASE(check_ne_long)
{
	T_ne_long(2L, 3L);
	T_ne_long(1L, 1L);
	T_quiet_ne_long(5L, 6L);
	T_quiet_ne_long(4L, 4L);
	T_step_ne_long(2, 8L, 9L);
	T_step_ne_long(3, 7L, 7L);
	T_assert_ne_long(11L, 12L);
	T_assert_ne_long(10L, 10L);
}

T_TEST_CASE(step_assert_ge_long)
{
	T_plan(2);
	T_step_assert_ge_long(0, 1L, 1L);
	T_step_assert_ge_long(1, 2L, 3L);
}

T_TEST_CASE(check_ge_long)
{
	T_ge_long(1L, 1L);
	T_ge_long(2L, 3L);
	T_quiet_ge_long(4L, 4L);
	T_quiet_ge_long(5L, 6L);
	T_step_ge_long(2, 7L, 7L);
	T_step_ge_long(3, 8L, 9L);
	T_assert_ge_long(10L, 10L);
	T_assert_ge_long(11L, 12L);
}

T_TEST_CASE(step_assert_gt_long)
{
	T_plan(2);
	T_step_assert_gt_long(0, 2L, 1L);
	T_step_assert_gt_long(1, 3L, 3L);
}

T_TEST_CASE(check_gt_long)
{
	T_gt_long(2L, 1L);
	T_gt_long(3L, 3L);
	T_quiet_gt_long(5L, 4L);
	T_quiet_gt_long(6L, 6L);
	T_step_gt_long(2, 8L, 7L);
	T_step_gt_long(3, 9L, 9L);
	T_assert_gt_long(10L, 11L);
	T_assert_gt_long(12L, 12L);
}

T_TEST_CASE(step_assert_le_long)
{
	T_plan(2);
	T_step_assert_le_long(0, 1L, 1L);
	T_step_assert_le_long(1, 3L, 2L);
}

T_TEST_CASE(check_le_long)
{
	T_le_long(1L, 1L);
	T_le_long(3L, 2L);
	T_quiet_le_long(4L, 4L);
	T_quiet_le_long(6L, 5L);
	T_step_le_long(2, 7L, 7L);
	T_step_le_long(3, 9L, 8L);
	T_assert_le_long(10L, 10L);
	T_assert_le_long(12L, 11L);
}

T_TEST_CASE(step_assert_lt_long)
{
	T_plan(2);
	T_step_assert_lt_long(0, 1L, 2L);
	T_step_assert_lt_long(1, 3L, 3L);
}

T_TEST_CASE(check_lt_long)
{
	T_lt_long(1L, 2L);
	T_lt_long(3L, 3L);
	T_quiet_lt_long(4L, 5L);
	T_quiet_lt_long(6L, 6L);
	T_step_lt_long(2, 7L, 8L);
	T_step_lt_long(3, 9L, 9L);
	T_assert_lt_long(10L, 11L);
	T_assert_lt_long(12L, 12L);
}

T_TEST_CASE(step_assert_eq_ulong)
{
	T_plan(2);
	T_step_assert_eq_ulong(0, 1UL, 1UL);
	T_step_assert_eq_ulong(1, 2UL, 3UL);
}

T_TEST_CASE(check_eq_ulong)
{
	T_eq_ulong(1UL, 1UL);
	T_eq_ulong(2UL, 3UL);
	T_quiet_eq_ulong(4UL, 4UL);
	T_quiet_eq_ulong(5UL, 6UL);
	T_step_eq_ulong(2, 7UL, 7UL);
	T_step_eq_ulong(3, 8UL, 9UL);
	T_assert_eq_ulong(10UL, 10UL);
	T_assert_eq_ulong(11UL, 12UL);
}

T_TEST_CASE(step_assert_ne_ulong)
{
	T_plan(2);
	T_step_assert_ne_ulong(0, 2UL, 3UL);
	T_step_assert_ne_ulong(1, 1UL, 1UL);
}

T_TEST_CASE(check_ne_ulong)
{
	T_ne_ulong(2UL, 3UL);
	T_ne_ulong(1UL, 1UL);
	T_quiet_ne_ulong(5UL, 6UL);
	T_quiet_ne_ulong(4UL, 4UL);
	T_step_ne_ulong(2, 8UL, 9UL);
	T_step_ne_ulong(3, 7UL, 7UL);
	T_assert_ne_ulong(11UL, 12UL);
	T_assert_ne_ulong(10UL, 10UL);
}

T_TEST_CASE(step_assert_ge_ulong)
{
	T_plan(2);
	T_step_assert_ge_ulong(0, 1UL, 1UL);
	T_step_assert_ge_ulong(1, 2UL, 3UL);
}

T_TEST_CASE(check_ge_ulong)
{
	T_ge_ulong(1UL, 1UL);
	T_ge_ulong(2UL, 3UL);
	T_quiet_ge_ulong(4UL, 4UL);
	T_quiet_ge_ulong(5UL, 6UL);
	T_step_ge_ulong(2, 7UL, 7UL);
	T_step_ge_ulong(3, 8UL, 9UL);
	T_assert_ge_ulong(10UL, 10UL);
	T_assert_ge_ulong(11UL, 12UL);
}

T_TEST_CASE(step_assert_gt_ulong)
{
	T_plan(2);
	T_step_assert_gt_ulong(0, 2UL, 1UL);
	T_step_assert_gt_ulong(1, 3UL, 3UL);
}

T_TEST_CASE(check_gt_ulong)
{
	T_gt_ulong(2UL, 1UL);
	T_gt_ulong(3UL, 3UL);
	T_quiet_gt_ulong(5UL, 4UL);
	T_quiet_gt_ulong(6UL, 6UL);
	T_step_gt_ulong(2, 8UL, 7UL);
	T_step_gt_ulong(3, 9UL, 9UL);
	T_assert_gt_ulong(10UL, 11UL);
	T_assert_gt_ulong(12UL, 12UL);
}

T_TEST_CASE(step_assert_le_ulong)
{
	T_plan(2);
	T_step_assert_le_ulong(0, 1UL, 1UL);
	T_step_assert_le_ulong(1, 3UL, 2UL);
}

T_TEST_CASE(check_le_ulong)
{
	T_le_ulong(1UL, 1UL);
	T_le_ulong(3UL, 2UL);
	T_quiet_le_ulong(4UL, 4UL);
	T_quiet_le_ulong(6UL, 5UL);
	T_step_le_ulong(2, 7UL, 7UL);
	T_step_le_ulong(3, 9UL, 8UL);
	T_assert_le_ulong(10UL, 10UL);
	T_assert_le_ulong(12UL, 11UL);
}

T_TEST_CASE(step_assert_lt_ulong)
{
	T_plan(2);
	T_step_assert_lt_ulong(0, 1UL, 2UL);
	T_step_assert_lt_ulong(1, 3UL, 3UL);
}

T_TEST_CASE(check_lt_ulong)
{
	T_lt_ulong(1UL, 2UL);
	T_lt_ulong(3UL, 3UL);
	T_quiet_lt_ulong(4UL, 5UL);
	T_quiet_lt_ulong(6UL, 6UL);
	T_step_lt_ulong(2, 7UL, 8UL);
	T_step_lt_ulong(3, 9UL, 9UL);
	T_assert_lt_ulong(10UL, 11UL);
	T_assert_lt_ulong(12UL, 12UL);
}

T_TEST_CASE(step_assert_eq_ll)
{
	T_plan(2);
	T_step_assert_eq_ll(0, 1LL, 1LL);
	T_step_assert_eq_ll(1, 2LL, 3LL);
}

T_TEST_CASE(check_eq_ll)
{
	T_eq_ll(1LL, 1LL);
	T_eq_ll(2LL, 3LL);
	T_quiet_eq_ll(4LL, 4LL);
	T_quiet_eq_ll(5LL, 6LL);
	T_step_eq_ll(2, 7LL, 7LL);
	T_step_eq_ll(3, 8LL, 9LL);
	T_assert_eq_ll(10LL, 10LL);
	T_assert_eq_ll(11LL, 12LL);
}

T_TEST_CASE(step_assert_ne_ll)
{
	T_plan(2);
	T_step_assert_ne_ll(0, 2LL, 3LL);
	T_step_assert_ne_ll(1, 1LL, 1LL);
}

T_TEST_CASE(check_ne_ll)
{
	T_ne_ll(2LL, 3LL);
	T_ne_ll(1LL, 1LL);
	T_quiet_ne_ll(5LL, 6LL);
	T_quiet_ne_ll(4LL, 4LL);
	T_step_ne_ll(2, 8LL, 9LL);
	T_step_ne_ll(3, 7LL, 7LL);
	T_assert_ne_ll(11LL, 12LL);
	T_assert_ne_ll(10LL, 10LL);
}

T_TEST_CASE(step_assert_ge_ll)
{
	T_plan(2);
	T_step_assert_ge_ll(0, 1LL, 1LL);
	T_step_assert_ge_ll(1, 2LL, 3LL);
}

T_TEST_CASE(check_ge_ll)
{
	T_ge_ll(1LL, 1LL);
	T_ge_ll(2LL, 3LL);
	T_quiet_ge_ll(4LL, 4LL);
	T_quiet_ge_ll(5LL, 6LL);
	T_step_ge_ll(2, 7LL, 7LL);
	T_step_ge_ll(3, 8LL, 9LL);
	T_assert_ge_ll(10LL, 10LL);
	T_assert_ge_ll(11LL, 12LL);
}

T_TEST_CASE(step_assert_gt_ll)
{
	T_plan(2);
	T_step_assert_gt_ll(0, 2LL, 1LL);
	T_step_assert_gt_ll(1, 3LL, 3LL);
}

T_TEST_CASE(check_gt_ll)
{
	T_gt_ll(2LL, 1LL);
	T_gt_ll(3LL, 3LL);
	T_quiet_gt_ll(5LL, 4LL);
	T_quiet_gt_ll(6LL, 6LL);
	T_step_gt_ll(2, 8LL, 7LL);
	T_step_gt_ll(3, 9LL, 9LL);
	T_assert_gt_ll(10LL, 11LL);
	T_assert_gt_ll(12LL, 12LL);
}

T_TEST_CASE(step_assert_le_ll)
{
	T_plan(2);
	T_step_assert_le_ll(0, 1LL, 1LL);
	T_step_assert_le_ll(1, 3LL, 2LL);
}

T_TEST_CASE(check_le_ll)
{
	T_le_ll(1LL, 1LL);
	T_le_ll(3LL, 2LL);
	T_quiet_le_ll(4LL, 4LL);
	T_quiet_le_ll(6LL, 5LL);
	T_step_le_ll(2, 7LL, 7LL);
	T_step_le_ll(3, 9LL, 8LL);
	T_assert_le_ll(10LL, 10LL);
	T_assert_le_ll(12LL, 11LL);
}

T_TEST_CASE(step_assert_lt_ll)
{
	T_plan(2);
	T_step_assert_lt_ll(0, 1LL, 2LL);
	T_step_assert_lt_ll(1, 3LL, 3LL);
}

T_TEST_CASE(check_lt_ll)
{
	T_lt_ll(1LL, 2LL);
	T_lt_ll(3LL, 3LL);
	T_quiet_lt_ll(4LL, 5LL);
	T_quiet_lt_ll(6LL, 6LL);
	T_step_lt_ll(2, 7LL, 8LL);
	T_step_lt_ll(3, 9LL, 9LL);
	T_assert_lt_ll(10LL, 11LL);
	T_assert_lt_ll(12LL, 12LL);
}

T_TEST_CASE(step_assert_eq_ull)
{
	T_plan(2);
	T_step_assert_eq_ull(0, 1ULL, 1ULL);
	T_step_assert_eq_ull(1, 2ULL, 3ULL);
}

T_TEST_CASE(check_eq_ull)
{
	T_eq_ull(1ULL, 1ULL);
	T_eq_ull(2ULL, 3ULL);
	T_quiet_eq_ull(4ULL, 4ULL);
	T_quiet_eq_ull(5ULL, 6ULL);
	T_step_eq_ull(2, 7ULL, 7ULL);
	T_step_eq_ull(3, 8ULL, 9ULL);
	T_assert_eq_ull(10ULL, 10ULL);
	T_assert_eq_ull(11ULL, 12ULL);
}

T_TEST_CASE(step_assert_ne_ull)
{
	T_plan(2);
	T_step_assert_ne_ull(0, 2ULL, 3ULL);
	T_step_assert_ne_ull(1, 1ULL, 1ULL);
}

T_TEST_CASE(check_ne_ull)
{
	T_ne_ull(2ULL, 3ULL);
	T_ne_ull(1ULL, 1ULL);
	T_quiet_ne_ull(5ULL, 6ULL);
	T_quiet_ne_ull(4ULL, 4ULL);
	T_step_ne_ull(2, 8ULL, 9ULL);
	T_step_ne_ull(3, 7ULL, 7ULL);
	T_assert_ne_ull(11ULL, 12ULL);
	T_assert_ne_ull(10ULL, 10ULL);
}

T_TEST_CASE(step_assert_ge_ull)
{
	T_plan(2);
	T_step_assert_ge_ull(0, 1ULL, 1ULL);
	T_step_assert_ge_ull(1, 2ULL, 3ULL);
}

T_TEST_CASE(check_ge_ull)
{
	T_ge_ull(1ULL, 1ULL);
	T_ge_ull(2ULL, 3ULL);
	T_quiet_ge_ull(4ULL, 4ULL);
	T_quiet_ge_ull(5ULL, 6ULL);
	T_step_ge_ull(2, 7ULL, 7ULL);
	T_step_ge_ull(3, 8ULL, 9ULL);
	T_assert_ge_ull(10ULL, 10ULL);
	T_assert_ge_ull(11ULL, 12ULL);
}

T_TEST_CASE(step_assert_gt_ull)
{
	T_plan(2);
	T_step_assert_gt_ull(0, 2ULL, 1ULL);
	T_step_assert_gt_ull(1, 3ULL, 3ULL);
}

T_TEST_CASE(check_gt_ull)
{
	T_gt_ull(2ULL, 1ULL);
	T_gt_ull(3ULL, 3ULL);
	T_quiet_gt_ull(5ULL, 4ULL);
	T_quiet_gt_ull(6ULL, 6ULL);
	T_step_gt_ull(2, 8ULL, 7ULL);
	T_step_gt_ull(3, 9ULL, 9ULL);
	T_assert_gt_ull(10ULL, 11ULL);
	T_assert_gt_ull(12ULL, 12ULL);
}

T_TEST_CASE(step_assert_le_ull)
{
	T_plan(2);
	T_step_assert_le_ull(0, 1ULL, 1ULL);
	T_step_assert_le_ull(1, 3ULL, 2ULL);
}

T_TEST_CASE(check_le_ull)
{
	T_le_ull(1ULL, 1ULL);
	T_le_ull(3ULL, 2ULL);
	T_quiet_le_ull(4ULL, 4ULL);
	T_quiet_le_ull(6ULL, 5ULL);
	T_step_le_ull(2, 7ULL, 7ULL);
	T_step_le_ull(3, 9ULL, 8ULL);
	T_assert_le_ull(10ULL, 10ULL);
	T_assert_le_ull(12ULL, 11ULL);
}

T_TEST_CASE(step_assert_lt_ull)
{
	T_plan(2);
	T_step_assert_lt_ull(0, 1ULL, 2ULL);
	T_step_assert_lt_ull(1, 3ULL, 3ULL);
}

T_TEST_CASE(check_lt_ull)
{
	T_lt_ull(1ULL, 2ULL);
	T_lt_ull(3ULL, 3ULL);
	T_quiet_lt_ull(4ULL, 5ULL);
	T_quiet_lt_ull(6ULL, 6ULL);
	T_step_lt_ull(2, 7ULL, 8ULL);
	T_step_lt_ull(3, 9ULL, 9ULL);
	T_assert_lt_ull(10ULL, 11ULL);
	T_assert_lt_ull(12ULL, 12ULL);
}

T_TEST_CASE(step_assert_eq_i8)
{
	T_plan(2);
	T_step_assert_eq_i8(0, INT8_C(1), INT8_C(1));
	T_step_assert_eq_i8(1, INT8_C(2), INT8_C(3));
}

T_TEST_CASE(check_eq_i8)
{
	T_eq_i8(INT8_C(1), INT8_C(1));
	T_eq_i8(INT8_C(2), INT8_C(3));
	T_quiet_eq_i8(INT8_C(4), INT8_C(4));
	T_quiet_eq_i8(INT8_C(5), INT8_C(6));
	T_step_eq_i8(2, INT8_C(7), INT8_C(7));
	T_step_eq_i8(3, INT8_C(8), INT8_C(9));
	T_assert_eq_i8(INT8_C(10), INT8_C(10));
	T_assert_eq_i8(INT8_C(11), INT8_C(12));
}

T_TEST_CASE(step_assert_ne_i8)
{
	T_plan(2);
	T_step_assert_ne_i8(0, INT8_C(2), INT8_C(3));
	T_step_assert_ne_i8(1, INT8_C(1), INT8_C(1));
}

T_TEST_CASE(check_ne_i8)
{
	T_ne_i8(INT8_C(2), INT8_C(3));
	T_ne_i8(INT8_C(1), INT8_C(1));
	T_quiet_ne_i8(INT8_C(5), INT8_C(6));
	T_quiet_ne_i8(INT8_C(4), INT8_C(4));
	T_step_ne_i8(2, INT8_C(8), INT8_C(9));
	T_step_ne_i8(3, INT8_C(7), INT8_C(7));
	T_assert_ne_i8(INT8_C(11), INT8_C(12));
	T_assert_ne_i8(INT8_C(10), INT8_C(10));
}

T_TEST_CASE(step_assert_ge_i8)
{
	T_plan(2);
	T_step_assert_ge_i8(0, INT8_C(1), INT8_C(1));
	T_step_assert_ge_i8(1, INT8_C(2), INT8_C(3));
}

T_TEST_CASE(check_ge_i8)
{
	T_ge_i8(INT8_C(1), INT8_C(1));
	T_ge_i8(INT8_C(2), INT8_C(3));
	T_quiet_ge_i8(INT8_C(4), INT8_C(4));
	T_quiet_ge_i8(INT8_C(5), INT8_C(6));
	T_step_ge_i8(2, INT8_C(7), INT8_C(7));
	T_step_ge_i8(3, INT8_C(8), INT8_C(9));
	T_assert_ge_i8(INT8_C(10), INT8_C(10));
	T_assert_ge_i8(INT8_C(11), INT8_C(12));
}

T_TEST_CASE(step_assert_gt_i8)
{
	T_plan(2);
	T_step_assert_gt_i8(0, INT8_C(2), INT8_C(1));
	T_step_assert_gt_i8(1, INT8_C(3), INT8_C(3));
}

T_TEST_CASE(check_gt_i8)
{
	T_gt_i8(INT8_C(2), INT8_C(1));
	T_gt_i8(INT8_C(3), INT8_C(3));
	T_quiet_gt_i8(INT8_C(5), INT8_C(4));
	T_quiet_gt_i8(INT8_C(6), INT8_C(6));
	T_step_gt_i8(2, INT8_C(8), INT8_C(7));
	T_step_gt_i8(3, INT8_C(9), INT8_C(9));
	T_assert_gt_i8(INT8_C(10), INT8_C(11));
	T_assert_gt_i8(INT8_C(12), INT8_C(12));
}

T_TEST_CASE(step_assert_le_i8)
{
	T_plan(2);
	T_step_assert_le_i8(0, INT8_C(1), INT8_C(1));
	T_step_assert_le_i8(1, INT8_C(3), INT8_C(2));
}

T_TEST_CASE(check_le_i8)
{
	T_le_i8(INT8_C(1), INT8_C(1));
	T_le_i8(INT8_C(3), INT8_C(2));
	T_quiet_le_i8(INT8_C(4), INT8_C(4));
	T_quiet_le_i8(INT8_C(6), INT8_C(5));
	T_step_le_i8(2, INT8_C(7), INT8_C(7));
	T_step_le_i8(3, INT8_C(9), INT8_C(8));
	T_assert_le_i8(INT8_C(10), INT8_C(10));
	T_assert_le_i8(INT8_C(12), INT8_C(11));
}

T_TEST_CASE(step_assert_lt_i8)
{
	T_plan(2);
	T_step_assert_lt_i8(0, INT8_C(1), INT8_C(2));
	T_step_assert_lt_i8(1, INT8_C(3), INT8_C(3));
}

T_TEST_CASE(check_lt_i8)
{
	T_lt_i8(INT8_C(1), INT8_C(2));
	T_lt_i8(INT8_C(3), INT8_C(3));
	T_quiet_lt_i8(INT8_C(4), INT8_C(5));
	T_quiet_lt_i8(INT8_C(6), INT8_C(6));
	T_step_lt_i8(2, INT8_C(7), INT8_C(8));
	T_step_lt_i8(3, INT8_C(9), INT8_C(9));
	T_assert_lt_i8(INT8_C(10), INT8_C(11));
	T_assert_lt_i8(INT8_C(12), INT8_C(12));
}

T_TEST_CASE(step_assert_eq_u8)
{
	T_plan(2);
	T_step_assert_eq_u8(0, UINT8_C(1), UINT8_C(1));
	T_step_assert_eq_u8(1, UINT8_C(2), UINT8_C(3));
}

T_TEST_CASE(check_eq_u8)
{
	T_eq_u8(UINT8_C(1), UINT8_C(1));
	T_eq_u8(UINT8_C(2), UINT8_C(3));
	T_quiet_eq_u8(UINT8_C(4), UINT8_C(4));
	T_quiet_eq_u8(UINT8_C(5), UINT8_C(6));
	T_step_eq_u8(2, UINT8_C(7), UINT8_C(7));
	T_step_eq_u8(3, UINT8_C(8), UINT8_C(9));
	T_assert_eq_u8(UINT8_C(10), UINT8_C(10));
	T_assert_eq_u8(UINT8_C(11), UINT8_C(12));
}

T_TEST_CASE(step_assert_ne_u8)
{
	T_plan(2);
	T_step_assert_ne_u8(0, UINT8_C(2), UINT8_C(3));
	T_step_assert_ne_u8(1, UINT8_C(1), UINT8_C(1));
}

T_TEST_CASE(check_ne_u8)
{
	T_ne_u8(UINT8_C(2), UINT8_C(3));
	T_ne_u8(UINT8_C(1), UINT8_C(1));
	T_quiet_ne_u8(UINT8_C(5), UINT8_C(6));
	T_quiet_ne_u8(UINT8_C(4), UINT8_C(4));
	T_step_ne_u8(2, UINT8_C(8), UINT8_C(9));
	T_step_ne_u8(3, UINT8_C(7), UINT8_C(7));
	T_assert_ne_u8(UINT8_C(11), UINT8_C(12));
	T_assert_ne_u8(UINT8_C(10), UINT8_C(10));
}

T_TEST_CASE(step_assert_ge_u8)
{
	T_plan(2);
	T_step_assert_ge_u8(0, UINT8_C(1), UINT8_C(1));
	T_step_assert_ge_u8(1, UINT8_C(2), UINT8_C(3));
}

T_TEST_CASE(check_ge_u8)
{
	T_ge_u8(UINT8_C(1), UINT8_C(1));
	T_ge_u8(UINT8_C(2), UINT8_C(3));
	T_quiet_ge_u8(UINT8_C(4), UINT8_C(4));
	T_quiet_ge_u8(UINT8_C(5), UINT8_C(6));
	T_step_ge_u8(2, UINT8_C(7), UINT8_C(7));
	T_step_ge_u8(3, UINT8_C(8), UINT8_C(9));
	T_assert_ge_u8(UINT8_C(10), UINT8_C(10));
	T_assert_ge_u8(UINT8_C(11), UINT8_C(12));
}

T_TEST_CASE(step_assert_gt_u8)
{
	T_plan(2);
	T_step_assert_gt_u8(0, UINT8_C(2), UINT8_C(1));
	T_step_assert_gt_u8(1, UINT8_C(3), UINT8_C(3));
}

T_TEST_CASE(check_gt_u8)
{
	T_gt_u8(UINT8_C(2), UINT8_C(1));
	T_gt_u8(UINT8_C(3), UINT8_C(3));
	T_quiet_gt_u8(UINT8_C(5), UINT8_C(4));
	T_quiet_gt_u8(UINT8_C(6), UINT8_C(6));
	T_step_gt_u8(2, UINT8_C(8), UINT8_C(7));
	T_step_gt_u8(3, UINT8_C(9), UINT8_C(9));
	T_assert_gt_u8(UINT8_C(10), UINT8_C(11));
	T_assert_gt_u8(UINT8_C(12), UINT8_C(12));
}

T_TEST_CASE(step_assert_le_u8)
{
	T_plan(2);
	T_step_assert_le_u8(0, UINT8_C(1), UINT8_C(1));
	T_step_assert_le_u8(1, UINT8_C(3), UINT8_C(2));
}

T_TEST_CASE(check_le_u8)
{
	T_le_u8(UINT8_C(1), UINT8_C(1));
	T_le_u8(UINT8_C(3), UINT8_C(2));
	T_quiet_le_u8(UINT8_C(4), UINT8_C(4));
	T_quiet_le_u8(UINT8_C(6), UINT8_C(5));
	T_step_le_u8(2, UINT8_C(7), UINT8_C(7));
	T_step_le_u8(3, UINT8_C(9), UINT8_C(8));
	T_assert_le_u8(UINT8_C(10), UINT8_C(10));
	T_assert_le_u8(UINT8_C(12), UINT8_C(11));
}

T_TEST_CASE(step_assert_lt_u8)
{
	T_plan(2);
	T_step_assert_lt_u8(0, UINT8_C(1), UINT8_C(2));
	T_step_assert_lt_u8(1, UINT8_C(3), UINT8_C(3));
}

T_TEST_CASE(check_lt_u8)
{
	T_lt_u8(UINT8_C(1), UINT8_C(2));
	T_lt_u8(UINT8_C(3), UINT8_C(3));
	T_quiet_lt_u8(UINT8_C(4), UINT8_C(5));
	T_quiet_lt_u8(UINT8_C(6), UINT8_C(6));
	T_step_lt_u8(2, UINT8_C(7), UINT8_C(8));
	T_step_lt_u8(3, UINT8_C(9), UINT8_C(9));
	T_assert_lt_u8(UINT8_C(10), UINT8_C(11));
	T_assert_lt_u8(UINT8_C(12), UINT8_C(12));
}

T_TEST_CASE(step_assert_eq_i16)
{
	T_plan(2);
	T_step_assert_eq_i16(0, INT16_C(1), INT16_C(1));
	T_step_assert_eq_i16(1, INT16_C(2), INT16_C(3));
}

T_TEST_CASE(check_eq_i16)
{
	T_eq_i16(INT16_C(1), INT16_C(1));
	T_eq_i16(INT16_C(2), INT16_C(3));
	T_quiet_eq_i16(INT16_C(4), INT16_C(4));
	T_quiet_eq_i16(INT16_C(5), INT16_C(6));
	T_step_eq_i16(2, INT16_C(7), INT16_C(7));
	T_step_eq_i16(3, INT16_C(8), INT16_C(9));
	T_assert_eq_i16(INT16_C(10), INT16_C(10));
	T_assert_eq_i16(INT16_C(11), INT16_C(12));
}

T_TEST_CASE(step_assert_ne_i16)
{
	T_plan(2);
	T_step_assert_ne_i16(0, INT16_C(2), INT16_C(3));
	T_step_assert_ne_i16(1, INT16_C(1), INT16_C(1));
}

T_TEST_CASE(check_ne_i16)
{
	T_ne_i16(INT16_C(2), INT16_C(3));
	T_ne_i16(INT16_C(1), INT16_C(1));
	T_quiet_ne_i16(INT16_C(5), INT16_C(6));
	T_quiet_ne_i16(INT16_C(4), INT16_C(4));
	T_step_ne_i16(2, INT16_C(8), INT16_C(9));
	T_step_ne_i16(3, INT16_C(7), INT16_C(7));
	T_assert_ne_i16(INT16_C(11), INT16_C(12));
	T_assert_ne_i16(INT16_C(10), INT16_C(10));
}

T_TEST_CASE(step_assert_ge_i16)
{
	T_plan(2);
	T_step_assert_ge_i16(0, INT16_C(1), INT16_C(1));
	T_step_assert_ge_i16(1, INT16_C(2), INT16_C(3));
}

T_TEST_CASE(check_ge_i16)
{
	T_ge_i16(INT16_C(1), INT16_C(1));
	T_ge_i16(INT16_C(2), INT16_C(3));
	T_quiet_ge_i16(INT16_C(4), INT16_C(4));
	T_quiet_ge_i16(INT16_C(5), INT16_C(6));
	T_step_ge_i16(2, INT16_C(7), INT16_C(7));
	T_step_ge_i16(3, INT16_C(8), INT16_C(9));
	T_assert_ge_i16(INT16_C(10), INT16_C(10));
	T_assert_ge_i16(INT16_C(11), INT16_C(12));
}

T_TEST_CASE(step_assert_gt_i16)
{
	T_plan(2);
	T_step_assert_gt_i16(0, INT16_C(2), INT16_C(1));
	T_step_assert_gt_i16(1, INT16_C(3), INT16_C(3));
}

T_TEST_CASE(check_gt_i16)
{
	T_gt_i16(INT16_C(2), INT16_C(1));
	T_gt_i16(INT16_C(3), INT16_C(3));
	T_quiet_gt_i16(INT16_C(5), INT16_C(4));
	T_quiet_gt_i16(INT16_C(6), INT16_C(6));
	T_step_gt_i16(2, INT16_C(8), INT16_C(7));
	T_step_gt_i16(3, INT16_C(9), INT16_C(9));
	T_assert_gt_i16(INT16_C(10), INT16_C(11));
	T_assert_gt_i16(INT16_C(12), INT16_C(12));
}

T_TEST_CASE(step_assert_le_i16)
{
	T_plan(2);
	T_step_assert_le_i16(0, INT16_C(1), INT16_C(1));
	T_step_assert_le_i16(1, INT16_C(3), INT16_C(2));
}

T_TEST_CASE(check_le_i16)
{
	T_le_i16(INT16_C(1), INT16_C(1));
	T_le_i16(INT16_C(3), INT16_C(2));
	T_quiet_le_i16(INT16_C(4), INT16_C(4));
	T_quiet_le_i16(INT16_C(6), INT16_C(5));
	T_step_le_i16(2, INT16_C(7), INT16_C(7));
	T_step_le_i16(3, INT16_C(9), INT16_C(8));
	T_assert_le_i16(INT16_C(10), INT16_C(10));
	T_assert_le_i16(INT16_C(12), INT16_C(11));
}

T_TEST_CASE(step_assert_lt_i16)
{
	T_plan(2);
	T_step_assert_lt_i16(0, INT16_C(1), INT16_C(2));
	T_step_assert_lt_i16(1, INT16_C(3), INT16_C(3));
}

T_TEST_CASE(check_lt_i16)
{
	T_lt_i16(INT16_C(1), INT16_C(2));
	T_lt_i16(INT16_C(3), INT16_C(3));
	T_quiet_lt_i16(INT16_C(4), INT16_C(5));
	T_quiet_lt_i16(INT16_C(6), INT16_C(6));
	T_step_lt_i16(2, INT16_C(7), INT16_C(8));
	T_step_lt_i16(3, INT16_C(9), INT16_C(9));
	T_assert_lt_i16(INT16_C(10), INT16_C(11));
	T_assert_lt_i16(INT16_C(12), INT16_C(12));
}

T_TEST_CASE(step_assert_eq_u16)
{
	T_plan(2);
	T_step_assert_eq_u16(0, UINT16_C(1), UINT16_C(1));
	T_step_assert_eq_u16(1, UINT16_C(2), UINT16_C(3));
}

T_TEST_CASE(check_eq_u16)
{
	T_eq_u16(UINT16_C(1), UINT16_C(1));
	T_eq_u16(UINT16_C(2), UINT16_C(3));
	T_quiet_eq_u16(UINT16_C(4), UINT16_C(4));
	T_quiet_eq_u16(UINT16_C(5), UINT16_C(6));
	T_step_eq_u16(2, UINT16_C(7), UINT16_C(7));
	T_step_eq_u16(3, UINT16_C(8), UINT16_C(9));
	T_assert_eq_u16(UINT16_C(10), UINT16_C(10));
	T_assert_eq_u16(UINT16_C(11), UINT16_C(12));
}

T_TEST_CASE(step_assert_ne_u16)
{
	T_plan(2);
	T_step_assert_ne_u16(0, UINT16_C(2), UINT16_C(3));
	T_step_assert_ne_u16(1, UINT16_C(1), UINT16_C(1));
}

T_TEST_CASE(check_ne_u16)
{
	T_ne_u16(UINT16_C(2), UINT16_C(3));
	T_ne_u16(UINT16_C(1), UINT16_C(1));
	T_quiet_ne_u16(UINT16_C(5), UINT16_C(6));
	T_quiet_ne_u16(UINT16_C(4), UINT16_C(4));
	T_step_ne_u16(2, UINT16_C(8), UINT16_C(9));
	T_step_ne_u16(3, UINT16_C(7), UINT16_C(7));
	T_assert_ne_u16(UINT16_C(11), UINT16_C(12));
	T_assert_ne_u16(UINT16_C(10), UINT16_C(10));
}

T_TEST_CASE(step_assert_ge_u16)
{
	T_plan(2);
	T_step_assert_ge_u16(0, UINT16_C(1), UINT16_C(1));
	T_step_assert_ge_u16(1, UINT16_C(2), UINT16_C(3));
}

T_TEST_CASE(check_ge_u16)
{
	T_ge_u16(UINT16_C(1), UINT16_C(1));
	T_ge_u16(UINT16_C(2), UINT16_C(3));
	T_quiet_ge_u16(UINT16_C(4), UINT16_C(4));
	T_quiet_ge_u16(UINT16_C(5), UINT16_C(6));
	T_step_ge_u16(2, UINT16_C(7), UINT16_C(7));
	T_step_ge_u16(3, UINT16_C(8), UINT16_C(9));
	T_assert_ge_u16(UINT16_C(10), UINT16_C(10));
	T_assert_ge_u16(UINT16_C(11), UINT16_C(12));
}

T_TEST_CASE(step_assert_gt_u16)
{
	T_plan(2);
	T_step_assert_gt_u16(0, UINT16_C(2), UINT16_C(1));
	T_step_assert_gt_u16(1, UINT16_C(3), UINT16_C(3));
}

T_TEST_CASE(check_gt_u16)
{
	T_gt_u16(UINT16_C(2), UINT16_C(1));
	T_gt_u16(UINT16_C(3), UINT16_C(3));
	T_quiet_gt_u16(UINT16_C(5), UINT16_C(4));
	T_quiet_gt_u16(UINT16_C(6), UINT16_C(6));
	T_step_gt_u16(2, UINT16_C(8), UINT16_C(7));
	T_step_gt_u16(3, UINT16_C(9), UINT16_C(9));
	T_assert_gt_u16(UINT16_C(10), UINT16_C(11));
	T_assert_gt_u16(UINT16_C(12), UINT16_C(12));
}

T_TEST_CASE(step_assert_le_u16)
{
	T_plan(2);
	T_step_assert_le_u16(0, UINT16_C(1), UINT16_C(1));
	T_step_assert_le_u16(1, UINT16_C(3), UINT16_C(2));
}

T_TEST_CASE(check_le_u16)
{
	T_le_u16(UINT16_C(1), UINT16_C(1));
	T_le_u16(UINT16_C(3), UINT16_C(2));
	T_quiet_le_u16(UINT16_C(4), UINT16_C(4));
	T_quiet_le_u16(UINT16_C(6), UINT16_C(5));
	T_step_le_u16(2, UINT16_C(7), UINT16_C(7));
	T_step_le_u16(3, UINT16_C(9), UINT16_C(8));
	T_assert_le_u16(UINT16_C(10), UINT16_C(10));
	T_assert_le_u16(UINT16_C(12), UINT16_C(11));
}

T_TEST_CASE(step_assert_lt_u16)
{
	T_plan(2);
	T_step_assert_lt_u16(0, UINT16_C(1), UINT16_C(2));
	T_step_assert_lt_u16(1, UINT16_C(3), UINT16_C(3));
}

T_TEST_CASE(check_lt_u16)
{
	T_lt_u16(UINT16_C(1), UINT16_C(2));
	T_lt_u16(UINT16_C(3), UINT16_C(3));
	T_quiet_lt_u16(UINT16_C(4), UINT16_C(5));
	T_quiet_lt_u16(UINT16_C(6), UINT16_C(6));
	T_step_lt_u16(2, UINT16_C(7), UINT16_C(8));
	T_step_lt_u16(3, UINT16_C(9), UINT16_C(9));
	T_assert_lt_u16(UINT16_C(10), UINT16_C(11));
	T_assert_lt_u16(UINT16_C(12), UINT16_C(12));
}

T_TEST_CASE(step_assert_eq_i32)
{
	T_plan(2);
	T_step_assert_eq_i32(0, INT32_C(1), INT32_C(1));
	T_step_assert_eq_i32(1, INT32_C(2), INT32_C(3));
}

T_TEST_CASE(check_eq_i32)
{
	T_eq_i32(INT32_C(1), INT32_C(1));
	T_eq_i32(INT32_C(2), INT32_C(3));
	T_quiet_eq_i32(INT32_C(4), INT32_C(4));
	T_quiet_eq_i32(INT32_C(5), INT32_C(6));
	T_step_eq_i32(2, INT32_C(7), INT32_C(7));
	T_step_eq_i32(3, INT32_C(8), INT32_C(9));
	T_assert_eq_i32(INT32_C(10), INT32_C(10));
	T_assert_eq_i32(INT32_C(11), INT32_C(12));
}

T_TEST_CASE(step_assert_ne_i32)
{
	T_plan(2);
	T_step_assert_ne_i32(0, INT32_C(2), INT32_C(3));
	T_step_assert_ne_i32(1, INT32_C(1), INT32_C(1));
}

T_TEST_CASE(check_ne_i32)
{
	T_ne_i32(INT32_C(2), INT32_C(3));
	T_ne_i32(INT32_C(1), INT32_C(1));
	T_quiet_ne_i32(INT32_C(5), INT32_C(6));
	T_quiet_ne_i32(INT32_C(4), INT32_C(4));
	T_step_ne_i32(2, INT32_C(8), INT32_C(9));
	T_step_ne_i32(3, INT32_C(7), INT32_C(7));
	T_assert_ne_i32(INT32_C(11), INT32_C(12));
	T_assert_ne_i32(INT32_C(10), INT32_C(10));
}

T_TEST_CASE(step_assert_ge_i32)
{
	T_plan(2);
	T_step_assert_ge_i32(0, INT32_C(1), INT32_C(1));
	T_step_assert_ge_i32(1, INT32_C(2), INT32_C(3));
}

T_TEST_CASE(check_ge_i32)
{
	T_ge_i32(INT32_C(1), INT32_C(1));
	T_ge_i32(INT32_C(2), INT32_C(3));
	T_quiet_ge_i32(INT32_C(4), INT32_C(4));
	T_quiet_ge_i32(INT32_C(5), INT32_C(6));
	T_step_ge_i32(2, INT32_C(7), INT32_C(7));
	T_step_ge_i32(3, INT32_C(8), INT32_C(9));
	T_assert_ge_i32(INT32_C(10), INT32_C(10));
	T_assert_ge_i32(INT32_C(11), INT32_C(12));
}

T_TEST_CASE(step_assert_gt_i32)
{
	T_plan(2);
	T_step_assert_gt_i32(0, INT32_C(2), INT32_C(1));
	T_step_assert_gt_i32(1, INT32_C(3), INT32_C(3));
}

T_TEST_CASE(check_gt_i32)
{
	T_gt_i32(INT32_C(2), INT32_C(1));
	T_gt_i32(INT32_C(3), INT32_C(3));
	T_quiet_gt_i32(INT32_C(5), INT32_C(4));
	T_quiet_gt_i32(INT32_C(6), INT32_C(6));
	T_step_gt_i32(2, INT32_C(8), INT32_C(7));
	T_step_gt_i32(3, INT32_C(9), INT32_C(9));
	T_assert_gt_i32(INT32_C(10), INT32_C(11));
	T_assert_gt_i32(INT32_C(12), INT32_C(12));
}

T_TEST_CASE(step_assert_le_i32)
{
	T_plan(2);
	T_step_assert_le_i32(0, INT32_C(1), INT32_C(1));
	T_step_assert_le_i32(1, INT32_C(3), INT32_C(2));
}

T_TEST_CASE(check_le_i32)
{
	T_le_i32(INT32_C(1), INT32_C(1));
	T_le_i32(INT32_C(3), INT32_C(2));
	T_quiet_le_i32(INT32_C(4), INT32_C(4));
	T_quiet_le_i32(INT32_C(6), INT32_C(5));
	T_step_le_i32(2, INT32_C(7), INT32_C(7));
	T_step_le_i32(3, INT32_C(9), INT32_C(8));
	T_assert_le_i32(INT32_C(10), INT32_C(10));
	T_assert_le_i32(INT32_C(12), INT32_C(11));
}

T_TEST_CASE(step_assert_lt_i32)
{
	T_plan(2);
	T_step_assert_lt_i32(0, INT32_C(1), INT32_C(2));
	T_step_assert_lt_i32(1, INT32_C(3), INT32_C(3));
}

T_TEST_CASE(check_lt_i32)
{
	T_lt_i32(INT32_C(1), INT32_C(2));
	T_lt_i32(INT32_C(3), INT32_C(3));
	T_quiet_lt_i32(INT32_C(4), INT32_C(5));
	T_quiet_lt_i32(INT32_C(6), INT32_C(6));
	T_step_lt_i32(2, INT32_C(7), INT32_C(8));
	T_step_lt_i32(3, INT32_C(9), INT32_C(9));
	T_assert_lt_i32(INT32_C(10), INT32_C(11));
	T_assert_lt_i32(INT32_C(12), INT32_C(12));
}

T_TEST_CASE(step_assert_eq_u32)
{
	T_plan(2);
	T_step_assert_eq_u32(0, UINT32_C(1), UINT32_C(1));
	T_step_assert_eq_u32(1, UINT32_C(2), UINT32_C(3));
}

T_TEST_CASE(check_eq_u32)
{
	T_eq_u32(UINT32_C(1), UINT32_C(1));
	T_eq_u32(UINT32_C(2), UINT32_C(3));
	T_quiet_eq_u32(UINT32_C(4), UINT32_C(4));
	T_quiet_eq_u32(UINT32_C(5), UINT32_C(6));
	T_step_eq_u32(2, UINT32_C(7), UINT32_C(7));
	T_step_eq_u32(3, UINT32_C(8), UINT32_C(9));
	T_assert_eq_u32(UINT32_C(10), UINT32_C(10));
	T_assert_eq_u32(UINT32_C(11), UINT32_C(12));
}

T_TEST_CASE(step_assert_ne_u32)
{
	T_plan(2);
	T_step_assert_ne_u32(0, UINT32_C(2), UINT32_C(3));
	T_step_assert_ne_u32(1, UINT32_C(1), UINT32_C(1));
}

T_TEST_CASE(check_ne_u32)
{
	T_ne_u32(UINT32_C(2), UINT32_C(3));
	T_ne_u32(UINT32_C(1), UINT32_C(1));
	T_quiet_ne_u32(UINT32_C(5), UINT32_C(6));
	T_quiet_ne_u32(UINT32_C(4), UINT32_C(4));
	T_step_ne_u32(2, UINT32_C(8), UINT32_C(9));
	T_step_ne_u32(3, UINT32_C(7), UINT32_C(7));
	T_assert_ne_u32(UINT32_C(11), UINT32_C(12));
	T_assert_ne_u32(UINT32_C(10), UINT32_C(10));
}

T_TEST_CASE(step_assert_ge_u32)
{
	T_plan(2);
	T_step_assert_ge_u32(0, UINT32_C(1), UINT32_C(1));
	T_step_assert_ge_u32(1, UINT32_C(2), UINT32_C(3));
}

T_TEST_CASE(check_ge_u32)
{
	T_ge_u32(UINT32_C(1), UINT32_C(1));
	T_ge_u32(UINT32_C(2), UINT32_C(3));
	T_quiet_ge_u32(UINT32_C(4), UINT32_C(4));
	T_quiet_ge_u32(UINT32_C(5), UINT32_C(6));
	T_step_ge_u32(2, UINT32_C(7), UINT32_C(7));
	T_step_ge_u32(3, UINT32_C(8), UINT32_C(9));
	T_assert_ge_u32(UINT32_C(10), UINT32_C(10));
	T_assert_ge_u32(UINT32_C(11), UINT32_C(12));
}

T_TEST_CASE(step_assert_gt_u32)
{
	T_plan(2);
	T_step_assert_gt_u32(0, UINT32_C(2), UINT32_C(1));
	T_step_assert_gt_u32(1, UINT32_C(3), UINT32_C(3));
}

T_TEST_CASE(check_gt_u32)
{
	T_gt_u32(UINT32_C(2), UINT32_C(1));
	T_gt_u32(UINT32_C(3), UINT32_C(3));
	T_quiet_gt_u32(UINT32_C(5), UINT32_C(4));
	T_quiet_gt_u32(UINT32_C(6), UINT32_C(6));
	T_step_gt_u32(2, UINT32_C(8), UINT32_C(7));
	T_step_gt_u32(3, UINT32_C(9), UINT32_C(9));
	T_assert_gt_u32(UINT32_C(10), UINT32_C(11));
	T_assert_gt_u32(UINT32_C(12), UINT32_C(12));
}

T_TEST_CASE(step_assert_le_u32)
{
	T_plan(2);
	T_step_assert_le_u32(0, UINT32_C(1), UINT32_C(1));
	T_step_assert_le_u32(1, UINT32_C(3), UINT32_C(2));
}

T_TEST_CASE(check_le_u32)
{
	T_le_u32(UINT32_C(1), UINT32_C(1));
	T_le_u32(UINT32_C(3), UINT32_C(2));
	T_quiet_le_u32(UINT32_C(4), UINT32_C(4));
	T_quiet_le_u32(UINT32_C(6), UINT32_C(5));
	T_step_le_u32(2, UINT32_C(7), UINT32_C(7));
	T_step_le_u32(3, UINT32_C(9), UINT32_C(8));
	T_assert_le_u32(UINT32_C(10), UINT32_C(10));
	T_assert_le_u32(UINT32_C(12), UINT32_C(11));
}

T_TEST_CASE(step_assert_lt_u32)
{
	T_plan(2);
	T_step_assert_lt_u32(0, UINT32_C(1), UINT32_C(2));
	T_step_assert_lt_u32(1, UINT32_C(3), UINT32_C(3));
}

T_TEST_CASE(check_lt_u32)
{
	T_lt_u32(UINT32_C(1), UINT32_C(2));
	T_lt_u32(UINT32_C(3), UINT32_C(3));
	T_quiet_lt_u32(UINT32_C(4), UINT32_C(5));
	T_quiet_lt_u32(UINT32_C(6), UINT32_C(6));
	T_step_lt_u32(2, UINT32_C(7), UINT32_C(8));
	T_step_lt_u32(3, UINT32_C(9), UINT32_C(9));
	T_assert_lt_u32(UINT32_C(10), UINT32_C(11));
	T_assert_lt_u32(UINT32_C(12), UINT32_C(12));
}

T_TEST_CASE(step_assert_eq_i64)
{
	T_plan(2);
	T_step_assert_eq_i64(0, INT64_C(1), INT64_C(1));
	T_step_assert_eq_i64(1, INT64_C(2), INT64_C(3));
}

T_TEST_CASE(check_eq_i64)
{
	T_eq_i64(INT64_C(1), INT64_C(1));
	T_eq_i64(INT64_C(2), INT64_C(3));
	T_quiet_eq_i64(INT64_C(4), INT64_C(4));
	T_quiet_eq_i64(INT64_C(5), INT64_C(6));
	T_step_eq_i64(2, INT64_C(7), INT64_C(7));
	T_step_eq_i64(3, INT64_C(8), INT64_C(9));
	T_assert_eq_i64(INT64_C(10), INT64_C(10));
	T_assert_eq_i64(INT64_C(11), INT64_C(12));
}

T_TEST_CASE(step_assert_ne_i64)
{
	T_plan(2);
	T_step_assert_ne_i64(0, INT64_C(2), INT64_C(3));
	T_step_assert_ne_i64(1, INT64_C(1), INT64_C(1));
}

T_TEST_CASE(check_ne_i64)
{
	T_ne_i64(INT64_C(2), INT64_C(3));
	T_ne_i64(INT64_C(1), INT64_C(1));
	T_quiet_ne_i64(INT64_C(5), INT64_C(6));
	T_quiet_ne_i64(INT64_C(4), INT64_C(4));
	T_step_ne_i64(2, INT64_C(8), INT64_C(9));
	T_step_ne_i64(3, INT64_C(7), INT64_C(7));
	T_assert_ne_i64(INT64_C(11), INT64_C(12));
	T_assert_ne_i64(INT64_C(10), INT64_C(10));
}

T_TEST_CASE(step_assert_ge_i64)
{
	T_plan(2);
	T_step_assert_ge_i64(0, INT64_C(1), INT64_C(1));
	T_step_assert_ge_i64(1, INT64_C(2), INT64_C(3));
}

T_TEST_CASE(check_ge_i64)
{
	T_ge_i64(INT64_C(1), INT64_C(1));
	T_ge_i64(INT64_C(2), INT64_C(3));
	T_quiet_ge_i64(INT64_C(4), INT64_C(4));
	T_quiet_ge_i64(INT64_C(5), INT64_C(6));
	T_step_ge_i64(2, INT64_C(7), INT64_C(7));
	T_step_ge_i64(3, INT64_C(8), INT64_C(9));
	T_assert_ge_i64(INT64_C(10), INT64_C(10));
	T_assert_ge_i64(INT64_C(11), INT64_C(12));
}

T_TEST_CASE(step_assert_gt_i64)
{
	T_plan(2);
	T_step_assert_gt_i64(0, INT64_C(2), INT64_C(1));
	T_step_assert_gt_i64(1, INT64_C(3), INT64_C(3));
}

T_TEST_CASE(check_gt_i64)
{
	T_gt_i64(INT64_C(2), INT64_C(1));
	T_gt_i64(INT64_C(3), INT64_C(3));
	T_quiet_gt_i64(INT64_C(5), INT64_C(4));
	T_quiet_gt_i64(INT64_C(6), INT64_C(6));
	T_step_gt_i64(2, INT64_C(8), INT64_C(7));
	T_step_gt_i64(3, INT64_C(9), INT64_C(9));
	T_assert_gt_i64(INT64_C(10), INT64_C(11));
	T_assert_gt_i64(INT64_C(12), INT64_C(12));
}

T_TEST_CASE(step_assert_le_i64)
{
	T_plan(2);
	T_step_assert_le_i64(0, INT64_C(1), INT64_C(1));
	T_step_assert_le_i64(1, INT64_C(3), INT64_C(2));
}

T_TEST_CASE(check_le_i64)
{
	T_le_i64(INT64_C(1), INT64_C(1));
	T_le_i64(INT64_C(3), INT64_C(2));
	T_quiet_le_i64(INT64_C(4), INT64_C(4));
	T_quiet_le_i64(INT64_C(6), INT64_C(5));
	T_step_le_i64(2, INT64_C(7), INT64_C(7));
	T_step_le_i64(3, INT64_C(9), INT64_C(8));
	T_assert_le_i64(INT64_C(10), INT64_C(10));
	T_assert_le_i64(INT64_C(12), INT64_C(11));
}

T_TEST_CASE(step_assert_lt_i64)
{
	T_plan(2);
	T_step_assert_lt_i64(0, INT64_C(1), INT64_C(2));
	T_step_assert_lt_i64(1, INT64_C(3), INT64_C(3));
}

T_TEST_CASE(check_lt_i64)
{
	T_lt_i64(INT64_C(1), INT64_C(2));
	T_lt_i64(INT64_C(3), INT64_C(3));
	T_quiet_lt_i64(INT64_C(4), INT64_C(5));
	T_quiet_lt_i64(INT64_C(6), INT64_C(6));
	T_step_lt_i64(2, INT64_C(7), INT64_C(8));
	T_step_lt_i64(3, INT64_C(9), INT64_C(9));
	T_assert_lt_i64(INT64_C(10), INT64_C(11));
	T_assert_lt_i64(INT64_C(12), INT64_C(12));
}

T_TEST_CASE(step_assert_eq_u64)
{
	T_plan(2);
	T_step_assert_eq_u64(0, UINT64_C(1), UINT64_C(1));
	T_step_assert_eq_u64(1, UINT64_C(2), UINT64_C(3));
}

T_TEST_CASE(check_eq_u64)
{
	T_eq_u64(UINT64_C(1), UINT64_C(1));
	T_eq_u64(UINT64_C(2), UINT64_C(3));
	T_quiet_eq_u64(UINT64_C(4), UINT64_C(4));
	T_quiet_eq_u64(UINT64_C(5), UINT64_C(6));
	T_step_eq_u64(2, UINT64_C(7), UINT64_C(7));
	T_step_eq_u64(3, UINT64_C(8), UINT64_C(9));
	T_assert_eq_u64(UINT64_C(10), UINT64_C(10));
	T_assert_eq_u64(UINT64_C(11), UINT64_C(12));
}

T_TEST_CASE(step_assert_ne_u64)
{
	T_plan(2);
	T_step_assert_ne_u64(0, UINT64_C(2), UINT64_C(3));
	T_step_assert_ne_u64(1, UINT64_C(1), UINT64_C(1));
}

T_TEST_CASE(check_ne_u64)
{
	T_ne_u64(UINT64_C(2), UINT64_C(3));
	T_ne_u64(UINT64_C(1), UINT64_C(1));
	T_quiet_ne_u64(UINT64_C(5), UINT64_C(6));
	T_quiet_ne_u64(UINT64_C(4), UINT64_C(4));
	T_step_ne_u64(2, UINT64_C(8), UINT64_C(9));
	T_step_ne_u64(3, UINT64_C(7), UINT64_C(7));
	T_assert_ne_u64(UINT64_C(11), UINT64_C(12));
	T_assert_ne_u64(UINT64_C(10), UINT64_C(10));
}

T_TEST_CASE(step_assert_ge_u64)
{
	T_plan(2);
	T_step_assert_ge_u64(0, UINT64_C(1), UINT64_C(1));
	T_step_assert_ge_u64(1, UINT64_C(2), UINT64_C(3));
}

T_TEST_CASE(check_ge_u64)
{
	T_ge_u64(UINT64_C(1), UINT64_C(1));
	T_ge_u64(UINT64_C(2), UINT64_C(3));
	T_quiet_ge_u64(UINT64_C(4), UINT64_C(4));
	T_quiet_ge_u64(UINT64_C(5), UINT64_C(6));
	T_step_ge_u64(2, UINT64_C(7), UINT64_C(7));
	T_step_ge_u64(3, UINT64_C(8), UINT64_C(9));
	T_assert_ge_u64(UINT64_C(10), UINT64_C(10));
	T_assert_ge_u64(UINT64_C(11), UINT64_C(12));
}

T_TEST_CASE(step_assert_gt_u64)
{
	T_plan(2);
	T_step_assert_gt_u64(0, UINT64_C(2), UINT64_C(1));
	T_step_assert_gt_u64(1, UINT64_C(3), UINT64_C(3));
}

T_TEST_CASE(check_gt_u64)
{
	T_gt_u64(UINT64_C(2), UINT64_C(1));
	T_gt_u64(UINT64_C(3), UINT64_C(3));
	T_quiet_gt_u64(UINT64_C(5), UINT64_C(4));
	T_quiet_gt_u64(UINT64_C(6), UINT64_C(6));
	T_step_gt_u64(2, UINT64_C(8), UINT64_C(7));
	T_step_gt_u64(3, UINT64_C(9), UINT64_C(9));
	T_assert_gt_u64(UINT64_C(10), UINT64_C(11));
	T_assert_gt_u64(UINT64_C(12), UINT64_C(12));
}

T_TEST_CASE(step_assert_le_u64)
{
	T_plan(2);
	T_step_assert_le_u64(0, UINT64_C(1), UINT64_C(1));
	T_step_assert_le_u64(1, UINT64_C(3), UINT64_C(2));
}

T_TEST_CASE(check_le_u64)
{
	T_le_u64(UINT64_C(1), UINT64_C(1));
	T_le_u64(UINT64_C(3), UINT64_C(2));
	T_quiet_le_u64(UINT64_C(4), UINT64_C(4));
	T_quiet_le_u64(UINT64_C(6), UINT64_C(5));
	T_step_le_u64(2, UINT64_C(7), UINT64_C(7));
	T_step_le_u64(3, UINT64_C(9), UINT64_C(8));
	T_assert_le_u64(UINT64_C(10), UINT64_C(10));
	T_assert_le_u64(UINT64_C(12), UINT64_C(11));
}

T_TEST_CASE(step_assert_lt_u64)
{
	T_plan(2);
	T_step_assert_lt_u64(0, UINT64_C(1), UINT64_C(2));
	T_step_assert_lt_u64(1, UINT64_C(3), UINT64_C(3));
}

T_TEST_CASE(check_lt_u64)
{
	T_lt_u64(UINT64_C(1), UINT64_C(2));
	T_lt_u64(UINT64_C(3), UINT64_C(3));
	T_quiet_lt_u64(UINT64_C(4), UINT64_C(5));
	T_quiet_lt_u64(UINT64_C(6), UINT64_C(6));
	T_step_lt_u64(2, UINT64_C(7), UINT64_C(8));
	T_step_lt_u64(3, UINT64_C(9), UINT64_C(9));
	T_assert_lt_u64(UINT64_C(10), UINT64_C(11));
	T_assert_lt_u64(UINT64_C(12), UINT64_C(12));
}

T_TEST_CASE(step_assert_eq_iptr)
{
	T_plan(2);
	T_step_assert_eq_iptr(0, (intptr_t)1, (intptr_t)1);
	T_step_assert_eq_iptr(1, (intptr_t)2, (intptr_t)3);
}

T_TEST_CASE(check_eq_iptr)
{
	T_eq_iptr((intptr_t)1, (intptr_t)1);
	T_eq_iptr((intptr_t)2, (intptr_t)3);
	T_quiet_eq_iptr((intptr_t)4, (intptr_t)4);
	T_quiet_eq_iptr((intptr_t)5, (intptr_t)6);
	T_step_eq_iptr(2, (intptr_t)7, (intptr_t)7);
	T_step_eq_iptr(3, (intptr_t)8, (intptr_t)9);
	T_assert_eq_iptr((intptr_t)10, (intptr_t)10);
	T_assert_eq_iptr((intptr_t)11, (intptr_t)12);
}

T_TEST_CASE(step_assert_ne_iptr)
{
	T_plan(2);
	T_step_assert_ne_iptr(0, (intptr_t)2, (intptr_t)3);
	T_step_assert_ne_iptr(1, (intptr_t)1, (intptr_t)1);
}

T_TEST_CASE(check_ne_iptr)
{
	T_ne_iptr((intptr_t)2, (intptr_t)3);
	T_ne_iptr((intptr_t)1, (intptr_t)1);
	T_quiet_ne_iptr((intptr_t)5, (intptr_t)6);
	T_quiet_ne_iptr((intptr_t)4, (intptr_t)4);
	T_step_ne_iptr(2, (intptr_t)8, (intptr_t)9);
	T_step_ne_iptr(3, (intptr_t)7, (intptr_t)7);
	T_assert_ne_iptr((intptr_t)11, (intptr_t)12);
	T_assert_ne_iptr((intptr_t)10, (intptr_t)10);
}

T_TEST_CASE(step_assert_ge_iptr)
{
	T_plan(2);
	T_step_assert_ge_iptr(0, (intptr_t)1, (intptr_t)1);
	T_step_assert_ge_iptr(1, (intptr_t)2, (intptr_t)3);
}

T_TEST_CASE(check_ge_iptr)
{
	T_ge_iptr((intptr_t)1, (intptr_t)1);
	T_ge_iptr((intptr_t)2, (intptr_t)3);
	T_quiet_ge_iptr((intptr_t)4, (intptr_t)4);
	T_quiet_ge_iptr((intptr_t)5, (intptr_t)6);
	T_step_ge_iptr(2, (intptr_t)7, (intptr_t)7);
	T_step_ge_iptr(3, (intptr_t)8, (intptr_t)9);
	T_assert_ge_iptr((intptr_t)10, (intptr_t)10);
	T_assert_ge_iptr((intptr_t)11, (intptr_t)12);
}

T_TEST_CASE(step_assert_gt_iptr)
{
	T_plan(2);
	T_step_assert_gt_iptr(0, (intptr_t)2, (intptr_t)1);
	T_step_assert_gt_iptr(1, (intptr_t)3, (intptr_t)3);
}

T_TEST_CASE(check_gt_iptr)
{
	T_gt_iptr((intptr_t)2, (intptr_t)1);
	T_gt_iptr((intptr_t)3, (intptr_t)3);
	T_quiet_gt_iptr((intptr_t)5, (intptr_t)4);
	T_quiet_gt_iptr((intptr_t)6, (intptr_t)6);
	T_step_gt_iptr(2, (intptr_t)8, (intptr_t)7);
	T_step_gt_iptr(3, (intptr_t)9, (intptr_t)9);
	T_assert_gt_iptr((intptr_t)10, (intptr_t)11);
	T_assert_gt_iptr((intptr_t)12, (intptr_t)12);
}

T_TEST_CASE(step_assert_le_iptr)
{
	T_plan(2);
	T_step_assert_le_iptr(0, (intptr_t)1, (intptr_t)1);
	T_step_assert_le_iptr(1, (intptr_t)3, (intptr_t)2);
}

T_TEST_CASE(check_le_iptr)
{
	T_le_iptr((intptr_t)1, (intptr_t)1);
	T_le_iptr((intptr_t)3, (intptr_t)2);
	T_quiet_le_iptr((intptr_t)4, (intptr_t)4);
	T_quiet_le_iptr((intptr_t)6, (intptr_t)5);
	T_step_le_iptr(2, (intptr_t)7, (intptr_t)7);
	T_step_le_iptr(3, (intptr_t)9, (intptr_t)8);
	T_assert_le_iptr((intptr_t)10, (intptr_t)10);
	T_assert_le_iptr((intptr_t)12, (intptr_t)11);
}

T_TEST_CASE(step_assert_lt_iptr)
{
	T_plan(2);
	T_step_assert_lt_iptr(0, (intptr_t)1, (intptr_t)2);
	T_step_assert_lt_iptr(1, (intptr_t)3, (intptr_t)3);
}

T_TEST_CASE(check_lt_iptr)
{
	T_lt_iptr((intptr_t)1, (intptr_t)2);
	T_lt_iptr((intptr_t)3, (intptr_t)3);
	T_quiet_lt_iptr((intptr_t)4, (intptr_t)5);
	T_quiet_lt_iptr((intptr_t)6, (intptr_t)6);
	T_step_lt_iptr(2, (intptr_t)7, (intptr_t)8);
	T_step_lt_iptr(3, (intptr_t)9, (intptr_t)9);
	T_assert_lt_iptr((intptr_t)10, (intptr_t)11);
	T_assert_lt_iptr((intptr_t)12, (intptr_t)12);
}

T_TEST_CASE(step_assert_eq_uptr)
{
	T_plan(2);
	T_step_assert_eq_uptr(0, (size_t)1, (size_t)1);
	T_step_assert_eq_uptr(1, (size_t)2, (size_t)3);
}

T_TEST_CASE(check_eq_uptr)
{
	T_eq_uptr((size_t)1, (size_t)1);
	T_eq_uptr((size_t)2, (size_t)3);
	T_quiet_eq_uptr((size_t)4, (size_t)4);
	T_quiet_eq_uptr((size_t)5, (size_t)6);
	T_step_eq_uptr(2, (size_t)7, (size_t)7);
	T_step_eq_uptr(3, (size_t)8, (size_t)9);
	T_assert_eq_uptr((size_t)10, (size_t)10);
	T_assert_eq_uptr((size_t)11, (size_t)12);
}

T_TEST_CASE(step_assert_ne_uptr)
{
	T_plan(2);
	T_step_assert_ne_uptr(0, (size_t)2, (size_t)3);
	T_step_assert_ne_uptr(1, (size_t)1, (size_t)1);
}

T_TEST_CASE(check_ne_uptr)
{
	T_ne_uptr((size_t)2, (size_t)3);
	T_ne_uptr((size_t)1, (size_t)1);
	T_quiet_ne_uptr((size_t)5, (size_t)6);
	T_quiet_ne_uptr((size_t)4, (size_t)4);
	T_step_ne_uptr(2, (size_t)8, (size_t)9);
	T_step_ne_uptr(3, (size_t)7, (size_t)7);
	T_assert_ne_uptr((size_t)11, (size_t)12);
	T_assert_ne_uptr((size_t)10, (size_t)10);
}

T_TEST_CASE(step_assert_ge_uptr)
{
	T_plan(2);
	T_step_assert_ge_uptr(0, (size_t)1, (size_t)1);
	T_step_assert_ge_uptr(1, (size_t)2, (size_t)3);
}

T_TEST_CASE(check_ge_uptr)
{
	T_ge_uptr((size_t)1, (size_t)1);
	T_ge_uptr((size_t)2, (size_t)3);
	T_quiet_ge_uptr((size_t)4, (size_t)4);
	T_quiet_ge_uptr((size_t)5, (size_t)6);
	T_step_ge_uptr(2, (size_t)7, (size_t)7);
	T_step_ge_uptr(3, (size_t)8, (size_t)9);
	T_assert_ge_uptr((size_t)10, (size_t)10);
	T_assert_ge_uptr((size_t)11, (size_t)12);
}

T_TEST_CASE(step_assert_gt_uptr)
{
	T_plan(2);
	T_step_assert_gt_uptr(0, (size_t)2, (size_t)1);
	T_step_assert_gt_uptr(1, (size_t)3, (size_t)3);
}

T_TEST_CASE(check_gt_uptr)
{
	T_gt_uptr((size_t)2, (size_t)1);
	T_gt_uptr((size_t)3, (size_t)3);
	T_quiet_gt_uptr((size_t)5, (size_t)4);
	T_quiet_gt_uptr((size_t)6, (size_t)6);
	T_step_gt_uptr(2, (size_t)8, (size_t)7);
	T_step_gt_uptr(3, (size_t)9, (size_t)9);
	T_assert_gt_uptr((size_t)10, (size_t)11);
	T_assert_gt_uptr((size_t)12, (size_t)12);
}

T_TEST_CASE(step_assert_le_uptr)
{
	T_plan(2);
	T_step_assert_le_uptr(0, (size_t)1, (size_t)1);
	T_step_assert_le_uptr(1, (size_t)3, (size_t)2);
}

T_TEST_CASE(check_le_uptr)
{
	T_le_uptr((size_t)1, (size_t)1);
	T_le_uptr((size_t)3, (size_t)2);
	T_quiet_le_uptr((size_t)4, (size_t)4);
	T_quiet_le_uptr((size_t)6, (size_t)5);
	T_step_le_uptr(2, (size_t)7, (size_t)7);
	T_step_le_uptr(3, (size_t)9, (size_t)8);
	T_assert_le_uptr((size_t)10, (size_t)10);
	T_assert_le_uptr((size_t)12, (size_t)11);
}

T_TEST_CASE(step_assert_lt_uptr)
{
	T_plan(2);
	T_step_assert_lt_uptr(0, (size_t)1, (size_t)2);
	T_step_assert_lt_uptr(1, (size_t)3, (size_t)3);
}

T_TEST_CASE(check_lt_uptr)
{
	T_lt_uptr((size_t)1, (size_t)2);
	T_lt_uptr((size_t)3, (size_t)3);
	T_quiet_lt_uptr((size_t)4, (size_t)5);
	T_quiet_lt_uptr((size_t)6, (size_t)6);
	T_step_lt_uptr(2, (size_t)7, (size_t)8);
	T_step_lt_uptr(3, (size_t)9, (size_t)9);
	T_assert_lt_uptr((size_t)10, (size_t)11);
	T_assert_lt_uptr((size_t)12, (size_t)12);
}

T_TEST_CASE(step_assert_eq_ssz)
{
	T_plan(2);
	T_step_assert_eq_ssz(0, (ssize_t)1, (ssize_t)1);
	T_step_assert_eq_ssz(1, (ssize_t)2, (ssize_t)3);
}

T_TEST_CASE(check_eq_ssz)
{
	T_eq_ssz((ssize_t)1, (ssize_t)1);
	T_eq_ssz((ssize_t)2, (ssize_t)3);
	T_quiet_eq_ssz((ssize_t)4, (ssize_t)4);
	T_quiet_eq_ssz((ssize_t)5, (ssize_t)6);
	T_step_eq_ssz(2, (ssize_t)7, (ssize_t)7);
	T_step_eq_ssz(3, (ssize_t)8, (ssize_t)9);
	T_assert_eq_ssz((ssize_t)10, (ssize_t)10);
	T_assert_eq_ssz((ssize_t)11, (ssize_t)12);
}

T_TEST_CASE(step_assert_ne_ssz)
{
	T_plan(2);
	T_step_assert_ne_ssz(0, (ssize_t)2, (ssize_t)3);
	T_step_assert_ne_ssz(1, (ssize_t)1, (ssize_t)1);
}

T_TEST_CASE(check_ne_ssz)
{
	T_ne_ssz((ssize_t)2, (ssize_t)3);
	T_ne_ssz((ssize_t)1, (ssize_t)1);
	T_quiet_ne_ssz((ssize_t)5, (ssize_t)6);
	T_quiet_ne_ssz((ssize_t)4, (ssize_t)4);
	T_step_ne_ssz(2, (ssize_t)8, (ssize_t)9);
	T_step_ne_ssz(3, (ssize_t)7, (ssize_t)7);
	T_assert_ne_ssz((ssize_t)11, (ssize_t)12);
	T_assert_ne_ssz((ssize_t)10, (ssize_t)10);
}

T_TEST_CASE(step_assert_ge_ssz)
{
	T_plan(2);
	T_step_assert_ge_ssz(0, (ssize_t)1, (ssize_t)1);
	T_step_assert_ge_ssz(1, (ssize_t)2, (ssize_t)3);
}

T_TEST_CASE(check_ge_ssz)
{
	T_ge_ssz((ssize_t)1, (ssize_t)1);
	T_ge_ssz((ssize_t)2, (ssize_t)3);
	T_quiet_ge_ssz((ssize_t)4, (ssize_t)4);
	T_quiet_ge_ssz((ssize_t)5, (ssize_t)6);
	T_step_ge_ssz(2, (ssize_t)7, (ssize_t)7);
	T_step_ge_ssz(3, (ssize_t)8, (ssize_t)9);
	T_assert_ge_ssz((ssize_t)10, (ssize_t)10);
	T_assert_ge_ssz((ssize_t)11, (ssize_t)12);
}

T_TEST_CASE(step_assert_gt_ssz)
{
	T_plan(2);
	T_step_assert_gt_ssz(0, (ssize_t)2, (ssize_t)1);
	T_step_assert_gt_ssz(1, (ssize_t)3, (ssize_t)3);
}

T_TEST_CASE(check_gt_ssz)
{
	T_gt_ssz((ssize_t)2, (ssize_t)1);
	T_gt_ssz((ssize_t)3, (ssize_t)3);
	T_quiet_gt_ssz((ssize_t)5, (ssize_t)4);
	T_quiet_gt_ssz((ssize_t)6, (ssize_t)6);
	T_step_gt_ssz(2, (ssize_t)8, (ssize_t)7);
	T_step_gt_ssz(3, (ssize_t)9, (ssize_t)9);
	T_assert_gt_ssz((ssize_t)10, (ssize_t)11);
	T_assert_gt_ssz((ssize_t)12, (ssize_t)12);
}

T_TEST_CASE(step_assert_le_ssz)
{
	T_plan(2);
	T_step_assert_le_ssz(0, (ssize_t)1, (ssize_t)1);
	T_step_assert_le_ssz(1, (ssize_t)3, (ssize_t)2);
}

T_TEST_CASE(check_le_ssz)
{
	T_le_ssz((ssize_t)1, (ssize_t)1);
	T_le_ssz((ssize_t)3, (ssize_t)2);
	T_quiet_le_ssz((ssize_t)4, (ssize_t)4);
	T_quiet_le_ssz((ssize_t)6, (ssize_t)5);
	T_step_le_ssz(2, (ssize_t)7, (ssize_t)7);
	T_step_le_ssz(3, (ssize_t)9, (ssize_t)8);
	T_assert_le_ssz((ssize_t)10, (ssize_t)10);
	T_assert_le_ssz((ssize_t)12, (ssize_t)11);
}

T_TEST_CASE(step_assert_lt_ssz)
{
	T_plan(2);
	T_step_assert_lt_ssz(0, (ssize_t)1, (ssize_t)2);
	T_step_assert_lt_ssz(1, (ssize_t)3, (ssize_t)3);
}

T_TEST_CASE(check_lt_ssz)
{
	T_lt_ssz((ssize_t)1, (ssize_t)2);
	T_lt_ssz((ssize_t)3, (ssize_t)3);
	T_quiet_lt_ssz((ssize_t)4, (ssize_t)5);
	T_quiet_lt_ssz((ssize_t)6, (ssize_t)6);
	T_step_lt_ssz(2, (ssize_t)7, (ssize_t)8);
	T_step_lt_ssz(3, (ssize_t)9, (ssize_t)9);
	T_assert_lt_ssz((ssize_t)10, (ssize_t)11);
	T_assert_lt_ssz((ssize_t)12, (ssize_t)12);
}

T_TEST_CASE(step_assert_eq_sz)
{
	T_plan(2);
	T_step_assert_eq_sz(0, (size_t)1, (size_t)1);
	T_step_assert_eq_sz(1, (size_t)2, (size_t)3);
}

T_TEST_CASE(check_eq_sz)
{
	T_eq_sz((size_t)1, (size_t)1);
	T_eq_sz((size_t)2, (size_t)3);
	T_quiet_eq_sz((size_t)4, (size_t)4);
	T_quiet_eq_sz((size_t)5, (size_t)6);
	T_step_eq_sz(2, (size_t)7, (size_t)7);
	T_step_eq_sz(3, (size_t)8, (size_t)9);
	T_assert_eq_sz((size_t)10, (size_t)10);
	T_assert_eq_sz((size_t)11, (size_t)12);
}

T_TEST_CASE(step_assert_ne_sz)
{
	T_plan(2);
	T_step_assert_ne_sz(0, (size_t)2, (size_t)3);
	T_step_assert_ne_sz(1, (size_t)1, (size_t)1);
}

T_TEST_CASE(check_ne_sz)
{
	T_ne_sz((size_t)2, (size_t)3);
	T_ne_sz((size_t)1, (size_t)1);
	T_quiet_ne_sz((size_t)5, (size_t)6);
	T_quiet_ne_sz((size_t)4, (size_t)4);
	T_step_ne_sz(2, (size_t)8, (size_t)9);
	T_step_ne_sz(3, (size_t)7, (size_t)7);
	T_assert_ne_sz((size_t)11, (size_t)12);
	T_assert_ne_sz((size_t)10, (size_t)10);
}

T_TEST_CASE(step_assert_ge_sz)
{
	T_plan(2);
	T_step_assert_ge_sz(0, (size_t)1, (size_t)1);
	T_step_assert_ge_sz(1, (size_t)2, (size_t)3);
}

T_TEST_CASE(check_ge_sz)
{
	T_ge_sz((size_t)1, (size_t)1);
	T_ge_sz((size_t)2, (size_t)3);
	T_quiet_ge_sz((size_t)4, (size_t)4);
	T_quiet_ge_sz((size_t)5, (size_t)6);
	T_step_ge_sz(2, (size_t)7, (size_t)7);
	T_step_ge_sz(3, (size_t)8, (size_t)9);
	T_assert_ge_sz((size_t)10, (size_t)10);
	T_assert_ge_sz((size_t)11, (size_t)12);
}

T_TEST_CASE(step_assert_gt_sz)
{
	T_plan(2);
	T_step_assert_gt_sz(0, (size_t)2, (size_t)1);
	T_step_assert_gt_sz(1, (size_t)3, (size_t)3);
}

T_TEST_CASE(check_gt_sz)
{
	T_gt_sz((size_t)2, (size_t)1);
	T_gt_sz((size_t)3, (size_t)3);
	T_quiet_gt_sz((size_t)5, (size_t)4);
	T_quiet_gt_sz((size_t)6, (size_t)6);
	T_step_gt_sz(2, (size_t)8, (size_t)7);
	T_step_gt_sz(3, (size_t)9, (size_t)9);
	T_assert_gt_sz((size_t)10, (size_t)11);
	T_assert_gt_sz((size_t)12, (size_t)12);
}

T_TEST_CASE(step_assert_le_sz)
{
	T_plan(2);
	T_step_assert_le_sz(0, (size_t)1, (size_t)1);
	T_step_assert_le_sz(1, (size_t)3, (size_t)2);
}

T_TEST_CASE(check_le_sz)
{
	T_le_sz((size_t)1, (size_t)1);
	T_le_sz((size_t)3, (size_t)2);
	T_quiet_le_sz((size_t)4, (size_t)4);
	T_quiet_le_sz((size_t)6, (size_t)5);
	T_step_le_sz(2, (size_t)7, (size_t)7);
	T_step_le_sz(3, (size_t)9, (size_t)8);
	T_assert_le_sz((size_t)10, (size_t)10);
	T_assert_le_sz((size_t)12, (size_t)11);
}

T_TEST_CASE(step_assert_lt_sz)
{
	T_plan(2);
	T_step_assert_lt_sz(0, (size_t)1, (size_t)2);
	T_step_assert_lt_sz(1, (size_t)3, (size_t)3);
}

T_TEST_CASE(check_lt_sz)
{
	T_lt_sz((size_t)1, (size_t)2);
	T_lt_sz((size_t)3, (size_t)3);
	T_quiet_lt_sz((size_t)4, (size_t)5);
	T_quiet_lt_sz((size_t)6, (size_t)6);
	T_step_lt_sz(2, (size_t)7, (size_t)8);
	T_step_lt_sz(3, (size_t)9, (size_t)9);
	T_assert_lt_sz((size_t)10, (size_t)11);
	T_assert_lt_sz((size_t)12, (size_t)12);
}

#include "t-self-test.h"

T_TEST_OUTPUT(step_assert_true,
"B:step_assert_true\n"
"P:0:0:UI1:test-checks.c:8\n"
"F:1:0:UI1:test-checks.c:9:a\n"
"E:step_assert_true:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_true,
"B:check_true\n"
"P:0:0:UI1:test-checks.c:14\n"
"F:1:0:UI1:test-checks.c:15:a\n"
"F:*:0:UI1:test-checks.c:17:ab 0\n"
"P:2:0:UI1:test-checks.c:18\n"
"F:3:0:UI1:test-checks.c:19:abc 0\n"
"P:4:0:UI1:test-checks.c:20\n"
"F:5:0:UI1:test-checks.c:21:abcd 0 1 2\n"
"E:check_true:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_false,
"B:step_assert_false\n"
"P:0:0:UI1:test-checks.c:27\n"
"F:1:0:UI1:test-checks.c:28:a\n"
"E:step_assert_false:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_false,
"B:check_false\n"
"P:0:0:UI1:test-checks.c:33\n"
"F:1:0:UI1:test-checks.c:34:a\n"
"F:*:0:UI1:test-checks.c:36:ab 0\n"
"P:2:0:UI1:test-checks.c:37\n"
"F:3:0:UI1:test-checks.c:38:abc 0\n"
"P:4:0:UI1:test-checks.c:39\n"
"F:5:0:UI1:test-checks.c:40:abcd 0 1 2\n"
"E:check_false:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq,
"B:step_assert_eq\n"
"P:0:0:UI1:test-checks.c:46\n"
"F:1:0:UI1:test-checks.c:47:1 == 2\n"
"E:step_assert_eq:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq,
"B:check_eq\n"
"P:0:0:UI1:test-checks.c:52\n"
"F:1:0:UI1:test-checks.c:53:1 == 2\n"
"F:*:0:UI1:test-checks.c:55:4 == 5\n"
"P:2:0:UI1:test-checks.c:56\n"
"F:3:0:UI1:test-checks.c:57:7 == 8\n"
"P:4:0:UI1:test-checks.c:58\n"
"F:5:0:UI1:test-checks.c:59:10 == 11\n"
"E:check_eq:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne,
"B:step_assert_ne\n"
"P:0:0:UI1:test-checks.c:65\n"
"F:1:0:UI1:test-checks.c:66:2 != 2\n"
"E:step_assert_ne:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne,
"B:check_ne\n"
"P:0:0:UI1:test-checks.c:71\n"
"F:1:0:UI1:test-checks.c:72:2 != 2\n"
"F:*:0:UI1:test-checks.c:74:5 != 5\n"
"P:2:0:UI1:test-checks.c:75\n"
"F:3:0:UI1:test-checks.c:76:5 != 5\n"
"P:4:0:UI1:test-checks.c:77\n"
"F:5:0:UI1:test-checks.c:78:11 != 11\n"
"E:check_ne:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_ptr,
"B:step_assert_eq_ptr\n"
"P:0:0:UI1:test-checks.c:87\n"
"F:1:0:UI1:test-checks.c:88:&a == &b\n"
"E:step_assert_eq_ptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_ptr,
"B:check_eq_ptr\n"
"P:0:0:UI1:test-checks.c:96\n"
"F:1:0:UI1:test-checks.c:97:&a == &b\n"
"F:*:0:UI1:test-checks.c:99:&a == &b\n"
"P:2:0:UI1:test-checks.c:100\n"
"F:3:0:UI1:test-checks.c:101:&a == &b\n"
"P:4:0:UI1:test-checks.c:102\n"
"F:5:0:UI1:test-checks.c:103:&a == &b\n"
"E:check_eq_ptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_ptr,
"B:step_assert_ne_ptr\n"
"P:0:0:UI1:test-checks.c:112\n"
"F:1:0:UI1:test-checks.c:113:&a != &a\n"
"E:step_assert_ne_ptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_ptr,
"B:check_ne_ptr\n"
"P:0:0:UI1:test-checks.c:121\n"
"F:1:0:UI1:test-checks.c:122:&a != &a\n"
"F:*:0:UI1:test-checks.c:124:&a != &a\n"
"P:2:0:UI1:test-checks.c:125\n"
"F:3:0:UI1:test-checks.c:126:&a != &a\n"
"P:4:0:UI1:test-checks.c:127\n"
"F:5:0:UI1:test-checks.c:128:&a != &a\n"
"E:check_ne_ptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_null,
"B:step_assert_null\n"
"P:0:0:UI1:test-checks.c:136\n"
"F:1:0:UI1:test-checks.c:137:&a == NULL\n"
"E:step_assert_null:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_null,
"B:check_null\n"
"P:0:0:UI1:test-checks.c:144\n"
"F:1:0:UI1:test-checks.c:145:&a == NULL\n"
"F:*:0:UI1:test-checks.c:147:&a == NULL\n"
"P:2:0:UI1:test-checks.c:148\n"
"F:3:0:UI1:test-checks.c:149:&a == NULL\n"
"P:4:0:UI1:test-checks.c:150\n"
"F:5:0:UI1:test-checks.c:151:&a == NULL\n"
"E:check_null:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_not_null,
"B:step_assert_not_null\n"
"P:0:0:UI1:test-checks.c:159\n"
"F:1:0:UI1:test-checks.c:160:NULL != NULL\n"
"E:step_assert_not_null:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_not_null,
"B:check_not_null\n"
"P:0:0:UI1:test-checks.c:167\n"
"F:1:0:UI1:test-checks.c:168:NULL != NULL\n"
"F:*:0:UI1:test-checks.c:170:NULL != NULL\n"
"P:2:0:UI1:test-checks.c:171\n"
"F:3:0:UI1:test-checks.c:172:NULL != NULL\n"
"P:4:0:UI1:test-checks.c:173\n"
"F:5:0:UI1:test-checks.c:174:NULL != NULL\n"
"E:check_not_null:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_mem,
"B:step_assert_eq_mem\n"
"P:0:0:UI1:test-checks.c:183\n"
"F:1:0:UI1:test-checks.c:184:memcmp(&a, &b, sizeof(a)) == 0\n"
"E:step_assert_eq_mem:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_mem,
"B:check_eq_mem\n"
"P:0:0:UI1:test-checks.c:192\n"
"F:1:0:UI1:test-checks.c:193:memcmp(&a, &b, sizeof(a)) == 0\n"
"F:*:0:UI1:test-checks.c:195:memcmp(&a, &b, sizeof(a)) == 0\n"
"P:2:0:UI1:test-checks.c:196\n"
"F:3:0:UI1:test-checks.c:197:memcmp(&a, &b, sizeof(a)) == 0\n"
"P:4:0:UI1:test-checks.c:198\n"
"F:5:0:UI1:test-checks.c:199:memcmp(&a, &b, sizeof(a)) == 0\n"
"E:check_eq_mem:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_mem,
"B:step_assert_ne_mem\n"
"P:0:0:UI1:test-checks.c:208\n"
"F:1:0:UI1:test-checks.c:209:memcmp(&a, &a, sizeof(a)) != 0\n"
"E:step_assert_ne_mem:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_mem,
"B:check_ne_mem\n"
"P:0:0:UI1:test-checks.c:217\n"
"F:1:0:UI1:test-checks.c:218:memcmp(&a, &a, sizeof(a)) != 0\n"
"F:*:0:UI1:test-checks.c:220:memcmp(&a, &a, sizeof(a)) != 0\n"
"P:2:0:UI1:test-checks.c:221\n"
"F:3:0:UI1:test-checks.c:222:memcmp(&a, &a, sizeof(a)) != 0\n"
"P:4:0:UI1:test-checks.c:223\n"
"F:5:0:UI1:test-checks.c:224:memcmp(&a, &a, sizeof(a)) != 0\n"
"E:check_ne_mem:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_str,
"B:step_assert_eq_str\n"
"P:0:0:UI1:test-checks.c:233\n"
"F:1:0:UI1:test-checks.c:234:\"a\" == \"b\"\n"
"E:step_assert_eq_str:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_str,
"B:check_eq_str\n"
"P:0:0:UI1:test-checks.c:242\n"
"F:1:0:UI1:test-checks.c:243:\"a\" == \"b\"\n"
"F:*:0:UI1:test-checks.c:245:\"a\" == \"b\"\n"
"P:2:0:UI1:test-checks.c:246\n"
"F:3:0:UI1:test-checks.c:247:\"a\" == \"b\"\n"
"P:4:0:UI1:test-checks.c:248\n"
"F:5:0:UI1:test-checks.c:249:\"a\" == \"b\"\n"
"E:check_eq_str:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_str,
"B:step_assert_ne_str\n"
"P:0:0:UI1:test-checks.c:258\n"
"F:1:0:UI1:test-checks.c:259:\"a\" != \"a\"\n"
"E:step_assert_ne_str:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_str,
"B:check_ne_str\n"
"P:0:0:UI1:test-checks.c:267\n"
"F:1:0:UI1:test-checks.c:268:\"a\" != \"a\"\n"
"F:*:0:UI1:test-checks.c:270:\"a\" != \"a\"\n"
"P:2:0:UI1:test-checks.c:271\n"
"F:3:0:UI1:test-checks.c:272:\"a\" != \"a\"\n"
"P:4:0:UI1:test-checks.c:273\n"
"F:5:0:UI1:test-checks.c:274:\"a\" != \"a\"\n"
"E:check_ne_str:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_nstr,
"B:step_assert_eq_nstr\n"
"P:0:0:UI1:test-checks.c:283\n"
"P:1:0:UI1:test-checks.c:284\n"
"F:2:0:UI1:test-checks.c:285:\"aaa\" == \"aaab\"\n"
"E:step_assert_eq_nstr:N:3:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_nstr,
"B:check_eq_nstr\n"
"P:0:0:UI1:test-checks.c:293\n"
"P:1:0:UI1:test-checks.c:294\n"
"F:2:0:UI1:test-checks.c:295:\"aaa\" == \"aaab\"\n"
"F:*:0:UI1:test-checks.c:298:\"aaa\" == \"aaab\"\n"
"P:3:0:UI1:test-checks.c:299\n"
"P:4:0:UI1:test-checks.c:300\n"
"F:5:0:UI1:test-checks.c:301:\"aaa\" == \"aaab\"\n"
"P:6:0:UI1:test-checks.c:302\n"
"P:7:0:UI1:test-checks.c:303\n"
"F:8:0:UI1:test-checks.c:304:\"aaa\" == \"aaab\"\n"
"E:check_eq_nstr:N:9:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_nstr,
"B:step_assert_ne_nstr\n"
"P:0:0:UI1:test-checks.c:313\n"
"F:1:0:UI1:test-checks.c:314:\"aaa\" != \"aaa\"\n"
"E:step_assert_ne_nstr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_nstr,
"B:check_ne_nstr\n"
"P:0:0:UI1:test-checks.c:323\n"
"F:1:0:UI1:test-checks.c:324:\"aaa\" != \"aaa\"\n"
"F:2:0:UI1:test-checks.c:325:\"aaa\" != \"aaa\"\n"
"F:*:0:UI1:test-checks.c:327:\"aaa\" != \"aaa\"\n"
"F:*:0:UI1:test-checks.c:328:\"aaa\" != \"aaa\"\n"
"P:3:0:UI1:test-checks.c:329\n"
"F:4:0:UI1:test-checks.c:330:\"aaa\" != \"aaa\"\n"
"F:5:0:UI1:test-checks.c:331:\"aaa\" != \"aaa\"\n"
"P:6:0:UI1:test-checks.c:332\n"
"F:7:0:UI1:test-checks.c:333:\"aaa\" != \"aaa\"\n"
"E:check_ne_nstr:N:8:F:7:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_char,
"B:step_assert_eq_char\n"
"P:0:0:UI1:test-checks.c:340\n"
"F:1:0:UI1:test-checks.c:341:\'2\' == \'3\'\n"
"E:step_assert_eq_char:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_char,
"B:check_eq_char\n"
"P:0:0:UI1:test-checks.c:346\n"
"F:1:0:UI1:test-checks.c:347:\'2\' == \'3\'\n"
"F:*:0:UI1:test-checks.c:349:\'5\' == \'6\'\n"
"P:2:0:UI1:test-checks.c:350\n"
"F:3:0:UI1:test-checks.c:351:\'8\' == \'9\'\n"
"P:4:0:UI1:test-checks.c:352\n"
"F:5:0:UI1:test-checks.c:353:\'B\' == \'C\'\n"
"E:check_eq_char:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_char,
"B:step_assert_ne_char\n"
"P:0:0:UI1:test-checks.c:359\n"
"F:1:0:UI1:test-checks.c:360:\'1\' != \'1\'\n"
"E:step_assert_ne_char:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_char,
"B:check_ne_char\n"
"P:0:0:UI1:test-checks.c:365\n"
"F:1:0:UI1:test-checks.c:366:\'1\' != \'1\'\n"
"F:*:0:UI1:test-checks.c:368:\'4\' != \'4\'\n"
"P:2:0:UI1:test-checks.c:369\n"
"F:3:0:UI1:test-checks.c:370:\'7\' != \'7\'\n"
"P:4:0:UI1:test-checks.c:371\n"
"F:5:0:UI1:test-checks.c:372:\'A\' != \'A\'\n"
"E:check_ne_char:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_schar,
"B:step_assert_eq_schar\n"
"P:0:0:UI1:test-checks.c:378\n"
"F:1:0:UI1:test-checks.c:379:2 == 3\n"
"E:step_assert_eq_schar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_schar,
"B:check_eq_schar\n"
"P:0:0:UI1:test-checks.c:384\n"
"F:1:0:UI1:test-checks.c:385:2 == 3\n"
"F:*:0:UI1:test-checks.c:387:5 == 6\n"
"P:2:0:UI1:test-checks.c:388\n"
"F:3:0:UI1:test-checks.c:389:8 == 9\n"
"P:4:0:UI1:test-checks.c:390\n"
"F:5:0:UI1:test-checks.c:391:11 == 12\n"
"E:check_eq_schar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_schar,
"B:step_assert_ne_schar\n"
"P:0:0:UI1:test-checks.c:397\n"
"F:1:0:UI1:test-checks.c:398:1 != 1\n"
"E:step_assert_ne_schar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_schar,
"B:check_ne_schar\n"
"P:0:0:UI1:test-checks.c:403\n"
"F:1:0:UI1:test-checks.c:404:1 != 1\n"
"F:*:0:UI1:test-checks.c:406:4 != 4\n"
"P:2:0:UI1:test-checks.c:407\n"
"F:3:0:UI1:test-checks.c:408:7 != 7\n"
"P:4:0:UI1:test-checks.c:409\n"
"F:5:0:UI1:test-checks.c:410:10 != 10\n"
"E:check_ne_schar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_schar,
"B:step_assert_ge_schar\n"
"P:0:0:UI1:test-checks.c:416\n"
"F:1:0:UI1:test-checks.c:417:2 >= 3\n"
"E:step_assert_ge_schar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_schar,
"B:check_ge_schar\n"
"P:0:0:UI1:test-checks.c:422\n"
"F:1:0:UI1:test-checks.c:423:2 >= 3\n"
"F:*:0:UI1:test-checks.c:425:5 >= 6\n"
"P:2:0:UI1:test-checks.c:426\n"
"F:3:0:UI1:test-checks.c:427:8 >= 9\n"
"P:4:0:UI1:test-checks.c:428\n"
"F:5:0:UI1:test-checks.c:429:11 >= 12\n"
"E:check_ge_schar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_schar,
"B:step_assert_gt_schar\n"
"P:0:0:UI1:test-checks.c:435\n"
"F:1:0:UI1:test-checks.c:436:3 > 3\n"
"E:step_assert_gt_schar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_schar,
"B:check_gt_schar\n"
"P:0:0:UI1:test-checks.c:441\n"
"F:1:0:UI1:test-checks.c:442:3 > 3\n"
"F:*:0:UI1:test-checks.c:444:6 > 6\n"
"P:2:0:UI1:test-checks.c:445\n"
"F:3:0:UI1:test-checks.c:446:9 > 9\n"
"F:4:0:UI1:test-checks.c:447:10 > 11\n"
"E:check_gt_schar:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_schar,
"B:step_assert_le_schar\n"
"P:0:0:UI1:test-checks.c:454\n"
"F:1:0:UI1:test-checks.c:455:3 <= 2\n"
"E:step_assert_le_schar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_schar,
"B:check_le_schar\n"
"P:0:0:UI1:test-checks.c:460\n"
"F:1:0:UI1:test-checks.c:461:3 <= 2\n"
"F:*:0:UI1:test-checks.c:463:6 <= 5\n"
"P:2:0:UI1:test-checks.c:464\n"
"F:3:0:UI1:test-checks.c:465:9 <= 8\n"
"P:4:0:UI1:test-checks.c:466\n"
"F:5:0:UI1:test-checks.c:467:12 <= 11\n"
"E:check_le_schar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_schar,
"B:step_assert_lt_schar\n"
"P:0:0:UI1:test-checks.c:473\n"
"F:1:0:UI1:test-checks.c:474:3 < 3\n"
"E:step_assert_lt_schar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_schar,
"B:check_lt_schar\n"
"P:0:0:UI1:test-checks.c:479\n"
"F:1:0:UI1:test-checks.c:480:3 < 3\n"
"F:*:0:UI1:test-checks.c:482:6 < 6\n"
"P:2:0:UI1:test-checks.c:483\n"
"F:3:0:UI1:test-checks.c:484:9 < 9\n"
"P:4:0:UI1:test-checks.c:485\n"
"F:5:0:UI1:test-checks.c:486:12 < 12\n"
"E:check_lt_schar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_uchar,
"B:step_assert_eq_uchar\n"
"P:0:0:UI1:test-checks.c:492\n"
"F:1:0:UI1:test-checks.c:493:2 == 3\n"
"E:step_assert_eq_uchar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_uchar,
"B:check_eq_uchar\n"
"P:0:0:UI1:test-checks.c:498\n"
"F:1:0:UI1:test-checks.c:499:2 == 3\n"
"F:*:0:UI1:test-checks.c:501:5 == 6\n"
"P:2:0:UI1:test-checks.c:502\n"
"F:3:0:UI1:test-checks.c:503:8 == 9\n"
"P:4:0:UI1:test-checks.c:504\n"
"F:5:0:UI1:test-checks.c:505:11 == 12\n"
"E:check_eq_uchar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_uchar,
"B:step_assert_ne_uchar\n"
"P:0:0:UI1:test-checks.c:511\n"
"F:1:0:UI1:test-checks.c:512:1 != 1\n"
"E:step_assert_ne_uchar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_uchar,
"B:check_ne_uchar\n"
"P:0:0:UI1:test-checks.c:517\n"
"F:1:0:UI1:test-checks.c:518:1 != 1\n"
"F:*:0:UI1:test-checks.c:520:4 != 4\n"
"P:2:0:UI1:test-checks.c:521\n"
"F:3:0:UI1:test-checks.c:522:7 != 7\n"
"P:4:0:UI1:test-checks.c:523\n"
"F:5:0:UI1:test-checks.c:524:10 != 10\n"
"E:check_ne_uchar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_uchar,
"B:step_assert_ge_uchar\n"
"P:0:0:UI1:test-checks.c:530\n"
"F:1:0:UI1:test-checks.c:531:2 >= 3\n"
"E:step_assert_ge_uchar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_uchar,
"B:check_ge_uchar\n"
"P:0:0:UI1:test-checks.c:536\n"
"F:1:0:UI1:test-checks.c:537:2 >= 3\n"
"F:*:0:UI1:test-checks.c:539:5 >= 6\n"
"P:2:0:UI1:test-checks.c:540\n"
"F:3:0:UI1:test-checks.c:541:8 >= 9\n"
"P:4:0:UI1:test-checks.c:542\n"
"F:5:0:UI1:test-checks.c:543:11 >= 12\n"
"E:check_ge_uchar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_uchar,
"B:step_assert_gt_uchar\n"
"P:0:0:UI1:test-checks.c:549\n"
"F:1:0:UI1:test-checks.c:550:3 > 3\n"
"E:step_assert_gt_uchar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_uchar,
"B:check_gt_uchar\n"
"P:0:0:UI1:test-checks.c:555\n"
"F:1:0:UI1:test-checks.c:556:3 > 3\n"
"F:*:0:UI1:test-checks.c:558:6 > 6\n"
"P:2:0:UI1:test-checks.c:559\n"
"F:3:0:UI1:test-checks.c:560:9 > 9\n"
"F:4:0:UI1:test-checks.c:561:10 > 11\n"
"E:check_gt_uchar:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_uchar,
"B:step_assert_le_uchar\n"
"P:0:0:UI1:test-checks.c:568\n"
"F:1:0:UI1:test-checks.c:569:3 <= 2\n"
"E:step_assert_le_uchar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_uchar,
"B:check_le_uchar\n"
"P:0:0:UI1:test-checks.c:574\n"
"F:1:0:UI1:test-checks.c:575:3 <= 2\n"
"F:*:0:UI1:test-checks.c:577:6 <= 5\n"
"P:2:0:UI1:test-checks.c:578\n"
"F:3:0:UI1:test-checks.c:579:9 <= 8\n"
"P:4:0:UI1:test-checks.c:580\n"
"F:5:0:UI1:test-checks.c:581:12 <= 11\n"
"E:check_le_uchar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_uchar,
"B:step_assert_lt_uchar\n"
"P:0:0:UI1:test-checks.c:587\n"
"F:1:0:UI1:test-checks.c:588:3 < 3\n"
"E:step_assert_lt_uchar:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_uchar,
"B:check_lt_uchar\n"
"P:0:0:UI1:test-checks.c:593\n"
"F:1:0:UI1:test-checks.c:594:3 < 3\n"
"F:*:0:UI1:test-checks.c:596:6 < 6\n"
"P:2:0:UI1:test-checks.c:597\n"
"F:3:0:UI1:test-checks.c:598:9 < 9\n"
"P:4:0:UI1:test-checks.c:599\n"
"F:5:0:UI1:test-checks.c:600:12 < 12\n"
"E:check_lt_uchar:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_short,
"B:step_assert_eq_short\n"
"P:0:0:UI1:test-checks.c:606\n"
"F:1:0:UI1:test-checks.c:607:2 == 3\n"
"E:step_assert_eq_short:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_short,
"B:check_eq_short\n"
"P:0:0:UI1:test-checks.c:612\n"
"F:1:0:UI1:test-checks.c:613:2 == 3\n"
"F:*:0:UI1:test-checks.c:615:5 == 6\n"
"P:2:0:UI1:test-checks.c:616\n"
"F:3:0:UI1:test-checks.c:617:8 == 9\n"
"P:4:0:UI1:test-checks.c:618\n"
"F:5:0:UI1:test-checks.c:619:11 == 12\n"
"E:check_eq_short:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_short,
"B:step_assert_ne_short\n"
"P:0:0:UI1:test-checks.c:625\n"
"F:1:0:UI1:test-checks.c:626:1 != 1\n"
"E:step_assert_ne_short:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_short,
"B:check_ne_short\n"
"P:0:0:UI1:test-checks.c:631\n"
"F:1:0:UI1:test-checks.c:632:1 != 1\n"
"F:*:0:UI1:test-checks.c:634:4 != 4\n"
"P:2:0:UI1:test-checks.c:635\n"
"F:3:0:UI1:test-checks.c:636:7 != 7\n"
"P:4:0:UI1:test-checks.c:637\n"
"F:5:0:UI1:test-checks.c:638:10 != 10\n"
"E:check_ne_short:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_short,
"B:step_assert_ge_short\n"
"P:0:0:UI1:test-checks.c:644\n"
"F:1:0:UI1:test-checks.c:645:2 >= 3\n"
"E:step_assert_ge_short:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_short,
"B:check_ge_short\n"
"P:0:0:UI1:test-checks.c:650\n"
"F:1:0:UI1:test-checks.c:651:2 >= 3\n"
"F:*:0:UI1:test-checks.c:653:5 >= 6\n"
"P:2:0:UI1:test-checks.c:654\n"
"F:3:0:UI1:test-checks.c:655:8 >= 9\n"
"P:4:0:UI1:test-checks.c:656\n"
"F:5:0:UI1:test-checks.c:657:11 >= 12\n"
"E:check_ge_short:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_short,
"B:step_assert_gt_short\n"
"P:0:0:UI1:test-checks.c:663\n"
"F:1:0:UI1:test-checks.c:664:3 > 3\n"
"E:step_assert_gt_short:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_short,
"B:check_gt_short\n"
"P:0:0:UI1:test-checks.c:669\n"
"F:1:0:UI1:test-checks.c:670:3 > 3\n"
"F:*:0:UI1:test-checks.c:672:6 > 6\n"
"P:2:0:UI1:test-checks.c:673\n"
"F:3:0:UI1:test-checks.c:674:9 > 9\n"
"F:4:0:UI1:test-checks.c:675:10 > 11\n"
"E:check_gt_short:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_short,
"B:step_assert_le_short\n"
"P:0:0:UI1:test-checks.c:682\n"
"F:1:0:UI1:test-checks.c:683:3 <= 2\n"
"E:step_assert_le_short:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_short,
"B:check_le_short\n"
"P:0:0:UI1:test-checks.c:688\n"
"F:1:0:UI1:test-checks.c:689:3 <= 2\n"
"F:*:0:UI1:test-checks.c:691:6 <= 5\n"
"P:2:0:UI1:test-checks.c:692\n"
"F:3:0:UI1:test-checks.c:693:9 <= 8\n"
"P:4:0:UI1:test-checks.c:694\n"
"F:5:0:UI1:test-checks.c:695:12 <= 11\n"
"E:check_le_short:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_short,
"B:step_assert_lt_short\n"
"P:0:0:UI1:test-checks.c:701\n"
"F:1:0:UI1:test-checks.c:702:3 < 3\n"
"E:step_assert_lt_short:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_short,
"B:check_lt_short\n"
"P:0:0:UI1:test-checks.c:707\n"
"F:1:0:UI1:test-checks.c:708:3 < 3\n"
"F:*:0:UI1:test-checks.c:710:6 < 6\n"
"P:2:0:UI1:test-checks.c:711\n"
"F:3:0:UI1:test-checks.c:712:9 < 9\n"
"P:4:0:UI1:test-checks.c:713\n"
"F:5:0:UI1:test-checks.c:714:12 < 12\n"
"E:check_lt_short:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_ushort,
"B:step_assert_eq_ushort\n"
"P:0:0:UI1:test-checks.c:720\n"
"F:1:0:UI1:test-checks.c:721:2 == 3\n"
"E:step_assert_eq_ushort:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_ushort,
"B:check_eq_ushort\n"
"P:0:0:UI1:test-checks.c:726\n"
"F:1:0:UI1:test-checks.c:727:2 == 3\n"
"F:*:0:UI1:test-checks.c:729:5 == 6\n"
"P:2:0:UI1:test-checks.c:730\n"
"F:3:0:UI1:test-checks.c:731:8 == 9\n"
"P:4:0:UI1:test-checks.c:732\n"
"F:5:0:UI1:test-checks.c:733:11 == 12\n"
"E:check_eq_ushort:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_ushort,
"B:step_assert_ne_ushort\n"
"P:0:0:UI1:test-checks.c:739\n"
"F:1:0:UI1:test-checks.c:740:1 != 1\n"
"E:step_assert_ne_ushort:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_ushort,
"B:check_ne_ushort\n"
"P:0:0:UI1:test-checks.c:745\n"
"F:1:0:UI1:test-checks.c:746:1 != 1\n"
"F:*:0:UI1:test-checks.c:748:4 != 4\n"
"P:2:0:UI1:test-checks.c:749\n"
"F:3:0:UI1:test-checks.c:750:7 != 7\n"
"P:4:0:UI1:test-checks.c:751\n"
"F:5:0:UI1:test-checks.c:752:10 != 10\n"
"E:check_ne_ushort:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_ushort,
"B:step_assert_ge_ushort\n"
"P:0:0:UI1:test-checks.c:758\n"
"F:1:0:UI1:test-checks.c:759:2 >= 3\n"
"E:step_assert_ge_ushort:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_ushort,
"B:check_ge_ushort\n"
"P:0:0:UI1:test-checks.c:764\n"
"F:1:0:UI1:test-checks.c:765:2 >= 3\n"
"F:*:0:UI1:test-checks.c:767:5 >= 6\n"
"P:2:0:UI1:test-checks.c:768\n"
"F:3:0:UI1:test-checks.c:769:8 >= 9\n"
"P:4:0:UI1:test-checks.c:770\n"
"F:5:0:UI1:test-checks.c:771:11 >= 12\n"
"E:check_ge_ushort:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_ushort,
"B:step_assert_gt_ushort\n"
"P:0:0:UI1:test-checks.c:777\n"
"F:1:0:UI1:test-checks.c:778:3 > 3\n"
"E:step_assert_gt_ushort:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_ushort,
"B:check_gt_ushort\n"
"P:0:0:UI1:test-checks.c:783\n"
"F:1:0:UI1:test-checks.c:784:3 > 3\n"
"F:*:0:UI1:test-checks.c:786:6 > 6\n"
"P:2:0:UI1:test-checks.c:787\n"
"F:3:0:UI1:test-checks.c:788:9 > 9\n"
"F:4:0:UI1:test-checks.c:789:10 > 11\n"
"E:check_gt_ushort:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_ushort,
"B:step_assert_le_ushort\n"
"P:0:0:UI1:test-checks.c:796\n"
"F:1:0:UI1:test-checks.c:797:3 <= 2\n"
"E:step_assert_le_ushort:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_ushort,
"B:check_le_ushort\n"
"P:0:0:UI1:test-checks.c:802\n"
"F:1:0:UI1:test-checks.c:803:3 <= 2\n"
"F:*:0:UI1:test-checks.c:805:6 <= 5\n"
"P:2:0:UI1:test-checks.c:806\n"
"F:3:0:UI1:test-checks.c:807:9 <= 8\n"
"P:4:0:UI1:test-checks.c:808\n"
"F:5:0:UI1:test-checks.c:809:12 <= 11\n"
"E:check_le_ushort:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_ushort,
"B:step_assert_lt_ushort\n"
"P:0:0:UI1:test-checks.c:815\n"
"F:1:0:UI1:test-checks.c:816:3 < 3\n"
"E:step_assert_lt_ushort:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_ushort,
"B:check_lt_ushort\n"
"P:0:0:UI1:test-checks.c:821\n"
"F:1:0:UI1:test-checks.c:822:3 < 3\n"
"F:*:0:UI1:test-checks.c:824:6 < 6\n"
"P:2:0:UI1:test-checks.c:825\n"
"F:3:0:UI1:test-checks.c:826:9 < 9\n"
"P:4:0:UI1:test-checks.c:827\n"
"F:5:0:UI1:test-checks.c:828:12 < 12\n"
"E:check_lt_ushort:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_int,
"B:step_assert_eq_int\n"
"P:0:0:UI1:test-checks.c:834\n"
"F:1:0:UI1:test-checks.c:835:2 == 3\n"
"E:step_assert_eq_int:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_int,
"B:check_eq_int\n"
"P:0:0:UI1:test-checks.c:840\n"
"F:1:0:UI1:test-checks.c:841:2 == 3\n"
"F:*:0:UI1:test-checks.c:843:5 == 6\n"
"P:2:0:UI1:test-checks.c:844\n"
"F:3:0:UI1:test-checks.c:845:8 == 9\n"
"P:4:0:UI1:test-checks.c:846\n"
"F:5:0:UI1:test-checks.c:847:11 == 12\n"
"E:check_eq_int:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_int,
"B:step_assert_ne_int\n"
"P:0:0:UI1:test-checks.c:853\n"
"F:1:0:UI1:test-checks.c:854:1 != 1\n"
"E:step_assert_ne_int:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_int,
"B:check_ne_int\n"
"P:0:0:UI1:test-checks.c:859\n"
"F:1:0:UI1:test-checks.c:860:1 != 1\n"
"F:*:0:UI1:test-checks.c:862:4 != 4\n"
"P:2:0:UI1:test-checks.c:863\n"
"F:3:0:UI1:test-checks.c:864:7 != 7\n"
"P:4:0:UI1:test-checks.c:865\n"
"F:5:0:UI1:test-checks.c:866:10 != 10\n"
"E:check_ne_int:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_int,
"B:step_assert_ge_int\n"
"P:0:0:UI1:test-checks.c:872\n"
"F:1:0:UI1:test-checks.c:873:2 >= 3\n"
"E:step_assert_ge_int:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_int,
"B:check_ge_int\n"
"P:0:0:UI1:test-checks.c:878\n"
"F:1:0:UI1:test-checks.c:879:2 >= 3\n"
"F:*:0:UI1:test-checks.c:881:5 >= 6\n"
"P:2:0:UI1:test-checks.c:882\n"
"F:3:0:UI1:test-checks.c:883:8 >= 9\n"
"P:4:0:UI1:test-checks.c:884\n"
"F:5:0:UI1:test-checks.c:885:11 >= 12\n"
"E:check_ge_int:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_int,
"B:step_assert_gt_int\n"
"P:0:0:UI1:test-checks.c:891\n"
"F:1:0:UI1:test-checks.c:892:3 > 3\n"
"E:step_assert_gt_int:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_int,
"B:check_gt_int\n"
"P:0:0:UI1:test-checks.c:897\n"
"F:1:0:UI1:test-checks.c:898:3 > 3\n"
"F:*:0:UI1:test-checks.c:900:6 > 6\n"
"P:2:0:UI1:test-checks.c:901\n"
"F:3:0:UI1:test-checks.c:902:9 > 9\n"
"F:4:0:UI1:test-checks.c:903:10 > 11\n"
"E:check_gt_int:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_int,
"B:step_assert_le_int\n"
"P:0:0:UI1:test-checks.c:910\n"
"F:1:0:UI1:test-checks.c:911:3 <= 2\n"
"E:step_assert_le_int:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_int,
"B:check_le_int\n"
"P:0:0:UI1:test-checks.c:916\n"
"F:1:0:UI1:test-checks.c:917:3 <= 2\n"
"F:*:0:UI1:test-checks.c:919:6 <= 5\n"
"P:2:0:UI1:test-checks.c:920\n"
"F:3:0:UI1:test-checks.c:921:9 <= 8\n"
"P:4:0:UI1:test-checks.c:922\n"
"F:5:0:UI1:test-checks.c:923:12 <= 11\n"
"E:check_le_int:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_int,
"B:step_assert_lt_int\n"
"P:0:0:UI1:test-checks.c:929\n"
"F:1:0:UI1:test-checks.c:930:3 < 3\n"
"E:step_assert_lt_int:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_int,
"B:check_lt_int\n"
"P:0:0:UI1:test-checks.c:935\n"
"F:1:0:UI1:test-checks.c:936:3 < 3\n"
"F:*:0:UI1:test-checks.c:938:6 < 6\n"
"P:2:0:UI1:test-checks.c:939\n"
"F:3:0:UI1:test-checks.c:940:9 < 9\n"
"P:4:0:UI1:test-checks.c:941\n"
"F:5:0:UI1:test-checks.c:942:12 < 12\n"
"E:check_lt_int:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_uint,
"B:step_assert_eq_uint\n"
"P:0:0:UI1:test-checks.c:948\n"
"F:1:0:UI1:test-checks.c:949:2 == 3\n"
"E:step_assert_eq_uint:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_uint,
"B:check_eq_uint\n"
"P:0:0:UI1:test-checks.c:954\n"
"F:1:0:UI1:test-checks.c:955:2 == 3\n"
"F:*:0:UI1:test-checks.c:957:5 == 6\n"
"P:2:0:UI1:test-checks.c:958\n"
"F:3:0:UI1:test-checks.c:959:8 == 9\n"
"P:4:0:UI1:test-checks.c:960\n"
"F:5:0:UI1:test-checks.c:961:11 == 12\n"
"E:check_eq_uint:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_uint,
"B:step_assert_ne_uint\n"
"P:0:0:UI1:test-checks.c:967\n"
"F:1:0:UI1:test-checks.c:968:1 != 1\n"
"E:step_assert_ne_uint:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_uint,
"B:check_ne_uint\n"
"P:0:0:UI1:test-checks.c:973\n"
"F:1:0:UI1:test-checks.c:974:1 != 1\n"
"F:*:0:UI1:test-checks.c:976:4 != 4\n"
"P:2:0:UI1:test-checks.c:977\n"
"F:3:0:UI1:test-checks.c:978:7 != 7\n"
"P:4:0:UI1:test-checks.c:979\n"
"F:5:0:UI1:test-checks.c:980:10 != 10\n"
"E:check_ne_uint:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_uint,
"B:step_assert_ge_uint\n"
"P:0:0:UI1:test-checks.c:986\n"
"F:1:0:UI1:test-checks.c:987:2 >= 3\n"
"E:step_assert_ge_uint:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_uint,
"B:check_ge_uint\n"
"P:0:0:UI1:test-checks.c:992\n"
"F:1:0:UI1:test-checks.c:993:2 >= 3\n"
"F:*:0:UI1:test-checks.c:995:5 >= 6\n"
"P:2:0:UI1:test-checks.c:996\n"
"F:3:0:UI1:test-checks.c:997:8 >= 9\n"
"P:4:0:UI1:test-checks.c:998\n"
"F:5:0:UI1:test-checks.c:999:11 >= 12\n"
"E:check_ge_uint:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_uint,
"B:step_assert_gt_uint\n"
"P:0:0:UI1:test-checks.c:1005\n"
"F:1:0:UI1:test-checks.c:1006:3 > 3\n"
"E:step_assert_gt_uint:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_uint,
"B:check_gt_uint\n"
"P:0:0:UI1:test-checks.c:1011\n"
"F:1:0:UI1:test-checks.c:1012:3 > 3\n"
"F:*:0:UI1:test-checks.c:1014:6 > 6\n"
"P:2:0:UI1:test-checks.c:1015\n"
"F:3:0:UI1:test-checks.c:1016:9 > 9\n"
"F:4:0:UI1:test-checks.c:1017:10 > 11\n"
"E:check_gt_uint:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_uint,
"B:step_assert_le_uint\n"
"P:0:0:UI1:test-checks.c:1024\n"
"F:1:0:UI1:test-checks.c:1025:3 <= 2\n"
"E:step_assert_le_uint:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_uint,
"B:check_le_uint\n"
"P:0:0:UI1:test-checks.c:1030\n"
"F:1:0:UI1:test-checks.c:1031:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1033:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1034\n"
"F:3:0:UI1:test-checks.c:1035:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1036\n"
"F:5:0:UI1:test-checks.c:1037:12 <= 11\n"
"E:check_le_uint:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_uint,
"B:step_assert_lt_uint\n"
"P:0:0:UI1:test-checks.c:1043\n"
"F:1:0:UI1:test-checks.c:1044:3 < 3\n"
"E:step_assert_lt_uint:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_uint,
"B:check_lt_uint\n"
"P:0:0:UI1:test-checks.c:1049\n"
"F:1:0:UI1:test-checks.c:1050:3 < 3\n"
"F:*:0:UI1:test-checks.c:1052:6 < 6\n"
"P:2:0:UI1:test-checks.c:1053\n"
"F:3:0:UI1:test-checks.c:1054:9 < 9\n"
"P:4:0:UI1:test-checks.c:1055\n"
"F:5:0:UI1:test-checks.c:1056:12 < 12\n"
"E:check_lt_uint:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_long,
"B:step_assert_eq_long\n"
"P:0:0:UI1:test-checks.c:1062\n"
"F:1:0:UI1:test-checks.c:1063:2 == 3\n"
"E:step_assert_eq_long:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_long,
"B:check_eq_long\n"
"P:0:0:UI1:test-checks.c:1068\n"
"F:1:0:UI1:test-checks.c:1069:2 == 3\n"
"F:*:0:UI1:test-checks.c:1071:5 == 6\n"
"P:2:0:UI1:test-checks.c:1072\n"
"F:3:0:UI1:test-checks.c:1073:8 == 9\n"
"P:4:0:UI1:test-checks.c:1074\n"
"F:5:0:UI1:test-checks.c:1075:11 == 12\n"
"E:check_eq_long:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_long,
"B:step_assert_ne_long\n"
"P:0:0:UI1:test-checks.c:1081\n"
"F:1:0:UI1:test-checks.c:1082:1 != 1\n"
"E:step_assert_ne_long:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_long,
"B:check_ne_long\n"
"P:0:0:UI1:test-checks.c:1087\n"
"F:1:0:UI1:test-checks.c:1088:1 != 1\n"
"F:*:0:UI1:test-checks.c:1090:4 != 4\n"
"P:2:0:UI1:test-checks.c:1091\n"
"F:3:0:UI1:test-checks.c:1092:7 != 7\n"
"P:4:0:UI1:test-checks.c:1093\n"
"F:5:0:UI1:test-checks.c:1094:10 != 10\n"
"E:check_ne_long:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_long,
"B:step_assert_ge_long\n"
"P:0:0:UI1:test-checks.c:1100\n"
"F:1:0:UI1:test-checks.c:1101:2 >= 3\n"
"E:step_assert_ge_long:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_long,
"B:check_ge_long\n"
"P:0:0:UI1:test-checks.c:1106\n"
"F:1:0:UI1:test-checks.c:1107:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1109:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1110\n"
"F:3:0:UI1:test-checks.c:1111:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1112\n"
"F:5:0:UI1:test-checks.c:1113:11 >= 12\n"
"E:check_ge_long:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_long,
"B:step_assert_gt_long\n"
"P:0:0:UI1:test-checks.c:1119\n"
"F:1:0:UI1:test-checks.c:1120:3 > 3\n"
"E:step_assert_gt_long:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_long,
"B:check_gt_long\n"
"P:0:0:UI1:test-checks.c:1125\n"
"F:1:0:UI1:test-checks.c:1126:3 > 3\n"
"F:*:0:UI1:test-checks.c:1128:6 > 6\n"
"P:2:0:UI1:test-checks.c:1129\n"
"F:3:0:UI1:test-checks.c:1130:9 > 9\n"
"F:4:0:UI1:test-checks.c:1131:10 > 11\n"
"E:check_gt_long:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_long,
"B:step_assert_le_long\n"
"P:0:0:UI1:test-checks.c:1138\n"
"F:1:0:UI1:test-checks.c:1139:3 <= 2\n"
"E:step_assert_le_long:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_long,
"B:check_le_long\n"
"P:0:0:UI1:test-checks.c:1144\n"
"F:1:0:UI1:test-checks.c:1145:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1147:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1148\n"
"F:3:0:UI1:test-checks.c:1149:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1150\n"
"F:5:0:UI1:test-checks.c:1151:12 <= 11\n"
"E:check_le_long:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_long,
"B:step_assert_lt_long\n"
"P:0:0:UI1:test-checks.c:1157\n"
"F:1:0:UI1:test-checks.c:1158:3 < 3\n"
"E:step_assert_lt_long:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_long,
"B:check_lt_long\n"
"P:0:0:UI1:test-checks.c:1163\n"
"F:1:0:UI1:test-checks.c:1164:3 < 3\n"
"F:*:0:UI1:test-checks.c:1166:6 < 6\n"
"P:2:0:UI1:test-checks.c:1167\n"
"F:3:0:UI1:test-checks.c:1168:9 < 9\n"
"P:4:0:UI1:test-checks.c:1169\n"
"F:5:0:UI1:test-checks.c:1170:12 < 12\n"
"E:check_lt_long:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_ulong,
"B:step_assert_eq_ulong\n"
"P:0:0:UI1:test-checks.c:1176\n"
"F:1:0:UI1:test-checks.c:1177:2 == 3\n"
"E:step_assert_eq_ulong:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_ulong,
"B:check_eq_ulong\n"
"P:0:0:UI1:test-checks.c:1182\n"
"F:1:0:UI1:test-checks.c:1183:2 == 3\n"
"F:*:0:UI1:test-checks.c:1185:5 == 6\n"
"P:2:0:UI1:test-checks.c:1186\n"
"F:3:0:UI1:test-checks.c:1187:8 == 9\n"
"P:4:0:UI1:test-checks.c:1188\n"
"F:5:0:UI1:test-checks.c:1189:11 == 12\n"
"E:check_eq_ulong:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_ulong,
"B:step_assert_ne_ulong\n"
"P:0:0:UI1:test-checks.c:1195\n"
"F:1:0:UI1:test-checks.c:1196:1 != 1\n"
"E:step_assert_ne_ulong:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_ulong,
"B:check_ne_ulong\n"
"P:0:0:UI1:test-checks.c:1201\n"
"F:1:0:UI1:test-checks.c:1202:1 != 1\n"
"F:*:0:UI1:test-checks.c:1204:4 != 4\n"
"P:2:0:UI1:test-checks.c:1205\n"
"F:3:0:UI1:test-checks.c:1206:7 != 7\n"
"P:4:0:UI1:test-checks.c:1207\n"
"F:5:0:UI1:test-checks.c:1208:10 != 10\n"
"E:check_ne_ulong:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_ulong,
"B:step_assert_ge_ulong\n"
"P:0:0:UI1:test-checks.c:1214\n"
"F:1:0:UI1:test-checks.c:1215:2 >= 3\n"
"E:step_assert_ge_ulong:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_ulong,
"B:check_ge_ulong\n"
"P:0:0:UI1:test-checks.c:1220\n"
"F:1:0:UI1:test-checks.c:1221:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1223:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1224\n"
"F:3:0:UI1:test-checks.c:1225:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1226\n"
"F:5:0:UI1:test-checks.c:1227:11 >= 12\n"
"E:check_ge_ulong:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_ulong,
"B:step_assert_gt_ulong\n"
"P:0:0:UI1:test-checks.c:1233\n"
"F:1:0:UI1:test-checks.c:1234:3 > 3\n"
"E:step_assert_gt_ulong:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_ulong,
"B:check_gt_ulong\n"
"P:0:0:UI1:test-checks.c:1239\n"
"F:1:0:UI1:test-checks.c:1240:3 > 3\n"
"F:*:0:UI1:test-checks.c:1242:6 > 6\n"
"P:2:0:UI1:test-checks.c:1243\n"
"F:3:0:UI1:test-checks.c:1244:9 > 9\n"
"F:4:0:UI1:test-checks.c:1245:10 > 11\n"
"E:check_gt_ulong:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_ulong,
"B:step_assert_le_ulong\n"
"P:0:0:UI1:test-checks.c:1252\n"
"F:1:0:UI1:test-checks.c:1253:3 <= 2\n"
"E:step_assert_le_ulong:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_ulong,
"B:check_le_ulong\n"
"P:0:0:UI1:test-checks.c:1258\n"
"F:1:0:UI1:test-checks.c:1259:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1261:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1262\n"
"F:3:0:UI1:test-checks.c:1263:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1264\n"
"F:5:0:UI1:test-checks.c:1265:12 <= 11\n"
"E:check_le_ulong:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_ulong,
"B:step_assert_lt_ulong\n"
"P:0:0:UI1:test-checks.c:1271\n"
"F:1:0:UI1:test-checks.c:1272:3 < 3\n"
"E:step_assert_lt_ulong:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_ulong,
"B:check_lt_ulong\n"
"P:0:0:UI1:test-checks.c:1277\n"
"F:1:0:UI1:test-checks.c:1278:3 < 3\n"
"F:*:0:UI1:test-checks.c:1280:6 < 6\n"
"P:2:0:UI1:test-checks.c:1281\n"
"F:3:0:UI1:test-checks.c:1282:9 < 9\n"
"P:4:0:UI1:test-checks.c:1283\n"
"F:5:0:UI1:test-checks.c:1284:12 < 12\n"
"E:check_lt_ulong:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_ll,
"B:step_assert_eq_ll\n"
"P:0:0:UI1:test-checks.c:1290\n"
"F:1:0:UI1:test-checks.c:1291:2 == 3\n"
"E:step_assert_eq_ll:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_ll,
"B:check_eq_ll\n"
"P:0:0:UI1:test-checks.c:1296\n"
"F:1:0:UI1:test-checks.c:1297:2 == 3\n"
"F:*:0:UI1:test-checks.c:1299:5 == 6\n"
"P:2:0:UI1:test-checks.c:1300\n"
"F:3:0:UI1:test-checks.c:1301:8 == 9\n"
"P:4:0:UI1:test-checks.c:1302\n"
"F:5:0:UI1:test-checks.c:1303:11 == 12\n"
"E:check_eq_ll:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_ll,
"B:step_assert_ne_ll\n"
"P:0:0:UI1:test-checks.c:1309\n"
"F:1:0:UI1:test-checks.c:1310:1 != 1\n"
"E:step_assert_ne_ll:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_ll,
"B:check_ne_ll\n"
"P:0:0:UI1:test-checks.c:1315\n"
"F:1:0:UI1:test-checks.c:1316:1 != 1\n"
"F:*:0:UI1:test-checks.c:1318:4 != 4\n"
"P:2:0:UI1:test-checks.c:1319\n"
"F:3:0:UI1:test-checks.c:1320:7 != 7\n"
"P:4:0:UI1:test-checks.c:1321\n"
"F:5:0:UI1:test-checks.c:1322:10 != 10\n"
"E:check_ne_ll:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_ll,
"B:step_assert_ge_ll\n"
"P:0:0:UI1:test-checks.c:1328\n"
"F:1:0:UI1:test-checks.c:1329:2 >= 3\n"
"E:step_assert_ge_ll:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_ll,
"B:check_ge_ll\n"
"P:0:0:UI1:test-checks.c:1334\n"
"F:1:0:UI1:test-checks.c:1335:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1337:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1338\n"
"F:3:0:UI1:test-checks.c:1339:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1340\n"
"F:5:0:UI1:test-checks.c:1341:11 >= 12\n"
"E:check_ge_ll:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_ll,
"B:step_assert_gt_ll\n"
"P:0:0:UI1:test-checks.c:1347\n"
"F:1:0:UI1:test-checks.c:1348:3 > 3\n"
"E:step_assert_gt_ll:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_ll,
"B:check_gt_ll\n"
"P:0:0:UI1:test-checks.c:1353\n"
"F:1:0:UI1:test-checks.c:1354:3 > 3\n"
"F:*:0:UI1:test-checks.c:1356:6 > 6\n"
"P:2:0:UI1:test-checks.c:1357\n"
"F:3:0:UI1:test-checks.c:1358:9 > 9\n"
"F:4:0:UI1:test-checks.c:1359:10 > 11\n"
"E:check_gt_ll:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_ll,
"B:step_assert_le_ll\n"
"P:0:0:UI1:test-checks.c:1366\n"
"F:1:0:UI1:test-checks.c:1367:3 <= 2\n"
"E:step_assert_le_ll:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_ll,
"B:check_le_ll\n"
"P:0:0:UI1:test-checks.c:1372\n"
"F:1:0:UI1:test-checks.c:1373:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1375:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1376\n"
"F:3:0:UI1:test-checks.c:1377:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1378\n"
"F:5:0:UI1:test-checks.c:1379:12 <= 11\n"
"E:check_le_ll:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_ll,
"B:step_assert_lt_ll\n"
"P:0:0:UI1:test-checks.c:1385\n"
"F:1:0:UI1:test-checks.c:1386:3 < 3\n"
"E:step_assert_lt_ll:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_ll,
"B:check_lt_ll\n"
"P:0:0:UI1:test-checks.c:1391\n"
"F:1:0:UI1:test-checks.c:1392:3 < 3\n"
"F:*:0:UI1:test-checks.c:1394:6 < 6\n"
"P:2:0:UI1:test-checks.c:1395\n"
"F:3:0:UI1:test-checks.c:1396:9 < 9\n"
"P:4:0:UI1:test-checks.c:1397\n"
"F:5:0:UI1:test-checks.c:1398:12 < 12\n"
"E:check_lt_ll:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_ull,
"B:step_assert_eq_ull\n"
"P:0:0:UI1:test-checks.c:1404\n"
"F:1:0:UI1:test-checks.c:1405:2 == 3\n"
"E:step_assert_eq_ull:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_ull,
"B:check_eq_ull\n"
"P:0:0:UI1:test-checks.c:1410\n"
"F:1:0:UI1:test-checks.c:1411:2 == 3\n"
"F:*:0:UI1:test-checks.c:1413:5 == 6\n"
"P:2:0:UI1:test-checks.c:1414\n"
"F:3:0:UI1:test-checks.c:1415:8 == 9\n"
"P:4:0:UI1:test-checks.c:1416\n"
"F:5:0:UI1:test-checks.c:1417:11 == 12\n"
"E:check_eq_ull:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_ull,
"B:step_assert_ne_ull\n"
"P:0:0:UI1:test-checks.c:1423\n"
"F:1:0:UI1:test-checks.c:1424:1 != 1\n"
"E:step_assert_ne_ull:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_ull,
"B:check_ne_ull\n"
"P:0:0:UI1:test-checks.c:1429\n"
"F:1:0:UI1:test-checks.c:1430:1 != 1\n"
"F:*:0:UI1:test-checks.c:1432:4 != 4\n"
"P:2:0:UI1:test-checks.c:1433\n"
"F:3:0:UI1:test-checks.c:1434:7 != 7\n"
"P:4:0:UI1:test-checks.c:1435\n"
"F:5:0:UI1:test-checks.c:1436:10 != 10\n"
"E:check_ne_ull:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_ull,
"B:step_assert_ge_ull\n"
"P:0:0:UI1:test-checks.c:1442\n"
"F:1:0:UI1:test-checks.c:1443:2 >= 3\n"
"E:step_assert_ge_ull:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_ull,
"B:check_ge_ull\n"
"P:0:0:UI1:test-checks.c:1448\n"
"F:1:0:UI1:test-checks.c:1449:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1451:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1452\n"
"F:3:0:UI1:test-checks.c:1453:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1454\n"
"F:5:0:UI1:test-checks.c:1455:11 >= 12\n"
"E:check_ge_ull:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_ull,
"B:step_assert_gt_ull\n"
"P:0:0:UI1:test-checks.c:1461\n"
"F:1:0:UI1:test-checks.c:1462:3 > 3\n"
"E:step_assert_gt_ull:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_ull,
"B:check_gt_ull\n"
"P:0:0:UI1:test-checks.c:1467\n"
"F:1:0:UI1:test-checks.c:1468:3 > 3\n"
"F:*:0:UI1:test-checks.c:1470:6 > 6\n"
"P:2:0:UI1:test-checks.c:1471\n"
"F:3:0:UI1:test-checks.c:1472:9 > 9\n"
"F:4:0:UI1:test-checks.c:1473:10 > 11\n"
"E:check_gt_ull:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_ull,
"B:step_assert_le_ull\n"
"P:0:0:UI1:test-checks.c:1480\n"
"F:1:0:UI1:test-checks.c:1481:3 <= 2\n"
"E:step_assert_le_ull:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_ull,
"B:check_le_ull\n"
"P:0:0:UI1:test-checks.c:1486\n"
"F:1:0:UI1:test-checks.c:1487:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1489:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1490\n"
"F:3:0:UI1:test-checks.c:1491:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1492\n"
"F:5:0:UI1:test-checks.c:1493:12 <= 11\n"
"E:check_le_ull:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_ull,
"B:step_assert_lt_ull\n"
"P:0:0:UI1:test-checks.c:1499\n"
"F:1:0:UI1:test-checks.c:1500:3 < 3\n"
"E:step_assert_lt_ull:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_ull,
"B:check_lt_ull\n"
"P:0:0:UI1:test-checks.c:1505\n"
"F:1:0:UI1:test-checks.c:1506:3 < 3\n"
"F:*:0:UI1:test-checks.c:1508:6 < 6\n"
"P:2:0:UI1:test-checks.c:1509\n"
"F:3:0:UI1:test-checks.c:1510:9 < 9\n"
"P:4:0:UI1:test-checks.c:1511\n"
"F:5:0:UI1:test-checks.c:1512:12 < 12\n"
"E:check_lt_ull:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_i8,
"B:step_assert_eq_i8\n"
"P:0:0:UI1:test-checks.c:1518\n"
"F:1:0:UI1:test-checks.c:1519:2 == 3\n"
"E:step_assert_eq_i8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_i8,
"B:check_eq_i8\n"
"P:0:0:UI1:test-checks.c:1524\n"
"F:1:0:UI1:test-checks.c:1525:2 == 3\n"
"F:*:0:UI1:test-checks.c:1527:5 == 6\n"
"P:2:0:UI1:test-checks.c:1528\n"
"F:3:0:UI1:test-checks.c:1529:8 == 9\n"
"P:4:0:UI1:test-checks.c:1530\n"
"F:5:0:UI1:test-checks.c:1531:11 == 12\n"
"E:check_eq_i8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_i8,
"B:step_assert_ne_i8\n"
"P:0:0:UI1:test-checks.c:1537\n"
"F:1:0:UI1:test-checks.c:1538:1 != 1\n"
"E:step_assert_ne_i8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_i8,
"B:check_ne_i8\n"
"P:0:0:UI1:test-checks.c:1543\n"
"F:1:0:UI1:test-checks.c:1544:1 != 1\n"
"F:*:0:UI1:test-checks.c:1546:4 != 4\n"
"P:2:0:UI1:test-checks.c:1547\n"
"F:3:0:UI1:test-checks.c:1548:7 != 7\n"
"P:4:0:UI1:test-checks.c:1549\n"
"F:5:0:UI1:test-checks.c:1550:10 != 10\n"
"E:check_ne_i8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_i8,
"B:step_assert_ge_i8\n"
"P:0:0:UI1:test-checks.c:1556\n"
"F:1:0:UI1:test-checks.c:1557:2 >= 3\n"
"E:step_assert_ge_i8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_i8,
"B:check_ge_i8\n"
"P:0:0:UI1:test-checks.c:1562\n"
"F:1:0:UI1:test-checks.c:1563:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1565:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1566\n"
"F:3:0:UI1:test-checks.c:1567:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1568\n"
"F:5:0:UI1:test-checks.c:1569:11 >= 12\n"
"E:check_ge_i8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_i8,
"B:step_assert_gt_i8\n"
"P:0:0:UI1:test-checks.c:1575\n"
"F:1:0:UI1:test-checks.c:1576:3 > 3\n"
"E:step_assert_gt_i8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_i8,
"B:check_gt_i8\n"
"P:0:0:UI1:test-checks.c:1581\n"
"F:1:0:UI1:test-checks.c:1582:3 > 3\n"
"F:*:0:UI1:test-checks.c:1584:6 > 6\n"
"P:2:0:UI1:test-checks.c:1585\n"
"F:3:0:UI1:test-checks.c:1586:9 > 9\n"
"F:4:0:UI1:test-checks.c:1587:10 > 11\n"
"E:check_gt_i8:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_i8,
"B:step_assert_le_i8\n"
"P:0:0:UI1:test-checks.c:1594\n"
"F:1:0:UI1:test-checks.c:1595:3 <= 2\n"
"E:step_assert_le_i8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_i8,
"B:check_le_i8\n"
"P:0:0:UI1:test-checks.c:1600\n"
"F:1:0:UI1:test-checks.c:1601:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1603:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1604\n"
"F:3:0:UI1:test-checks.c:1605:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1606\n"
"F:5:0:UI1:test-checks.c:1607:12 <= 11\n"
"E:check_le_i8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_i8,
"B:step_assert_lt_i8\n"
"P:0:0:UI1:test-checks.c:1613\n"
"F:1:0:UI1:test-checks.c:1614:3 < 3\n"
"E:step_assert_lt_i8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_i8,
"B:check_lt_i8\n"
"P:0:0:UI1:test-checks.c:1619\n"
"F:1:0:UI1:test-checks.c:1620:3 < 3\n"
"F:*:0:UI1:test-checks.c:1622:6 < 6\n"
"P:2:0:UI1:test-checks.c:1623\n"
"F:3:0:UI1:test-checks.c:1624:9 < 9\n"
"P:4:0:UI1:test-checks.c:1625\n"
"F:5:0:UI1:test-checks.c:1626:12 < 12\n"
"E:check_lt_i8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_u8,
"B:step_assert_eq_u8\n"
"P:0:0:UI1:test-checks.c:1632\n"
"F:1:0:UI1:test-checks.c:1633:2 == 3\n"
"E:step_assert_eq_u8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_u8,
"B:check_eq_u8\n"
"P:0:0:UI1:test-checks.c:1638\n"
"F:1:0:UI1:test-checks.c:1639:2 == 3\n"
"F:*:0:UI1:test-checks.c:1641:5 == 6\n"
"P:2:0:UI1:test-checks.c:1642\n"
"F:3:0:UI1:test-checks.c:1643:8 == 9\n"
"P:4:0:UI1:test-checks.c:1644\n"
"F:5:0:UI1:test-checks.c:1645:11 == 12\n"
"E:check_eq_u8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_u8,
"B:step_assert_ne_u8\n"
"P:0:0:UI1:test-checks.c:1651\n"
"F:1:0:UI1:test-checks.c:1652:1 != 1\n"
"E:step_assert_ne_u8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_u8,
"B:check_ne_u8\n"
"P:0:0:UI1:test-checks.c:1657\n"
"F:1:0:UI1:test-checks.c:1658:1 != 1\n"
"F:*:0:UI1:test-checks.c:1660:4 != 4\n"
"P:2:0:UI1:test-checks.c:1661\n"
"F:3:0:UI1:test-checks.c:1662:7 != 7\n"
"P:4:0:UI1:test-checks.c:1663\n"
"F:5:0:UI1:test-checks.c:1664:10 != 10\n"
"E:check_ne_u8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_u8,
"B:step_assert_ge_u8\n"
"P:0:0:UI1:test-checks.c:1670\n"
"F:1:0:UI1:test-checks.c:1671:2 >= 3\n"
"E:step_assert_ge_u8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_u8,
"B:check_ge_u8\n"
"P:0:0:UI1:test-checks.c:1676\n"
"F:1:0:UI1:test-checks.c:1677:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1679:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1680\n"
"F:3:0:UI1:test-checks.c:1681:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1682\n"
"F:5:0:UI1:test-checks.c:1683:11 >= 12\n"
"E:check_ge_u8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_u8,
"B:step_assert_gt_u8\n"
"P:0:0:UI1:test-checks.c:1689\n"
"F:1:0:UI1:test-checks.c:1690:3 > 3\n"
"E:step_assert_gt_u8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_u8,
"B:check_gt_u8\n"
"P:0:0:UI1:test-checks.c:1695\n"
"F:1:0:UI1:test-checks.c:1696:3 > 3\n"
"F:*:0:UI1:test-checks.c:1698:6 > 6\n"
"P:2:0:UI1:test-checks.c:1699\n"
"F:3:0:UI1:test-checks.c:1700:9 > 9\n"
"F:4:0:UI1:test-checks.c:1701:10 > 11\n"
"E:check_gt_u8:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_u8,
"B:step_assert_le_u8\n"
"P:0:0:UI1:test-checks.c:1708\n"
"F:1:0:UI1:test-checks.c:1709:3 <= 2\n"
"E:step_assert_le_u8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_u8,
"B:check_le_u8\n"
"P:0:0:UI1:test-checks.c:1714\n"
"F:1:0:UI1:test-checks.c:1715:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1717:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1718\n"
"F:3:0:UI1:test-checks.c:1719:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1720\n"
"F:5:0:UI1:test-checks.c:1721:12 <= 11\n"
"E:check_le_u8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_u8,
"B:step_assert_lt_u8\n"
"P:0:0:UI1:test-checks.c:1727\n"
"F:1:0:UI1:test-checks.c:1728:3 < 3\n"
"E:step_assert_lt_u8:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_u8,
"B:check_lt_u8\n"
"P:0:0:UI1:test-checks.c:1733\n"
"F:1:0:UI1:test-checks.c:1734:3 < 3\n"
"F:*:0:UI1:test-checks.c:1736:6 < 6\n"
"P:2:0:UI1:test-checks.c:1737\n"
"F:3:0:UI1:test-checks.c:1738:9 < 9\n"
"P:4:0:UI1:test-checks.c:1739\n"
"F:5:0:UI1:test-checks.c:1740:12 < 12\n"
"E:check_lt_u8:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_i16,
"B:step_assert_eq_i16\n"
"P:0:0:UI1:test-checks.c:1746\n"
"F:1:0:UI1:test-checks.c:1747:2 == 3\n"
"E:step_assert_eq_i16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_i16,
"B:check_eq_i16\n"
"P:0:0:UI1:test-checks.c:1752\n"
"F:1:0:UI1:test-checks.c:1753:2 == 3\n"
"F:*:0:UI1:test-checks.c:1755:5 == 6\n"
"P:2:0:UI1:test-checks.c:1756\n"
"F:3:0:UI1:test-checks.c:1757:8 == 9\n"
"P:4:0:UI1:test-checks.c:1758\n"
"F:5:0:UI1:test-checks.c:1759:11 == 12\n"
"E:check_eq_i16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_i16,
"B:step_assert_ne_i16\n"
"P:0:0:UI1:test-checks.c:1765\n"
"F:1:0:UI1:test-checks.c:1766:1 != 1\n"
"E:step_assert_ne_i16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_i16,
"B:check_ne_i16\n"
"P:0:0:UI1:test-checks.c:1771\n"
"F:1:0:UI1:test-checks.c:1772:1 != 1\n"
"F:*:0:UI1:test-checks.c:1774:4 != 4\n"
"P:2:0:UI1:test-checks.c:1775\n"
"F:3:0:UI1:test-checks.c:1776:7 != 7\n"
"P:4:0:UI1:test-checks.c:1777\n"
"F:5:0:UI1:test-checks.c:1778:10 != 10\n"
"E:check_ne_i16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_i16,
"B:step_assert_ge_i16\n"
"P:0:0:UI1:test-checks.c:1784\n"
"F:1:0:UI1:test-checks.c:1785:2 >= 3\n"
"E:step_assert_ge_i16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_i16,
"B:check_ge_i16\n"
"P:0:0:UI1:test-checks.c:1790\n"
"F:1:0:UI1:test-checks.c:1791:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1793:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1794\n"
"F:3:0:UI1:test-checks.c:1795:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1796\n"
"F:5:0:UI1:test-checks.c:1797:11 >= 12\n"
"E:check_ge_i16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_i16,
"B:step_assert_gt_i16\n"
"P:0:0:UI1:test-checks.c:1803\n"
"F:1:0:UI1:test-checks.c:1804:3 > 3\n"
"E:step_assert_gt_i16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_i16,
"B:check_gt_i16\n"
"P:0:0:UI1:test-checks.c:1809\n"
"F:1:0:UI1:test-checks.c:1810:3 > 3\n"
"F:*:0:UI1:test-checks.c:1812:6 > 6\n"
"P:2:0:UI1:test-checks.c:1813\n"
"F:3:0:UI1:test-checks.c:1814:9 > 9\n"
"F:4:0:UI1:test-checks.c:1815:10 > 11\n"
"E:check_gt_i16:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_i16,
"B:step_assert_le_i16\n"
"P:0:0:UI1:test-checks.c:1822\n"
"F:1:0:UI1:test-checks.c:1823:3 <= 2\n"
"E:step_assert_le_i16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_i16,
"B:check_le_i16\n"
"P:0:0:UI1:test-checks.c:1828\n"
"F:1:0:UI1:test-checks.c:1829:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1831:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1832\n"
"F:3:0:UI1:test-checks.c:1833:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1834\n"
"F:5:0:UI1:test-checks.c:1835:12 <= 11\n"
"E:check_le_i16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_i16,
"B:step_assert_lt_i16\n"
"P:0:0:UI1:test-checks.c:1841\n"
"F:1:0:UI1:test-checks.c:1842:3 < 3\n"
"E:step_assert_lt_i16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_i16,
"B:check_lt_i16\n"
"P:0:0:UI1:test-checks.c:1847\n"
"F:1:0:UI1:test-checks.c:1848:3 < 3\n"
"F:*:0:UI1:test-checks.c:1850:6 < 6\n"
"P:2:0:UI1:test-checks.c:1851\n"
"F:3:0:UI1:test-checks.c:1852:9 < 9\n"
"P:4:0:UI1:test-checks.c:1853\n"
"F:5:0:UI1:test-checks.c:1854:12 < 12\n"
"E:check_lt_i16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_u16,
"B:step_assert_eq_u16\n"
"P:0:0:UI1:test-checks.c:1860\n"
"F:1:0:UI1:test-checks.c:1861:2 == 3\n"
"E:step_assert_eq_u16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_u16,
"B:check_eq_u16\n"
"P:0:0:UI1:test-checks.c:1866\n"
"F:1:0:UI1:test-checks.c:1867:2 == 3\n"
"F:*:0:UI1:test-checks.c:1869:5 == 6\n"
"P:2:0:UI1:test-checks.c:1870\n"
"F:3:0:UI1:test-checks.c:1871:8 == 9\n"
"P:4:0:UI1:test-checks.c:1872\n"
"F:5:0:UI1:test-checks.c:1873:11 == 12\n"
"E:check_eq_u16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_u16,
"B:step_assert_ne_u16\n"
"P:0:0:UI1:test-checks.c:1879\n"
"F:1:0:UI1:test-checks.c:1880:1 != 1\n"
"E:step_assert_ne_u16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_u16,
"B:check_ne_u16\n"
"P:0:0:UI1:test-checks.c:1885\n"
"F:1:0:UI1:test-checks.c:1886:1 != 1\n"
"F:*:0:UI1:test-checks.c:1888:4 != 4\n"
"P:2:0:UI1:test-checks.c:1889\n"
"F:3:0:UI1:test-checks.c:1890:7 != 7\n"
"P:4:0:UI1:test-checks.c:1891\n"
"F:5:0:UI1:test-checks.c:1892:10 != 10\n"
"E:check_ne_u16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_u16,
"B:step_assert_ge_u16\n"
"P:0:0:UI1:test-checks.c:1898\n"
"F:1:0:UI1:test-checks.c:1899:2 >= 3\n"
"E:step_assert_ge_u16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_u16,
"B:check_ge_u16\n"
"P:0:0:UI1:test-checks.c:1904\n"
"F:1:0:UI1:test-checks.c:1905:2 >= 3\n"
"F:*:0:UI1:test-checks.c:1907:5 >= 6\n"
"P:2:0:UI1:test-checks.c:1908\n"
"F:3:0:UI1:test-checks.c:1909:8 >= 9\n"
"P:4:0:UI1:test-checks.c:1910\n"
"F:5:0:UI1:test-checks.c:1911:11 >= 12\n"
"E:check_ge_u16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_u16,
"B:step_assert_gt_u16\n"
"P:0:0:UI1:test-checks.c:1917\n"
"F:1:0:UI1:test-checks.c:1918:3 > 3\n"
"E:step_assert_gt_u16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_u16,
"B:check_gt_u16\n"
"P:0:0:UI1:test-checks.c:1923\n"
"F:1:0:UI1:test-checks.c:1924:3 > 3\n"
"F:*:0:UI1:test-checks.c:1926:6 > 6\n"
"P:2:0:UI1:test-checks.c:1927\n"
"F:3:0:UI1:test-checks.c:1928:9 > 9\n"
"F:4:0:UI1:test-checks.c:1929:10 > 11\n"
"E:check_gt_u16:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_u16,
"B:step_assert_le_u16\n"
"P:0:0:UI1:test-checks.c:1936\n"
"F:1:0:UI1:test-checks.c:1937:3 <= 2\n"
"E:step_assert_le_u16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_u16,
"B:check_le_u16\n"
"P:0:0:UI1:test-checks.c:1942\n"
"F:1:0:UI1:test-checks.c:1943:3 <= 2\n"
"F:*:0:UI1:test-checks.c:1945:6 <= 5\n"
"P:2:0:UI1:test-checks.c:1946\n"
"F:3:0:UI1:test-checks.c:1947:9 <= 8\n"
"P:4:0:UI1:test-checks.c:1948\n"
"F:5:0:UI1:test-checks.c:1949:12 <= 11\n"
"E:check_le_u16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_u16,
"B:step_assert_lt_u16\n"
"P:0:0:UI1:test-checks.c:1955\n"
"F:1:0:UI1:test-checks.c:1956:3 < 3\n"
"E:step_assert_lt_u16:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_u16,
"B:check_lt_u16\n"
"P:0:0:UI1:test-checks.c:1961\n"
"F:1:0:UI1:test-checks.c:1962:3 < 3\n"
"F:*:0:UI1:test-checks.c:1964:6 < 6\n"
"P:2:0:UI1:test-checks.c:1965\n"
"F:3:0:UI1:test-checks.c:1966:9 < 9\n"
"P:4:0:UI1:test-checks.c:1967\n"
"F:5:0:UI1:test-checks.c:1968:12 < 12\n"
"E:check_lt_u16:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_i32,
"B:step_assert_eq_i32\n"
"P:0:0:UI1:test-checks.c:1974\n"
"F:1:0:UI1:test-checks.c:1975:2 == 3\n"
"E:step_assert_eq_i32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_i32,
"B:check_eq_i32\n"
"P:0:0:UI1:test-checks.c:1980\n"
"F:1:0:UI1:test-checks.c:1981:2 == 3\n"
"F:*:0:UI1:test-checks.c:1983:5 == 6\n"
"P:2:0:UI1:test-checks.c:1984\n"
"F:3:0:UI1:test-checks.c:1985:8 == 9\n"
"P:4:0:UI1:test-checks.c:1986\n"
"F:5:0:UI1:test-checks.c:1987:11 == 12\n"
"E:check_eq_i32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_i32,
"B:step_assert_ne_i32\n"
"P:0:0:UI1:test-checks.c:1993\n"
"F:1:0:UI1:test-checks.c:1994:1 != 1\n"
"E:step_assert_ne_i32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_i32,
"B:check_ne_i32\n"
"P:0:0:UI1:test-checks.c:1999\n"
"F:1:0:UI1:test-checks.c:2000:1 != 1\n"
"F:*:0:UI1:test-checks.c:2002:4 != 4\n"
"P:2:0:UI1:test-checks.c:2003\n"
"F:3:0:UI1:test-checks.c:2004:7 != 7\n"
"P:4:0:UI1:test-checks.c:2005\n"
"F:5:0:UI1:test-checks.c:2006:10 != 10\n"
"E:check_ne_i32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_i32,
"B:step_assert_ge_i32\n"
"P:0:0:UI1:test-checks.c:2012\n"
"F:1:0:UI1:test-checks.c:2013:2 >= 3\n"
"E:step_assert_ge_i32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_i32,
"B:check_ge_i32\n"
"P:0:0:UI1:test-checks.c:2018\n"
"F:1:0:UI1:test-checks.c:2019:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2021:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2022\n"
"F:3:0:UI1:test-checks.c:2023:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2024\n"
"F:5:0:UI1:test-checks.c:2025:11 >= 12\n"
"E:check_ge_i32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_i32,
"B:step_assert_gt_i32\n"
"P:0:0:UI1:test-checks.c:2031\n"
"F:1:0:UI1:test-checks.c:2032:3 > 3\n"
"E:step_assert_gt_i32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_i32,
"B:check_gt_i32\n"
"P:0:0:UI1:test-checks.c:2037\n"
"F:1:0:UI1:test-checks.c:2038:3 > 3\n"
"F:*:0:UI1:test-checks.c:2040:6 > 6\n"
"P:2:0:UI1:test-checks.c:2041\n"
"F:3:0:UI1:test-checks.c:2042:9 > 9\n"
"F:4:0:UI1:test-checks.c:2043:10 > 11\n"
"E:check_gt_i32:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_i32,
"B:step_assert_le_i32\n"
"P:0:0:UI1:test-checks.c:2050\n"
"F:1:0:UI1:test-checks.c:2051:3 <= 2\n"
"E:step_assert_le_i32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_i32,
"B:check_le_i32\n"
"P:0:0:UI1:test-checks.c:2056\n"
"F:1:0:UI1:test-checks.c:2057:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2059:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2060\n"
"F:3:0:UI1:test-checks.c:2061:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2062\n"
"F:5:0:UI1:test-checks.c:2063:12 <= 11\n"
"E:check_le_i32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_i32,
"B:step_assert_lt_i32\n"
"P:0:0:UI1:test-checks.c:2069\n"
"F:1:0:UI1:test-checks.c:2070:3 < 3\n"
"E:step_assert_lt_i32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_i32,
"B:check_lt_i32\n"
"P:0:0:UI1:test-checks.c:2075\n"
"F:1:0:UI1:test-checks.c:2076:3 < 3\n"
"F:*:0:UI1:test-checks.c:2078:6 < 6\n"
"P:2:0:UI1:test-checks.c:2079\n"
"F:3:0:UI1:test-checks.c:2080:9 < 9\n"
"P:4:0:UI1:test-checks.c:2081\n"
"F:5:0:UI1:test-checks.c:2082:12 < 12\n"
"E:check_lt_i32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_u32,
"B:step_assert_eq_u32\n"
"P:0:0:UI1:test-checks.c:2088\n"
"F:1:0:UI1:test-checks.c:2089:2 == 3\n"
"E:step_assert_eq_u32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_u32,
"B:check_eq_u32\n"
"P:0:0:UI1:test-checks.c:2094\n"
"F:1:0:UI1:test-checks.c:2095:2 == 3\n"
"F:*:0:UI1:test-checks.c:2097:5 == 6\n"
"P:2:0:UI1:test-checks.c:2098\n"
"F:3:0:UI1:test-checks.c:2099:8 == 9\n"
"P:4:0:UI1:test-checks.c:2100\n"
"F:5:0:UI1:test-checks.c:2101:11 == 12\n"
"E:check_eq_u32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_u32,
"B:step_assert_ne_u32\n"
"P:0:0:UI1:test-checks.c:2107\n"
"F:1:0:UI1:test-checks.c:2108:1 != 1\n"
"E:step_assert_ne_u32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_u32,
"B:check_ne_u32\n"
"P:0:0:UI1:test-checks.c:2113\n"
"F:1:0:UI1:test-checks.c:2114:1 != 1\n"
"F:*:0:UI1:test-checks.c:2116:4 != 4\n"
"P:2:0:UI1:test-checks.c:2117\n"
"F:3:0:UI1:test-checks.c:2118:7 != 7\n"
"P:4:0:UI1:test-checks.c:2119\n"
"F:5:0:UI1:test-checks.c:2120:10 != 10\n"
"E:check_ne_u32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_u32,
"B:step_assert_ge_u32\n"
"P:0:0:UI1:test-checks.c:2126\n"
"F:1:0:UI1:test-checks.c:2127:2 >= 3\n"
"E:step_assert_ge_u32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_u32,
"B:check_ge_u32\n"
"P:0:0:UI1:test-checks.c:2132\n"
"F:1:0:UI1:test-checks.c:2133:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2135:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2136\n"
"F:3:0:UI1:test-checks.c:2137:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2138\n"
"F:5:0:UI1:test-checks.c:2139:11 >= 12\n"
"E:check_ge_u32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_u32,
"B:step_assert_gt_u32\n"
"P:0:0:UI1:test-checks.c:2145\n"
"F:1:0:UI1:test-checks.c:2146:3 > 3\n"
"E:step_assert_gt_u32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_u32,
"B:check_gt_u32\n"
"P:0:0:UI1:test-checks.c:2151\n"
"F:1:0:UI1:test-checks.c:2152:3 > 3\n"
"F:*:0:UI1:test-checks.c:2154:6 > 6\n"
"P:2:0:UI1:test-checks.c:2155\n"
"F:3:0:UI1:test-checks.c:2156:9 > 9\n"
"F:4:0:UI1:test-checks.c:2157:10 > 11\n"
"E:check_gt_u32:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_u32,
"B:step_assert_le_u32\n"
"P:0:0:UI1:test-checks.c:2164\n"
"F:1:0:UI1:test-checks.c:2165:3 <= 2\n"
"E:step_assert_le_u32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_u32,
"B:check_le_u32\n"
"P:0:0:UI1:test-checks.c:2170\n"
"F:1:0:UI1:test-checks.c:2171:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2173:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2174\n"
"F:3:0:UI1:test-checks.c:2175:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2176\n"
"F:5:0:UI1:test-checks.c:2177:12 <= 11\n"
"E:check_le_u32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_u32,
"B:step_assert_lt_u32\n"
"P:0:0:UI1:test-checks.c:2183\n"
"F:1:0:UI1:test-checks.c:2184:3 < 3\n"
"E:step_assert_lt_u32:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_u32,
"B:check_lt_u32\n"
"P:0:0:UI1:test-checks.c:2189\n"
"F:1:0:UI1:test-checks.c:2190:3 < 3\n"
"F:*:0:UI1:test-checks.c:2192:6 < 6\n"
"P:2:0:UI1:test-checks.c:2193\n"
"F:3:0:UI1:test-checks.c:2194:9 < 9\n"
"P:4:0:UI1:test-checks.c:2195\n"
"F:5:0:UI1:test-checks.c:2196:12 < 12\n"
"E:check_lt_u32:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_i64,
"B:step_assert_eq_i64\n"
"P:0:0:UI1:test-checks.c:2202\n"
"F:1:0:UI1:test-checks.c:2203:2 == 3\n"
"E:step_assert_eq_i64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_i64,
"B:check_eq_i64\n"
"P:0:0:UI1:test-checks.c:2208\n"
"F:1:0:UI1:test-checks.c:2209:2 == 3\n"
"F:*:0:UI1:test-checks.c:2211:5 == 6\n"
"P:2:0:UI1:test-checks.c:2212\n"
"F:3:0:UI1:test-checks.c:2213:8 == 9\n"
"P:4:0:UI1:test-checks.c:2214\n"
"F:5:0:UI1:test-checks.c:2215:11 == 12\n"
"E:check_eq_i64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_i64,
"B:step_assert_ne_i64\n"
"P:0:0:UI1:test-checks.c:2221\n"
"F:1:0:UI1:test-checks.c:2222:1 != 1\n"
"E:step_assert_ne_i64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_i64,
"B:check_ne_i64\n"
"P:0:0:UI1:test-checks.c:2227\n"
"F:1:0:UI1:test-checks.c:2228:1 != 1\n"
"F:*:0:UI1:test-checks.c:2230:4 != 4\n"
"P:2:0:UI1:test-checks.c:2231\n"
"F:3:0:UI1:test-checks.c:2232:7 != 7\n"
"P:4:0:UI1:test-checks.c:2233\n"
"F:5:0:UI1:test-checks.c:2234:10 != 10\n"
"E:check_ne_i64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_i64,
"B:step_assert_ge_i64\n"
"P:0:0:UI1:test-checks.c:2240\n"
"F:1:0:UI1:test-checks.c:2241:2 >= 3\n"
"E:step_assert_ge_i64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_i64,
"B:check_ge_i64\n"
"P:0:0:UI1:test-checks.c:2246\n"
"F:1:0:UI1:test-checks.c:2247:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2249:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2250\n"
"F:3:0:UI1:test-checks.c:2251:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2252\n"
"F:5:0:UI1:test-checks.c:2253:11 >= 12\n"
"E:check_ge_i64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_i64,
"B:step_assert_gt_i64\n"
"P:0:0:UI1:test-checks.c:2259\n"
"F:1:0:UI1:test-checks.c:2260:3 > 3\n"
"E:step_assert_gt_i64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_i64,
"B:check_gt_i64\n"
"P:0:0:UI1:test-checks.c:2265\n"
"F:1:0:UI1:test-checks.c:2266:3 > 3\n"
"F:*:0:UI1:test-checks.c:2268:6 > 6\n"
"P:2:0:UI1:test-checks.c:2269\n"
"F:3:0:UI1:test-checks.c:2270:9 > 9\n"
"F:4:0:UI1:test-checks.c:2271:10 > 11\n"
"E:check_gt_i64:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_i64,
"B:step_assert_le_i64\n"
"P:0:0:UI1:test-checks.c:2278\n"
"F:1:0:UI1:test-checks.c:2279:3 <= 2\n"
"E:step_assert_le_i64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_i64,
"B:check_le_i64\n"
"P:0:0:UI1:test-checks.c:2284\n"
"F:1:0:UI1:test-checks.c:2285:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2287:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2288\n"
"F:3:0:UI1:test-checks.c:2289:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2290\n"
"F:5:0:UI1:test-checks.c:2291:12 <= 11\n"
"E:check_le_i64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_i64,
"B:step_assert_lt_i64\n"
"P:0:0:UI1:test-checks.c:2297\n"
"F:1:0:UI1:test-checks.c:2298:3 < 3\n"
"E:step_assert_lt_i64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_i64,
"B:check_lt_i64\n"
"P:0:0:UI1:test-checks.c:2303\n"
"F:1:0:UI1:test-checks.c:2304:3 < 3\n"
"F:*:0:UI1:test-checks.c:2306:6 < 6\n"
"P:2:0:UI1:test-checks.c:2307\n"
"F:3:0:UI1:test-checks.c:2308:9 < 9\n"
"P:4:0:UI1:test-checks.c:2309\n"
"F:5:0:UI1:test-checks.c:2310:12 < 12\n"
"E:check_lt_i64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_u64,
"B:step_assert_eq_u64\n"
"P:0:0:UI1:test-checks.c:2316\n"
"F:1:0:UI1:test-checks.c:2317:2 == 3\n"
"E:step_assert_eq_u64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_u64,
"B:check_eq_u64\n"
"P:0:0:UI1:test-checks.c:2322\n"
"F:1:0:UI1:test-checks.c:2323:2 == 3\n"
"F:*:0:UI1:test-checks.c:2325:5 == 6\n"
"P:2:0:UI1:test-checks.c:2326\n"
"F:3:0:UI1:test-checks.c:2327:8 == 9\n"
"P:4:0:UI1:test-checks.c:2328\n"
"F:5:0:UI1:test-checks.c:2329:11 == 12\n"
"E:check_eq_u64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_u64,
"B:step_assert_ne_u64\n"
"P:0:0:UI1:test-checks.c:2335\n"
"F:1:0:UI1:test-checks.c:2336:1 != 1\n"
"E:step_assert_ne_u64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_u64,
"B:check_ne_u64\n"
"P:0:0:UI1:test-checks.c:2341\n"
"F:1:0:UI1:test-checks.c:2342:1 != 1\n"
"F:*:0:UI1:test-checks.c:2344:4 != 4\n"
"P:2:0:UI1:test-checks.c:2345\n"
"F:3:0:UI1:test-checks.c:2346:7 != 7\n"
"P:4:0:UI1:test-checks.c:2347\n"
"F:5:0:UI1:test-checks.c:2348:10 != 10\n"
"E:check_ne_u64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_u64,
"B:step_assert_ge_u64\n"
"P:0:0:UI1:test-checks.c:2354\n"
"F:1:0:UI1:test-checks.c:2355:2 >= 3\n"
"E:step_assert_ge_u64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_u64,
"B:check_ge_u64\n"
"P:0:0:UI1:test-checks.c:2360\n"
"F:1:0:UI1:test-checks.c:2361:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2363:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2364\n"
"F:3:0:UI1:test-checks.c:2365:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2366\n"
"F:5:0:UI1:test-checks.c:2367:11 >= 12\n"
"E:check_ge_u64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_u64,
"B:step_assert_gt_u64\n"
"P:0:0:UI1:test-checks.c:2373\n"
"F:1:0:UI1:test-checks.c:2374:3 > 3\n"
"E:step_assert_gt_u64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_u64,
"B:check_gt_u64\n"
"P:0:0:UI1:test-checks.c:2379\n"
"F:1:0:UI1:test-checks.c:2380:3 > 3\n"
"F:*:0:UI1:test-checks.c:2382:6 > 6\n"
"P:2:0:UI1:test-checks.c:2383\n"
"F:3:0:UI1:test-checks.c:2384:9 > 9\n"
"F:4:0:UI1:test-checks.c:2385:10 > 11\n"
"E:check_gt_u64:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_u64,
"B:step_assert_le_u64\n"
"P:0:0:UI1:test-checks.c:2392\n"
"F:1:0:UI1:test-checks.c:2393:3 <= 2\n"
"E:step_assert_le_u64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_u64,
"B:check_le_u64\n"
"P:0:0:UI1:test-checks.c:2398\n"
"F:1:0:UI1:test-checks.c:2399:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2401:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2402\n"
"F:3:0:UI1:test-checks.c:2403:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2404\n"
"F:5:0:UI1:test-checks.c:2405:12 <= 11\n"
"E:check_le_u64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_u64,
"B:step_assert_lt_u64\n"
"P:0:0:UI1:test-checks.c:2411\n"
"F:1:0:UI1:test-checks.c:2412:3 < 3\n"
"E:step_assert_lt_u64:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_u64,
"B:check_lt_u64\n"
"P:0:0:UI1:test-checks.c:2417\n"
"F:1:0:UI1:test-checks.c:2418:3 < 3\n"
"F:*:0:UI1:test-checks.c:2420:6 < 6\n"
"P:2:0:UI1:test-checks.c:2421\n"
"F:3:0:UI1:test-checks.c:2422:9 < 9\n"
"P:4:0:UI1:test-checks.c:2423\n"
"F:5:0:UI1:test-checks.c:2424:12 < 12\n"
"E:check_lt_u64:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_iptr,
"B:step_assert_eq_iptr\n"
"P:0:0:UI1:test-checks.c:2430\n"
"F:1:0:UI1:test-checks.c:2431:2 == 3\n"
"E:step_assert_eq_iptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_iptr,
"B:check_eq_iptr\n"
"P:0:0:UI1:test-checks.c:2436\n"
"F:1:0:UI1:test-checks.c:2437:2 == 3\n"
"F:*:0:UI1:test-checks.c:2439:5 == 6\n"
"P:2:0:UI1:test-checks.c:2440\n"
"F:3:0:UI1:test-checks.c:2441:8 == 9\n"
"P:4:0:UI1:test-checks.c:2442\n"
"F:5:0:UI1:test-checks.c:2443:11 == 12\n"
"E:check_eq_iptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_iptr,
"B:step_assert_ne_iptr\n"
"P:0:0:UI1:test-checks.c:2449\n"
"F:1:0:UI1:test-checks.c:2450:1 != 1\n"
"E:step_assert_ne_iptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_iptr,
"B:check_ne_iptr\n"
"P:0:0:UI1:test-checks.c:2455\n"
"F:1:0:UI1:test-checks.c:2456:1 != 1\n"
"F:*:0:UI1:test-checks.c:2458:4 != 4\n"
"P:2:0:UI1:test-checks.c:2459\n"
"F:3:0:UI1:test-checks.c:2460:7 != 7\n"
"P:4:0:UI1:test-checks.c:2461\n"
"F:5:0:UI1:test-checks.c:2462:10 != 10\n"
"E:check_ne_iptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_iptr,
"B:step_assert_ge_iptr\n"
"P:0:0:UI1:test-checks.c:2468\n"
"F:1:0:UI1:test-checks.c:2469:2 >= 3\n"
"E:step_assert_ge_iptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_iptr,
"B:check_ge_iptr\n"
"P:0:0:UI1:test-checks.c:2474\n"
"F:1:0:UI1:test-checks.c:2475:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2477:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2478\n"
"F:3:0:UI1:test-checks.c:2479:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2480\n"
"F:5:0:UI1:test-checks.c:2481:11 >= 12\n"
"E:check_ge_iptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_iptr,
"B:step_assert_gt_iptr\n"
"P:0:0:UI1:test-checks.c:2487\n"
"F:1:0:UI1:test-checks.c:2488:3 > 3\n"
"E:step_assert_gt_iptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_iptr,
"B:check_gt_iptr\n"
"P:0:0:UI1:test-checks.c:2493\n"
"F:1:0:UI1:test-checks.c:2494:3 > 3\n"
"F:*:0:UI1:test-checks.c:2496:6 > 6\n"
"P:2:0:UI1:test-checks.c:2497\n"
"F:3:0:UI1:test-checks.c:2498:9 > 9\n"
"F:4:0:UI1:test-checks.c:2499:10 > 11\n"
"E:check_gt_iptr:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_iptr,
"B:step_assert_le_iptr\n"
"P:0:0:UI1:test-checks.c:2506\n"
"F:1:0:UI1:test-checks.c:2507:3 <= 2\n"
"E:step_assert_le_iptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_iptr,
"B:check_le_iptr\n"
"P:0:0:UI1:test-checks.c:2512\n"
"F:1:0:UI1:test-checks.c:2513:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2515:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2516\n"
"F:3:0:UI1:test-checks.c:2517:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2518\n"
"F:5:0:UI1:test-checks.c:2519:12 <= 11\n"
"E:check_le_iptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_iptr,
"B:step_assert_lt_iptr\n"
"P:0:0:UI1:test-checks.c:2525\n"
"F:1:0:UI1:test-checks.c:2526:3 < 3\n"
"E:step_assert_lt_iptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_iptr,
"B:check_lt_iptr\n"
"P:0:0:UI1:test-checks.c:2531\n"
"F:1:0:UI1:test-checks.c:2532:3 < 3\n"
"F:*:0:UI1:test-checks.c:2534:6 < 6\n"
"P:2:0:UI1:test-checks.c:2535\n"
"F:3:0:UI1:test-checks.c:2536:9 < 9\n"
"P:4:0:UI1:test-checks.c:2537\n"
"F:5:0:UI1:test-checks.c:2538:12 < 12\n"
"E:check_lt_iptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_uptr,
"B:step_assert_eq_uptr\n"
"P:0:0:UI1:test-checks.c:2544\n"
"F:1:0:UI1:test-checks.c:2545:2 == 3\n"
"E:step_assert_eq_uptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_uptr,
"B:check_eq_uptr\n"
"P:0:0:UI1:test-checks.c:2550\n"
"F:1:0:UI1:test-checks.c:2551:2 == 3\n"
"F:*:0:UI1:test-checks.c:2553:5 == 6\n"
"P:2:0:UI1:test-checks.c:2554\n"
"F:3:0:UI1:test-checks.c:2555:8 == 9\n"
"P:4:0:UI1:test-checks.c:2556\n"
"F:5:0:UI1:test-checks.c:2557:11 == 12\n"
"E:check_eq_uptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_uptr,
"B:step_assert_ne_uptr\n"
"P:0:0:UI1:test-checks.c:2563\n"
"F:1:0:UI1:test-checks.c:2564:1 != 1\n"
"E:step_assert_ne_uptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_uptr,
"B:check_ne_uptr\n"
"P:0:0:UI1:test-checks.c:2569\n"
"F:1:0:UI1:test-checks.c:2570:1 != 1\n"
"F:*:0:UI1:test-checks.c:2572:4 != 4\n"
"P:2:0:UI1:test-checks.c:2573\n"
"F:3:0:UI1:test-checks.c:2574:7 != 7\n"
"P:4:0:UI1:test-checks.c:2575\n"
"F:5:0:UI1:test-checks.c:2576:10 != 10\n"
"E:check_ne_uptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_uptr,
"B:step_assert_ge_uptr\n"
"P:0:0:UI1:test-checks.c:2582\n"
"F:1:0:UI1:test-checks.c:2583:2 >= 3\n"
"E:step_assert_ge_uptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_uptr,
"B:check_ge_uptr\n"
"P:0:0:UI1:test-checks.c:2588\n"
"F:1:0:UI1:test-checks.c:2589:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2591:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2592\n"
"F:3:0:UI1:test-checks.c:2593:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2594\n"
"F:5:0:UI1:test-checks.c:2595:11 >= 12\n"
"E:check_ge_uptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_uptr,
"B:step_assert_gt_uptr\n"
"P:0:0:UI1:test-checks.c:2601\n"
"F:1:0:UI1:test-checks.c:2602:3 > 3\n"
"E:step_assert_gt_uptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_uptr,
"B:check_gt_uptr\n"
"P:0:0:UI1:test-checks.c:2607\n"
"F:1:0:UI1:test-checks.c:2608:3 > 3\n"
"F:*:0:UI1:test-checks.c:2610:6 > 6\n"
"P:2:0:UI1:test-checks.c:2611\n"
"F:3:0:UI1:test-checks.c:2612:9 > 9\n"
"F:4:0:UI1:test-checks.c:2613:10 > 11\n"
"E:check_gt_uptr:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_uptr,
"B:step_assert_le_uptr\n"
"P:0:0:UI1:test-checks.c:2620\n"
"F:1:0:UI1:test-checks.c:2621:3 <= 2\n"
"E:step_assert_le_uptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_uptr,
"B:check_le_uptr\n"
"P:0:0:UI1:test-checks.c:2626\n"
"F:1:0:UI1:test-checks.c:2627:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2629:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2630\n"
"F:3:0:UI1:test-checks.c:2631:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2632\n"
"F:5:0:UI1:test-checks.c:2633:12 <= 11\n"
"E:check_le_uptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_uptr,
"B:step_assert_lt_uptr\n"
"P:0:0:UI1:test-checks.c:2639\n"
"F:1:0:UI1:test-checks.c:2640:3 < 3\n"
"E:step_assert_lt_uptr:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_uptr,
"B:check_lt_uptr\n"
"P:0:0:UI1:test-checks.c:2645\n"
"F:1:0:UI1:test-checks.c:2646:3 < 3\n"
"F:*:0:UI1:test-checks.c:2648:6 < 6\n"
"P:2:0:UI1:test-checks.c:2649\n"
"F:3:0:UI1:test-checks.c:2650:9 < 9\n"
"P:4:0:UI1:test-checks.c:2651\n"
"F:5:0:UI1:test-checks.c:2652:12 < 12\n"
"E:check_lt_uptr:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_ssz,
"B:step_assert_eq_ssz\n"
"P:0:0:UI1:test-checks.c:2658\n"
"F:1:0:UI1:test-checks.c:2659:2 == 3\n"
"E:step_assert_eq_ssz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_ssz,
"B:check_eq_ssz\n"
"P:0:0:UI1:test-checks.c:2664\n"
"F:1:0:UI1:test-checks.c:2665:2 == 3\n"
"F:*:0:UI1:test-checks.c:2667:5 == 6\n"
"P:2:0:UI1:test-checks.c:2668\n"
"F:3:0:UI1:test-checks.c:2669:8 == 9\n"
"P:4:0:UI1:test-checks.c:2670\n"
"F:5:0:UI1:test-checks.c:2671:11 == 12\n"
"E:check_eq_ssz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_ssz,
"B:step_assert_ne_ssz\n"
"P:0:0:UI1:test-checks.c:2677\n"
"F:1:0:UI1:test-checks.c:2678:1 != 1\n"
"E:step_assert_ne_ssz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_ssz,
"B:check_ne_ssz\n"
"P:0:0:UI1:test-checks.c:2683\n"
"F:1:0:UI1:test-checks.c:2684:1 != 1\n"
"F:*:0:UI1:test-checks.c:2686:4 != 4\n"
"P:2:0:UI1:test-checks.c:2687\n"
"F:3:0:UI1:test-checks.c:2688:7 != 7\n"
"P:4:0:UI1:test-checks.c:2689\n"
"F:5:0:UI1:test-checks.c:2690:10 != 10\n"
"E:check_ne_ssz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_ssz,
"B:step_assert_ge_ssz\n"
"P:0:0:UI1:test-checks.c:2696\n"
"F:1:0:UI1:test-checks.c:2697:2 >= 3\n"
"E:step_assert_ge_ssz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_ssz,
"B:check_ge_ssz\n"
"P:0:0:UI1:test-checks.c:2702\n"
"F:1:0:UI1:test-checks.c:2703:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2705:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2706\n"
"F:3:0:UI1:test-checks.c:2707:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2708\n"
"F:5:0:UI1:test-checks.c:2709:11 >= 12\n"
"E:check_ge_ssz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_ssz,
"B:step_assert_gt_ssz\n"
"P:0:0:UI1:test-checks.c:2715\n"
"F:1:0:UI1:test-checks.c:2716:3 > 3\n"
"E:step_assert_gt_ssz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_ssz,
"B:check_gt_ssz\n"
"P:0:0:UI1:test-checks.c:2721\n"
"F:1:0:UI1:test-checks.c:2722:3 > 3\n"
"F:*:0:UI1:test-checks.c:2724:6 > 6\n"
"P:2:0:UI1:test-checks.c:2725\n"
"F:3:0:UI1:test-checks.c:2726:9 > 9\n"
"F:4:0:UI1:test-checks.c:2727:10 > 11\n"
"E:check_gt_ssz:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_ssz,
"B:step_assert_le_ssz\n"
"P:0:0:UI1:test-checks.c:2734\n"
"F:1:0:UI1:test-checks.c:2735:3 <= 2\n"
"E:step_assert_le_ssz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_ssz,
"B:check_le_ssz\n"
"P:0:0:UI1:test-checks.c:2740\n"
"F:1:0:UI1:test-checks.c:2741:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2743:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2744\n"
"F:3:0:UI1:test-checks.c:2745:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2746\n"
"F:5:0:UI1:test-checks.c:2747:12 <= 11\n"
"E:check_le_ssz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_ssz,
"B:step_assert_lt_ssz\n"
"P:0:0:UI1:test-checks.c:2753\n"
"F:1:0:UI1:test-checks.c:2754:3 < 3\n"
"E:step_assert_lt_ssz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_ssz,
"B:check_lt_ssz\n"
"P:0:0:UI1:test-checks.c:2759\n"
"F:1:0:UI1:test-checks.c:2760:3 < 3\n"
"F:*:0:UI1:test-checks.c:2762:6 < 6\n"
"P:2:0:UI1:test-checks.c:2763\n"
"F:3:0:UI1:test-checks.c:2764:9 < 9\n"
"P:4:0:UI1:test-checks.c:2765\n"
"F:5:0:UI1:test-checks.c:2766:12 < 12\n"
"E:check_lt_ssz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_eq_sz,
"B:step_assert_eq_sz\n"
"P:0:0:UI1:test-checks.c:2772\n"
"F:1:0:UI1:test-checks.c:2773:2 == 3\n"
"E:step_assert_eq_sz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_eq_sz,
"B:check_eq_sz\n"
"P:0:0:UI1:test-checks.c:2778\n"
"F:1:0:UI1:test-checks.c:2779:2 == 3\n"
"F:*:0:UI1:test-checks.c:2781:5 == 6\n"
"P:2:0:UI1:test-checks.c:2782\n"
"F:3:0:UI1:test-checks.c:2783:8 == 9\n"
"P:4:0:UI1:test-checks.c:2784\n"
"F:5:0:UI1:test-checks.c:2785:11 == 12\n"
"E:check_eq_sz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ne_sz,
"B:step_assert_ne_sz\n"
"P:0:0:UI1:test-checks.c:2791\n"
"F:1:0:UI1:test-checks.c:2792:1 != 1\n"
"E:step_assert_ne_sz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ne_sz,
"B:check_ne_sz\n"
"P:0:0:UI1:test-checks.c:2797\n"
"F:1:0:UI1:test-checks.c:2798:1 != 1\n"
"F:*:0:UI1:test-checks.c:2800:4 != 4\n"
"P:2:0:UI1:test-checks.c:2801\n"
"F:3:0:UI1:test-checks.c:2802:7 != 7\n"
"P:4:0:UI1:test-checks.c:2803\n"
"F:5:0:UI1:test-checks.c:2804:10 != 10\n"
"E:check_ne_sz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_ge_sz,
"B:step_assert_ge_sz\n"
"P:0:0:UI1:test-checks.c:2810\n"
"F:1:0:UI1:test-checks.c:2811:2 >= 3\n"
"E:step_assert_ge_sz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_ge_sz,
"B:check_ge_sz\n"
"P:0:0:UI1:test-checks.c:2816\n"
"F:1:0:UI1:test-checks.c:2817:2 >= 3\n"
"F:*:0:UI1:test-checks.c:2819:5 >= 6\n"
"P:2:0:UI1:test-checks.c:2820\n"
"F:3:0:UI1:test-checks.c:2821:8 >= 9\n"
"P:4:0:UI1:test-checks.c:2822\n"
"F:5:0:UI1:test-checks.c:2823:11 >= 12\n"
"E:check_ge_sz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_gt_sz,
"B:step_assert_gt_sz\n"
"P:0:0:UI1:test-checks.c:2829\n"
"F:1:0:UI1:test-checks.c:2830:3 > 3\n"
"E:step_assert_gt_sz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_gt_sz,
"B:check_gt_sz\n"
"P:0:0:UI1:test-checks.c:2835\n"
"F:1:0:UI1:test-checks.c:2836:3 > 3\n"
"F:*:0:UI1:test-checks.c:2838:6 > 6\n"
"P:2:0:UI1:test-checks.c:2839\n"
"F:3:0:UI1:test-checks.c:2840:9 > 9\n"
"F:4:0:UI1:test-checks.c:2841:10 > 11\n"
"E:check_gt_sz:N:5:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_le_sz,
"B:step_assert_le_sz\n"
"P:0:0:UI1:test-checks.c:2848\n"
"F:1:0:UI1:test-checks.c:2849:3 <= 2\n"
"E:step_assert_le_sz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_le_sz,
"B:check_le_sz\n"
"P:0:0:UI1:test-checks.c:2854\n"
"F:1:0:UI1:test-checks.c:2855:3 <= 2\n"
"F:*:0:UI1:test-checks.c:2857:6 <= 5\n"
"P:2:0:UI1:test-checks.c:2858\n"
"F:3:0:UI1:test-checks.c:2859:9 <= 8\n"
"P:4:0:UI1:test-checks.c:2860\n"
"F:5:0:UI1:test-checks.c:2861:12 <= 11\n"
"E:check_le_sz:N:6:F:4:D:0.001000\n");

T_TEST_OUTPUT(step_assert_lt_sz,
"B:step_assert_lt_sz\n"
"P:0:0:UI1:test-checks.c:2867\n"
"F:1:0:UI1:test-checks.c:2868:3 < 3\n"
"E:step_assert_lt_sz:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(check_lt_sz,
"B:check_lt_sz\n"
"P:0:0:UI1:test-checks.c:2873\n"
"F:1:0:UI1:test-checks.c:2874:3 < 3\n"
"F:*:0:UI1:test-checks.c:2876:6 < 6\n"
"P:2:0:UI1:test-checks.c:2877\n"
"F:3:0:UI1:test-checks.c:2878:9 < 9\n"
"P:4:0:UI1:test-checks.c:2879\n"
"F:5:0:UI1:test-checks.c:2880:12 < 12\n"
"E:check_lt_sz:N:6:F:4:D:0.001000\n");

/*
 * The license is at the end of the file to be able to use the test code and
 * output in examples in the documentation.  This is also the reason for the
 * dual licensing.  The license for RTEMS documentation is CC-BY-SA-4.0.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause OR CC-BY-SA-4.0
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International Public License as
 * published by Creative Commons, PO Box 1866, Mountain View, CA 94042
 * (https://creativecommons.org/licenses/by-sa/4.0/legalcode).
 */
