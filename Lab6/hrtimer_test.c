/* Based on code found at https://gist.github.com/maggocnx/5946907
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
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
unsigned long *bptr, *set,*sel,*clr;
int mydev_id;
//part2
unsigned long setPb = 0x1F0000; //set 5 push button to 1, 0001 1111 0 0 0 0
unsigned long *event,*Pdown,*Penable,*edge;

unsigned long timer_interval_ns = 200000;	// timer interval length (nano sec part)
static struct hrtimer hr_timer;			// timer structure
static int count = 1;

static irqreturn_t button_isr(int irq, void *dev_id)
{
    // In general, you want to disable the interrupt while handling it.
    disable_irq_nosync(79);
    
    // This same handler will be called regardless of what button was pushed,
    // assuming that they were properly configured.
    // How can you determine which button was the one actually pushed?
    
    // use event detect status registers to detect to pin associated to the pushbutton
    
    // DO STUFF (whatever you need to do, based on the button that was pushed)
    printk("%lu\n",*event & setPb);
    if((*event & setPb) == 65536){//10000hex in decimal
        //0 0001 0 0 0 0, 16th bit is push button 1
        timer_interval_ns = 900000;
        printk("button 1 pushed\n");
    }
    else if((*event & setPb) == 131072){//20000 in decimal
        //0 0010 0 0 0 0, 17th bit is PB2
        timer_interval_ns = 750000;
        printk("button 2 pushed\n");
        
    }
    else if((*event & setPb) == 262144){//40000 in decimal
        //0 0100 0 0 0 0, 18th bit is pb3
        timer_interval_ns = 600000;
        printk("button 3 pushed\n");
        
    }
    else if((*event & setPb) == 524288){//80000 in decimal
        //0 1000 0 0 0 0, 19th bit is pb4
        timer_interval_ns = 450000;
        printk("button 4 pushed\n");
        
    }
    else if((*event & setPb) == 1048576){//100000 in decimal
        //0001 0 0 0 0 0 20th bit is pb5
        timer_interval_ns = 300000;
        printk("button 5 pushed\n");
        
    }
    
    // IMPORTANT: Clear the Event Detect status register before leaving.
    *event = *event | setPb;//clear it
    
    printk("Interrupt handled\n");
    enable_irq(79);        // re-enable interrupt
    
    return IRQ_HANDLED;
}

// Timer callback function: this executes when the timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
   

    
  	ktime_t currtime, interval;	// time type, in nanoseconds
	unsigned long overruns = 0;
	
	// Re-configure the timer parameters (if needed/desired)
  	currtime  = ktime_get();
  	interval = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)
	
	// Advance the expiration time to the next interval. This returns how many
	// intervals have passed. More than 1 may happen if the system load is too high.
  	overruns = hrtimer_forward(timer_for_restart, currtime, interval);
	
	
	// The following printk only executes once every 1000 cycles.
	if(count % 2 == 0){
        *set = *set | 0x40; //set 6th bit to be on, which is speaker
        count++;

	}
    else{
        *clr = *clr | 0x40; //clear 6th bit to be 0, which is speaker
        count++;

    }
    
    


	
	return HRTIMER_RESTART;	// Return this value to restart the timer.
							// If you don't want/need a recurring timer, return
							// HRTIMER_NORESTART (and don't forward the timer).
}

int timer_init(void)
{
	// Configure and initialize timer
	ktime_t ktime = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)
	
	// CLOCK_MONOTONIC: always move forward in time, even if system time changes
	// HRTIMER_MODE_REL: time relative to current time.
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	
	// Attach callback function to the timer
	hr_timer.function = &timer_callback;
	
    int irqdummy = 0;

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
    
    irqdummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

    
    
    
	// Start the timer
 	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	
	return 0;
}

void timer_exit(void)
{
	int ret;
    free_irq(79, &mydev_id);

  	ret = hrtimer_cancel(&hr_timer);	// cancels the timer.
  	if(ret)
		printk("The timer was still in use...\n");
	else
		printk("The timer was already canceled...\n");	// if not restarted or
														// canceled before
	
  	printk("HR Timer module uninstalling\n");
	
}

// Notice this alternative way to define your init_module()
// and cleanup_module(). "timer_init" will execute when you install your
// module. "timer_exit" will execute when you remove your module.
// You can give different names to those functions.
module_init(timer_init);
module_exit(timer_exit);
