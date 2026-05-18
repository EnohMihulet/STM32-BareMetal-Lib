.syntax unified
.cpu cortex-m4
.thumb

.global VectorTable
.global EntryPoint
.global DefaultHandler

.extern main

/* These symbols must be provided by the linker script */
.extern _estack
.extern _sdata
.extern _edata
.extern _sidata
.extern _sbss
.extern _ebss


.section .text.EntryPoint, "ax", %progbits
.weak EntryPoint
.type EntryPoint, %function
.thumb_func

EntryPoint:
	ldr sp, =_estack

/* Copy .data from FLASH to RAM */
CopyDataInit:
	ldr r0, =_sdata      /* RAM destination start */
	ldr r1, =_edata      /* RAM destination end */
	ldr r2, =_sidata     /* FLASH source start */

CopyDataInitLoop:
	cmp r0, r1
	bcc CopyDataCopy
	b ZeroBss

CopyDataCopy:
	ldr r3, [r2], #4
	str r3, [r0], #4
	b CopyDataInitLoop


/* Zero .bss */
ZeroBss:
	ldr r0, =_sbss
	ldr r1, =_ebss
	movs r2, #0

ZeroBssLoop:
	cmp r0, r1
	bcc ZeroBssStore
	b CallMain

ZeroBssStore:
	str r2, [r0], #4
	b ZeroBssLoop

/* Branch to main */
CallMain:
	bl main

/* If main returns, trap forever */
MainReturned:
	b MainReturned

.size EntryPoint, . - EntryPoint


.section .text.DefaultHandler, "ax", %progbits
.type DefaultHandler, %function
.thumb_func

DefaultHandler:
InfiniteLoop:
	b InfiniteLoop

.size DefaultHandler, . - DefaultHandler


/* Weak aliases for exception handlers */
.weak NMI_Handler
.thumb_set NMI_Handler, DefaultHandler

.weak HardFault_Handler
.thumb_set HardFault_Handler, DefaultHandler

.weak MemManage_Handler
.thumb_set MemManage_Handler, DefaultHandler

.weak BusFault_Handler
.thumb_set BusFault_Handler, DefaultHandler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, DefaultHandler

.weak SVC_Handler
.thumb_set SVC_Handler, DefaultHandler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, DefaultHandler

.weak PendSV_Handler
.thumb_set PendSV_Handler, DefaultHandler

.weak SysTick_Handler
.thumb_set SysTick_Handler, DefaultHandler


.section .isr_vector, "a", %progbits
.type VectorTable, %object
.align 2

VectorTable:
	.word _estack
	.word EntryPoint

	.word NMI_Handler
	.word HardFault_Handler
	.word MemManage_Handler
	.word BusFault_Handler
	.word UsageFault_Handler
	.word 0
	.word 0
	.word 0
	.word 0
	.word SVC_Handler
	.word DebugMon_Handler
	.word 0
	.word PendSV_Handler
	.word SysTick_Handler

.size VectorTable, . - VectorTable
