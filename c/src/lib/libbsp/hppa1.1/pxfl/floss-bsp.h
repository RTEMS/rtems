/* 
 * we need a bsp.h in the rtems tree to keep the src/tests/...
 * happy.
 * But our bsp.h really lives in floss tree.
 *
 * Fake it out by installing this one in rtems
 *
 #  $Id$
 */

#include <floss/bsp.h>
