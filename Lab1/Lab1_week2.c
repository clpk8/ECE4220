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
	printk("Installed");
	unsigned long *basePtr, *set, *sel,*clr;
	basePtr = (unsigned long*)ioremap(0x3F200000,4096);
	sel = basePtr;
	set = basePtr;


	*sel = *sel | 0x9240; //GPFSEL which turn LEDS to output
	*set = *set + (0x001C/4); //gpset the pin to 1
	*set = *set &0x00;
	printk("Installed");


	//                      00000111100  ==  0x3C

	return 0;
}



void cleanup_module(void)
{
	printk("Removed");
	unsigned long *basePtr, *set, *sel,*clr;
	basePtr = (unsigned long*)ioremap(0x3F200000,4096);
	sel = basePtr;
	set = basePtr;
	clr = basePtr;

	*sel = *sel | 0x9240;
	*set = *set + (0x0020/4);
	*set = *set & 0x33;
	printk("Removed");


}


