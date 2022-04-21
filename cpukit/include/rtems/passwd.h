/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Default password for telnetd
 */

/*
 * COPYRIGHT (C) 2007 On-Line Applications Research Corporation (OAR).
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
 */

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
