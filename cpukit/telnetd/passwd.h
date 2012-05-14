/* Define a default password for telnetd here.
 * NOTES:
 *  - this can be overridden at run-time by setting
 *    the "TELNETD_PASSWD" environment variable.
 *    As soon as that variable is set, the new password
 *    is effective - no need to restart telnetd.
 *  - this must be set to an _encrypted_ password, NOT
 *    the cleartext. Use the 'genpw' utility to generate
 *    a password string:
 *
 *    1) Compile 'genpw.c' for the HOST, i.e.
 *         cc -o genpw genpw.c -lcrypt
 *    1) delete an old password definition from this file.
 *    2) run './genpw >> passwd.h'. This will append
 *       a new definition to this file.
 *
 *  - if no password is defined here, no authentication
 *    is needed, i.e. telnet is open to the world.
 *
 *    T. Straumann <strauman@slac.stanford.edu>
 */

/* #undef TELNETD_DEFAULT_PASSWD */
/* Default password: 'rtems' */
#define TELNETD_DEFAULT_PASSWD "tduDcyLX12owo"
