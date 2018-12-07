/**
 *  @file
 *  @brief makecontext() API Conformance Test
 */

/*
 *  COPYRIGHT (c) 2018.
 *  Jacob Shin
 *
 *  Permission to use, copy, modify, and/or distribute this software
 *  for any purpose with or without fee is hereby granted.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define STACK_SIZE (1<<15) // 32KiB

#define _XOPEN_SOURCE_EXTENDED 1
#include <ucontext.h>
#include <stdlib.h>

int test( void );
void func( void );

int test( void )
{
  ucontext_t context;
  context.uc_stack.ss_sp = (char *) malloc(STACK_SIZE);
  context.uc_stack.ss_size = STACK_SIZE;
  context.uc_link = 0;
  context.uc_stack.ss_flags = 0;
  ucontext_t *ucp = &context;
  getcontext(ucp);

  makecontext(ucp, (void *)func, 0);

  setcontext(ucp);
  abort();
  return 0;
}

void func( void )
{
	return;
}