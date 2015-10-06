#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>

#include <rtdk.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>

RT_SEM sem;
RT_MUTEX mu_a;
RT_MUTEX mu_b;

static RT_TASK tsk[4];

void print_pri(RT_TASK *tsk, char *str){
    struct rt_task_info tmp;
    rt_task_inquire(tsk, &tmp);
    rt_printf("b:%i, c:%i ", tmp.bprio, tmp.cprio);
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
    rt_mutex_acquire(&mu_a, TM_INFINITE);
    print_pri(&tsk[0], "Low task locked A\n");
    rt_task_set_priority(NULL, 50);
    busy_wait_ms(3);

    rt_mutex_acquire(&mu_b, TM_INFINITE);
    print_pri(&tsk[0], "Low task locked B\n");
    busy_wait_ms(3);
    rt_mutex_release(&mu_b);
    rt_mutex_release(&mu_a);
    busy_wait_ms(1);
    rt_task_set_priority(NULL, 25);
    print_pri(&tsk[0], "Low task done\n");
}

void tskHig(void *i){
    rt_printf("High task waiting for semaphore\n");
    rt_sem_p(&sem, TM_INFINITE);
    rt_task_sleep_ms(1);
    rt_mutex_acquire(&mu_b, TM_INFINITE);
    print_pri(&tsk[2], "High task locked B\n");
    busy_wait_ms(1);
    rt_mutex_acquire(&mu_a, TM_INFINITE);
    print_pri(&tsk[2], "High task locked A\n");
    busy_wait_ms(2); 
    rt_mutex_release(&mu_a);
    rt_mutex_release(&mu_b);
    busy_wait_ms(1);
    print_pri(&tsk[2], "High task done\n");
}

void mainTsk(void *i){
    rt_printf("Broadcasting Sem\n");
    rt_sem_broadcast(&sem);
}


int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);
	rt_mutex_create(&mu_a, "A");
	rt_mutex_create(&mu_b, "B");
	
    rt_task_create(&(tsk[3]), "Main", 0, 75, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[0]), "L", 0, 25, T_CPU(0) | T_JOINABLE);
	rt_task_create(&(tsk[2]), "H", 0, 50, T_CPU(0) | T_JOINABLE);

    rt_task_start(&(tsk[0]), &tskLow, (void*) 1);
    rt_task_start(&(tsk[2]), &tskHig, (void*) 3);
    usleep(100*1000);
    rt_task_start(&(tsk[3]), &mainTsk, (void *) NULL);

    for(int i = 0; i < 4; i++){
        rt_task_join(&tsk[i]);
    }

    rt_mutex_delete(&mu_a);
    rt_printf("muA delted\n");
    rt_mutex_delete(&mu_b);
    rt_printf("muB delted\n");
    rt_sem_delete(&sem);
    rt_printf("Sem delted, exit\n");

    return 0;

};

