/*  ac97.c
 *
 *  Sound driver for Milkymist SoC
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libio.h>
#include <rtems/status-checks.h>
#include "../include/system_conf.h"
#include "milkymist_ac97.h"

#define SND_DEVICE_NAME "/dev/snd"
#define MIXER_DEVICE_NAME "/dev/mixer"

static rtems_id cr_write_sem;
static rtems_id cr_read_sem;

static rtems_isr crrequest_handler(rtems_vector_number n)
{
  rtems_semaphore_release(cr_write_sem);
  lm32_interrupt_ack(1 << MM_IRQ_AC97CRREQUEST);
}

static rtems_isr crreply_handler(rtems_vector_number n)
{
  rtems_semaphore_release(cr_read_sem);
  lm32_interrupt_ack(1 << MM_IRQ_AC97CRREPLY);
}

/* queued playback buffers */
#define PLAY_Q_SIZE 8
#define PLAY_Q_MASK (PLAY_Q_SIZE-1)

static struct snd_buffer *play_q[PLAY_Q_SIZE];
static int play_produce;
static int play_consume;
static int play_level;

/* buffers played, for application to collect */
static rtems_id play_q_done;

static void play_start(struct snd_buffer *buf)
{
  if (buf->nsamples > (AC97_MAX_DMASIZE/4))
    buf->nsamples = AC97_MAX_DMASIZE/4;

  MM_WRITE(MM_AC97_DADDRESS, (unsigned int)buf->samples);
  MM_WRITE(MM_AC97_DREMAINING, buf->nsamples*4);
  MM_WRITE(MM_AC97_DCTL, AC97_SCTL_EN);
}

static rtems_isr pcmplay_handler(rtems_vector_number n)
{
  lm32_interrupt_ack(1 << MM_IRQ_AC97DMAR);

  rtems_message_queue_send(play_q_done, &play_q[play_consume],
    sizeof(void *));

  play_consume = (play_consume + 1) & PLAY_Q_MASK;
  play_level--;

  if(play_level > 0)
    play_start(play_q[play_consume]);
  else
    MM_WRITE(MM_AC97_DCTL, 0);
}

/* queued record buffers */
#define RECORD_Q_SIZE 8
#define RECORD_Q_MASK (RECORD_Q_SIZE-1)

static struct snd_buffer *record_q[RECORD_Q_SIZE];
static int record_produce;
static int record_consume;
static int record_level;

/* buffers recorded, for application to collect */
static rtems_id record_q_done;

static void record_start(struct snd_buffer *buf)
{
  if (buf->nsamples > (AC97_MAX_DMASIZE/4))
    buf->nsamples = AC97_MAX_DMASIZE/4;

  MM_WRITE(MM_AC97_UADDRESS, (unsigned int)buf->samples);
  MM_WRITE(MM_AC97_UREMAINING, buf->nsamples*4);
  MM_WRITE(MM_AC97_UCTL, AC97_SCTL_EN);
}

static rtems_isr pcmrecord_handler(rtems_vector_number n)
{
  lm32_interrupt_ack(1 << MM_IRQ_AC97DMAW);

  __asm__ volatile( /* Invalidate Level-1 data cache */
      "wcsr DCC, r0\n"
      "nop\n"
    );

  rtems_message_queue_send(record_q_done, &record_q[record_consume],
    sizeof(void *));

  record_consume = (record_consume + 1) & RECORD_Q_MASK;
  record_level--;

  if(record_level > 0)
    record_start(record_q[record_consume]);
  else
    MM_WRITE(MM_AC97_UCTL, 0);
}

rtems_device_driver ac97_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  sc = rtems_io_register_name(SND_DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create snd device");

  sc = rtems_io_register_name(MIXER_DEVICE_NAME, major, 1);
  RTEMS_CHECK_SC(sc, "create mixer device");

  sc = rtems_semaphore_create(
    rtems_build_name('C', 'R', 'W', 'S'),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &cr_write_sem
  );
  RTEMS_CHECK_SC(sc, "create AC97 register write semaphore");

  sc = rtems_semaphore_create(
    rtems_build_name('C', 'R', 'R', 'S'),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &cr_read_sem
  );
  RTEMS_CHECK_SC(sc, "create AC97 register read semaphore");

  sc = rtems_message_queue_create(
    rtems_build_name('P', 'L', 'Y', 'Q'),
    PLAY_Q_SIZE*2,
    sizeof(void *),
    0,
    &play_q_done
  );
  RTEMS_CHECK_SC(sc, "create playback done queue");

  sc = rtems_message_queue_create(
    rtems_build_name('R', 'E', 'C', 'Q'),
    RECORD_Q_SIZE*2,
    sizeof(void *),
    0,
    &record_q_done
  );
  RTEMS_CHECK_SC(sc, "create record done queue");

  rtems_interrupt_catch(crrequest_handler, MM_IRQ_AC97CRREQUEST, &dummy);
  rtems_interrupt_catch(crreply_handler, MM_IRQ_AC97CRREPLY, &dummy);
  rtems_interrupt_catch(pcmplay_handler, MM_IRQ_AC97DMAR, &dummy);
  rtems_interrupt_catch(pcmrecord_handler, MM_IRQ_AC97DMAW, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_AC97CRREQUEST);
  bsp_interrupt_vector_enable(MM_IRQ_AC97CRREPLY);
  bsp_interrupt_vector_enable(MM_IRQ_AC97DMAR);
  bsp_interrupt_vector_enable(MM_IRQ_AC97DMAW);

  play_produce = 0;
  play_consume = 0;
  play_level = 0;

  record_produce = 0;
  record_consume = 0;
  record_level = 0;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code submit_play(struct snd_buffer *buf)
{
  bsp_interrupt_vector_disable(MM_IRQ_AC97DMAR);
  if (play_level == PLAY_Q_SIZE) {
    bsp_interrupt_vector_enable(MM_IRQ_AC97DMAR);
    return RTEMS_UNSATISFIED;
  }
  play_q[play_produce] = buf;
  play_produce = (play_produce + 1) & PLAY_Q_MASK;
  play_level++;

  if (play_level == 1)
    play_start(buf);

  bsp_interrupt_vector_enable(MM_IRQ_AC97DMAR);
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code collect_play(struct snd_buffer **buf)
{
  size_t s;

  return rtems_message_queue_receive(
    play_q_done,
    buf,
    &s,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
}

static rtems_status_code submit_record(struct snd_buffer *buf)
{
  bsp_interrupt_vector_disable(MM_IRQ_AC97DMAW);
  if (record_level == RECORD_Q_SIZE) {
    bsp_interrupt_vector_enable(MM_IRQ_AC97DMAW);
    return RTEMS_UNSATISFIED;
  }
  record_q[record_produce] = buf;
  record_produce = (record_produce + 1) & RECORD_Q_MASK;
  record_level++;

  if (record_level == 1)
    record_start(buf);

  bsp_interrupt_vector_enable(MM_IRQ_AC97DMAW);
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code collect_record(struct snd_buffer **buf)
{
  size_t s;

  return rtems_message_queue_receive(
    record_q_done,
    buf,
    &s,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
}

#define CR_TIMEOUT 10

static int read_cr(unsigned int adr)
{
  rtems_status_code sc;

  MM_WRITE(MM_AC97_CRADDR, adr);
  MM_WRITE(MM_AC97_CRCTL, AC97_CRCTL_RQEN);
  sc = rtems_semaphore_obtain(cr_write_sem, RTEMS_WAIT, CR_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return -1;
  sc = rtems_semaphore_obtain(cr_read_sem, RTEMS_WAIT, CR_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return -1;
  return MM_READ(MM_AC97_CRDATAIN);
}

static int write_cr(unsigned int adr, unsigned int val)
{
  rtems_status_code sc;

  MM_WRITE(MM_AC97_CRADDR, adr);
  MM_WRITE(MM_AC97_CRDATAOUT, val);
  MM_WRITE(MM_AC97_CRCTL, AC97_CRCTL_RQEN|AC97_CRCTL_WRITE);
  sc = rtems_semaphore_obtain(cr_write_sem, RTEMS_WAIT, CR_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    return 0;
  return 1;
}

rtems_device_driver ac97_open(
   rtems_device_major_number major,
   rtems_device_minor_number minor,
   void *arg
)
{
  int codec_id;
  
  if (minor == 0) {
    /* snd */
    return RTEMS_SUCCESSFUL;
  } else {
    /* mixer */
    codec_id = read_cr(0x00);
    if ((codec_id != 0x0d50) && (codec_id != 0x6150)) {
      printk("AC97 codec detection failed\n");
      return RTEMS_UNSATISFIED;
    }
    write_cr(0x02, 0x0000); /* master volume */
    write_cr(0x04, 0x0f0f); /* headphones volume */
    write_cr(0x18, 0x0000); /* PCM out volume */
    write_cr(0x1c, 0x0f0f); /* record gain */

    write_cr(0x1a, 0x0505); /* record select: stereo mix */

    return RTEMS_SUCCESSFUL;
  }
}

static rtems_status_code ioctl_read_channel(void *buf,
  unsigned int chan, int mono)
{
  unsigned int *val = (unsigned int *)buf;
  int mic_boost;
  int codec;
  int left, right;

  codec = read_cr(chan);
  if (codec < 0)
    return RTEMS_UNSATISFIED;
  if (codec & 0x8000) {
    /* muted */
    *val = 0;
    return RTEMS_SUCCESSFUL;
  }
  if (mono) {
    left = 100-(((codec & 0x1f) + 1)*100)/32;
    mic_boost = (codec & (1 << 6)) >> 6;
    *val = left | mic_boost << 8;
  } else {
    right = 100-(((codec & 0x1f) + 1)*100)/32;
    left = 100-((((codec & 0x1f00) >> 8) + 1)*100)/32;
    *val = left | (right << 8);
  }
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code ioctl_write_channel(void *buf,
  unsigned int chan, int mono)
{
  unsigned int *val = (unsigned int *)buf;
  int mic_boost;
  int left, right;
  int codec;
  rtems_status_code sc;

  left = *val & 0xff;
  left = (left*32)/100 - 1;
  if (left < 0)
    left = 0;

  if (mono) {
    mic_boost = *val >> 8;
    right = 31;
  } else {
    right = (*val >> 8) & 0xff;
    right = (right*32)/100 - 1;
    if (right < 0)
      right = 0;
  }

  if ((left == 0) && (right == 0))
    /* mute */
    codec = 0x8000;
  else
    codec = (31-left) | ((31-right) << 8);

  if (mono) {
    if (mic_boost)
      codec |= (1 << 6);
    else
      codec &= ~(1 << 6);
  }

  if (!write_cr(chan, codec))
    sc = RTEMS_UNSATISFIED;
  else
    sc = RTEMS_SUCCESSFUL;
  return sc;
}

rtems_device_driver ac97_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;
  rtems_status_code sc;

  args->ioctl_return = -1;
  if(minor == 0) {
    /* dsp */
    switch (args->command) {
      case SOUND_SND_SUBMIT_PLAY:
        return submit_play((struct snd_buffer *)args->buffer);
      case SOUND_SND_COLLECT_PLAY:
        return collect_play((struct snd_buffer **)args->buffer);
      case SOUND_SND_SUBMIT_RECORD:
        return submit_record((struct snd_buffer *)args->buffer);
      case SOUND_SND_COLLECT_RECORD:
        return collect_record((struct snd_buffer **)args->buffer);
      default:
        return RTEMS_UNSATISFIED;
    }
  } else {
    /* mixer */
    switch (args->command) {
      case SOUND_MIXER_READ(SOUND_MIXER_MIC):
        sc = ioctl_read_channel(args->buffer, 0x0e, 1);
        if(sc == RTEMS_SUCCESSFUL)
          args->ioctl_return = 0;
        return sc;
      case SOUND_MIXER_READ(SOUND_MIXER_LINE):
        sc = ioctl_read_channel(args->buffer, 0x10, 0);
        if(sc == RTEMS_SUCCESSFUL)
          args->ioctl_return = 0;
        return sc;
      case SOUND_MIXER_WRITE(SOUND_MIXER_MIC):
        sc = ioctl_write_channel(args->buffer, 0x0e, 1);
        if(sc == RTEMS_SUCCESSFUL)
          args->ioctl_return = 0;
        return sc;
      case SOUND_MIXER_WRITE(SOUND_MIXER_LINE):
        sc = ioctl_write_channel(args->buffer, 0x10, 0);
        if(sc == RTEMS_SUCCESSFUL)
          args->ioctl_return = 0;
        return sc;
      default:
        return RTEMS_UNSATISFIED;
    }
  }
}
