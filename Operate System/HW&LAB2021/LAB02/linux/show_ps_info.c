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
