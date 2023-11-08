#ifndef __LINUX_WORKQUEUE_H__
#define __LINUX_WORKQUEUE_H__

#include <rtems/chain.h>
#include <linux/mutex.h>

struct work_struct { rtems_chain_node node; };

#define queue_delayed_work(workqueue, delayed_work, delay_ms) ({ \
  jffs2_queue_delayed_work(delayed_work, delay_ms); \
  0; \
})

#define INIT_DELAYED_WORK(delayed_work, delayed_workqueue_callback) ({ \
  (delayed_work)->callback = delayed_workqueue_callback; \
})

#define msecs_to_jiffies(a) (a)

typedef void (*work_callback_t)(struct work_struct *work);
struct delayed_work {
	struct work_struct work;
	struct mutex dw_mutex;
	volatile bool pending;
	volatile uint64_t execution_time;
	work_callback_t callback;
	/* Superblock provided for locking */
	struct super_block *sb;
};

#define to_delayed_work(work) RTEMS_CONTAINER_OF(work, struct delayed_work, work)
void jffs2_queue_delayed_work(struct delayed_work *work, int delay_ms);

#endif /* __LINUX_WORKQUEUE_H__ */
