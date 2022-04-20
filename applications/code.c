#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;

/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1)
    {
        rt_kprintf("thread1 low pro %d\n", count ++);
        rt_thread_mdelay(100);
    }
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;
/* 线程2入口 */
static void thread2_entry(void *param)
{
    rt_uint32_t count = 0;

    /* 线程2拥有较高的优先级，以抢占线程1而获得执行 */
    for (count = 0; count < 30 ; count++)
    {
        /* 线程2打印计数值 */
        rt_kprintf("Waaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaagh!!!!!!\n");
    }
    rt_kprintf("The thread is over now\n");
    
}           


/* 删除线程示例的初始化 */
int My_thread(void)
{
        /* 创建线程1，名称是thread1，入口是thread1_entry*/
        //（动态线程）
        tid1 = rt_thread_create("thread1",
                                thread1_entry, RT_NULL,
                                THREAD_STACK_SIZE,
                                THREAD_PRIORITY, THREAD_TIMESLICE);
        
        /* 如果获得线程控制块，启动这个线程 */
        if (tid1 != RT_NULL)
            rt_thread_startup(tid1);

    /* 初始化线程2，名称是thread2，入口是thread2_entry */
    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(My_thread, thread sample);





