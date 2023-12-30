[GLOBAL] gdt_flush

gdt_flush:
	mov eax, [esp+4]; get param from stack
	lgdt [eax]		; load From GDT ptr

	mov ax, 0x10	; in this time, 0x10 addr is third entry of my GDT->kernel data segment
	mov ds, ax      ; load all data segment select
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush ; to user code segment far Jump
	; each gdt offset is 8Byte
.flush:
	ret

[GLOBAL] idt_flush

idt_flush:
	mov eax, [esp+4] ; get point to IDT
	lidt [eax]
	ret

