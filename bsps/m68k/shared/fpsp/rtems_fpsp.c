#include <rtems/system.h>
/*
#include <rtems/score/isr.h>
*/

/*
 * User exception handlers
 */
proc_ptr M68040FPSPUserExceptionHandlers[9];

/*
 * Intercept requests to install an exception handler.
 * FPSP exceptions get special treatment.
 */
static int
FPSP_install_raw_handler (uint32_t   vector, proc_ptr new_handler, proc_ptr *old_handler)
{
  int fpspVector;

  switch (vector) {
  default:	return 0;	/* Non-FPSP vector */
  case 11:	fpspVector = 0;	break;	/* F-line */
  case 48:	fpspVector = 1;	break;	/* BSUN */
  case 49:	fpspVector = 2;	break;	/* INEXACT */
  case 50:	fpspVector = 3;	break;	/* DIVIDE-BY-ZERO */
  case 51:	fpspVector = 4;	break;	/* UNDERFLOW */
  case 52:	fpspVector = 5;	break;	/* OPERAND ERROR */
  case 53:	fpspVector = 6;	break;	/* OVERFLOW */
  case 54:	fpspVector = 7;	break;	/* SIGNALLING NAN */
  case 55:	fpspVector = 8;	break;	/* UNIMPLEMENTED DATA TYPE */
  }
  *old_handler = M68040FPSPUserExceptionHandlers[fpspVector];
  M68040FPSPUserExceptionHandlers[fpspVector] = new_handler;
  return 1;
}

/*
 *  Exception handlers provided by FPSP package.
 */
extern void _fpspEntry_fline(void);
extern void _fpspEntry_bsun(void);
extern void _fpspEntry_inex(void);
extern void _fpspEntry_dz(void);
extern void _fpspEntry_unfl(void);
extern void _fpspEntry_ovfl(void);
extern void _fpspEntry_operr(void);
extern void _fpspEntry_snan(void);
extern void _fpspEntry_unsupp(void);

/*
 * Attach floating point exception vectors to M68040FPSP entry points
 *
 *  NOTE: Uses M68K rather than M68040 in the name so all CPUs having
 *        an FPSP can share the same code in RTEMS proper.
 */
void
M68KFPSPInstallExceptionHandlers (void)
{
  static struct {
    int  vector_number;
    void  (*handler)(void);
  } fpspHandlers[] = {
    { 11,  _fpspEntry_fline },
    { 48,  _fpspEntry_bsun },
    { 49,  _fpspEntry_inex },
    { 50,  _fpspEntry_dz },
    { 51,  _fpspEntry_unfl },
    { 52,  _fpspEntry_operr },
    { 53,  _fpspEntry_ovfl },
    { 54,  _fpspEntry_snan },
    { 55,  _fpspEntry_unsupp },
  };
  int i;
  proc_ptr oldHandler;

  for (i = 0 ; i < sizeof fpspHandlers / sizeof fpspHandlers[0] ; i++) {
    _CPU_ISR_install_raw_handler(fpspHandlers[i].vector_number, fpspHandlers[i].handler, &oldHandler);
      M68040FPSPUserExceptionHandlers[i] = oldHandler;
  }
  _FPSP_install_raw_handler = FPSP_install_raw_handler;
}
