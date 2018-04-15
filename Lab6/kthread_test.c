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
#include <asm/io.h>
#include <linux/interrupt.h>


MODULE_LICENSE("GPL");
unsigned long *bptr, *set,*sel,*clr;
int fqcy,mydev_id;

//part2
unsigned long setPb = 0x1F0000; //set 5 push button to 1, 0001 1111 0 0 0 0
unsigned long *event,*Pdown,*Penable,*edge;

// structure for the kthread.
static struct task_struct *kthread1;

static irqreturn_t button_isr(int irq, void *dev_id)
{
    // In general, you want to disable the interrupt while handling it.
    disable_irq_nosync(79);
    
    // This same handler will be called regardless of what button was pushed,
    // assuming that they were properly configured.
    // How can you determine which button was the one actually pushed?
    
    // use event detect status registers to detect to pin associated to the pushbutton
    
    // DO STUFF (whatever you need to do, based on the button that was pushed)
    if((*event & setPb) == 10000){
        //0 0001 0 0 0 0, 16th bit is push button 1
        fqcy = 900;
        printk("button 1 pushed\n");
    }
    else if((*event & setPb) == 20000){
        //0 0010 0 0 0 0, 17th bit is PB2
        fqcy = 750;
        printk("button 2 pushed\n");

    }
    else if((*event & setPb) == 40000){
        //0 0100 0 0 0 0, 18th bit is pb3
        fqcy = 600;
        printk("button 3 pushed\n");

    }
    else if((*event & setPb) == 80000){
        //0 1000 0 0 0 0, 19th bit is pb4
        fqcy = 450;
        printk("button 4 pushed\n");

    }
    else if((*event & setPb) == 100000){
        //0001 0 0 0 0 0 20th bit is pb5
        fqcy = 300;
        printk("button 5 pushed\n");

    }

    // IMPORTANT: Clear the Event Detect status register before leaving.
    *event = *event | setPb;//clear it
    
    printk("Interrupt handled\n");
    enable_irq(79);        // re-enable interrupt
    
    return IRQ_HANDLED;
}

// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
   
    printk("Before loop\n");

    while(1)
    {
        *set = *set | 0x40; //set 6th bit to be on, which is speaker

        udelay(fqcy);    // good for a few us (micro s)
       
        
        
        *clr = *clr | 0x40; //clear 6th bit to be 0, which is speaker
        
        udelay(fqcy);    // good for a few us (micro s)
        
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
    int dummy = 0;
    fqcy = 200;
    char kthread_name[11]="my_kthread";    // try running  ps -ef | grep my_kthread
    // when the thread is active.
    printk("In init module\n");
    
    bptr = (unsigned long *) ioremap(0x3F200000, 4096);
    sel = bptr;
    set = bptr + 0x1C/4; //point at gpset register
    clr = bptr + 0x28/4; //point at gpclr register
    
    *sel = *sel | 0x40000;//turn speaker as output 001 000 000 000 000 000 000

    
    
    //part 2
    event = bptr + 0x40/4; //even detect
    
    //pull-down setting
    //1)
    Pdown = bptr + 0x94/4;//point at gppud register
    *Pdown = *Pdown | 0x155; //0001 0101 0101 enable pud as pull down control
    //2)
    udelay(100); //wait 150 cycle
    //3)
    Penable = bptr + 0x98/4;//point at gppudclk0 to enable clock
    *Penable = *Penable | setPb;//only set the pin corsponding to push button to asynchronous falling edge
    //4)
    udelay(100); //wait 150 cycle
    //5)
    *Pdown = *Pdown & ~(0x155);//remove the control signal, only apply to the bit we turn to 1 before
    //6)
    *Penable = *Penable & ~(setPb);//remove the clock, only to the bit associated to button
    
    
    // Enable (Async) Rising Edge detection for all 5 GPIO ports.
    edge = bptr + 0x4C/4;//point at rising edge detect enable 0
    *edge = *edge | setPb;
    
    dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

    
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
    free_irq(79, &mydev_id);

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

