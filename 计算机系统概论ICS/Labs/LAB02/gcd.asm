            .ORIG x3000
            LD R0 NUMBER1
            BRnz ERROR
            LD R1 NUMBER2   
            BRnz ERROR      ;输入非正整数，输出错误
CHECKEQL    NOT R2,R1
            ADD R2,R2,#1        
            ADD R3,R0,R2    ;R3=R0-R1
            BRz DONE        ;若R3为0，R0=R1则退出
            BRp LTR1        ;否则当R0-R1>0时，跳转到LTR1
                            ;注意此时CC尚未改变，因此可以再次BR
            NOT R2,R0
            ADD R2,R2,#1    
            ADD R1,R1,R2    ;R1=R1-R0
            BRnzp CHECKEQL
LTR1        ADD R0,R0,R2    ;R0=R0-R1
            BRnzp CHECKEQL  
ERROR       LD R0 ERRNUM
DONE        HALT
NUMBER1     .FILL x0AE9     
NUMBER2     .FILL x11AF     
ERRNUM      .FILL xFFFF     ;错误代码
            .END
    