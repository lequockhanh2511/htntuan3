.syntax unified
.cpu cortex-m3
.thumb

.global _estack
.global Reset_Handler

.extern main

.section .isr_vector,"a",%progbits
.word _estack
.word Reset_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word 0
.word 0
.word 0
.word 0
.word Default_Handler
.word Default_Handler
.word 0
.word Default_Handler
.word Default_Handler
.word Default_Handler

.section .text
.thumb_func

Reset_Handler:
    bl main

Loop:
    b Loop

.thumb_func

Default_Handler:
    b Default_Handler
