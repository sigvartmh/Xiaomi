#include "io.h"
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>

#include <rtdk.h>

#include <native/task.h>
#include <native/timer.h>

void respond(void* data);
int set_cpu(int cpu_number);
void busy_wait(void* data);

void respond(void* data)
{
	rt_task_set_periodic(NULL,TM_NOW,  100);
	int pin = (int)(size_t)data;
	int response = 1;
	while(1){
		rt_task_wait_period(NULL); 
		response = io_read(pin);
		io_write(pin,response);
		

	}

}

int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);

	rt_print_auto_init(1);
	io_init();
	int i;


	RT_TASK tasks[3];
	

	rt_task_create(&(tasks[0]), "A", 0, 99, T_CPU(0));
	rt_task_create(&(tasks[1]), "B", 0, 99, T_CPU(0));
	rt_task_create(&(tasks[2]), "C", 0, 99, T_CPU(0));

	if(rt_task_start(&(tasks[0]),&respond,(void*)(1)))  {printf("error");}
	if(rt_task_start(&(tasks[1]),&respond,(void*)(2)))  {printf("error");}
	if(rt_task_start(&(tasks[2]),&respond,(void*)(3)))  {printf("error");}



	pthread_t disturbance[10];
	for (i=0; i<10; i++){
		pthread_create(&(disturbance[i]),NULL,&busy_wait,NULL);
	}
	
	for (i=0; i<10; i++){
		pthread_join(disturbance[i],NULL);
	}

	while(1){
	}
	return 0;
};

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t),&cpu);
} 


void busy_wait(void* data){
	int i = 1000;
	while(1)
	{
		i= i*1000;
		i= i/1000;
	} 
}




