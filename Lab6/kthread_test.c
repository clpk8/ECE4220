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
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>

#define MSG_SIZE 40
#define CDEV_NAME "Lab6"    // "YourDevName"

//part3
static int major;
//created 2 msg because sometime read will overwrite write
static char msg[MSG_SIZE];
static char msg2[MSG_SIZE];

MODULE_LICENSE("GPL");
unsigned long *bptr, *set,*sel,*clr;//part 1, only speaker
long fqcy;
int mydev_id;

//part2
unsigned long setPb = 0x1F0000; //set 5 push button to 1, 0001 1111 0 0 0 0
unsigned long *event,*Pdown,*Penable,*edge;//part2


//function called when user space program reads the chardev
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    if(fqcy == 900){
        strcpy(msg,"@A");
    }
    else if(fqcy == 750){
        strcpy(msg,"@B");
    }
    else if(fqcy == 600){
        strcpy(msg,"@C");
    }
    else if(fqcy == 450){
        strcpy(msg,"@D");
    }
    else if(fqcy == 300){
        strcpy(msg,"@E");
    }
    else{
        msg[0] = '\0';
    }
    // Whatever is in msg will be placed into buffer, which will be copied into user space
    ssize_t dummy = copy_to_user(buffer, msg, length);    // dummy will be 0 if successful

    // msg should be protected (e.g. semaphore). Not implemented here, but you can add it.
    msg[0] = '\0';    // "Clear" the message, in case the device is read again.
    // This way, the same message will not be read twice.
    // Also convenient for checking if there is nothing new, in user space.

    return length;
}
// Function called when the user space program writes to the Character Device.
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
    ssize_t dummy;

    if(len > MSG_SIZE)
        return -EINVAL;

    // unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
    dummy = copy_from_user(msg2, buff, len);    // Transfers the data from user space to kernel space
//    if(len == MSG_SIZE)
//        msg[len-1] = '\0';    // will ignore the last character received.
//    else
//        msg[len] = '\0';

    // You may want to remove the following printk in your final version.
    printk("Message from user space: %s\n", msg2);
    if (msg2[0] == 'A'){
        fqcy = 900;
    }
    else if(msg2[0] == 'B'){
        fqcy = 750;
    }
    else if(msg2[0] == 'C'){
        fqcy = 600;
    }
    else if(msg2[0] == 'D'){
        fqcy = 450;
    }
    else if(msg2[0] == 'E'){
        fqcy = 300;
    }
    else{
        printk("Nothing, :%c\n",msg2[1]);
    }


    return len;        // the number of bytes that were written to the Character Device.
}
// structure needed when registering the Character Device. Members are the callback
// functions when the device is read from or written to.
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
};



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
    printk("%lu\n",*event & setPb);
    if((*event & setPb) == 65536){//10000hex in decimal
        //0 0001 0 0 0 0, 16th bit is push button 1
        fqcy = 900;
        printk("button 1 pushed\n");
    }
    else if((*event & setPb) == 131072){//20000 in decimal
        //0 0010 0 0 0 0, 17th bit is PB2
        fqcy = 750;
        printk("button 2 pushed\n");

    }
    else if((*event & setPb) == 262144){//40000 in decimal
        //0 0100 0 0 0 0, 18th bit is pb3
        fqcy = 600;
        printk("button 3 pushed\n");

    }
    else if((*event & setPb) == 524288){//80000 in decimal
        //0 1000 0 0 0 0, 19th bit is pb4
        fqcy = 450;
        printk("button 4 pushed\n");

    }
    else if((*event & setPb) == 1048576){//100000 in decimal
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
//kthread2
//read, and comapre.




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
    //Part 3
    // register the Characted Device and obtain the major (assigned by the system)
    major = register_chrdev(0, CDEV_NAME, &fops);
    if (major < 0) {
        printk("Registering the character device failed with %d\n", major);
        return major;
    }
    printk("Lab6_cdev_kmod example, assigned major: %d\n", major);
    printk("Create Char Device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, major);

    int dummy = 0;
    fqcy = 200;//set it to a default value
    //create another thread
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

    //request irq
    dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

    //creating kthread
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
    unregister_chrdev(major, CDEV_NAME);
    printk("Char Device /dev/%s unregistered.\n", CDEV_NAME);

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

