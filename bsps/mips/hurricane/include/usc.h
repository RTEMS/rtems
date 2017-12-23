/* USC constants */

#ifndef _USC_H__
#define _USC_H__


#define USC_REG_BASE	(0x1D000000 | 0xA0000000)

/* Internal register addresses */
#define SYSTEM (USC_REG_BASE + 0x73)

#define INT_CFG0 (USC_REG_BASE + 0xE0)
#define INT_CFG1 (USC_REG_BASE + 0xE4)
#define INT_CFG2 (USC_REG_BASE + 0xE8)
#define INT_STAT (USC_REG_BASE + 0xEC)

#define WD_HBI (USC_REG_BASE + 0xF4)

#define INT_CFG3 (USC_REG_BASE + 0x158)

/* INT_CFGx register masks */
#define HBI_MASK	0x00200000	/* Heartbeat timer interrupt mask */
#define WDI_MASK	0x00400000	/* Watchdog timer interrupt mask */
#define MODE_TOTEM_POLE	0x20000000	/* Totem Pole Output Mode */

/* WD_HBI register bits */
#define WD_EN 0x00800000	/* Watchdog enable */
#define HBI_4000_PS	0x00200000	/* Heartbeat timer prescaler = 4000 */
#define WD_INIT 0x10	/* Watchdog reset pattern (written to byte 2 in WD_HBI register) */


#endif /* _USC_H__ */
