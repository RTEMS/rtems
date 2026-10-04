/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of the register check.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test-register-check.h>

#include <inttypes.h>

#include <rtems/test.h>

static uint64_t SlotsOfSource( const RegisterCheck *self, size_t source )
{
  uint64_t slots;
  size_t   i;

  slots = 0;

  for ( i = 0; i < self->slot_count; ++i ) {
    if ( self->slots[ i ].source == source ) {
      slots |= UINT64_C( 1 ) << i;
    }
  }

  return slots;
}

/*
 * A table check counts no step, so the test report shows the verdicts of the
 * register check alone.  A failed table check stops the test.
 */
#define CHECK_TABLE( ... ) \
  T_flags_true( T_CHECK_QUIET | T_CHECK_STOP, __VA_ARGS__ )

static void CheckTables( const RegisterCheck *self )
{
  size_t i;
  size_t j;

  CHECK_TABLE(
    self->source_count <= REGISTER_CHECK_MAX,
    "the source count %zu exceeds %d",
    self->source_count,
    REGISTER_CHECK_MAX
  );
  CHECK_TABLE(
    self->slot_count <= REGISTER_CHECK_MAX,
    "the slot count %zu exceeds %d",
    self->slot_count,
    REGISTER_CHECK_MAX
  );

  for ( i = 0; i < self->slot_count; ++i ) {
    CHECK_TABLE(
      self->slots[ i ].source < self->source_count,
      "the slot %s names no source",
      self->slots[ i ].name
    );
  }

  for ( i = 0; i < self->source_count; ++i ) {
    const RegisterCheckSource *source;

    source = &self->sources[ i ];
    CHECK_TABLE(
      source->mask != 0,
      "the source %s has no mask",
      source->name
    );
    CHECK_TABLE(
      SlotsOfSource( self, i ) != 0,
      "the source %s feeds no slot",
      source->name
    );

    if ( source->kind != REGISTER_CHECK_INITIAL ) {
      continue;
    }

    /*
     * The changed value of a source differs from the pattern of every source,
     * so a mix-up of two registers cannot hide the change.
     */
    for ( j = 0; j < self->source_count; ++j ) {
      if ( self->sources[ j ].kind == REGISTER_CHECK_INITIAL ) {
        CHECK_TABLE(
          ( self->initial[ i ] ^ source->mask ) != self->initial[ j ],
          "the changed value of %s equals the pattern of %s",
          source->name,
          self->sources[ j ].name
        );
      }
    }
  }
}

static void RunOnce( RegisterCheck *self, size_t changed )
{
  self->changed = changed;
  self->recorded = 0;
  self->mismatch = 0;
  ( *self->run )( self, self->arg );
}

void RegisterCheckRun( RegisterCheck *self )
{
  uint64_t all;
  size_t   i;

  CheckTables( self );

  if ( self->slot_count < REGISTER_CHECK_MAX ) {
    all = ( UINT64_C( 1 ) << self->slot_count ) - 1;
  } else {
    all = UINT64_MAX;
  }

  self->incomplete = 0;

  for ( i = 0; i < self->source_count; ++i ) {
    const RegisterCheckSource *source;

    source = &self->sources[ i ];

    if ( source->kind == REGISTER_CHECK_INITIAL ) {
      self->initial[ i ] ^= source->mask;
    }

    RunOnce( self, i );

    if ( source->kind == REGISTER_CHECK_INITIAL ) {
      self->initial[ i ] ^= source->mask;
    }

    self->flagged[ i ] = self->mismatch;

    if ( self->recorded != all ) {
      self->incomplete |= UINT64_C( 1 ) << i;
    }
  }

  RunOnce( self, self->source_count );
  T_eq(
    self->recorded,
    all,
    "the unchanged run recorded the slots %#" PRIx64 ", expected %#" PRIx64,
    self->recorded,
    all
  );
}

void RegisterCheckRecord(
  RegisterCheck *self,
  size_t         slot,
  uint64_t       actual,
  uint64_t       expected
)
{
  const RegisterCheckSlot *info;
  uint64_t                 bit;

  info = &self->slots[ slot ];
  bit = UINT64_C( 1 ) << slot;

  if (
    info->source == self->changed &&
    self->sources[ info->source ].kind == REGISTER_CHECK_EXPECTED
  ) {
    expected ^= self->sources[ info->source ].mask;
  }

  actual &= info->mask;
  expected &= info->mask;
  self->actual[ slot ] = actual;
  self->expected[ slot ] = expected;
  self->recorded |= bit;

  if ( actual != expected ) {
    self->mismatch |= bit;
  }
}

void RegisterCheckVerify( const RegisterCheck *self, size_t slot )
{
  T_true(
    ( self->recorded & ( UINT64_C( 1 ) << slot ) ) != 0 &&
      self->actual[ slot ] == self->expected[ slot ],
    "%s: recorded %d, actual %#" PRIx64 ", expected %#" PRIx64,
    self->slots[ slot ].name,
    ( self->recorded & ( UINT64_C( 1 ) << slot ) ) != 0,
    self->actual[ slot ],
    self->expected[ slot ]
  );
}

void RegisterCheckReport( const RegisterCheck *self )
{
  size_t i;

  T_eq(
    self->incomplete,
    0,
    "the changed runs of the sources %#" PRIx64 " recorded less than every "
    "slot",
    self->incomplete
  );

  for ( i = 0; i < self->source_count; ++i ) {
    uint64_t expected;

    expected = SlotsOfSource( self, i );
    T_eq(
      self->flagged[ i ],
      expected,
      "%s: flagged %#" PRIx64 ", expected %#" PRIx64,
      self->sources[ i ].name,
      self->flagged[ i ],
      expected
    );
  }
}
