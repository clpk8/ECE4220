#ifndef MODULE
#define MODULE 
#endif

#ifndef __KERNEL__
#define __KERNEL__ 
#endif

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int init_module(void)
{
	unsigned long *basePtr, *set, *sel;
	basePtr = (unsigned long*) ioremap(0x3F200000,4096);
	*sel = *basePtr | 0x9240;
	*set = *basePtr + (0x001C/4);
	 


	return 0;
}



void cleanup_module(void)
{


}


