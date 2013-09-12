#ifndef __LINUX_KERNEL_H__
#define __LINUX_KERNEL_H__

#define jiffies ((unsigned long)cyg_current_time())

#define ERR_PTR(err) ((void*)(err))
#define PTR_ERR(err) ((unsigned long)(err))
#define IS_ERR(err) ((unsigned long)err > (unsigned long)-1000L)

#define CURRENT_TIME cyg_timestamp()

#define	KERN_EMERG              "<0>" // system is unusable
#define	KERN_ALERT              "<1>" // action must be taken immediately
#define	KERN_CRIT               "<2>" // critical conditions
#define	KERN_ERR                "<3>" // error conditions
#define	KERN_WARNING            "<4>" // warning conditions
#define	KERN_NOTICE             "<5>" // normal but significant condition
#define	KERN_INFO               "<6>" // informational
#define	KERN_DEBUG              "<7>" // debug-level messages
#define printk diag_printf

#define min(x,y) (x<y?x:y)
#define max(x,y) (x<y?y:x)
#define min_t(t, x,y) ((t)x<(t)y?(t)x:(t)y)


#endif /* __LINUX_KERNEL_H__ */




