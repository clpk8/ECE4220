/* Based on example from: http://tuxthink.blogspot.com/2011/02/kernel-thread-creation-1.html
 Modified and commented by: Luis Rivera
 
 Compile using the Makefile
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>    // for kthreads
#include <linux/sched.h>    // for task_struct
#include <linux/time.h>        // for using jiffies
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
unsigned long *bptr, *set,*sel,*clr;
// structure for the kthread.
static struct task_struct *kthread1;

// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
   
    printk("Before loop\n");

    while(1)
    {
        *set = *set | 0x40;

        udelay(1000);    // good for a few us (micro s)
        //msleep_interruptible(1000); // good for > 10 ms
        //udelay(unsigned long usecs);    // good for a few us (micro s)
        //usleep_range(unsigned long min, unsigned long max); // good for 10us - 20 ms
        
        
        *clr = *clr | 0x40;
        
        udelay(1000);    // good for a few us (micro s)
        
        // In an infinite loop, you should check if the kthread_stop
        // function has been called (e.g. in clean up module). If so,
        // the kthread should exit. If this is not done, the thread
        // will persist even after removing the module.
        if(kthread_should_stop()) {
            do_exit(0);
        }
        
        // comment out if your loop is going "fast". You don't want to
        // printk too often. Sporadically or every second or so, it's okay.
    }
    
    return 0;
}

int thread_init(void)
{
    
    char kthread_name[11]="my_kthread";    // try running  ps -ef | grep my_kthread
    // when the thread is active.
    printk("In init module\n");
    
    bptr = (unsigned long *) ioremap(0x3F200000, 4096);
    sel = bptr;
    set = bptr + 0x1C/4; //point at gpset register
    clr = bptr + 0x28/4; //point at gpclr register
    
    *sel = *sel | 0x40000;//turn speaker as output 001 000 000 000 000 000 000

    
    
    kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
    
    if((kthread1))    // true if kthread creation is successful
    {
        printk("Inside if\n");
        // kthread is dormant after creation. Needs to be woken up
        wake_up_process(kthread1);
    }
    
    return 0;
}

void thread_cleanup(void) {
    int ret;
    // the following doesn't actually stop the thread, but signals that
    // the thread should stop itself (with do_exit above).
    // kthread should not be called if the thread has already stopped.
    ret = kthread_stop(kthread1);
    
    if(!ret)
        printk("Kthread stopped\n");
}

// Notice this alternative way to define your init_module()
// and cleanup_module(). "thread_init" will execute when you install your
// module. "thread_cleanup" will execute when you remove your module.
// You can give different names to those functions.
module_init(thread_init);
module_exit(thread_cleanup);

