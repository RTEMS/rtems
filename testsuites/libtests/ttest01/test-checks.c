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
