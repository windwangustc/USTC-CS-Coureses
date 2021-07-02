// 必备头函数

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/delay.h>

// 该模块的LICENSE
MODULE_LICENSE("GPL");
// 该模块的作者
MODULE_AUTHOR("PE20060014");
// 该模块的说明
MODULE_DESCRIPTION("This is a simple example!/n");

// 该模块需要传递的参数
static int func = -1;
module_param(func, int, 0644);

static int pid = 0;
module_param(pid, int, 0644);

//定时器服务函数
struct timer_list my_timer_list;	//global timer
void timer_function(struct timer_list* t){
       int i = 0;
       struct task_struct* task;
	for_each_process(task){
	//kernel thread
	if(task->mm==NULL){
		i++;
    	    } 
    	}
    	printk("The number of kernel process is %d\n",i);
    	mod_timer(&my_timer_list, jiffies + 5 * HZ);
}

// 初始化入口
// 模块安装时执行
// 这里的__init 同样是宏定义，主要的目的在于
// 告诉内核，加载该模块之后，可以回收init.text的区间
static int __init print_hello_init(void)
{
    // 输出信息，类似于printf()
    // printk适用于内核模块
    printk(KERN_ALERT" my_module_init() begin\n");
    if(func==1){
	printk("PID	STAT	COMMAND\n");
	struct task_struct* task;
	for_each_process(task){
	//kernel thread
	if(task->mm==NULL){
	//task->state is a 8-bit(like 0'b0000_0011) flag which indicates the process state
	//please check this URL for more details
	//https://blog.csdn.net/qq_41209741/article/details/82870876
	printk("%u	%lu	[%s]\n",task->pid,task->state,task->comm);
    	} 
    	}
    }
    else if(func==2){
    	//setup timer 
	timer_setup(&my_timer_list,timer_function,0);
	my_timer_list.expires = jiffies + 5*HZ;	//interval=5s
	add_timer(&my_timer_list);
    }
    else if(func==3){
    	printk("pid receive successfully:%d!\n",pid);	    
    	struct task_struct* task;
    	for_each_process(task){
    		if(task->pid == pid){
    		  //find father process
    		  printk("His father is :%u	%lu	%s\n",task->parent->pid,task->parent->state,task->parent->comm);
    		  struct list_head *p;
    		  //find children's sibling
    		  list_for_each(p,&(task->children)){
    		  	struct task_struct* child;
    		  	child = list_entry(p,struct task_struct,sibling);
    		  	printk("His children is :%u	%lu	%s\n",child->pid,child->state,child->comm);
    		  }
    		  //find it's father's children
    		  list_for_each(p,&(task->parent->children)){
    		  	struct task_struct* brother;
    		  	brother = list_entry(p,struct task_struct,sibling);
    		  	printk("His sibling is :%u	%lu	%s\n",brother->pid,brother->state,brother->comm);
    		  }	
    		  //find it's thread	  
		  struct task_struct *t = task;
		  do {
   			printk("His thread is :%u	%lu	%s\n",t->pid,t->state,t->comm);
		  }while_each_thread(task, t);	   		 
    	}
      }
    }
    return 0;
}


// 模块卸载时执行
// 同上
static void __exit print_hello_exit(void)
{
    //delete timer when exit
    del_timer_sync(&my_timer_list);
    printk(KERN_ALERT" module has exitedi!\n");
}

// 模块初始化宏，用于加载该模块
module_init(print_hello_init);
// 模块卸载宏，用于卸载该模块
module_exit(print_hello_exit);
