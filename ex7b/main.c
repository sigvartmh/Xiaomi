#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>

#include <rtdk.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>

static RT_TASK tsk[4];

RT_SEM sem;
RT_SEM res;

void print_pri(RT_TASK *tsk, char *str){
    struct rt_task_info tmp;
    rt_task_inquire(tsk, &tmp);
    rt_printf("b:%i, c:%i \n", tmp.bprio, tmp.cprio);
    rt_printf(str);
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
    print_pri(&tsk[1], "med task Waiting\n");
    busy_wait_ms(5);
    print_pri(&tsk[1], "Med task done\n");
}

void tskHig(void *i){
    rt_printf("High task waiting for semaphore\n");
    rt_sem_p(&sem, TM_INFINITE);
    rt_task_sleep_ms(3);
    rt_sem_p(&res, TM_INFINITE);
    print_pri(&tsk[2], "High task locked resource\n");
    busy_wait_ms(5);
    print_pri(&tsk[2], "High task done\n");
    rt_sem_v(&res);
}

void mainTsk(void *i){
    rt_printf("Broadcasting Sem\n");
    rt_sem_broadcast(&sem);
}


int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

   	rt_sem_create(&sem, "S", 0, S_PRIO);
   	rt_sem_create(&res, "R", 1, S_PRIO);

	rt_task_create(&(tsk[0]), "L", 0, 25, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[1]), "M", 0, 50, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[2]), "H", 0, 75, T_CPU(0) | T_JOINABLE);
    	rt_task_create(&(tsk[3]), "Main", 0, 75, T_CPU(0) | T_JOINABLE);

    rt_task_start(&(tsk[0]), &tskLow, (void*) 1);
    rt_task_start(&(tsk[1]), &tskMed, (void*) 2);
    rt_task_start(&(tsk[2]), &tskHig, (void*) 3);
    usleep(100000);
    rt_task_start(&(tsk[3]), &mainTsk, (void *) NULL);


    for(int i = 0; i < 4; i++){
        rt_printf("Joining tasks\n");
	rt_task_join(&tsk[i]);
    }

    rt_sem_delete(&res);
    rt_printf("Res delted\n");
    rt_sem_delete(&sem);
    rt_printf("Sem delted, exit\n");


    return 0;

};

