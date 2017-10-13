.global swap_context
.global save_context
.global save_register_context
.global restore_register_context

#include <avr/io.h>

#define XL r26
#define XH r27
#define YL r28
#define YH r29
#define ZL r30
#define ZH r31

#define SPLO    61
#define SPHI    62
#define SR      63

;
; USE r18-r27 and r30-r31 without save/restoring
;

;--------------------------
; save_register_context
;
; pushes our call saved regs and simply saves the stack pointer
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;--------------------------
_save_register_context:
	MOV		ZL, r24				; Point Z register at TCB
	MOV		ZH, r25

	ADIW	Z,	2				; Skip to register values

	ST		Z+,	R0				; Store register context
	ST		Z+,	R1
	ST		Z+,	R2
	ST		Z+,	R3
	ST		Z+,	R4
	ST		Z+,	R5
	ST		Z+,	R6
	ST		Z+,	R7
	ST		Z+,	R8
	ST		Z+,	R9
	ST		Z+,	R10
	ST		Z+,	R11
	ST		Z+,	R12
	ST		Z+,	R13
	ST		Z+,	R14
	ST		Z+,	R15
	ST		Z+,	R16
	ST		Z+,	R17
	ST		Z+,	R18				; Optional - param list
	ST		Z+,	R19				; Optional - param list
	ST		Z+,	R20				; Optional - param list
	ST		Z+,	R21				; Optional - param list
	ST		Z+,	R22				; Optional - param list
	ST		Z+,	R23				; Optional - param list
	ST		Z+,	R24				; Optional - param list
	ST		Z+,	R25				; Optional - param list
	ST		Z+,	R28
	ST		Z+,	R29

	IN		XL, SPLO			; Point X register at stack
	IN		XH, SPHI

	ADIW	X, 4				; rewind past the call of the context switcher to get the return address.

	ST		Z+, XL				; store the Stack Pointer
	ST		Z+, XH

						
	LD		R1, X				
	ST		Z+, R1

	LD		R1, -X				
	ST		Z+, R1

	CLR		R1					; Restore register

	RET

;--------------------------
; restore_register_context
;
; simply restores the stack pointer in the case of the AVR
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;--------------------------
_restore_register_context:
	MOV		ZL, r24				; Point Z register at TCB
	MOV		ZH, r25
	
	ADIW	Z, 30				; Skip to SP
	LD		XL, Z+
	LD		XH, Z+

	SBIW	Z, 32				; Skip back to STACK_BASE
	LD		YL, Z+
	LD		YH, Z+

	SUB		YL, XL				; If a stack is present, rewind stack pointer by 2 bytes so we can rewrite the return address
	SUB		YH, XH

	ADIW	Y, 0				; Zero test
	BREQ	RESTORE_REGISTERS	; If no stack is present, don't rewind the Stack Pointer.

RESTORE_REGISTERS:
	LD		R0, Z+				; Restore register context
	LD		R1, Z+
	LD		R2, Z+
	LD		R3, Z+
	LD		R4, Z+
	LD		R5, Z+
	LD		R6, Z+
	LD		R7, Z+
	LD		R8, Z+
	LD		R9, Z+
	LD		R10, Z+
	LD		R11, Z+
	LD		R12, Z+
	LD		R13, Z+
	LD		R14, Z+
	LD		R15, Z+
	LD		R16, Z+
	LD		R17, Z+
	LD		R18, Z+
	LD		R19, Z+
	LD		R20, Z+
	LD		R21, Z+
	LD		R22, Z+
	LD		R23, Z+
	LD		R24, Z+
	LD		R25, Z+
	LD		R28, Z+
	LD		R29, Z+

	OUT		SPLO, XL
	OUT		SPHI, XH

	ADIW	Z, 2				; Skip to LR
	LD		XL, Z+				; Load the LR (return address)
	LD		XH, Z+

	PUSH	XL
	PUSH	XH

	RET							; Leap into the code stored in the LR field code.

;--------------------------
; save_stack_context
;
; Snapshot the entire stack, and store it int he location specified.
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;       * Pointer to where the stack should be saved (r22, r23)
;--------------------------
_save_stack_context:
	PUSH	YL
	PUSH	YH

	MOV		ZL, r24				; Point Z register at TCB
	MOV		ZH, r25

	LD		YL, Z+				; Point X register at the STACK_BASE
	LD		YH, Z+
	ADIW	Y, 1				; AVR loops pre-decrement, so offset here.
	MOVW	X, Y				; Snapshot STACK_BASE pointer for later.

	IN		R1, SPLO
	SUB		YL, R1				; 16 bit Y = STACKBASE - STACK.

	BRCC	STORE_CARRY_COMPLETE
	DEC		YH
STORE_CARRY_COMPLETE:
	IN		R1, SPHI
	SUB		YH, R1

	SBIW	Y, 5				; Ignore last 4 bytes (one internal subroutine calls, and the YL/YH PUSH), and one more because the stackpointer is one byte ahead of the data

	MOV		ZL, r22				; Point Z register at memory region to write to
	MOV		ZH, r23

STORE_STACK_BYTE:	
	LD		R1, -X
	ST		-Z, R1
	SBIW	Y, 1
	BRNE	STORE_STACK_BYTE

	CLR		R1
	POP		YH
	POP		YL

	RET


;--------------------------
; restore_stack_context
;
; Saves the entire stack of the old fiber to RAM, and pages in another fibers
; stack.
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;       * Pointer to where the stack is saved (r22, r23)
;--------------------------
_restore_stack_context:

	POP		R20					; Extract our return address - we'll need it later.
	POP		R21

	MOV		ZL, r24				; Point Z register at TCB
	MOV		ZH, r25

	LD		XL, Z+				; Point X register at the STACK_BASE
	LD		XH, Z+

	OUT		SPLO, XL			; Update stack pointer whilst we're here to STACK_BASE.
	OUT		SPHI, XH

	ADIW	Z, 28				; Move to stored stack pointer in TCB
	LD		R1, Z+

	SUB		XL, R1				; 16 bit X = STACKBASE - STACK.
	BRCC	RESTORE_CARRY_COMPLETE
	DEC		XH

RESTORE_CARRY_COMPLETE:
	LD		R1, Z+
	SUB		XH, R1						
	
	ADIW	X, 0						; Zero test. 
	BREQ	RESTORE_STACK_COMPLETE		; Ensure there is a stack to restore (a new fiber may not have one).

	MOV		ZL, r22				; Point Z register at stack image to restore
	MOV		ZH, r23

RESTORE_STACK_BYTE:	
	LD		R1, -Z
	PUSH	R1

	SBIW	X, 1
	BRNE	RESTORE_STACK_BYTE

RESTORE_STACK_COMPLETE:
	CLR		R1
	PUSH	R21
	PUSH	R20

	RET


;--------------------------
; swap_context
;
; Saves the entire stack of the old fiber to RAM, and pages in another fibers
; stack.
;
; ARGS:
;       * Pointer to the old Fibers' TCB (r24, r25)
;       * Pointer to the new Fibers' TCB (r22, r23)
;       * Pointer to the where the old Fibers' stack should be saved (r20, r21)
;       * Pointer to the where the new Fibers' stack is saved (r18, r19)
;--------------------------
swap_context:
	TST		r24
	BRNE	SAVE_FIBER_CONTEXT
	TST		r25
	BREQ	RESTORE_FIBER

SAVE_FIBER_CONTEXT:
	CALL	_save_stack_context
	CALL	_save_register_context

RESTORE_FIBER:
	MOV		R25, R21
	MOV		R24, R20
	MOV		R23, R19
	MOV		R22, R18

	CALL	_restore_stack_context
	CALL	_restore_register_context

	RET


;--------------------------
; save_register_context
;
; pushes our call saved regs and simply saves the stack pointer
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;--------------------------
save_register_context:
	CALL	_save_register_context
	RET

;--------------------------
; restore_register_context
;
; simply restores the stack pointer in the case of the AVR
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;--------------------------
restore_register_context:
	CALL	_restore_register_context
	RET

;--------------------------
; save_context
;
; pushes our call saved regs and simply saves the stack pointer
;
; ARGS:
;       * Pointer to the Fibers' TCB (r24, r25)
;       * Pointer to where the stack should be saved (r22, r23)
;--------------------------
save_context:
	CALL	_save_stack_context
	CALL	_save_register_context
	RET

