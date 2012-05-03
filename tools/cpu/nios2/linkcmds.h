/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __LINKCMDS_H
#define __LINKCMDS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ptf.h"
#include "devices.h"

void fwrite_linkcmds_file(FILE *file, struct ptf *cfg, struct ptf *cpu, device_desc *devices, memory_desc *memory);

#endif


