/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2003-2007
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <crypt.h>
#include <stdio.h>
#include <unistd.h>

static void
usage(char *nm)
{
  fprintf(stderr,"Usage: %s [-h] [-s salt] cleartext_password\n", nm);
}

int
main(int argc, char **argv)
{
int ch;
char *salt="td";
  while ( (ch=getopt(argc, argv, "hs:")) >=0 ) {
    switch (ch) {
      default:  fprintf(stderr,"Unknown Option '%c'\n",ch);
      case 'h': usage(argv[0]);
      return 0;
      case 's': salt=optarg;
      break;
    }
  }
  if ( optind >= argc ) {
    usage(argv[0]);
    return 1;
  }
  printf("#define TELNETD_DEFAULT_PASSWD \"%s\"\n",crypt(argv[optind],salt));
}
