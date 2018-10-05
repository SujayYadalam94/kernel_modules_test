//-------------------------------------------------------------------
//	pgfaults.c
//
//	This module installs a kernel function which intercepts all
//	'page-fault' exceptions and reports diagnostic information.
//
//	NOTE: Developed and tested on Linux kernel version 2.4.20.
//
//	programmer: ALLAN CRUSE
//	date begun: 17 MAR 2003
//-------------------------------------------------------------------

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>	// for init_module() 
#include <linux/mm.h>		// for get_free_page()

#define	SUCCESS	0
#define PGFAULT_INT 0x0E

static char modname[] = "pgfaults";
static unsigned short oldidtr[4], newidtr[4];
static unsigned long long *oldidt, *newidt;
static unsigned long isr_orig, kpage;
static char *why[]={ "sra", "srp", "swa", "swp", "ura", "urp", "uwa", "uwp" };

static void my_intercept( unsigned long *tos )
{
	// stack-layout:
	// es,ds,edi,esi,ebp,esp,ebx,edx,ecx,eax,err,eip,cs,efl
	//  0  1   2   3   4   5   6   7   8   9  10  11 12  13  
	volatile unsigned long	vaddr;
	unsigned long	err = tos[ 10 ];	
	unsigned long	eip = tos[ 11 ];
	static int	count = 0;
	int		exe, len = 0;
	char		msg[80]="";
	
	// get the faulting virtual address from register CR2
	asm(" mov %%cr2, %%eax ; movl %%eax, %0 " : "=m" (vaddr) ); 

	// construct the diagnostic message
	len += sprintf( msg+len, "#%-6d ", ++count );
	len += sprintf( msg+len, "%16s  ", current->comm );
	len += sprintf( msg+len, "pid=%-5d  ", current->pid );
	len += sprintf( msg+len, "CR2=%08X  ", vaddr );
	len += sprintf( msg+len, "EIP=%08X  ", eip );
	len += sprintf( msg+len, "%s ", why[ err ] );
	// note if an instruction-fetch caused the page-fault
	if ( vaddr == eip ) exe = 'x'; else exe = ' ';
	len += sprintf( msg+len, "%c ", exe );
	// print this diagnostic message to the kernel log
	printk( "<1>  %s \n", msg );
}




//----------  NEW PAGE-FAULT EXCEPTION-HANDLER  ---------//
asmlinkage void isr0x0E( void );
asm("	.text						");
asm("	.type	isr0x0E, @function			");
asm("isr0x0E:						");
//asm("	pushal						");
/* SUJAY: make sure this is correct */
asm("	pushq	%rax					");
asm("	pushq	%rcx					");
asm("	pushq	%rdx					");
asm("	pushq	%r8					");
asm("	pushq	%r9					");
asm("	pushq	%r10					");
asm("	pushq	%r11					");

/* The segment registers hold no value in long mode. Link:
https://stackoverflow.com/questions/6837392/how-to-save-the-registers-on-x86-64-for-an-interrupt-service-routine */
//asm("	push	%ds					");	
//asm("	push	%es					");
//
//asm("	movl	%ss, %eax				");	
//asm("	movl	%eax, %ds				");
//asm("	movl	%eax, %es				");
//
//asm("	push	%esp					");	
asm("	call	my_intercept				");
//asm("	addl	$4, %esp				");
//
//asm("	pop	%es					");	
//asm("	pop	%ds					");
//asm("	popa						");
/* SUJAY:make sure this is correct */				
asm("	popq	%r11					");
asm("	popq	%r10					");
asm("	popq	%r9					");
asm("	popq	%r8					");
asm("	popq	%rdx					");
asm("	popq	%rcx					");
asm("	popq	%rax					");

asm("	jmp	*isr_orig				");
//-------------------------------------------------------//



static void load_IDTR( void *regimage )
{
	asm(" lidt %0 " : : "m" (*(unsigned short*)regimage) );
}



int __init init_module( void )
{
	unsigned long long	gate_desc;
	int			i;

	// allocate a mapped kernel page for our new IDT
	kpage = __get_free_page( GFP_KERNEL );
	if ( !kpage ) return -ENOMEM;

	// initialize our other global variables
	asm(" sidt oldidtr ; sidt newidtr ");
	/* SUJAY: Check if we need to copy more/less than 6 bytes) */
	memcpy( newidtr+1, &kpage, 6 );
	oldidt = (unsigned long long *)(*(unsigned long*)(oldidtr+1));
	newidt = (unsigned long long *)(*(unsigned long*)(newidtr+1));

	// extract and save entry-point to original page-pault handler
	gate_desc = oldidt[ PGFAULT_INT ];
	gate_desc &= 0xFFFF00000000FFFF;
	gate_desc |= ( gate_desc >> 32 );
	isr_orig = (unsigned long)gate_desc;

	// initialize our new Interrupt Descriptor Table
	memcpy( newidt, oldidt, 256*sizeof( unsigned long long ) );
	gate_desc = (unsigned long)isr0x0E;
	gate_desc &= 0x00000000FFFFFFFF;
	gate_desc |= ( gate_desc << 32 );
	gate_desc &= 0xFFFF00000000FFFF;
	gate_desc |= 0x0000EF0000100000;	// trap-gate
	newidt[ PGFAULT_INT ] = gate_desc;

	// activate the new IDT
	load_IDTR( newidtr );
	smp_call_function( load_IDTR, newidtr, 1);
	return	SUCCESS;
}



void __exit cleanup_module( void )
{
	// reactivate the old IDT
	load_IDTR( oldidtr );
	smp_call_function( load_IDTR, oldidtr, 1);
	
	// release allocated kernel page 
	if ( kpage ) free_page( kpage );
}

MODULE_LICENSE("GPL"); 


