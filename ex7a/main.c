#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>

#include <rtdk.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>

RT_SEM sem;

void taskSem(void *T){

    int n = (int)(size_t) T

    rt_printf("Task%d: Waiting for sem\n", n);
    rt_sem_p(&sem, TM_INFINITE);
    rt_printf("Task%d: Recived sem exiting", n);

}

void master(void *T){

    int n = (int)(size_t) T;

    rt_printf("Broadcasting\n");
    rt_sem_broadcast(&sem);
}


int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);
	RT_TASK tsk[3];

    rt_sem_create(&sem, (const char) "S", 0, S_PRIO);


	rt_task_create(&(tsk[0]), (const char) "1", 0, 25, T_CPU(0));
	rt_task_create(&(tsk[1]), (const char) "2", 0, 50, T_CPU(0));
	rt_task_create(&(tsk[2]), (const char) "M", 0, 75, T_CPU(0));

    rt_task_start(&(tsk[0]), &taskSem, (void*) 1);
    rt_task_start(&(tsk[1]), &taskSem, (void*) 2);

    usleep(1000*1000);
    rt_task_start(&(tsk[2]), &master, (void*) NULL);
    usleep(1000*1000);

    rt_sem_delete(&sem);

    rt_printf("Sem delted, exit\n")l

    return 0;

};

