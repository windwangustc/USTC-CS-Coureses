	.file	"sort.s"
	.text
	.align	2
	.global	strcmp
	.type	strcmp, %function
strcmp:
	str	fp, [sp, #-4]!
	add	fp, sp, #0          
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L2
.L4:
	ldr	r3, [fp, #-8]       ;依次将字符串str1内的值装载入r3寄存器
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L2:
	ldr	r3, [fp, #-8]       
	ldr	r2, [fp, #-16]      ;将str2内对应位置的字符值装载入r2寄存器
	add	r3, r2, r3
	ldrb	r2, [r3, #0]    
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-20]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	
	cmp	r2, r3              ;比较r2，r3内的值
	bne	.L3                 ;如果不相同，跳转到L3，进入返回值程序段
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-16]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	cmp	r3, #0              ;比较r3是否为结束符
	bne	.L4                 ;若不是，跳转到L4继续比较下一个，否则进入返回程序段
.L3:
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-16]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]
	mov	r2, r3
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-20]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]
	rsb	r3, r3, r2
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}       ;还原现场
	bx	lr                  ;子程序结束，返回调用位置

	.size	strcmp, .-strcmp
	.align	2
	.global	strsort
	.type	strsort, %function
strsort:							;字符串排序程序段
	stmfd	sp!, {fp, lr}			;保存现场
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-24]
	str	r1, [fp, #-28]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L6
.L10:                             ;冒泡排序
	ldr	r3, [fp, #-8]             
	add	r3, r3, #1                ;i++
	str	r3, [fp, #-12]
	b	.L7                       ;j=i+1
.swap:                            ;swap指针程序段
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-12]
	mov	r3, r3, asl #2
	ldr	r1, [fp, #-24]
	add	r3, r1, r3
	ldr	r3, [r3, #0]
	mov	r0, r2
	mov	r1, r3
	bl	strcmp                    ;调用strcmp函数比较r0,r1
	mov	r3, r0
	cmp	r3, #0                    
	ble	.L8                       ;若相等转入L8，j++，否则执行swap
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldr	r3, [r3, #0]
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldr	r2, [fp, #-12]
	mov	r2, r2, asl #2
	ldr	r1, [fp, #-24]
	add	r2, r1, r2
	ldr	r2, [r2, #0]
	str	r2, [r3, #0]
	ldr	r3, [fp, #-12]
	mov	r3, r3, asl #2
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldr	r2, [fp, #-16]
	str	r2, [r3, #0]
.L8:
	ldr	r3, [fp, #-12]        ;j++
	add	r3, r3, #1
	str	r3, [fp, #-12]
.L7:                        
	ldr	r2, [fp, #-12]
	ldr	r3, [fp, #-28]
	cmp	r2, r3
	blt	.swap                 ;若大于0,跳转到swap
	ldr	r3, [fp, #-8]
	add	r3, r3, #1            ;j++
	str	r3, [fp, #-8]
.L6:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-28]
	cmp	r2, r3
	blt	.L10                ;若大于0,跳转到L10
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
	.size	strsort, .-strsort