/*
 *  Just enough to make newlib return an error.
 *
 *  $Id$
 */

int getdents(
  int   fd,
  void *buf,
  int   len
)
{
  return -1;
}
