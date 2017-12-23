/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief mailbox support.
 */
/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_MAILBOX_H
#define LIBBSP_ARM_RASPBERRYPI_MAILBOX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern unsigned int  raspberrypi_mailbox_read(unsigned int channel);
extern void raspberrypi_mailbox_write(unsigned int channel, unsigned int data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* LIBBSP_ARM_RASPBERRYPI_MAILBOX_H */
