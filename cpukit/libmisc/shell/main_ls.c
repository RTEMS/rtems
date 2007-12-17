/*
 *  LS Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_ls(int argc, char *argv[])
{
  char           *fname;
  DIR            *dirp;
  struct dirent  *dp;
  struct stat     stat_buf;
  struct passwd  *pwd;
  struct group   *grp;
  char           *user;
  char           *group;
  char            sbuf[256];
  char            nbuf[1024];
  int             n;
  int             size;

  fname = ".";
  if (argc>1)
    fname = argv[1];

  if ((dirp = opendir(fname)) == NULL) {
    fprintf(stdout,"%s: No such file or directory.\n", fname);
    return errno;
  }
  n = 0;
  size = 0;
  while ((dp = readdir(dirp)) != NULL) {
    strcpy(nbuf,fname);
    if (nbuf[strlen(nbuf)-1]!='/')
      strcat(nbuf,"/");
    strcat(nbuf,dp->d_name); /* always the fullpathname. Avoid ftpd problem.*/
    if (stat(nbuf, &stat_buf) == 0) { /* AWFUL buts works...*/
      strftime(sbuf,sizeof(sbuf)-1,"%b %d %H:%M",gmtime(&stat_buf.st_mtime));
      pwd = getpwuid(stat_buf.st_uid);
      user = pwd?pwd->pw_name:"nouser";
      grp = getgrgid(stat_buf.st_gid);
      group = grp?grp->gr_name:"nogrp";
      fprintf(stdout,"%c%c%c%c%c%c%c%c%c%c %3d %6.6s %6.6s %11d %s %s%c\n",
              (S_ISLNK(stat_buf.st_mode)?('l'):
              (S_ISDIR(stat_buf.st_mode)?('d'):('-'))),
              (stat_buf.st_mode & S_IRUSR)?('r'):('-'),
              (stat_buf.st_mode & S_IWUSR)?('w'):('-'),
              (stat_buf.st_mode & S_IXUSR)?('x'):('-'),
              (stat_buf.st_mode & S_IRGRP)?('r'):('-'),
              (stat_buf.st_mode & S_IWGRP)?('w'):('-'),
              (stat_buf.st_mode & S_IXGRP)?('x'):('-'),
              (stat_buf.st_mode & S_IROTH)?('r'):('-'),
              (stat_buf.st_mode & S_IWOTH)?('w'):('-'),
              (stat_buf.st_mode & S_IXOTH)?('x'):('-'),
              (int)stat_buf.st_nlink,
              user,group,
              (int)stat_buf.st_size,
              sbuf,
              dp->d_name,
              S_ISDIR(stat_buf.st_mode)?'/':' ');
      n++;
      size += stat_buf.st_size;
    }
  }
  fprintf(stdout,"%d files %d bytes occupied\n",n,size);
  closedir(dirp);
  return 0;
}

rtems_shell_cmd_t rtems_shell_LS_Command = {
  "ls",                                         /* name */
  "ls [dir]     # list files in the directory", /* usage */
  "files",                                      /* topic */
  rtems_shell_main_ls,                          /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
