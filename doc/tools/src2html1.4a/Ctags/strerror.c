char *strerror(i)
 int i;
 {
  extern char *sys_errlist[];
  return sys_errlist[i];
 }
