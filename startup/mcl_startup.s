.syntax unified
.cpu cortex-m4
.thumb

.global VectorTable
.global EntryPoint
.global Default_Handler

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

/* Enable CP10/CP11 FPU access for the hard-float build. */
	ldr r0, =0xE000ED88
	ldr r1, [r0]
	ldr r2, =0x00F00000
	orrs r1, r1, r2
	str r1, [r0]
	dsb
	isb

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


.section .text.Default_Handler, "ax", %progbits
.type Default_Handler, %function
.thumb_func

Default_Handler:
InfiniteLoop:
	b InfiniteLoop

.size Default_Handler, . - Default_Handler


/* Weak aliases for exception handlers */
.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak SVC_Handler
.thumb_set SVC_Handler, Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler

.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler

.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler

.weak EXTI0_IRQHandler
.thumb_set EXTI0_IRQHandler, Default_Handler

.weak EXTI1_IRQHandler
.thumb_set EXTI1_IRQHandler, Default_Handler

.weak EXTI2_IRQHandler
.thumb_set EXTI2_IRQHandler, Default_Handler

.weak EXTI3_IRQHandler
.thumb_set EXTI3_IRQHandler, Default_Handler

.weak EXTI4_IRQHandler
.thumb_set EXTI4_IRQHandler, Default_Handler

.weak EXTI9_5_IRQHandler
.thumb_set EXTI9_5_IRQHandler, Default_Handler

.weak TIM2_IRQHandler
.thumb_set TIM2_IRQHandler, Default_Handler

.weak TIM3_IRQHandler
.thumb_set TIM3_IRQHandler, Default_Handler

.weak TIM4_IRQHandler
.thumb_set TIM4_IRQHandler, Default_Handler

.weak EXTI15_10_IRQHandler
.thumb_set EXTI15_10_IRQHandler, Default_Handler

.weak TIM5_IRQHandler
.thumb_set TIM5_IRQHandler, Default_Handler


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
	.rept 6
		.word Default_Handler
	.endr
	.word EXTI0_IRQHandler
	.word EXTI1_IRQHandler
	.word EXTI2_IRQHandler
	.word EXTI3_IRQHandler
	.word EXTI4_IRQHandler
	.rept 12
		.word Default_Handler
	.endr
	.word EXTI9_5_IRQHandler
	.rept 4
		.word Default_Handler
	.endr
	
	.word TIM2_IRQHandler
	.word TIM3_IRQHandler
	.word TIM4_IRQHandler
	.rept 9
		.word Default_Handler
	.endr
	.word EXTI15_10_IRQHandler
	.rept 9
		.word Default_Handler
	.endr
	.word TIM5_IRQHandler

.size VectorTable, . - VectorTable
