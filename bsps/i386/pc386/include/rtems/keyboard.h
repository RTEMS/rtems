/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Keyboard definitions.
 */

/*
 *  Submitted by: Rosimildo da Silva:  rdasilva@connecttel.com
 */

#ifndef __RTEMS_KEYBOARD_H
#define __RTEMS_KEYBOARD_H

#include <rtems/kd.h>

#define KG_SHIFT	0
#define KG_CTRL		2
#define KG_ALT		3
#define KG_ALTGR	1
#define KG_SHIFTL	4
#define KG_SHIFTR	5
#define KG_CTRLL	6
#define KG_CTRLR	7
#define KG_CAPSSHIFT	8

#define NR_SHIFT	9

#define NR_KEYS		   128
#define MAX_NR_KEYMAPS	256
/* This means 64Kb if all keymaps are allocated. Only the superuser
	may increase the number of keymaps beyond MAX_NR_OF_USER_KEYMAPS. */
#define MAX_NR_OF_USER_KEYMAPS 256 	/* should be at least 7 */

extern const int NR_TYPES;
extern unsigned short *key_maps[MAX_NR_KEYMAPS];
extern unsigned short plain_map[NR_KEYS];
extern unsigned char keyboard_type;

#define MAX_NR_FUNC	256	/* max nr of strings assigned to keys */
#define MAX_NR_CONSOLES 1

extern char *func_table[MAX_NR_FUNC];

#define KT_LATIN	0	/* we depend on this being zero */
#define KT_LETTER	11	/* symbol that can be acted upon by CapsLock */
#define KT_FN		1
#define KT_SPEC		2
#define KT_PAD		3
#define KT_DEAD		4
#define KT_CONS		5
#define KT_CUR		6
#define KT_SHIFT	7
#define KT_META		8
#define KT_ASCII	9
#define KT_LOCK		10
#define KT_SLOCK	12

#define K(t,v)		(((t)<<8)|(v))
#define KTYP(x)		((x) >> 8)
#define KVAL(x)		((x) & 0xff)

#define K_F1		K(KT_FN,0)
#define K_F2		K(KT_FN,1)
#define K_F3		K(KT_FN,2)
#define K_F4		K(KT_FN,3)
#define K_F5		K(KT_FN,4)
#define K_F6		K(KT_FN,5)
#define K_F7		K(KT_FN,6)
#define K_F8		K(KT_FN,7)
#define K_F9		K(KT_FN,8)
#define K_F10		K(KT_FN,9)
#define K_F11		K(KT_FN,10)
#define K_F12		K(KT_FN,11)
#define K_F13		K(KT_FN,12)
#define K_F14		K(KT_FN,13)
#define K_F15		K(KT_FN,14)
#define K_F16		K(KT_FN,15)
#define K_F17		K(KT_FN,16)
#define K_F18		K(KT_FN,17)
#define K_F19		K(KT_FN,18)
#define K_F20		K(KT_FN,19)
#define K_FIND		K(KT_FN,20)
#define K_INSERT	K(KT_FN,21)
#define K_REMOVE	K(KT_FN,22)
#define K_SELECT	K(KT_FN,23)
#define K_PGUP		K(KT_FN,24) /* PGUP is a synonym for PRIOR */
#define K_PGDN		K(KT_FN,25) /* PGDN is a synonym for NEXT */
#define K_MACRO	 	K(KT_FN,26)
#define K_HELP		K(KT_FN,27)
#define K_DO		K(KT_FN,28)
#define K_PAUSE	 	K(KT_FN,29)
#define K_F21		K(KT_FN,30)
#define K_F22		K(KT_FN,31)
#define K_F23		K(KT_FN,32)
#define K_F24		K(KT_FN,33)
#define K_F25		K(KT_FN,34)
#define K_F26		K(KT_FN,35)
#define K_F27		K(KT_FN,36)
#define K_F28		K(KT_FN,37)
#define K_F29		K(KT_FN,38)
#define K_F30		K(KT_FN,39)
#define K_F31		K(KT_FN,40)
#define K_F32		K(KT_FN,41)
#define K_F33		K(KT_FN,42)
#define K_F34		K(KT_FN,43)
#define K_F35		K(KT_FN,44)
#define K_F36		K(KT_FN,45)
#define K_F37		K(KT_FN,46)
#define K_F38		K(KT_FN,47)
#define K_F39		K(KT_FN,48)
#define K_F40		K(KT_FN,49)
#define K_F41		K(KT_FN,50)
#define K_F42		K(KT_FN,51)
#define K_F43		K(KT_FN,52)
#define K_F44		K(KT_FN,53)
#define K_F45		K(KT_FN,54)
#define K_F46		K(KT_FN,55)
#define K_F47		K(KT_FN,56)
#define K_F48		K(KT_FN,57)
#define K_F49		K(KT_FN,58)
#define K_F50		K(KT_FN,59)
#define K_F51		K(KT_FN,60)
#define K_F52		K(KT_FN,61)
#define K_F53		K(KT_FN,62)
#define K_F54		K(KT_FN,63)
#define K_F55		K(KT_FN,64)
#define K_F56		K(KT_FN,65)
#define K_F57		K(KT_FN,66)
#define K_F58		K(KT_FN,67)
#define K_F59		K(KT_FN,68)
#define K_F60		K(KT_FN,69)
#define K_F61		K(KT_FN,70)
#define K_F62		K(KT_FN,71)
#define K_F63		K(KT_FN,72)
#define K_F64		K(KT_FN,73)
#define K_F65		K(KT_FN,74)
#define K_F66		K(KT_FN,75)
#define K_F67		K(KT_FN,76)
#define K_F68		K(KT_FN,77)
#define K_F69		K(KT_FN,78)
#define K_F70		K(KT_FN,79)
#define K_F71		K(KT_FN,80)
#define K_F72		K(KT_FN,81)
#define K_F73		K(KT_FN,82)
#define K_F74		K(KT_FN,83)
#define K_F75		K(KT_FN,84)
#define K_F76		K(KT_FN,85)
#define K_F77		K(KT_FN,86)
#define K_F78		K(KT_FN,87)
#define K_F79		K(KT_FN,88)
#define K_F80		K(KT_FN,89)
#define K_F81		K(KT_FN,90)
#define K_F82		K(KT_FN,91)
#define K_F83		K(KT_FN,92)
#define K_F84		K(KT_FN,93)
#define K_F85		K(KT_FN,94)
#define K_F86		K(KT_FN,95)
#define K_F87		K(KT_FN,96)
#define K_F88		K(KT_FN,97)
#define K_F89		K(KT_FN,98)
#define K_F90		K(KT_FN,99)
#define K_F91		K(KT_FN,100)
#define K_F92		K(KT_FN,101)
#define K_F93		K(KT_FN,102)
#define K_F94		K(KT_FN,103)
#define K_F95		K(KT_FN,104)
#define K_F96		K(KT_FN,105)
#define K_F97		K(KT_FN,106)
#define K_F98		K(KT_FN,107)
#define K_F99		K(KT_FN,108)
#define K_F100		K(KT_FN,109)
#define K_F101		K(KT_FN,110)
#define K_F102		K(KT_FN,111)
#define K_F103		K(KT_FN,112)
#define K_F104		K(KT_FN,113)
#define K_F105		K(KT_FN,114)
#define K_F106		K(KT_FN,115)
#define K_F107		K(KT_FN,116)
#define K_F108		K(KT_FN,117)
#define K_F109		K(KT_FN,118)
#define K_F110		K(KT_FN,119)
#define K_F111		K(KT_FN,120)
#define K_F112		K(KT_FN,121)
#define K_F113		K(KT_FN,122)
#define K_F114		K(KT_FN,123)
#define K_F115		K(KT_FN,124)
#define K_F116		K(KT_FN,125)
#define K_F117		K(KT_FN,126)
#define K_F118		K(KT_FN,127)
#define K_F119		K(KT_FN,128)
#define K_F120		K(KT_FN,129)
#define K_F121		K(KT_FN,130)
#define K_F122		K(KT_FN,131)
#define K_F123		K(KT_FN,132)
#define K_F124		K(KT_FN,133)
#define K_F125		K(KT_FN,134)
#define K_F126		K(KT_FN,135)
#define K_F127		K(KT_FN,136)
#define K_F128		K(KT_FN,137)
#define K_F129		K(KT_FN,138)
#define K_F130		K(KT_FN,139)
#define K_F131		K(KT_FN,140)
#define K_F132		K(KT_FN,141)
#define K_F133		K(KT_FN,142)
#define K_F134		K(KT_FN,143)
#define K_F135		K(KT_FN,144)
#define K_F136		K(KT_FN,145)
#define K_F137		K(KT_FN,146)
#define K_F138		K(KT_FN,147)
#define K_F139		K(KT_FN,148)
#define K_F140		K(KT_FN,149)
#define K_F141		K(KT_FN,150)
#define K_F142		K(KT_FN,151)
#define K_F143		K(KT_FN,152)
#define K_F144		K(KT_FN,153)
#define K_F145		K(KT_FN,154)
#define K_F146		K(KT_FN,155)
#define K_F147		K(KT_FN,156)
#define K_F148		K(KT_FN,157)
#define K_F149		K(KT_FN,158)
#define K_F150		K(KT_FN,159)
#define K_F151		K(KT_FN,160)
#define K_F152		K(KT_FN,161)
#define K_F153		K(KT_FN,162)
#define K_F154		K(KT_FN,163)
#define K_F155		K(KT_FN,164)
#define K_F156		K(KT_FN,165)
#define K_F157		K(KT_FN,166)
#define K_F158		K(KT_FN,167)
#define K_F159		K(KT_FN,168)
#define K_F160		K(KT_FN,169)
#define K_F161		K(KT_FN,170)
#define K_F162		K(KT_FN,171)
#define K_F163		K(KT_FN,172)
#define K_F164		K(KT_FN,173)
#define K_F165		K(KT_FN,174)
#define K_F166		K(KT_FN,175)
#define K_F167		K(KT_FN,176)
#define K_F168		K(KT_FN,177)
#define K_F169		K(KT_FN,178)
#define K_F170		K(KT_FN,179)
#define K_F171		K(KT_FN,180)
#define K_F172		K(KT_FN,181)
#define K_F173		K(KT_FN,182)
#define K_F174		K(KT_FN,183)
#define K_F175		K(KT_FN,184)
#define K_F176		K(KT_FN,185)
#define K_F177		K(KT_FN,186)
#define K_F178		K(KT_FN,187)
#define K_F179		K(KT_FN,188)
#define K_F180		K(KT_FN,189)
#define K_F181		K(KT_FN,190)
#define K_F182		K(KT_FN,191)
#define K_F183		K(KT_FN,192)
#define K_F184		K(KT_FN,193)
#define K_F185		K(KT_FN,194)
#define K_F186		K(KT_FN,195)
#define K_F187		K(KT_FN,196)
#define K_F188		K(KT_FN,197)
#define K_F189		K(KT_FN,198)
#define K_F190		K(KT_FN,199)
#define K_F191		K(KT_FN,200)
#define K_F192		K(KT_FN,201)
#define K_F193		K(KT_FN,202)
#define K_F194		K(KT_FN,203)
#define K_F195		K(KT_FN,204)
#define K_F196		K(KT_FN,205)
#define K_F197		K(KT_FN,206)
#define K_F198		K(KT_FN,207)
#define K_F199		K(KT_FN,208)
#define K_F200		K(KT_FN,209)
#define K_F201		K(KT_FN,210)
#define K_F202		K(KT_FN,211)
#define K_F203		K(KT_FN,212)
#define K_F204		K(KT_FN,213)
#define K_F205		K(KT_FN,214)
#define K_F206		K(KT_FN,215)
#define K_F207		K(KT_FN,216)
#define K_F208		K(KT_FN,217)
#define K_F209		K(KT_FN,218)
#define K_F210		K(KT_FN,219)
#define K_F211		K(KT_FN,220)
#define K_F212		K(KT_FN,221)
#define K_F213		K(KT_FN,222)
#define K_F214		K(KT_FN,223)
#define K_F215		K(KT_FN,224)
#define K_F216		K(KT_FN,225)
#define K_F217		K(KT_FN,226)
#define K_F218		K(KT_FN,227)
#define K_F219		K(KT_FN,228)
#define K_F220		K(KT_FN,229)
#define K_F221		K(KT_FN,230)
#define K_F222		K(KT_FN,231)
#define K_F223		K(KT_FN,232)
#define K_F224		K(KT_FN,233)
#define K_F225		K(KT_FN,234)
#define K_F226		K(KT_FN,235)
#define K_F227		K(KT_FN,236)
#define K_F228		K(KT_FN,237)
#define K_F229		K(KT_FN,238)
#define K_F230		K(KT_FN,239)
#define K_F231		K(KT_FN,240)
#define K_F232		K(KT_FN,241)
#define K_F233		K(KT_FN,242)
#define K_F234		K(KT_FN,243)
#define K_F235		K(KT_FN,244)
#define K_F236		K(KT_FN,245)
#define K_F237		K(KT_FN,246)
#define K_F238		K(KT_FN,247)
#define K_F239		K(KT_FN,248)
#define K_F240		K(KT_FN,249)
#define K_F241		K(KT_FN,250)
#define K_F242		K(KT_FN,251)
#define K_F243		K(KT_FN,252)
#define K_F244		K(KT_FN,253)
#define K_F245		K(KT_FN,254)
#define K_UNDO		K(KT_FN,255)

#define K_HOLE		K(KT_SPEC,0)
#define K_ENTER		K(KT_SPEC,1)
#define K_SH_REGS	K(KT_SPEC,2)
#define K_SH_MEM	K(KT_SPEC,3)
#define K_SH_STAT	K(KT_SPEC,4)
#define K_BREAK		K(KT_SPEC,5)
#define K_CONS		K(KT_SPEC,6)
#define K_CAPS		K(KT_SPEC,7)
#define K_NUM		K(KT_SPEC,8)
#define K_HOLD		K(KT_SPEC,9)
#define K_SCROLLFORW	K(KT_SPEC,10)
#define K_SCROLLBACK	K(KT_SPEC,11)
#define K_BOOT		K(KT_SPEC,12)
#define K_CAPSON	K(KT_SPEC,13)
#define K_COMPOSE	K(KT_SPEC,14)
#define K_SAK		K(KT_SPEC,15)
#define K_DECRCONSOLE	K(KT_SPEC,16)
#define K_INCRCONSOLE	K(KT_SPEC,17)
#define K_SPAWNCONSOLE	K(KT_SPEC,18)
#define K_BARENUMLOCK	K(KT_SPEC,19)

#define K_ALLOCATED	K(KT_SPEC,126) /* dynamically allocated keymap */
#define K_NOSUCHMAP	K(KT_SPEC,127) /* returned by KDGKBENT */

#define K_P0		K(KT_PAD,0)
#define K_P1		K(KT_PAD,1)
#define K_P2		K(KT_PAD,2)
#define K_P3		K(KT_PAD,3)
#define K_P4		K(KT_PAD,4)
#define K_P5		K(KT_PAD,5)
#define K_P6		K(KT_PAD,6)
#define K_P7		K(KT_PAD,7)
#define K_P8		K(KT_PAD,8)
#define K_P9		K(KT_PAD,9)
#define K_PPLUS		K(KT_PAD,10)	/* key-pad plus */
#define K_PMINUS	K(KT_PAD,11)	/* key-pad minus */
#define K_PSTAR		K(KT_PAD,12)	/* key-pad asterisk (star) */
#define K_PSLASH	K(KT_PAD,13)	/* key-pad slash */
#define K_PENTER	K(KT_PAD,14)	/* key-pad enter */
#define K_PCOMMA	K(KT_PAD,15)	/* key-pad comma: kludge... */
#define K_PDOT		K(KT_PAD,16)	/* key-pad dot (period): kludge... */
#define K_PPLUSMINUS	K(KT_PAD,17)	/* key-pad plus/minus */
#define K_PPARENL	K(KT_PAD,18)	/* key-pad left parenthesis */
#define K_PPARENR	K(KT_PAD,19)	/* key-pad right parenthesis */

#define NR_PAD		20

#define K_DGRAVE	K(KT_DEAD,0)
#define K_DACUTE	K(KT_DEAD,1)
#define K_DCIRCM	K(KT_DEAD,2)
#define K_DTILDE	K(KT_DEAD,3)
#define K_DDIERE	K(KT_DEAD,4)
#define K_DCEDIL	K(KT_DEAD,5)

#define NR_DEAD		6

#define K_DOWN		K(KT_CUR,0)
#define K_LEFT		K(KT_CUR,1)
#define K_RIGHT		K(KT_CUR,2)
#define K_UP		K(KT_CUR,3)

#define K_SHIFT		K(KT_SHIFT,KG_SHIFT)
#define K_CTRL		K(KT_SHIFT,KG_CTRL)
#define K_ALT		K(KT_SHIFT,KG_ALT)
#define K_ALTGR		K(KT_SHIFT,KG_ALTGR)
#define K_SHIFTL	K(KT_SHIFT,KG_SHIFTL)
#define K_SHIFTR	K(KT_SHIFT,KG_SHIFTR)
#define K_CTRLL	 	K(KT_SHIFT,KG_CTRLL)
#define K_CTRLR	 	K(KT_SHIFT,KG_CTRLR)
#define K_CAPSSHIFT	K(KT_SHIFT,KG_CAPSSHIFT)

#define K_ASC0		K(KT_ASCII,0)
#define K_ASC1		K(KT_ASCII,1)
#define K_ASC2		K(KT_ASCII,2)
#define K_ASC3		K(KT_ASCII,3)
#define K_ASC4		K(KT_ASCII,4)
#define K_ASC5		K(KT_ASCII,5)
#define K_ASC6		K(KT_ASCII,6)
#define K_ASC7		K(KT_ASCII,7)
#define K_ASC8		K(KT_ASCII,8)
#define K_ASC9		K(KT_ASCII,9)
#define K_HEX0		K(KT_ASCII,10)
#define K_HEX1		K(KT_ASCII,11)
#define K_HEX2		K(KT_ASCII,12)
#define K_HEX3		K(KT_ASCII,13)
#define K_HEX4		K(KT_ASCII,14)
#define K_HEX5		K(KT_ASCII,15)
#define K_HEX6		K(KT_ASCII,16)
#define K_HEX7		K(KT_ASCII,17)
#define K_HEX8		K(KT_ASCII,18)
#define K_HEX9		K(KT_ASCII,19)
#define K_HEXa		K(KT_ASCII,20)
#define K_HEXb		K(KT_ASCII,21)
#define K_HEXc		K(KT_ASCII,22)
#define K_HEXd		K(KT_ASCII,23)
#define K_HEXe		K(KT_ASCII,24)
#define K_HEXf		K(KT_ASCII,25)

#define NR_ASCII	26

#define K_SHIFTLOCK	K(KT_LOCK,KG_SHIFT)
#define K_CTRLLOCK	K(KT_LOCK,KG_CTRL)
#define K_ALTLOCK	K(KT_LOCK,KG_ALT)
#define K_ALTGRLOCK	K(KT_LOCK,KG_ALTGR)
#define K_SHIFTLLOCK	K(KT_LOCK,KG_SHIFTL)
#define K_SHIFTRLOCK	K(KT_LOCK,KG_SHIFTR)
#define K_CTRLLLOCK	K(KT_LOCK,KG_CTRLL)
#define K_CTRLRLOCK	K(KT_LOCK,KG_CTRLR)

#define K_SHIFT_SLOCK	K(KT_SLOCK,KG_SHIFT)
#define K_CTRL_SLOCK	K(KT_SLOCK,KG_CTRL)
#define K_ALT_SLOCK	K(KT_SLOCK,KG_ALT)
#define K_ALTGR_SLOCK	K(KT_SLOCK,KG_ALTGR)
#define K_SHIFTL_SLOCK	K(KT_SLOCK,KG_SHIFTL)
#define K_SHIFTR_SLOCK	K(KT_SLOCK,KG_SHIFTR)
#define K_CTRLL_SLOCK	K(KT_SLOCK,KG_CTRLL)
#define K_CTRLR_SLOCK	K(KT_SLOCK,KG_CTRLR)

#define NR_LOCK		8

#define MAX_DIACR	256

extern struct kbdiacr accent_table[MAX_DIACR];
extern unsigned int accent_table_size;

/* kbd_kern.h --- header file from linux ---  */
extern int shift_state;

extern char *func_table[MAX_NR_FUNC];
extern char func_buf[];
extern char *funcbufptr;
extern int funcbufsize, funcbufleft;

/*
 * kbd->xxx contains the VC-local things (flag settings etc..)
 *
 * Note: externally visible are LED_SCR, LED_NUM, LED_CAP defined in kd.h
 *       The code in KDGETLED / KDSETLED depends on the internal and
 *       external order being the same.
 *
 * Note: lockstate is used as index in the array key_map.
 */
struct kbd_struct {

	unsigned char lockstate;
/* 8 modifiers - the names do not have any meaning at all;
   they can be associated to arbitrarily chosen keys */
#define VC_SHIFTLOCK	KG_SHIFT	/* shift lock mode */
#define VC_ALTGRLOCK	KG_ALTGR	/* altgr lock mode */
#define VC_CTRLLOCK	KG_CTRL 	/* control lock mode */
#define VC_ALTLOCK	KG_ALT  	/* alt lock mode */
#define VC_SHIFTLLOCK	KG_SHIFTL	/* shiftl lock mode */
#define VC_SHIFTRLOCK	KG_SHIFTR	/* shiftr lock mode */
#define VC_CTRLLLOCK	KG_CTRLL 	/* ctrll lock mode */
#define VC_CTRLRLOCK	KG_CTRLR 	/* ctrlr lock mode */
	unsigned char slockstate; 	/* for `sticky' Shift, Ctrl, etc. */

	unsigned char ledmode:2; 	/* one 2-bit value */
#define LED_SHOW_FLAGS 0        /* traditional state */
#define LED_SHOW_IOCTL 1        /* only change leds upon ioctl */
#define LED_SHOW_MEM 2          /* `heartbeat': peek into memory */

	unsigned char ledflagstate:3;	/* flags, not lights */
	unsigned char default_ledflagstate:3;
#define VC_SCROLLOCK	0	/* scroll-lock mode */
#define VC_NUMLOCK	1	/* numeric lock mode */
#define VC_CAPSLOCK	2	/* capslock mode */

	unsigned char kbdmode:2;	/* one 2-bit value */
#define VC_XLATE	0	/* translate keycodes using keymap */
#define VC_MEDIUMRAW	1	/* medium raw (keycode) mode */
#define VC_RAW		2	/* raw (scancode) mode */
#define VC_UNICODE	3	/* Unicode mode */

	unsigned char modeflags:5;
#define VC_APPLIC	0	/* application key mode */
#define VC_CKMODE	1	/* cursor key mode */
#define VC_REPEAT	2	/* keyboard repeat */
#define VC_CRLF		3	/* 0 - enter sends CR, 1 - enter sends CRLF */
#define VC_META		4	/* 0 - meta, 1 - meta=prefix with ESC */
};

extern struct kbd_struct kbd_table[];


void kbd_set_driver_handler(
  void ( *handler )( void *, unsigned short, unsigned long )
);

static inline void show_console(void)
{
}

static inline void set_console(int nr)
{
  (void) nr;
}

void set_leds(void);

static inline int vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
  return ((kbd->modeflags >> flag) & 1);
}

static inline int vc_kbd_led(struct kbd_struct * kbd, int flag)
{
  return ((kbd->ledflagstate >> flag) & 1);
}

static inline void set_vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
  kbd->modeflags |= 1 << flag;
}

static inline void set_vc_kbd_led(struct kbd_struct * kbd, int flag)
{
  kbd->ledflagstate |= 1 << flag;
}

static inline void clr_vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
  kbd->modeflags &= ~(1 << flag);
}

static inline void clr_vc_kbd_led(struct kbd_struct * kbd, int flag)
{
  kbd->ledflagstate &= ~(1 << flag);
}

static inline void chg_vc_kbd_lock(struct kbd_struct * kbd, int flag)
{
  kbd->lockstate ^= 1 << flag;
}

static inline void chg_vc_kbd_slock(struct kbd_struct * kbd, int flag)
{
  kbd->slockstate ^= 1 << flag;
}

static inline void chg_vc_kbd_mode(struct kbd_struct * kbd, int flag)
{
  kbd->modeflags ^= 1 << flag;
}

static inline void chg_vc_kbd_led(struct kbd_struct * kbd, int flag)
{
  kbd->ledflagstate ^= 1 << flag;
  set_leds();
}

#define U(x) ((x) ^ 0xf000)

/* keyboard.c */
int kbd_init(void);
int getkeycode(unsigned int scancode);
int setkeycode(unsigned int scancode, unsigned int keycode);
void compute_shiftstate(void);
unsigned char getledstate(void);
void setledstate(struct kbd_struct *kbd, unsigned int led);
void handle_scancode(unsigned char scancode, int down);

/* kbd_parser.c */
void register_kbd_msg_queue( char *qname, int port );
void unregister_kbd_msg_queue( int port );

/* defkeymap.c */
extern unsigned int keymap_count;

/* inch.c */
void add_to_queue( unsigned short );
int getch( void );
int BSP_wait_polled_input(void);
int rtems_kbpoll( void );

/* outch.c */
void _IBMPC_initVideo(void);

/* pc_keyb.c */
void keyboard_interrupt(void *unused);

/* vt.c */
int vt_ioctl( unsigned int cmd, unsigned long arg);

#endif
