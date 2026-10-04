/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the interfaces of the register check.
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

#ifndef _RTEMS_TEST_REGISTER_CHECK_H
#define _RTEMS_TEST_REGISTER_CHECK_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

/**
 * @brief This constant defines the maximum count of sources and of slots of
 *   a register check.
 */
#define REGISTER_CHECK_MAX 64

/**
 * @brief This enumeration defines where the register check changes the value
 *   of a source.
 */
typedef enum {
  /**
   * @brief The register check inverts the initial value, which the scenario
   *   loads into the register.
   */
  REGISTER_CHECK_INITIAL,

  /**
   * @brief The register check inverts the expected value of each slot of the
   *   source.  This kind serves values which the machine or the code fixes.
   */
  REGISTER_CHECK_EXPECTED
} RegisterCheckKind;

/**
 * @brief This structure describes a source of a register check.
 *
 * A source is a value which the register check changes in one of its runs.
 */
typedef struct {
  /**
   * @brief This member names the source in the test report.
   */
  const char *name;

  /**
   * @brief This member defines where the register check changes the value.
   */
  RegisterCheckKind kind;

  /**
   * @brief This member defines the bits which the register check inverts.
   */
  uint64_t mask;
} RegisterCheckSource;

/**
 * @brief This structure describes a slot of a register check.
 *
 * A slot is one comparison of an actual value with an expected value.
 */
typedef struct {
  /**
   * @brief This member names the slot.
   */
  const char *name;

  /**
   * @brief This member is the index of the source which feeds the slot.
   */
  size_t source;

  /**
   * @brief This member defines the bits which the slot compares.
   */
  uint64_t mask;
} RegisterCheckSlot;

typedef struct RegisterCheck RegisterCheck;

/**
 * @brief This structure provides the context of a register check.
 *
 * The test initializes the members from sources to arg.  The register check
 * uses the other members.
 */
struct RegisterCheck {
  /**
   * @brief This member references the table of sources.
   */
  const RegisterCheckSource *sources;

  /**
   * @brief This member is the count of sources.
   */
  size_t source_count;

  /**
   * @brief This member references the table of slots.
   */
  const RegisterCheckSlot *slots;

  /**
   * @brief This member is the count of slots.
   */
  size_t slot_count;

  /**
   * @brief This member references the initial values, one per source.
   *
   * The scenario loads the initial value of each source of the kind
   * #REGISTER_CHECK_INITIAL from this table.
   */
  uint64_t *initial;

  /**
   * @brief This member references the handler which runs the scenario once.
   *
   * The handler shall call RegisterCheckRecord() once for each slot.
   */
  void ( *run )( RegisterCheck *self, void *arg );

  /**
   * @brief This member is the argument of the run handler.
   */
  void *arg;

  /**
   * @brief This member is the index of the changed source of the current
   *   run, or the source count in the unchanged run.
   */
  size_t changed;

  /**
   * @brief This member contains the bit of each slot which the current run
   *   recorded.
   */
  uint64_t recorded;

  /**
   * @brief This member contains the bit of each slot whose comparison failed
   *   in the current run.
   */
  uint64_t mismatch;

  /**
   * @brief This member contains the bit of each source whose run recorded
   *   less than every slot.
   */
  uint64_t incomplete;

  /**
   * @brief This member contains the masked actual value of each slot of the
   *   last run.
   */
  uint64_t actual[ REGISTER_CHECK_MAX ];

  /**
   * @brief This member contains the masked expected value of each slot of
   *   the last run.
   */
  uint64_t expected[ REGISTER_CHECK_MAX ];

  /**
   * @brief This member contains the mismatch bits of the run which changed
   *   the source of the index.
   */
  uint64_t flagged[ REGISTER_CHECK_MAX ];
};

/**
 * @brief Runs the scenario once for each source with the changed source,
 *   then once unchanged.
 *
 * The run of a source of the kind #REGISTER_CHECK_INITIAL inverts the masked
 * bits of its initial value.  The run of a source of the kind
 * #REGISTER_CHECK_EXPECTED inverts the masked bits of the expected value of
 * each of its slots.  The last run changes no source.  Members actual and
 * expected hold the values of this run.
 *
 * @param[in, out] self is the register check.
 */
void RegisterCheckRun( RegisterCheck *self );

/**
 * @brief Records the actual and the expected value of a slot.
 *
 * @param[in, out] self is the register check.
 *
 * @param slot is the index of the slot.
 *
 * @param actual is the actual value.
 *
 * @param expected is the expected value.
 */
void RegisterCheckRecord(
  RegisterCheck *self,
  size_t         slot,
  uint64_t       actual,
  uint64_t       expected
);

/**
 * @brief Checks that the unchanged run recorded the slot and that its actual
 *   value equals its expected value.
 *
 * @param[in] self is the register check.
 *
 * @param slot is the index of the slot.
 */
void RegisterCheckVerify( const RegisterCheck *self, size_t slot );

/**
 * @brief Checks that each changed run flagged exactly the slots of its
 *   source and recorded every slot.
 *
 * @param[in] self is the register check.
 */
void RegisterCheckReport( const RegisterCheck *self );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_TEST_REGISTER_CHECK_H */
