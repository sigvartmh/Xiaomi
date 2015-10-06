#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>

#include <rtdk.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>

RT_SEM sem;
RT_SEM res;

void print_pri(RT_TASK *tsk, char *str){
    struct rt_task_info tmp;
    rt_task_inquire(task, &tmp);
    rt_printf("b:%i, c:%i ", tmp.bprio, temp.cprio);
    rt_printf(s);
}

int rt_task_sleep_ms(unsigned long delay){
     return rt_task_sleep(1000*1000*delay);
}

void busy_wait_ms(unsigned long delay){
    unsigned long count = 0;
    while (count <= delay*10){
        rt_timer_spin(1000*100);
        count++;
    }
}


void tskLow(void *i){
    rt_printf("Low task waiting for semaphore\n");
    rt_sem_p(&sem, TM_INFINITE);
    rt_sem_p(&res, TM_INFINITE);
    print_pri(&tsk[0], "Low task locked resource\n");
    busy_wait_ms(3);
    print_pri(&tsk[0], "Low task done\n");
    rt_sem_v(&res);
}

void tskMed(void *i){
    rt_printf("Med task waiting for semaphore\n");
    rt_sem_p(&sem, TM_INFINITE);
    rt_task_sleep_ms(1);
    print_pri(&tsk[1], "med task locked resource\n");
    busy_wait_ms(5);
    print_pri(&tsk[0], "Med task done\n");
}

void tskHig(void *i){
    rt_printf("High task waiting for semaphore\n");
    rt_sem_p(&sem, TM_INFINITE);
    rt_task_sleep_ms(3);
    rt_sem_p(&res, TM_INFINITE);
    print_pri(&tsk[0], "High task locked resource\n");
    busy_wait_ms(5);
    print_pri(&tsk[0], "High task done\n");
    rt_sem_v(&res);
}

void mainTsk(void *i){
    rt_printf("Broadcasting Sem");
    rt_sem_broadcast(&sem);
}


int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);
	RT_TASK tsk[3];

    rt_sem_create(&sem, (const char) "S", 0, S_PRIO);
    rt_sem_create(&res, (const char) "S", 0, S_PRIO);

    rt_task_create(&(tsk[3]), (const char) "S", 0, 75, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[0]), (const char) "L", 0, 25, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[1]), (const char) "M", 0, 50, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[2]), (const char) "H", 0, 75, T_CPU(0) | T_JOINABLE);

    rt_task_start(&(tsk[0]), &tskLow, (void*) 1);
    rt_task_start(&(tsk[1]), &tskMed, (void*) 2);
    rt_task_start(&(tsk[2]), &tskHig, (void*) 3);
    usleep(100*1000);
    rt_task_start(&(tsk[3], &mainTsk, (void *) NULL);

    for(int i; i < 4; i++){
        rt_task_join(&tsk[i]);
    }

    rt_sem_delete(&res);
    rt_printf("Res delted\n")l
    rt_sem_delete(&sem);
    rt_printf("Sem delted, exit\n")l


    return 0;

};

