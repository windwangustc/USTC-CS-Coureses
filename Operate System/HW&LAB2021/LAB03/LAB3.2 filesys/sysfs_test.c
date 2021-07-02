#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/err.h>

#include <linux/types.h>
#include <linux/freezer.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/rmap.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PE20060014");
MODULE_DESCRIPTION("SYSFS_TEST!");
MODULE_VERSION("1.0");

//sysfs
#define SYSFS_TEST_RUN_STOP 0
#define SYSFS_TEST_RUN_START 1
#define MAX_PIDNAMELEN 10

// /sys/kerbel/mm/sysfs_test/pid
static unsigned int pid = 0;
// /sys/kerbel/mm/sysfs_test/func
static unsigned int sysfs_test_func = 0;
//  /sys/kernel/mm/sysfs_test/sysfs_test_run
static unsigned int sysfs_test_run = SYSFS_TEST_RUN_STOP;
//  /sys/kernel/mm/sysfs_test/sleep_millisecs
static unsigned int sysfs_test_thread_sleep_millisecs = 5000;

static struct task_struct* sysfs_test_thread;

static DECLARE_WAIT_QUEUE_HEAD(sysfs_test_thread_wait);
static DEFINE_MUTEX(sysfs_test_thread_mutex);

//procfs variables
static struct proc_dir_entry* proc_kmscan;
static struct proc_dir_entry* proc_pid=NULL;
static int file=0, active_file=0,anon=0,active_anon=0,vma_count=0;
static unsigned int old_pid=0;

static int test_show(struct seq_file* m, void* v)
{
    pid_t* pid_n = (pid_t*)m->private;
    if (pid_n != NULL)
    {
        seq_printf(m, "%d\n", *pid_n);
    }
    return 0;
}

static int test_open(struct inode* inode, struct file* file)
{
    return single_open(file, test_show, PDE_DATA(inode));
}

// linux5.9.0
// 低版本需要查看proc_fs.h文件定义的结构体
struct proc_ops test_ops = {
    .proc_open = test_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

static int sysfs_testd_should_run(void){
    return (sysfs_test_run & SYSFS_TEST_RUN_START);
}

//create kmscan files
static int create_kmscan_files(void) {
    char pidname[MAX_PIDNAMELEN];
    //delete if exists
    if (proc_pid!=NULL)
    	proc_remove(proc_pid);
    // transfer pid into str for proc_mkdir to use
    snprintf(pidname, MAX_PIDNAMELEN, "%d", pid);
    // 在 kmscan 根目录创建 pid 文件夹
    proc_pid = proc_mkdir(pidname,proc_kmscan);
    if (proc_pid == NULL) {
        printk("%s proc create %s failed\n", __func__, "kmscan");
        return -EINVAL;
    }
    proc_create_data("file", 0664, proc_pid, &test_ops, &file);
    proc_create_data("active_file", 0664, proc_pid, &test_ops, &active_file);
    proc_create_data("anon", 0664, proc_pid, &test_ops, &anon);
    proc_create_data("active_anon", 0664, proc_pid, &test_ops, &active_anon);
    proc_create_data("vma_count", 0664, proc_pid, &test_ops, &vma_count);
    old_pid = pid;
    return 0;
}


//定时器服务函数
//struct timer_list my_timer_list;	
static void countVMA(struct timer_list* t)
{
    int i = 0;		//vma counter
    struct task_struct* task;
    for_each_process(task){
	if(task->pid == pid){
	     struct vm_area_struct *vma;
	     struct mm_struct *mm;
	     unsigned long start, end, length;
	     mm= task->mm;
	     vma= mm->mmap;
	     /* protect from simultaneous modification */
	     //printk("vmaNo.:                vma        start          end        length\n");
	     while (vma) {
		i++;
		start = vma->vm_start;
		end = vma->vm_end;
		length = end - start;
		//printk("%6d: %16p %12lx %12lx   %8ld\n", i, vma, start, end, length);
		vma = vma->vm_next;
	   }
		break;
	}
   }
   vma_count = i;
   printk("VMA counts as : %d\n",vma_count);
    file = 0;
    active_file = 0;
    anon = 0;
    active_anon = 0;
        //mod_timer(&my_timer_list, jiffies + 5 * HZ);
}

// 全局为follow_page定义别名
typedef typeof(follow_page)* my_follow_page;
typedef typeof(page_referenced)* my_page_referenced;

static void account_pages(void)
{
    file=0; active_file=0;anon=0;active_anon=0;vma_count=0;

    struct task_struct* task;
    //get pid's task struct
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    struct vm_area_struct *vma;
    struct mm_struct *mm;
    mm= task->mm;
    vma= mm->mmap;
    //traverse VMA
    struct page *pageaquired;
    unsigned long page_addr=0;
    int page_ref=0;
    unsigned long vm_flags=0;
	// 需要使用函数的地方
	my_follow_page mfollow_page;
	my_page_referenced mpage_referenced;
	mfollow_page = (my_follow_page)0xffffffff96677e10;
	mpage_referenced = (my_page_referenced)0xffffffff96693590;
    while (vma) {
    	page_addr=vma->vm_start;
	do{
	    pageaquired=mfollow_page(vma, page_addr,FOLL_GET);
	    if(!IS_ERR_OR_NULL(pageaquired)){
	    	file+=1;
	    	page_ref = mpage_referenced(pageaquired,1,pageaquired->mem_cgroup,&vm_flags);
	    	 if(page_ref!=0){
                    active_file += 1;
                }
                if(PageAnon(pageaquired)){
                    anon += 1;
                    if(page_ref!=0){
                        active_anon += 1;
                    }
                }
	    }
	    page_addr+=PAGE_SIZE;	//PAGE_SIZE=4K
	}while (page_addr<= (vma->vm_end));
	vma = vma->vm_next;
}
  printk("file counts as : %d\n",file);
  printk("anon counts as : %d\n",anon);
  printk("anon counts as : %d\n",file);
  printk("active_anon counts as : %d\n",anon);
}


static void sysfs_test_to_do(void)
{   
    if (sysfs_test_func == 1){
        //setup timer 
	//timer_setup(&my_timer_list,countVMA,0);
	//my_timer_list.expires = jiffies + 5 * HZ;
	//add_timer(&my_timer_list);
	countVMA(NULL);
     }
    else if (sysfs_test_func == 2)
        account_pages();
    create_kmscan_files();
}

static int sysfs_testd_thread(void* nothing)
{
    set_freezable();
    set_user_nice(current, 5);
    while (!kthread_should_stop())
    {
        mutex_lock(&sysfs_test_thread_mutex);
        if (sysfs_testd_should_run())
            sysfs_test_to_do();
        mutex_unlock(&sysfs_test_thread_mutex);
        try_to_freeze();
        if (sysfs_testd_should_run())
        {
            schedule_timeout_interruptible(
                msecs_to_jiffies(sysfs_test_thread_sleep_millisecs));
        }
        else
        {
            wait_event_freezable(sysfs_test_thread_wait,
                sysfs_testd_should_run() || kthread_should_stop());
        }
    }
    return 0;
}


#ifdef CONFIG_SYSFS

/*
 * This all compiles without CONFIG_SYSFS, but is a waste of space.
 */

#define SYSFS_TEST_ATTR_RO(_name) \
        static struct kobj_attribute _name##_attr = __ATTR_RO(_name)

#define SYSFS_TEST_ATTR(_name)                         \
        static struct kobj_attribute _name##_attr = \
                __ATTR(_name, 0644, _name##_show, _name##_store)

static ssize_t sleep_millisecs_show(struct kobject* kobj,
    struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%u\n", sysfs_test_thread_sleep_millisecs);
}

static ssize_t sleep_millisecs_store(struct kobject* kobj,
    struct kobj_attribute* attr,
    const char* buf, size_t count)
{
    unsigned long msecs;
    int err;

    err = kstrtoul(buf, 10, &msecs);
    if (err || msecs > UINT_MAX)
        return -EINVAL;

    sysfs_test_thread_sleep_millisecs = msecs;

    return count;
}
SYSFS_TEST_ATTR(sleep_millisecs);

static ssize_t pid_show(struct kobject* kobj,
    struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%u\n", pid);
}

static ssize_t pid_store(struct kobject* kobj,
    struct kobj_attribute* attr,
    const char* buf, size_t count)
{
    unsigned long tmp;
    int err;

    err = kstrtoul(buf, 10, &tmp);
    if (err || tmp > UINT_MAX)
        return -EINVAL;

    pid = tmp;

    return count;
}
SYSFS_TEST_ATTR(pid);


static ssize_t func_show(struct kobject* kobj,
    struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%u\n", sysfs_test_func);
}

static ssize_t func_store(struct kobject* kobj,
    struct kobj_attribute* attr,
    const char* buf, size_t count)
{
    unsigned long tmp;
    int err;

    err = kstrtoul(buf, 10, &tmp);
    if (err || tmp > UINT_MAX)
        return -EINVAL;

    sysfs_test_func = tmp;

    return count;
}
SYSFS_TEST_ATTR(func);

static ssize_t run_show(struct kobject* kobj, struct kobj_attribute* attr,
    char* buf)
{
    return sprintf(buf, "%u\n", sysfs_test_run);
}

static ssize_t run_store(struct kobject* kobj, struct kobj_attribute* attr,
    const char* buf, size_t count)
{
    int err;
    unsigned long flags;
    err = kstrtoul(buf, 10, &flags);
    if (err || flags > UINT_MAX)
        return -EINVAL;
    if (flags > SYSFS_TEST_RUN_START)
        return -EINVAL;
    mutex_lock(&sysfs_test_thread_mutex);
    if (sysfs_test_run != flags)
    {
        sysfs_test_run = flags;
    }
    mutex_unlock(&sysfs_test_thread_mutex);

    if (flags & SYSFS_TEST_RUN_START)
        wake_up_interruptible(&sysfs_test_thread_wait);
    return count;
}
SYSFS_TEST_ATTR(run);



static struct attribute* sysfs_test_attrs[] = {
    // 扫描进程的扫描间隔 默认为20秒 
    &sleep_millisecs_attr.attr,
    &pid_attr.attr,
    &func_attr.attr,
    &run_attr.attr,
    NULL,
};


static struct attribute_group sysfs_test_attr_group = {
    .attrs = sysfs_test_attrs,
    .name = "kmscan",
};
#endif /* CONFIG_SYSFS */


static int sysfs_test_init(void)
{
    int err;
    // 在 proc 根目录创建 kmscan 文件夹
    proc_kmscan = proc_mkdir("kmscan", NULL);
    if (proc_kmscan == NULL) {
        printk("%s proc create %s failed\n", __func__, "kmscan");
        return -EINVAL;
    }
    sysfs_test_thread = kthread_run(sysfs_testd_thread, NULL, "kmscan");
    if (IS_ERR(sysfs_test_thread))
    {
        pr_err("sysfs_test: creating kthread failed\n");
        err = PTR_ERR(sysfs_test_thread);
        goto out;
    }

#ifdef CONFIG_SYSFS
    err = sysfs_create_group(mm_kobj, &sysfs_test_attr_group);
    if (err)
    {
        pr_err("sysfs_test: register sysfs failed\n");
        kthread_stop(sysfs_test_thread);
        goto out;
    }
#else
    sysfs_test_run = KSCAN_RUN_STOP;
#endif  /* CONFIG_SYSFS */

out:
    return err;
}

static void sysfs_test_exit(void)
{
    if (sysfs_test_thread)
    {
        kthread_stop(sysfs_test_thread);
        sysfs_test_thread = NULL;
    }
    // remove kmscan when exit
    proc_remove(proc_kmscan);

#ifdef CONFIG_SYSFS

    sysfs_remove_group(mm_kobj, &sysfs_test_attr_group);

#endif
//delete timer
    //del_timer_sync(&my_timer_list);
    printk("sysfs_test exit success!\n");
}

/* --- 随内核启动  ---  */
// subsys_initcall(kscan_init);
module_init(sysfs_test_init);
module_exit(sysfs_test_exit);
