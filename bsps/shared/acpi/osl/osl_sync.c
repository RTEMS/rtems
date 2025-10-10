/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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

#include <acpi/acpica/acpi.h>
#include <rtems.h>

#include <errno.h>
#include <pthread.h>

static void ms_timeout_to_abs_timespec(uint32_t timeout, struct timespec *abstime)
{
  clock_gettime(CLOCK_REALTIME, abstime);
  abstime->tv_sec += timeout / 1000;
  abstime->tv_nsec += (timeout % 1000) * 1000000;
}

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t wait_cond;
  pthread_cond_t delete_cond;
  uint32_t units;
  uint32_t max_units;
  uint32_t waiters;
  bool is_pending_delete;
} acpi_semaphore;

ACPI_STATUS AcpiOsCreateSemaphore(
  UINT32 MaxUnits,
  UINT32 InitialUnits,
  ACPI_SEMAPHORE* OutHandle
)
{
  acpi_semaphore* ac_sem;

  if (OutHandle == NULL || MaxUnits == 0 || InitialUnits > MaxUnits)
    return (AE_BAD_PARAMETER);

  ac_sem = malloc(sizeof(acpi_semaphore));
  if (ac_sem == NULL) {
    return (AE_NO_MEMORY);
  }

  if (pthread_mutex_init(&ac_sem->mutex, NULL) != 0 ||
      pthread_cond_init(&ac_sem->wait_cond, NULL) != 0 ||
      pthread_cond_init(&ac_sem->delete_cond, NULL) != 0)
  {
    free(ac_sem);
    return (AE_ERROR);
  }
  ac_sem->units = InitialUnits;
  ac_sem->max_units = MaxUnits;
  ac_sem->waiters = 0;
  ac_sem->is_pending_delete = false;

  /* Since we don't define ACPI_SEMAPHORE OutHandle is of type void** */
  *OutHandle = (ACPI_SEMAPHORE) ac_sem;

  return (AE_OK);
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle)
{
  int eno;
  acpi_semaphore* ac_sem = (acpi_semaphore*) Handle;

  if (ac_sem == NULL) {
    return (AE_BAD_PARAMETER);
  }

  eno = pthread_mutex_lock(&ac_sem->mutex);
  if (eno != 0) {
    return (AE_ERROR);
  }

  /* Ensure there are no waiters left */
  ac_sem->is_pending_delete = true;
  pthread_cond_broadcast(&ac_sem->wait_cond);
  while (ac_sem->waiters > 0) {
    pthread_cond_wait(&ac_sem->delete_cond, &ac_sem->mutex);
  }

  pthread_mutex_unlock(&ac_sem->mutex);

  pthread_mutex_destroy(&ac_sem->mutex);
  pthread_cond_destroy(&ac_sem->wait_cond);
  pthread_cond_destroy(&ac_sem->delete_cond);

  free(ac_sem);

  return (AE_OK);
}

ACPI_STATUS AcpiOsWaitSemaphore(
  ACPI_SEMAPHORE Handle,
  UINT32 Units,
  UINT16 Timeout
)
{
  int eno;
  ACPI_STATUS ac_status = (AE_OK);
  acpi_semaphore* ac_sem = (acpi_semaphore*) Handle;

  if (ac_sem == NULL || Units == 0) {
    return (AE_BAD_PARAMETER);
  }

  eno = pthread_mutex_lock(&ac_sem->mutex);
  if (eno != 0) {
    return (AE_ERROR);
  }

  if (ac_sem->max_units != ACPI_NO_UNIT_LIMIT && ac_sem->max_units < Units) {
    return (AE_LIMIT);
  }

  switch (Timeout) {
  case ACPI_DO_NOT_WAIT:
    if (ac_sem->units < Units) {
      ac_status = (AE_TIME);
    }

    break;
  case ACPI_WAIT_FOREVER:
    ac_sem->waiters++;
    while (ac_sem->units < Units) {
      pthread_cond_wait(&ac_sem->wait_cond, &ac_sem->mutex);

      if (ac_sem->is_pending_delete) {
        ac_status = (AE_ERROR);
        pthread_cond_signal(&ac_sem->delete_cond);
        break;
      }
    }
    ac_sem->waiters--;

    break;
  default:
    struct timespec abstime;
    ms_timeout_to_abs_timespec(Timeout, &abstime);

    ac_sem->waiters++;
    while (ac_sem->units < Units) {
      eno = pthread_cond_timedwait(&ac_sem->wait_cond, &ac_sem->mutex, &abstime);

      if (ac_sem->is_pending_delete) {
        ac_status = (AE_ERROR);
        pthread_cond_signal(&ac_sem->delete_cond);
        break;
      }

      if (eno == ETIMEDOUT) {
        ac_status = (AE_TIME);
        break;
      }
    }
    ac_sem->waiters--;

    break;
  }

  if (ac_status == (AE_OK)) {
    ac_sem->units -= Units;
  }

  pthread_mutex_unlock(&ac_sem->mutex);

  return ac_status;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units)
{
  int eno;
  acpi_semaphore* ac_sem = (acpi_semaphore*) Handle;

  if (ac_sem == NULL || Units == 0) {
    return (AE_BAD_PARAMETER);
  }

  eno = pthread_mutex_lock(&ac_sem->mutex);
  if (eno != 0) {
    return (AE_ERROR);
  }

  if (ac_sem->max_units != ACPI_NO_UNIT_LIMIT &&
      ac_sem->units + Units > ac_sem->max_units)
  {
    return (AE_LIMIT);
  }

  ac_sem->units += Units;
  /*
   * Signal condition "Units" times to wake up the maximum amount of threads
   * that could consume the new units
   */
  while (Units--) {
    pthread_cond_signal(&ac_sem->wait_cond);
  }

  pthread_mutex_unlock(&ac_sem->mutex);

  return (AE_OK);
}

typedef struct {
  pthread_mutex_t mutex;
} acpi_spinlock;

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK* OutHandle)
{
  acpi_spinlock* lock;

  if (OutHandle == NULL)
    return (AE_BAD_PARAMETER);

  lock = malloc(sizeof(acpi_spinlock));
  if (lock == NULL) {
    return (AE_NO_MEMORY);
  }

  pthread_mutex_init(&lock->mutex, NULL);

  /* Since we don't define ACPI_SPINLOCK OutHandle is of type void** */
  *OutHandle = (ACPI_SPINLOCK) lock;

  return (AE_OK);
}

void AcpiOsDeleteLock(ACPI_SPINLOCK Handle)
{
  acpi_spinlock* lock = (acpi_spinlock*) Handle;

  if (lock == NULL) {
    return;
  }

  pthread_mutex_destroy(&lock->mutex);

  free(lock);
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle)
{
  acpi_spinlock* lock = (acpi_spinlock*) Handle;

  if (lock == NULL) {
    return 0;
  }

  pthread_mutex_lock(&lock->mutex);

  return 0;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags)
{
#ifdef __rtems__
    (void) Flags;
#endif
  acpi_spinlock* lock = (acpi_spinlock*) Handle;

  if (lock == NULL) {
    return;
  }

  pthread_mutex_unlock(&lock->mutex);
}
