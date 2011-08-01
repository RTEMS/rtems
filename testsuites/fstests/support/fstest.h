/*
 *
 *  $Id$
 */

#ifndef __FSTEST_H
#define __FSTEST_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pmacros.h>

#include "fs_config.h"

#define TIME_PRECISION  (2)
#define time_equal(x,y) (abs((x)-(y))<TIME_PRECISION)
#define ALLPERMS        (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)
#endif 



