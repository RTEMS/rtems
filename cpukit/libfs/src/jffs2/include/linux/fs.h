#ifndef __LINUX_FS_H__
#define __LINUX_FS_H__

#include <linux/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define DT_UNKNOWN	0
#define DT_DIR		4
#define DT_REG		8
#define DT_LNK		10

#define ATTR_MODE	(1U << 0)
#define ATTR_UID	(1U << 1)
#define ATTR_GID	(1U << 2)
#define ATTR_SIZE	(1U << 3)
#define ATTR_ATIME	(1U << 4)
#define ATTR_MTIME	(1U << 5)
#define ATTR_CTIME	(1U << 6)

struct iattr {
	unsigned int	ia_valid;
	mode_t		ia_mode;
	uid_t		ia_uid;
	gid_t		ia_gid;
	off_t		ia_size;
	time_t		ia_atime;
	time_t		ia_mtime;
	time_t		ia_ctime;
};

#endif /* __LINUX_FS_H__ */
