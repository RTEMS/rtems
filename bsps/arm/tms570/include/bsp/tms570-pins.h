/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief Select pin mapping according to selected chip.
 *        Defaults to TMS570LS3137ZWT for now.
 */

#include <bspopts.h>

#if TMS570_VARIANT == 4357
#include <bsp/tms570lc4357-pins.h>
#else
#include <bsp/tms570ls3137zwt-pins.h>
#endif
