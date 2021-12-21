;/*****************************************************************************/
;/* SWI.S: SWI Handler                                                        */
;/*****************************************************************************/
;/* This file is part of the uVision/ARM development tools.                   */
;/* Copyright (c) 2005-2006 Keil Software. All rights reserved.               */
;/* This software may only be used under the terms of a valid, current,       */
;/* end user licence from KEIL for a compatible version of KEIL software      */
;/* development tools. Nothing else gives you the right to use this software. */
;/*****************************************************************************/


T_Bit           EQU     0x20
I_Bit			EQU     0x80
F_Bit			EQU		0x40

                PRESERVE8                      ; 8-Byte aligned Stack
                AREA    SWI_Area, CODE, READONLY
                ARM


;Esta funci�n se encarga de determinar  el n�mero de
;interrupciones que ha ocurrido y seg�n este se dirige a una de las llamadas al sistema, tras
;esto volver� a modo usuario donde se produjo la interrupcion
                EXPORT  SWI_Handler
SWI_Handler

                STMFD   SP!, {R12, LR}         ; Store R12, LR
                MRS     R12, SPSR              ; Get SPSR
                STMFD   SP!, {R8, R12}         ; Store R8, SPSR
                TST     R12, #T_Bit            ; Check Thumb Bit
                LDRNEH  R12, [LR,#-2]          ; Thumb: Load Halfword
                BICNE   R12, R12, #0xFF00      ; Extract SWI Number
                LDREQ   R12, [LR,#-4]          ; ARM:   Load Word
                BICEQ   R12, R12, #0xFF000000  ; Extract SWI Number

; add code to enable/disable the global IRQ flag
                CMP     R12,#0xFF              
                BEQ     __enable_isr			;salta a enable_isr
				CMP     R12,#0xFE              
                BEQ     __disable_isr		    ;salta a disable_isr
				CMP     R12,#0xFD              
                BEQ     __enable_isr_fiq	    ;salta a enable_isr_fiq
				CMP     R12,#0xFC              
                BEQ     __disable_isr_fiq     ;salta a disable_isr_fiq

                LDR     R8, SWI_Count
                CMP     R12, R8
                BHS     SWI_Dead               ; Overflow
                ADR     R8, SWI_Table
                LDR     R12, [R8,R12,LSL #2]   ; Load SWI Function Address
                MOV     LR, PC                 ; Return Address
                BX      R12                    ; Call SWI Function 

                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

SWI_Dead        B       SWI_Dead               ; None Existing SWI

SWI_Cnt         EQU    (SWI_End-SWI_Table)/4
SWI_Count       DCD     SWI_Cnt

                IMPORT  __SWI_0
;               IMPORT  __SWI_1
;               IMPORT  __SWI_2
;               IMPORT  __SWI_3
SWI_Table
                DCD     __SWI_0                ; SWI 0 Function Entry
;               DCD     __SWI_1                ; SWI 1 Function Entry
;               DCD     __SWI_2                ; SWI 2 Function Entry
;               DCD     __SWI_3                ; SWI 3 Function Entry

;               ...
SWI_End

                EXTERN shared_var [DATA,SIZE=4]

;__decrease_var
;                LDR R8, =shared_var
;		LDR R12, [r8]
;                SUB R12, R12, #1
;                STR R12, [R8]
;                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
;                MSR     SPSR_cxsf, R12         ; Set SPSR
;                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return


;                END


__enable_isr				
				MRS r1,CPSR
				BIC r1, r1, #I_Bit
				MSR CPSR_c,r1
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

__disable_isr

				MRS r12, cpsr
				ORR r12, r12, #I_Bit ;disable IRQs
				MSR cpsr_c, r12
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
				
__enable_isr_fiq

				MRS r8,CPSR
				BIC r8, r8, #0xc0
				MSR CPSR_c,r8
				
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
				
__disable_isr_fiq

				MRS r8, cpsr
				ORR r8, r8, #0xc0
				MSR cpsr_c, r8
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 a

;disable_IRQ
;                LDMFD   SP!, {R8, R12}    ; Load R8, SPSR
;                ORR     R12, R12, #0x80   ; Set IRQ flag to disable it
;                MSR     SPSR_cxsf, R12    ; Set SPSR
;                LDMFD   SP!, {R12, PC}^   ; Restore R12 and Return

;enable_IRQ
;                LDMFD   SP!, {R8, R12}    ; Load R8, SPSR
;                BIC     R12, R12, #0x80   ; Set IRQ flag to disable it
;                MSR     SPSR_cxsf, R12    ; Set SPSR
;                LDMFD   SP!, {R12, PC}^   ; Restore R12 and Return

				END
					

