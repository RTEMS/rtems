#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static struct passwd pw_passwd;  /* password structure */
static FILE *passwd_fp;

static char logname[8];
static char password[1024];
static char comment[1024];
static char gecos[1024];
static char dir[1024];
static char shell[1024];

int getpwnam_r(
  const char     *name,
  struct passwd  *pwd,
  char           *buffer,
  size_t          bufsize,
  struct passwd **result
)
{
  FILE *fp;

  if ((fp = fopen ("/etc/passwd", "r")) == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (fgets (buffer, bufsize, fp)) {
    sscanf (buffer, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:]:%s\n",
        logname, password, &pwd->pw_uid,
        &pwd->pw_gid, comment, gecos,
        dir, shell);
    pwd->pw_name = logname;
    pwd->pw_passwd = password;
    pwd->pw_comment = comment;
    pwd->pw_gecos = gecos;
    pwd->pw_dir = dir;
    pwd->pw_shell = shell;

    if (!strcmp (logname, name)) {
      fclose (fp);
      *result = pwd;
      return 0;
    }
  }
  fclose (fp);
  errno = EINVAL;
  return -1;
}

struct passwd *getpwnam(
  const char *name
)
{
  char   buf[1024];
  struct passwd *p;

  if ( getpwnam_r( name, &pw_passwd, buf, 1024, &p ) )
    return NULL;

  return p;
}

int getpwuid_r(
  uid_t           uid,
  struct passwd  *pwd,
  char           *buffer,
  size_t          bufsize,
  struct passwd **result
)
{
  FILE *fp;

  if ((fp = fopen ("/etc/passwd", "r")) == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (fgets (buffer, bufsize, fp)) {
    sscanf (buffer, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:]:%s\n",
     logname, password, &pw_passwd.pw_uid,
     &pw_passwd.pw_gid, comment, gecos,
     dir, shell);
    pwd->pw_name = logname;
    pwd->pw_passwd = password;
    pwd->pw_comment = comment;
    pwd->pw_gecos = gecos;
    pwd->pw_dir = dir;
    pwd->pw_shell = shell;

    if (uid == pwd->pw_uid) {
      fclose (fp);
      *result = pwd;
      return 0;
    }
  }
  fclose (fp);
  errno = EINVAL;
  return -1;
}

struct passwd *getpwuid(
  uid_t uid
)
{
  char   buf[1024];
  struct passwd *p;

  if ( getpwuid_r( uid, &pw_passwd, buf, 1024, &p ) )
    return NULL;

  return p;
}

struct passwd *getpwent()
{
  char buf[1024];

  if (passwd_fp == NULL)
    return NULL;

  if (fgets (buf, sizeof (buf), passwd_fp) == NULL)
    return NULL;

  sscanf (buf, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:]:%s\n",
    logname, password, &pw_passwd.pw_uid,
    &pw_passwd.pw_gid, comment, gecos,
    dir, shell);
  pw_passwd.pw_name = logname;
  pw_passwd.pw_passwd = password;
  pw_passwd.pw_comment = comment;
  pw_passwd.pw_gecos = gecos;
  pw_passwd.pw_dir = dir;
  pw_passwd.pw_shell = shell;

  return &pw_passwd;
}

void setpwent( void )
{
  if (passwd_fp != NULL)
    fclose (passwd_fp);

  passwd_fp = fopen ("/etc/passwd", "r");
}

void endpwent( void )
{
  if (passwd_fp != NULL)
    fclose (passwd_fp);
}
