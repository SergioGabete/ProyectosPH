;/*****************************************************************************/
;/* SWI.S: SWI Handler                                                        */
;/*****************************************************************************/
;/* This file is part of the uVision/ARM development tools.                   */
;/* Copyright (c) 2005-2006 Keil Software. All rights reserved.               */
;/* This software may only be used under the terms of a valid, current,       */
;/* end user licence from KEIL for a compatible version of KEIL software      */
;/* development tools. Nothing else gives you the right to use this software. */
;/*****************************************************************************/

I_Bit			EQU     0x80
F_Bit			EQU		0x40
T_Bit           EQU     0x20

                PRESERVE8                      ; 8-Byte aligned Stack
                AREA    SWI_Area, CODE, READONLY
                ARM

                EXPORT  SWI_Handler2
SWI_Handler2  

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
                BEQ     enable_isr			;salta a enable_isr
				CMP     R12,#0xFE              
                BEQ     disable_isr		    ;salta a disable_isr
				CMP     R12,#0xFD              
                BEQ     enable_isr_fiq	    ;salta a enable_isr_fiq
				CMP     R12,#0xFC              
                BEQ     disable_isr_fiq     ;salta a disable_isr_fiq

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
enable_isr
				MSR cpsr_c, r8
				BIC r8, r8, #I_Bit
				MSR cpsr_c, r8				   ; Enable IRQ ;MRS cambiao
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

disable_isr

				MSR cpsr_c, r8
				ORR r8, r8, #I_Bit
				MSR cpsr_c, r8				   ; Disable IRQ
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
				
enable_isr_fiq
				MSR cpsr_c, r8
				BIC r8, r8, #F_Bit
				MSR cpsr_c, r8				   ; Enable FIQ
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

disable_isr_fiq
				MSR cpsr_c, r8
				ORR r8, r8, #F_Bit
				MSR cpsr_c, r8				   ; Disable FIQ
                LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
                MSR     SPSR_cxsf, R12         ; Set SPSR
                LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

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
					

