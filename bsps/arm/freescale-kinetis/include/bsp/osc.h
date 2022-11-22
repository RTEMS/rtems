#ifndef KINETIS_OSC_H
#define KINETIS_OSC_H

/** OSC - Register Layout Typedef */
typedef struct {
  unsigned char cr;                                 /**< OSC Control Register, offset: 0x0 */
} kinetis_osc_t;

/*
 * OSC - Peripheral instance base addresses
 */
#define OSC           ((kinetis_osc_t *)0x40065000u)

#endif