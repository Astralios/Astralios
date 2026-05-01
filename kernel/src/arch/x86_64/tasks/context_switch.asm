TASK_RSP_OFFSET equ 16

extern _context_switch

global context_switch
context_switch:
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    
    ; TODO: Maybe in the future add generators :pensive:
    mov [rdi + TASK_RSP_OFFSET], rsp
    mov rsp, [rsi + TASK_RSP_OFFSET]

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    jmp _context_switch

