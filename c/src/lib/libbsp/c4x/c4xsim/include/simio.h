/*
 *  C4x simulator IO interface routines based on code provided
 *  by Herman ten Brugge <Haj.Ten.Brugge@net.HCC.nl>
 *
 *  $Id$
 */

#ifndef __C4X_SIMIO_IFACE_h
#define __C4X_SIMIO_IFACE_h

#include <stdio.h>

void sim_exit(void);
int sim_open(const char *path, int flags, int fno);
int sim_close(int fno);
int sim_read(int fno, char *buf, unsigned count);
int sim_write(int fno, const char *buf, unsigned count);
fpos_t sim_lseek(int fno, fpos_t offset, int origin);
int sim_unlink(const char *path);
int sim_rename(const char *old, const char *new);
char *sim_getenv(const char *str);
int sim_gettime(void);
int sim_getclock(void);

#endif
