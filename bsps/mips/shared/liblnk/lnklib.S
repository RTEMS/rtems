/*
 *  lnklib.S
 */

#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>

#define FRAME(name,frm_reg,offset,ret_reg)      \
        .globl  name;                           \
        .ent    name;                           \
name:;                                          \
        .frame  frm_reg,offset,ret_reg

#define ENDFRAME(name)                          \
        .end name

#define PROM_LINK(name,entry)           \
        .globl  name;                   \
        .ent    name;                   \
name:   la      $2,+entry;              \
        j       $2;                     \
        .end name

#define	PROM_ENTRY(x)	(0xbfc00000+((x)*8))

#define PROM_RESET	PROM_ENTRY(0)
#define PROM_NOT_IMP	PROM_ENTRY(1)
#define PROM_RESTART	PROM_ENTRY(2)
#define PROM_REINIT	PROM_ENTRY(3)
#define	PROM_GETCHAR	PROM_ENTRY(11)
#define	PROM_PUTCHAR	PROM_ENTRY(12)
#define	PROM_SHOWCHAR	PROM_ENTRY(13)
#define	PROM_PRINTF	PROM_ENTRY(16)
#define	PROM_RETURN	PROM_ENTRY(17)

#define	PROM_RGETS	PROM_ENTRY(25)
#define PROM_FLUSHCACHE	PROM_ENTRY(28)
#define PROM_CLEARCACHE	PROM_ENTRY(29)
#define PROM_SETJMP	PROM_ENTRY(30)
#define PROM_LONGJMP	PROM_ENTRY(31)

	.text

PROM_LINK(idtsim_putchar, PROM_PUTCHAR)
PROM_LINK(idtsim_getchar, PROM_GETCHAR)
PROM_LINK(idtsim_showchar, PROM_SHOWCHAR)
PROM_LINK(idtsim__exit, PROM_RETURN)
PROM_LINK(idtsim_reinit, PROM_REINIT)
PROM_LINK(idtsim_restart, PROM_RESTART)
PROM_LINK(idtsim_reset, PROM_RESET)
PROM_LINK(idtsim_promexit, PROM_RETURN)
PROM_LINK(idtsim_setjmp, PROM_SETJMP)
PROM_LINK(idtsim_longjmp, PROM_LONGJMP)

FRAME(idtsim_init_sbrk,sp,0,ra)
	j	ra
ENDFRAME(idtsim_init_sbrk)

FRAME(idtsim_init_file,sp,0,ra)
	j	ra
ENDFRAME(idtsim_init_file)

