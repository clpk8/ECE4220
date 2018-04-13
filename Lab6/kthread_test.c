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
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
//base pointer




// structure for the kthread.
static struct task_struct *kthread1;

// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
    unsigned long *basePtr, *set, *sel;
    basePtr = (unsigned long*)ioremap(0x3F200000,4096);
    sel = basePtr;
    set = basePtr;
    *sel = *sel | 0x9240;



	unsigned long j0, j1;
	int count = 0;

	printk("In kthread1\n");
	j0 = jiffies;		// number of clock ticks since system started;
						// current "time" in jiffies
	j1 = j0 + 1*HZ;	// HZ is the number of ticks per second, that is
						// 1 HZ is 1 second in jiffies
	
	while(time_before(jiffies, j1))	// true when current "time" is less than j1
        schedule();		// voluntarily tell the scheduler that it can schedule
						// some other process
	
	printk("Before loop\n");
	
	// The ktrhead does not need to run forever. It can execute something
	// and then leave.
	while(1)
	{
//        *sel = *sel | 0x40040;//turn speaker as output 001 000 000 000 000 000 000
//        set = set + (0x001c / 4);    //GPIO Pin Output Set 0
//        *set = *set & 0x40;
        //test

        basePtr = (unsigned long*)ioremap(0x3F200000,4096);
        sel = basePtr;
        set = basePtr;
        
        //set LED as output 0x 1001 0010 0100 0000 will set bit 2,3,4,5 as output
        *sel = *sel | 0x9240;
        
        //GPSET1 register
        set = set + (0x001C/4);//gpset the pin to 0
        //set 4 led to 1
        *set = *set & ~(0x3C);//complement of 3C
      //  printk("Removed");
        
        
		msleep(1000);	// good for > 10 ms
		//msleep_interruptible(1000); // good for > 10 ms
		//udelay(unsigned long usecs);	// good for a few us (micro s)
		//usleep_range(unsigned long min, unsigned long max); // good for 10us - 20 ms
        
//        *sel = *sel | 0x40040;//turn speaker as output 001 000 000 000 000 000 000
//        set = set + (0x0028 / 4);    //GPIO Pin Output clear 0
//        *set = *set & 0x40;
        basePtr = (unsigned long*)ioremap(0x3F200000,4096);
        sel = basePtr;
        set = basePtr;
        
        //GPFSEL to set bit 2,3,4,5 as output
        *sel = *sel | 0x9240; //GPFSEL which turn LEDS to output  0x001001001001000000
        set = set + (0x0020/4); //gpset the pin to 1
        *set = *set & 0x3C; //set 4 leds to 1        00x 0011 1100

        msleep(1000);
		// In an infinite loop, you should check if the kthread_stop
		// function has been called (e.g. in clean up module). If so,
		// the kthread should exit. If this is not done, the thread
		// will persist even after removing the module.
		if(kthread_should_stop()) {
			do_exit(0);
		}
				
		// comment out if your loop is going "fast". You don't want to
		// printk too often. Sporadically or every second or so, it's okay.
		printk("Count: %d\n", ++count);
	}
	
	return 0;
}

int thread_init(void)
{
	char kthread_name[11]="my_kthread";	// try running  ps -ef | grep my_kthread
										// when the thread is active.
	printk("In init module\n");



    kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
    if((kthread1))	// true if kthread creation is successful
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
