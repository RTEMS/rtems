/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3AMBA
 */

/*
 *  AMBA Plag & Play Bus Driver Macros
 *
 *  Macros used for AMBA Plug & Play bus scanning
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __AMBA_H__
#define __AMBA_H__

/**
 * @defgroup RTEMSBSPsSPARCLEON3AMBA LEON3 AMBA Driver Handler
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief AMBA Plag & Play Bus Driver Macros
 *
 * @{
 */

#define LEON3_IO_AREA 0xfff00000
#define LEON3_CONF_AREA 0xff000
#define LEON3_AHB_SLAVE_CONF_AREA (1 << 11)

#define LEON3_AHB_CONF_WORDS 8
#define LEON3_APB_CONF_WORDS 2
#define LEON3_AHB_MASTERS 64
#define LEON3_AHB_SLAVES 64
#define LEON3_APB_SLAVES 16

#include <grlib/ambapp.h>
#include <grlib/grlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The AMBA Plug&Play info of the bus that the LEON3 sits on */
extern struct ambapp_bus ambapp_plb;

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AMBA_H__ */
