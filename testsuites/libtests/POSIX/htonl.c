/*
 * Copyright (c) 2010 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <arpa/inet.h>

int
main (void)
{
  uint32_t u32;
  uint16_t u16;

  u32 = htonl(0x12345678);
  u16 = htons(0x1234);

  u32 = ntohl(0x12345678);
  u16 = ntohs(0x1234);
  
  return 0;
}
