# oslab2实验报告  PE20060014王晨

## 第一部分 系统调用

​		这部分的实验主要是两个注意点：1. 第一是掌握用户态和内核态参数传递问题，也就是如何在用户态进行系统调用后，接收由内核态系统调用函数中得到的值，然后再到用户态函数中进行处理。2. 第二是正确找到struct_task中相应的成员变量取值。

### 1.1 注册系统调用

```shell
$cd linux-4.9.263/arch/x86/entry/syscalls
$vim syscall_64.tbl 
# 打开文件后在如下位置注册两个系统调用，332号系统调用传递当前进程信息给用户空间，333号系统调用获取当前进程数量。
#...
332	common  ps_show_info	sys_ps_show_info
333 common  ps_counter		sys_ps_counter
#...
```

### 1.2 定义函数原型

```shell
$cd linux-4.9.263/include/linux/syscall
$vim syscalls.h
```

​		打开文件后，在文件最后添加系统调用的声明。这里task_info结构体是我们在sys_ps_show_info系统调用中需要传递的参数，其中的成员对应了<linux/sched.h>中struct_task结构体里我们需要的信息，后面还会讲到struct_task。变量类型按struct_task相应的类型定义即可，例如pid的类型在struct_task中是pid_t，也就是unsigned int。

​		__user *表示这是用户空间的地址。

```c
//additional syscall by chen wang
typedef struct{
	unsigned int pid;
	unsigned int uid;
	long unsigned int time;
	char command[50];
}task_info;
asmlinkage long sys_ps_show_info(int __user * num,task_info __user * info);
asmlinkage long sys_ps_counter(int __user * num);
```

### 1.3 实现函数

​		在 linux-4.9.264/kernel/sys.c 文件的最后添加上面两个系统调用函数原型的实现，这里先贴333号系统调用sysy_ps_counter的代码，和实验文档中给的是一样的，功能是统计当前的进程数量，这个结果之后在用户态函数中还要用到。

```c
//self-defined syscall function by chen wang
//syscall code 333
SYSCALL_DEFINE1(ps_counter, int __user *, num){
    struct task_struct* task;
    int counter = 0;
    printk("[Syscall] ps_counter\n");
    for_each_process(task){
			counter ++; 
    }
    copy_to_user(num, &counter, sizeof(int));
return 0; 
}
```

​		下面是333号系统调用sysy_ps_show_info的实现代码，功能是传递第i个进程的信息给用户空间。这里的进程信息就是在1.2中定义的task_info结构体，包括了每个进程的pid，uid，time，command四个信息。

​		我们关注下int __user * num，这是用户空间传进来的参数，表示当前用户空间需要获取第 num 个进程的信息，使用copy_from_user将num传给内核空间的局部变量counter。之后的for循环中是在找第counter个进程，找到后赋给tmp，然后使用copy_to_user将tmp传递给用户空间，这样就完成了在内核空间获取进程信息，交给用户态函数处理。

```c
//self-define syscall function by chen wang
//syscall code 332
SYSCALL_DEFINE2(ps_show_info, int __user *, num, task_info __user *, info ){
    struct task_struct* task;
    task_info tmp={0,0,0};
    int counter = 0;
    copy_from_user(&counter, num, sizeof(int));
    int j=0;
    for_each_process(task){
    	if(j==counter){
					tmp.pid=task->pid;
					tmp.uid=task->loginuid.val;
					tmp.time=task->utime+task->stime;
					strcpy(tmp.command, task->comm);
					break;
			}else j++;
    }
    copy_to_user(info, &tmp, sizeof(task_info));
    return 0; 
}
```

​        task_struct结构体包含了进程的相关信息，为了正确获取我们需要的信息，需要对这个结构体进行了解。这个结构体在include/linux/sched.h中定义，里面的相关成员变量非常多，阅读源码可以找到有关pid，cputime，command等信息（uid没找到，找了最接近的loginuid，不确定是不是）。其他的可以参考以下博客，task_struct结构体详解：

```http
https://blog.csdn.net/qq_35032155/article/details/70147835
```

### 1.4 编写用户态测试代码

​		将测试代码命名其为show_ps_info.c。上面新增的332，332号系统调用可以使用 syscall (long int sysno, 参数1，参数2，...) 进行调用。

​		下面的代码首先调用333号系统调用，获取当前进程数，传递给result用于后面的for循环，对于每一个进程，调用332号系统调用，获取该进程的相关信息给info然后进行打印。

```c
#include<unistd.h>
#include<sys/syscall.h>
#include<linux/sched.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define TASK_COMM_LEN 50

typedef struct{
		pid_t pid;
		unsigned int uid;
		long unsigned int time;
		char comm[TASK_COMM_LEN];
}task_info;

int main(void)
{
	int result=0;
	task_info info={0,0,0};
	syscall(333,&result);
	printf("process number is %d\n",result);
	printf("PID	USER	TIME	COMMAND\n");
	for(int i =0;i<result;i++){
	   syscall(332,&i,&info);
	   printf("%u	%d	%lu	%s\n",info.pid,info.uid,info.time,info.comm);
	}
	return 0;
}
```

执行编译：

```shell
gcc -static -o show_ps_info show_ps_info.c
```

### 1.5 运行测试程序

​		将编译出的可执行文件 show_ps_info 放到 busybox-1.32.1/_install 下面，执行_

```shell
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/Desktop/oslab1/initramfs-busybox-x64.cpio.gz #路径改成自己的就行
```

重新制作 initramfs文件，这样我们才能在qemu中看见编译好的 show_ps_info 可执行文件。制作完成后，在 /linux-4.9.263 下执行make编译，之后打开qemu:

```shell
 bash openqemu.sh
 #在qemu界面下执行
 $ls
 $./show_ps_info
```

执行后可以看到成功显示进程数量和进程信息（userid因为找的是loginuid，所以可能不太对）：

![image-20210424151047355](/Users/chen/Library/Application Support/typora-user-images/image-20210424151047355.png)



## 第二部分 实现Linux Shell

### 2.1 任务目标

代码填空实现一个shell。这个shell不要求在qemu下运行。功能包括:1.支持基本的单条命令运行，且命令行上能显示当前目录。2.支持两条命令间的管道和内建命令。3.支持多条命令间的管道操作。

这部分实验的难点在于理解管道创建和连接的过程，以及多个子进程连接管道时的操作方式。

### 2.2 任务实现

我们选取几个重点的Todo为索引进行说明。完整的代码附在压缩包中。

#### 2.21 增加打印当前目录，格式类似"shell:/home/oslab ->"

在每次进入while循环时，先获取当前的工作目录输出：

```c
char absolute_addr[MAX_ADDRESS_LENGTH];   //用于存储当前地址
memset(absolute_addr,0,sizeof(absolute_addr));
//getcwd功能: 取得当前的工作目录
getcwd(absolute_addr, sizeof(absolute_addr));
printf("shell:%s-> ",absolute_addr);
fflush(stdout);						//清空输出流
```

#### 2.2.2 处理参数，分出命令名和参数

这里没有使用框架中提供的int split_string( )函数，原因是在直接输入回车时，它仍然会计算cmd_count=1,而这样exec_builtin( )中argc==0的分支就没有意义了。

我使用了strtok进行参数分割，效果是一样的，以每个" "作为分隔符，统计参数，并将分割后的片段存入char *argv[ ]中。分割完成后调用exec_builtin或execute函数执行命令。

```c
else if(cmd_count == 1) {     // 没有管道的单一命令
            char *argv[MAX_CMD_ARG_NUM];
            // TODO:处理参数，分出命令名和参数
            int argc = 0;
            argv[0] = strtok(cmdline, " ");
            if (argv[0] && strcmp(argv[0], "\n") != 0) {
                argc = 1;
            }
            while ((argv[argc] = strtok(NULL, " "))) {
                argc++;
            }
        /* 在没有管道时，内建命令直接在主进程中完成，外部命令通过创建子进程完成 */
            if (exec_builtin(argc, argv) == 0) {
                continue;
            }else {
                // TODO:创建子进程，运行命令，等待命令运行结束
                int pid = fork();
                //子进程
                if (pid == 0) {
                    execute(argc, argv);
                }
                while (wait(NULL) > 0);
            }
				}
```

#### 2.2.3 exec_builtin和execute函数

exec_builtin( )用于实现内建命令。以cd为例，如果不使用内建命令而是fork出一个子进程并且在子进程中exec一个cd程序的话，因为执行结束了又回到了父shell环境，父shell的路径根本没有被改变，所以就会失败。所以同理，不仅是cd，改变当前shell的参数(如 source 命令、 exit 命令)基本都是由 shell内建命令实现的。

同样的，内建命令不能直接使用exec函数，否则exec会覆盖源程序的代码段，并在执行完成后直接退出。因此，exec_builtin( )中的内建命令都需要自己实现：

```c
int exec_builtin(int argc, char**argv) {
    if(argc == 0) {
        return 0;
    }
    /* TODO: 添加和实现内置指令 */
    if (strcmp(argv[0], "cd") == 0) {
      	//chdir打开指定路径
        if(chdir(argv[1])!=0){
            printf("%s:%s No such file or dictionary\n",argv[0],argv[1]);
        }
        return 0;
    } else if (strcmp(argv[0], "pwd") == 0) {
        char absolute_addr[MAX_ADDRESS_LENGTH];
        memset(absolute_addr, 0, sizeof(absolute_addr));
        //getcwd功能: 取得当前的工作目录
        getcwd(absolute_addr, MAX_BUF_SIZE * sizeof(char));
        printf("%s\n", absolute_addr);
        return 0;
    } else if (strcmp(argv[0], "exit") == 0){
        printf("进程已结束\n");
        exit(0);
    }else {
        // 不是内置指令时
        return -1;
    }
}
```

execute( )用于执行外部命令，这些命令是由shell fork出子进程后，调用execvp( )执行的。execvp( )会根据传入的argv[0]，自行到环境变量路径中搜索相应文件进行执行，argv是执行所需要的参数。

```c
int execute(int argc, char** argv) {
    if(exec_builtin(argc, argv) == 0) {
        exit(0);
    }
    /* TODO:运行命令 */
    //char newpath[MAX_CMDLINE_LENGTH]="/bin/";
    //strcat(newpath,argv[0]);
    execvp(argv[0],argv);
}
```

#### 2.2.4 两个命令间的管道处理

​		两个管道的处理是最简单的情况。父进程创建出管道后，fork出的子进程也会对管道进行同样的连接（父子进程PCB中对于文件描述符是同一个），因此当fork出两个子进程进行通信时，只需要一个写，一个读，关闭不需要的端口，父进程不对管道进行读写，那么关闭读写端口即可。子进程A通过将标准输出重定向到管道写端，子进程B通过将标准输入重定向到管道读端，完成将子进程A中命令的运行输出，作为子进程B中命令的运行输入（如果B中命令需要的话）。

它们的连接图示大致是这样的：

<img src="/Users/chen/Library/Application Support/typora-user-images/image-20210424160254415.png" alt="image-20210424160254415"  />

理解了这个过程后，重定向就很容易写出了，以子进程1为例，fd0读端用不着，关闭，fd1在调用dup2将标准输出重定向后，也不需要了，关闭。子进程2也是类似的，父进程在fork完成后就可以关闭读写端了。代码如下：

```c
else if(cmd_count == 2) {     // 两个命令间的管道
    int pipefd[2];
    int ret = pipe(pipefd);
    if(ret < 0) {
        printf("pipe error!\n");
        continue;
    }
    // 子进程1
    int pid = fork();
    if(pid == 0) {
        /*TODO:子进程1 将标准输出重定向到管道，注意这里数组的下标被挖空了要补全*/
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        /*
            在使用管道时，为了可以并发运行，所以内建命令也在子进程中运行
            因此我们用了一个封装好的execute函数
         */
        char *argv[MAX_CMD_ARG_NUM];
        int argc = split_string(commands[0], " ", argv);
        execute(argc, argv);
        exit(255);
    }
    // 因为在shell的设计中，管道是并发执行的，所以我们不在每个子进程结束后才运行下一个
    // 而是直接创建下一个子进程
    // 子进程2
    pid = fork();
    if(pid == 0) {
        /* TODO:子进程2 将标准输入重定向到管道，注意这里数组的下标被挖空了要补全 */
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        /* TODO:处理参数，分出命令名和参数，并使用execute运行
         * 在使用管道时，为了可以并发运行，所以内建命令也在子进程中运行
         * 因此我们用了一个封装好的execute函数
         */
        char *argv[MAX_CMD_ARG_NUM];
        int argc = split_string(commands[1], " ", argv);
        execute(argc, argv);
        exit(255);
    }
    close(pipefd[WRITE_END]);
    close(pipefd[READ_END]);
    while (wait(NULL) > 0);			//阻塞，等待所有子进程结束
}
```

#### 2.2.5 任意多个命令间的管道处理

理解了如何实现两个命令间的管道处理后，多个管道只是n个命令间的一般化，思想都是关闭不需要的端口，重定向后关闭端口，只是多个命令间，除了最后一条命令外，都将标准输出重定向到当前管道入口，除了第一条命令外，都将标准输入重定向到上一个管道出口。

```c
else {    // 三个以上的命令
            int read_fd;    // 上一个管道的读端口（出口）
            for(int i=0; i<cmd_count; i++) {
                int pipefd[2];
                // TODO:创建管道，n条命令只需要n-1个管道，所以有一次循环中是不用创建管道的
                if(i!=cmd_count-1) {
                    int ret = pipe(pipefd);
                    if (ret < 0) {
                        printf("pipe error!\n");
                        continue;
                    }
                }
                int pid = fork();
                if(pid == 0) {
                    /* TODO:除了最后一条命令外，都将标准输出重定向到当前管道入口
                     */
                    if(i!=(cmd_count-1)){
                        close(pipefd[0]);
                        dup2(pipefd[1], STDOUT_FILENO);
                        close(pipefd[1]);
                    }
                    /* TODO:除了第一条命令外，都将标准输入重定向到上一个管道出口
                     */
                    if(i!=0){
                        close(pipefd[1]);
                        dup2(read_fd, STDIN_FILENO);
                        close(read_fd);     //上个管道出口也不需要了 关闭
                        close(pipefd[0]);
                    }
                    /* TODO:处理参数，分出命令名和参数，并使用execute运行
                     * 在使用管道时，为了可以并发运行，所以内建命令也在子进程中运行
                     * 因此我们用了一个封装好的execute函数
                     */
                    char *argv[MAX_CMD_ARG_NUM];
                    int argc = split_string(commands[i], " ", argv);
                    execute(argc, argv);
                }
                //父进程除了第一条命令，都需要关闭当前命令用完的上一个管道读端口
                if (i!=0) close(read_fd);
                 // 父进程除了最后一条命令，都需要保存当前命令的管道读端口
                if (i!=cmd_count-1) read_fd=pipefd[READ_END];
                 //记得关闭父进程没用的管道写端口
                close(pipefd[WRITE_END]);
                // 因为在shell的设计中，管道是并发执行的，所以我们不在每个子进程结束后才运行下一个
                // 而是直接创建下一个子进程
            }
            // TODO:等待所有子进程结束
            while (wait(NULL) > 0);
        }
```

### 2.3 shell测试

编译shell的源文件：

```shell
$ gcc -o main main.c 
$ ./main
```

按如下方式依次输入命令：

我们重点关注多个管道命令的运行情况，ls打印目录下文件信息，执行 ls｜grep main后应该只显示包含main的，ls｜grep main｜grep c后应该只显示main.c了，这样说明前一个命令的输出成功通过管道传输给了下一个命令。另外由于管道的打开关闭都是正确的，因此多次执行管道命令，也不会发生阻塞，或者读写异常。

![image-20210424162259150](/Users/chen/Library/Application Support/typora-user-images/image-20210424162259150.png)





