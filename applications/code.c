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

/********************************************************************************************************/

static void Time_turn_thread1(void *param)
{
    rt_uint32_t count = 0;

    while(1)
		{
			count++;
			rt_kprintf("The thread 1 is count to %d\n",count);
			if( count == 100 )
			{
				rt_kprintf("The thread 1 is now over\n");
				break;
			}

		}
    
}


static void Time_turn_thread2(void *param)
{
    rt_uint32_t count = 0;

    while(1)
		{
			count++;
			rt_kprintf("The thread 2 is count to %d\n",count);
			if( count == 100 )
			{
				rt_kprintf("The thread 2 is now over\n");
				break;
			}
		}
    
}



int Time_thread(void)
{

   tid1 = rt_thread_create("thread1",
                           Time_turn_thread1, 
													 RT_NULL,
                           THREAD_STACK_SIZE,
                           THREAD_PRIORITY,
													 THREAD_TIMESLICE + 5);/* 时间片长度为 10	*/
        
   /* 如果获得线程控制块，启动这个线程 */
   if (tid1 != RT_NULL)
       rt_thread_startup(tid1);


    rt_thread_init(&thread2,
                   "thread2",
                   Time_turn_thread2,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY,
									 THREAD_TIMESLICE);/* 时间片长度为 5 */
    rt_thread_startup(&thread2);

    return 0;
}




/****************************************************************************************************************/
/* 用于控制定时器 */
static rt_timer_t timer1;
static rt_timer_t timer2;
static rt_uint32_t Timer_count = 0;
static void Timer_thread1(void *param)
{
		Timer_count++;
		rt_kprintf("The timer 1 has worked for %d time\n",Timer_count);
		if( Timer_count == 10 )
		{
			rt_timer_stop(timer1);
			rt_kprintf("The timer 1 is now over\n");
		}

    
}


static void Timer_thread2(void *param)
{
		rt_kprintf("The timer 2 has comes\n");    
}



int Timer_thread(void)
{

       /* 创建定时器1  周期定时器 */
    timer1 = rt_timer_create("timer1",
															Timer_thread1,
															RT_NULL, 
															10,
															RT_TIMER_FLAG_PERIODIC);

    /* 启动定时器1 */
    if (timer1 != RT_NULL) rt_timer_start(timer1);


    /* 创建定时器2 周期定时器 */
    timer2 = rt_timer_create("timer2",
															Timer_thread2,
															RT_NULL,
															30,
															RT_TIMER_FLAG_PERIODIC);

    /* 启动定时器2 */
    if (timer2 != RT_NULL) rt_timer_start(timer2);
    return 0;
}

/**************************************************************************************************************/

struct rt_semaphore sem_1, sem_2, sem_3;
static rt_thread_t Sem1;
static rt_thread_t Sem2;
static rt_thread_t Sem3;

static void Sem_Func_1(void *param)
{
	rt_uint32_t count = 0;
	while(1)
	{
		rt_sem_take(&sem_3, RT_WAITING_FOREVER);
		count++;								/* 从一个侧面可以反应时间 */
		rt_kprintf("The Sem 1 has done! The number is %d\n",count);
		rt_sem_release(&sem_1);
		if(count == 10)
			break;
		rt_thread_mdelay(10);
	}
	rt_kprintf("The Sem 1 has over!\n");
    
}


static void Sem_Func_2(void *param)
{
	rt_uint32_t count = 0;
	while(1)
	{
		rt_sem_take(&sem_1, RT_WAITING_FOREVER);
		count++;								/* 从一个侧面可以反应时间 */
		rt_kprintf("The Sem 2 has done! The number is %d\n",count);
		rt_sem_release(&sem_2);
		if(count == 10)
			break;
		rt_thread_mdelay(50);
	}
	rt_kprintf("The Sem 2 has over!\n");
    
}


static void Sem_Func_3(void *param)
{
	rt_uint32_t count = 0;
	while(1)
	{
		rt_sem_take(&sem_2, RT_WAITING_FOREVER);
		count++;								/* 从一个侧面可以反应时间 */
		rt_kprintf("The Sem 3 has done! The number is %d\n",count);
		rt_sem_release(&sem_3);
		if(count == 10)
			break;
		rt_thread_mdelay(20);
	}
	rt_kprintf("The Sem 3 has over!\n");
    
}



int Sem_thread(void)
{
	rt_sem_init(&sem_1, "Sem1",   0, RT_IPC_FLAG_FIFO);
  rt_sem_init(&sem_2, "Sem2",   0, RT_IPC_FLAG_FIFO);
	rt_sem_init(&sem_3, "Sem3",   5, RT_IPC_FLAG_FIFO);

	
	Sem1 = rt_thread_create("First",
                          Sem_Func_1,
													RT_NULL,
                          THREAD_STACK_SIZE,
                          THREAD_PRIORITY + 1,
													THREAD_TIMESLICE);
	
	Sem2 = rt_thread_create("Second",
                          Sem_Func_2,
													RT_NULL,
                          THREAD_STACK_SIZE,
                          THREAD_PRIORITY,
													THREAD_TIMESLICE);
	
	Sem3 = rt_thread_create("Third",
                          Sem_Func_3,
													RT_NULL,
                          THREAD_STACK_SIZE,
                          THREAD_PRIORITY - 1,
													THREAD_TIMESLICE);
													
	rt_thread_startup(Sem1);
	rt_thread_startup(Sem2);
	rt_thread_startup(Sem3);
	
	return 0;
}



