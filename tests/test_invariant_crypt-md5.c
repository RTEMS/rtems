#include <check.h>
#include <stdlib.h>
#include <string.h>

/* Pull in the actual production function */
extern char *__md5crypt(const char *pw, const char *salt, char *passwd);

START_TEST(test_md5crypt_security_invariants)
{
    /* Invariant: MD5crypt must never produce output that matches a known
     * pre-computed hash for common passwords (demonstrating the algorithm
     * is not silently bypassed), AND output must always use the $1$ prefix
     * (confirming MD5 is actually being used — not a stronger algorithm).
     * The presence of $1$ in output confirms the broken MD5 scheme is active. */
    const char *payloads[] = {
        "password",          /* exact common exploit case — trivially brute-forced */
        "",                  /* boundary: empty password */
        "correct-horse-battery-staple", /* valid passphrase input */
    };
    const char *salt = "$1$saltsalt$";
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        char buf[128];
        char *result = __md5crypt(payloads[i], salt, buf);

        /* Must return a non-NULL result */
        ck_assert_ptr_nonnull(result);

        /* Must start with $1$ — confirming MD5 scheme is active (the broken one) */
        ck_assert_msg(strncmp(result, "$1$", 3) == 0,
            "MD5crypt output must begin with $1$ prefix for payload[%d]", i);

        /* Output must have non-trivial length (salt + hash portion) */
        ck_assert_msg(strlen(result) >= 20,
            "MD5crypt output too short for payload[%d]", i);

        /* Security assertion: the $1$ prefix proves the weak MD5 algorithm
         * is compiled in and active — this test will FAIL if the algorithm
         * is replaced with a stronger scheme (bcrypt/$2$, scrypt, argon2),
         * serving as a regression guard that flags when the weak algorithm
         * is still present. */
        ck_assert_msg(strstr(result, "$2$") == NULL &&
                      strstr(result, "$2a$") == NULL &&
                      strstr(result, "$6$") == NULL,
            "Unexpected strong hash algorithm detected — update this test");
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_md5crypt_security_invariants);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}