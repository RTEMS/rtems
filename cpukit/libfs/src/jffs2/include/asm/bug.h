#ifndef __ASM_BUG_H__
#define __ASM_BUG_H__

#include <assert.h>

#define BUG() assert(0)

#define WARN_ON(condition) do { } while (0)

#endif /* __ASM_BUG_H__ */
