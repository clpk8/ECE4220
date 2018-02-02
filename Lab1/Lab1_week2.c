#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <asm/io.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int init_module(void)
{
	printk("Installed");//print out information

    //declear pointers
	unsigned long *basePtr, *set, *sel,*clr;

    //base pointer
	basePtr = (unsigned long*)ioremap(0x3F200000,4096);
	sel = basePtr;
	set = basePtr;

    //GPFSEL to set bit 2,3,4,5 as output
	*sel = *sel | 0x9240; //GPFSEL which turn LEDS to output
	*set = *set + (0x0020/4); //gpset the pin to 1
    *set = *set & 0x3C; //set 4 leds to 1
	printk("Installed");


	//                      00000111100  ==  0x3C

	return 0;
}



void cleanup_module(void)
{
    //print out information
	printk("Removed");

    //declear pointers
	unsigned long *basePtr, *set, *sel,*clr;
	basePtr = (unsigned long*)ioremap(0x3F200000,4096);
	sel = basePtr;
	set = basePtr;

    //set LED as output 0x 1001 0010 0100 0000 will set bit 2,3,4,5 as output
	*sel = *sel | 0x9240;

    //GPSET1 register
	*set = *set + (0x001C/4);//gpset the pin to 0
    //set 4 led to 1
	*set = *set & ~(0x3C);
	printk("Removed");


}


