#void mixcolumns_asm(uint8_t state[4][4]);
.section .text
.globl mixcolumns_asm
mixcolumns_asm:
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %esi
    pushl %edi

    #prostor od 8 bajtova za originale i stime vrednosti jedne kolone
    #(esp + 0) = s0
    #(esp + 1) = s1
    #(esp + 2) = s2
    #(esp + 3) = s3
    #(esp + 4) = xtime(s0)
    #(esp + 5) = xtime(s1)
    #(esp + 6) = xtime(s2)
    #(esp + 7) = xtime(s3)
    subl $8, %esp    

    movl 8(%ebp), %esi  #pokazivac na pocetak matrice state
    xorl %edi, %edi     #brojac kolona int j = 0

kolone_petlja:
    cmpl $4, %edi
    jge kolone_petlja_kraj

    #kopira originalne vrednosti kolone (s0-s3) i cuva ih na steku
    movzbl (%esi, %edi, 1), %eax
    movb %al, (%esp)
    movzbl 4(%esi, %edi, 1), %eax
    movb %al, 1(%esp)
    movzbl 8(%esi, %edi, 1), %eax
    movb %al, 2(%esp)
    movzbl 12(%esi, %edi, 1), %eax
    movb %al, 3(%esp)

    #ucita s0 u al i uradi levi shift za 1
    movb (%esp), %al
    shlb $1, %al
    jnc xs0_ok
    xorb $0x1b, %al
xs0_ok:
    #sacuvaj xtime(s0) na stek
    movb %al, 4(%esp)

    #ucita s1 u al i uradi levi shif za 1
    movb 1(%esp), %al
    shlb $1, %al
    jnc xs1_ok
    xorb $0x1b, %al 
xs1_ok:
    #sacuvaj xtime(s1) na stek
    movb %al, 5(%esp)

    #ucita s2 u al i uradi levi shift za 1
    movb 2(%esp), %al 
    shlb $1, %al
    jnc xs2_ok
    xorb $0x1b, %al
xs2_ok:
    #sacuvaj xtime(s2) na stek
    movb %al, 6(%esp)

    #ucita s3 u al i uradi levi shift za 1
    movb 3(%esp), %al
    shlb $1, %al
    jnc xs3_ok
    xorb $0x1b, %al
xs3_ok:
    #sacuvaj xtime(s3) na stek
    movb %al, 7(%esp)

    #s0 = xtime(s0) ^ xtime(s1) ^ s1 ^ s2 ^ s3
    movb 4(%esp), %al #xtime(s0)
    xorb 5(%esp), %al #xtime(s1) 
    xorb 1(%esp), %al # ^ s1    ovo je ekvivalentno sa + 3*s1
    xorb 2(%esp), %al # ^ s2
    xorb 3(%esp), %al # ^ s3
    movb %al, (%esi, %edi, 1)

    #s1 = s0 ^ xtime(s1) ^ xtime(s2) ^ s2 ^ s3
    movb (%esp), %al
    xorb 5(%esp), %al
    xorb 6(%esp), %al
    xorb 2(%esp), %al
    xorb 3(%esp), %al
    movb %al, 4(%esi, %edi, 1)

    #s2 = s0 ^ s1 ^ xtime(s2) ^ xtime(s3) ^ s3
    movb (%esp), %al
    xorb 1(%esp), %al
    xorb 6(%esp), %al
    xorb 7(%esp), %al
    xorb 3(%esp), %al
    movb %al, 8(%esi, %edi, 1)

    #s3 = s0 ^ xtime(s0) ^ s1 ^ s2 ^ xtime(s3)
    movb (%esp), %al
    xorb 4(%esp), %al
    xorb 1(%esp), %al
    xorb 2(%esp), %al
    xorb 7(%esp), %al
    movb %al, 12(%esi, %edi, 1)

    incl %edi
    jmp kolone_petlja

kolone_petlja_kraj:

mixcolumns_asm_kraj:
    addl $8, %esp
    popl %edi
    popl %esi
    popl %ebx
    popl %ebp
    ret