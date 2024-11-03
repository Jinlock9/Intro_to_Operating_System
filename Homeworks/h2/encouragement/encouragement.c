#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(encouragement)
{
    printk(KERN_INFO "Mum is proud of you!\n");
    return 0;
}
