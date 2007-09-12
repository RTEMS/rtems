/*
 *  linux/drivers/char/vt.c
 *
 *  Copyright (C) 1992 obz under the linux copyright
 *
 *  Dynamic diacritical handling - aeb@cwi.nl - Dec 1993
 *  Dynamic keymap and string allocation - aeb@cwi.nl - May 1994
 *  Restrict VT switching via ioctl() - grif@cs.ucr.edu - Dec 1995
 *  Some code moved for less code duplication - Andi Kleen - Mar 1997
 *
 *
 * by: Rosimildo da Silva --
 * Ported to RTEMS to provide the basic interface to the console
 * driver. Removed all stuff not required, such as VT_, Fonts, etc.
 */

#include <string.h>  /* memcpy */
#include <sys/types.h>
#include <errno.h>

#include <i386_io.h>
#include <rtems.h>
#include <rtems/kd.h>
#include <rtems/keyboard.h>

/*
 * Console (vt and kd) routines, as defined by USL SVR4 manual, and by
 * experimentation and study of X386 SYSV handling.
 *
 * One point of difference: SYSV vt's are /dev/vtX, which X >= 0, and
 * /dev/console is a separate ttyp. Under Linux, /dev/tty0 is /dev/console,
 * and the vc start at /dev/ttyX, X >= 1. We maintain that here, so we will
 * always treat our set of vt as numbered 1..MAX_NR_CONSOLES (corresponding to
 * ttys 0..MAX_NR_CONSOLES-1). Explicitly naming VT 0 is illegal, but using
 * /dev/tty0 (fg_console) as a target is legal, since an implicit aliasing
 * to the current console is done by the main ioctl code.
 */

struct vt_struct *vt_cons[MAX_NR_CONSOLES];

/* Keyboard type: Default is KB_101, but can be set by machine
 * specific code.
 */
unsigned char keyboard_type = KB_101;

/*
 * Generates sound of some frequency for some number of clock ticks
 *
 * If freq is 0, will turn off sound, else will turn it on for that time.
 * If msec is 0, will return immediately, else will sleep for msec time, then
 * turn sound off.
 *
 * We also return immediately, which is what was implied within the X
 * comments - KDMKTONE doesn't put the process to sleep.
 */

#if defined(__i386__) || defined(__alpha__) || defined(__powerpc__) \
    || (defined(__mips__) && !defined(CONFIG_SGI))

static void
kd_nosound(unsigned long ignored)
{
  /* disable counter 2 */
  outb(inb_p(0x61)&0xFC, 0x61);
  return;
}

void
_kd_mksound(unsigned int hz, unsigned int ticks)
{
  unsigned int          count = 0;
  rtems_interrupt_level level;

  if (hz > 20 && hz < 32767)
    count = 1193180 / hz;

   rtems_interrupt_disable(level);
/*  del_timer(&sound_timer);  */
  if (count) {
    /* enable counter 2 */
    outb_p(inb_p(0x61)|3, 0x61);
    /* set command for counter 2, 2 byte write */
    outb_p(0xB6, 0x43);
    /* select desired HZ */
    outb_p(count & 0xff, 0x42);
    outb((count >> 8) & 0xff, 0x42);

/*
    if (ticks) {
      sound_timer.expires = jiffies+ticks;
      add_timer(&sound_timer);
    }
*/
  } else
    kd_nosound(0);

   rtems_interrupt_enable(level);
  return;
}

#else

void
_kd_mksound(unsigned int hz, unsigned int ticks)
{
}

#endif

void (*kd_mksound)(unsigned int hz, unsigned int ticks) = _kd_mksound;

#define i (tmp.kb_index)
#define s (tmp.kb_table)
#define v (tmp.kb_value)
static inline int
do_kdsk_ioctl(int cmd, struct kbentry *user_kbe, int perm, struct kbd_struct *kbd)
{
  struct kbentry tmp;
  ushort *key_map, val;

  tmp = *user_kbe;
  if (i >= NR_KEYS)  /*  s cannot be >= MAX_NR_KEYMAPS */
    return -EINVAL;

  switch (cmd) {
  case KDGKBENT:
    key_map = key_maps[s];
    if (key_map) {
        val = U(key_map[i]);
        if (kbd->kbdmode != VC_UNICODE && KTYP(val) >= NR_TYPES)
      val = K_HOLE;
    } else
        val = (i ? K_HOLE : K_NOSUCHMAP);
    user_kbe->kb_value = val;
    return 0;

  case KDSKBENT:
    return -EINVAL;
  }
  return 0;
}
#undef i
#undef s
#undef v

#define  HZ  100

static inline int
do_kbkeycode_ioctl(int cmd, struct kbkeycode *user_kbkc, int perm)
{
  struct kbkeycode tmp;
  int kc = 0;

  tmp = *user_kbkc;
  switch (cmd) {
  case KDGETKEYCODE:
    kc = getkeycode(tmp.scancode);
    if (kc >= 0)
      user_kbkc->keycode = kc;
    break;
  case KDSETKEYCODE:
    if (!perm)
      return -EPERM;
    kc = setkeycode(tmp.scancode, tmp.keycode);
    break;
  }
  return kc;
}

static inline int
do_kdgkb_ioctl(int cmd, struct kbsentry *user_kdgkb, int perm)
{
  return -EINVAL;
}

/*
 * We handle the console-specific ioctl's here.  We allow the
 * capability to modify any console, not just the fg_console.
 */
int vt_ioctl( unsigned int cmd, unsigned long arg)
{
  int perm;
  unsigned int console;
  unsigned char ucval;
  struct kbd_struct * kbd;

   console = 0;
  /*
   * To have permissions to do most of the vt ioctls, we either have
   * to be the owner of the tty, or super-user.
   */
  perm = 1;
  kbd = kbd_table + console;
  switch (cmd) {
  case KIOCSOUND:
    if (!perm)
      return -EPERM;
    if (arg)
      arg = 1193180 / arg;
    kd_mksound(arg, 0);
    return 0;

  case KDMKTONE:
    if (!perm)
      return -EPERM;
  {
    unsigned int ticks, count;

    /*
     * Generate the tone for the appropriate number of ticks.
     * If the time is zero, turn off sound ourselves.
     */
    ticks = HZ * ((arg >> 16) & 0xffff) / 1000;
    count = ticks ? (arg & 0xffff) : 0;
    if (count)
      count = 1193180 / count;
    kd_mksound(count, ticks);
    return 0;
  }

  case KDGKBTYPE:
    /*
     * this is naive.
     */
    ucval = keyboard_type;
    goto setchar;

  case KDSETMODE:
  case KDGETMODE:
    return -EINVAL;

  case KDSKBMODE:
    if (!perm)
      return -EPERM;
    switch(arg) {
      case K_RAW:
      kbd->kbdmode = VC_RAW;
      break;
      case K_MEDIUMRAW:
      kbd->kbdmode = VC_MEDIUMRAW;
      break;
      case K_XLATE:
      kbd->kbdmode = VC_XLATE;
      compute_shiftstate();
      break;
      case K_UNICODE:
      kbd->kbdmode = VC_UNICODE;
      compute_shiftstate();
      break;
      default:
      return -EINVAL;
    }
    return 0;

  case KDGKBMODE:
    ucval = ((kbd->kbdmode == VC_RAW) ? K_RAW :
         (kbd->kbdmode == VC_MEDIUMRAW) ? K_MEDIUMRAW :
         (kbd->kbdmode == VC_UNICODE) ? K_UNICODE :
         K_XLATE);
    goto setint;

  /* this could be folded into KDSKBMODE, but for compatibility
     reasons it is not so easy to fold KDGKBMETA into KDGKBMODE */
  case KDSKBMETA:
    switch(arg) {
      case K_METABIT:
      clr_vc_kbd_mode(kbd, VC_META);
      break;
      case K_ESCPREFIX:
      set_vc_kbd_mode(kbd, VC_META);
      break;
      default:
      return -EINVAL;
    }
    return 0;

  case KDGKBMETA:
    ucval = (vc_kbd_mode(kbd, VC_META) ? K_ESCPREFIX : K_METABIT);
  setint:
    *(int *)arg = ucval;
    return 0;

  case KDGETKEYCODE:
  case KDSETKEYCODE:
    return do_kbkeycode_ioctl(cmd, (struct kbkeycode *)arg, perm);

  case KDGKBENT:
  case KDSKBENT:
    return do_kdsk_ioctl(cmd, (struct kbentry *)arg, perm, kbd);

  case KDGKBDIACR:
  {
    struct kbdiacrs *a = (struct kbdiacrs *)arg;
    a->kb_cnt = accent_table_size;
    memcpy( a->kbdiacr, accent_table, accent_table_size*sizeof(struct kbdiacr) );
    return 0;
  }

  case KDSKBDIACR:
  {
    struct kbdiacrs *a = (struct kbdiacrs *)arg;
    unsigned int ct;

    if (!perm)
      return -EPERM;
    ct = a->kb_cnt;
    if (ct >= MAX_DIACR)
      return -EINVAL;
    accent_table_size = ct;
    memcpy(accent_table, a->kbdiacr, ct*sizeof(struct kbdiacr));
    return 0;
  }

  /* the ioctls below read/set the flags usually shown in the leds */
  /* don't use them - they will go away without warning */
  case KDGKBLED:
    ucval = kbd->ledflagstate | (kbd->default_ledflagstate << 4);
    goto setchar;

  case KDSKBLED:
    if (!perm)
      return -EPERM;
    if (arg & ~0x77)
      return -EINVAL;
    kbd->ledflagstate = (arg & 7);
    kbd->default_ledflagstate = ((arg >> 4) & 7);
    set_leds();
    return 0;

  /* the ioctls below only set the lights, not the functions */
  /* for those, see KDGKBLED and KDSKBLED above */
  case KDGETLED:
    ucval = getledstate();
  setchar:
    *(char*)arg = ucval;
      return 0;

  case KDSETLED:
    if (!perm)
      return -EPERM;
    setledstate(kbd, arg);
    return 0;

  default:
    return -EINVAL;
  }
}
