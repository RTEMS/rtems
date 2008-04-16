#ifndef __iosupp_h__
#define __iosupp_h__

#include <reent.h>

enum	{
	STD_IN,
	STD_OUT,
	STD_ERR,
	STD_MAX = 16
};

#define _SHIFTL(v, s, w)	\
    ((unsigned int) (((unsigned int)(v) & ((0x01 << (w)) - 1)) << (s)))
#define _SHIFTR(v, s, w)	\
    ((unsigned int)(((unsigned int)(v) >> (s)) & ((0x01 << (w)) - 1)))

typedef struct {
	const char *name;
	int (*open_r)(struct _reent *r,const char *path,int flags,int mode);
	int (*close_r)(struct _reent *r,int fd);
	int (*write_r)(struct _reent *r,int fd,const char *ptr,int len);
	int (*read_r)(struct _reent *r,int fd,char *ptr,int len);
	int (*seek_r)(struct _reent *r,int fd,int pos,int dir);
	int (*stat_r)(struct _reent *r,int fd,struct stat *st);
} devoptab_t;

extern const devoptab_t *devoptab_list[];

int AddDevice( const devoptab_t* device);

#endif
