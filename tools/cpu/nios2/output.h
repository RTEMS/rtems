/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __OUTPUT_H
#define __OUTPUT_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ptf.h"
#include "clocks.h"
#include "devices.h"

void fwrite_value(struct ptf_item *pi, void *arg);
void fwrite_header_file(FILE *file, struct ptf *cfg, device_desc *devices, clock_desc *clocks);

#endif


