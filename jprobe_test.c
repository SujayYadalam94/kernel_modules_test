/*
 Source obtained from the below link:
 https://opensourceforu.com/2011/04/kernel-debugging-using-kprobe-and-jprobe/
*/

#include<linux/module.h> 
#include<linux/version.h> 
#include<linux/kernel.h> 
#include<linux/init.h> 
#include<linux/kprobes.h> 
#include<net/ip.h> 

/* The below macros are used to print the IP address.
   It was present in older versions of the kernel.
   Need to add it to the newer versions */
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

int my_handler (struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev){ 
 
    struct iphdr *my_iph; 
    u32 S_ip,D_ip; 
    my_iph = ip_hdr(skb); 
    S_ip = my_iph->saddr; 
    D_ip = my_iph->daddr; 
    printk("Source IP: \n"NIPQUAD_FMT,NIPQUAD(S_ip)); 
    jprobe_return(); 
} 
 
static struct jprobe my_probe; 
 
int myinit(void) 
{ 
int ret;
    my_probe.kp.addr = (kprobe_opcode_t *)0xffffffff8a037010; 
    my_probe.entry = (kprobe_opcode_t *)my_handler; 
ret = register_jprobe(&my_probe); 
if(ret == 0)
	printk(KERN_INFO "Module initialized\n");
else
	printk(KERN_ERR "Register return value=%d\n",ret);
    return 0; 
} 
 
void myexit(void) 
{ 
    unregister_jprobe(&my_probe); 
    printk(KERN_INFO "module removed\n "); 
} 
 
module_init(myinit); 
module_exit(myexit); 
 
/*Kernel module Comments*/
MODULE_AUTHOR("Manoj"); 
MODULE_DESCRIPTION("SIMPLE MODULE"); 
MODULE_LICENSE("GPL"); 
//MODULE_LICENSE("GPL v2");
