#ifndef __LINUX_SCHED_H__
#define __LINUX_SCHED_H__

#define schedule() do { } while(0)
#define cond_resched() do { } while(0)
#define signal_pending(x) (0)
#define set_current_state(x) do { } while (0)

#endif /* __LINUX_SCHED_H__ */
