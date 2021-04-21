;冒泡排序，两层循环，第i次外循环结束，使得链表的第i个元素比它之后的元素都要小（只交换value域）。
            .ORIG x3000
            LD R0 FirstAddr    
            LDR R1,R0,#0        ;检查R0指针域为x0000直接退出
            BRz Exit
            
IntLoop     LDR R1,R0,#0        ;内循环开始，R1=R0->nextaddr
Load        LDR R2,R0,#1        ;R2取数据1
            LDR R3,R1,#1        ;R3取数据2,下面开始比较这两个数据
            NOT R4,R3           
            ADD R4,R4,#1
            ADD R4,R2,R4
            BRp SWAP            ;若R2大于R3,交换value field
Nextaddr    LDR R1,R1,#0        ;否则 R1=R1->nextaddr
            BRz ExtLoop         ;若R1=x0000，一次内循环结束，跳转到外循环
            BRnzp Load
            
SWAP        STR R3,R0,#1
            STR R2,R1,#1
            BRnzp Nextaddr
            
ExtLoop     LDR R0,R0,#0        ;R0=R0->nextaddr
            BRp IntLoop         ;若R0=x0000,外循环结束
Exit        HALT

FirstAddr   .FILL x3100
            .END
            
            