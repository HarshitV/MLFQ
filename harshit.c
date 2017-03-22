#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

struct PROCESS {
	int totaltimeLeft;
	int arrivalTime;
	int round_robin_left;
	int time_left_in_queue;
}processes[100];

int PBT;
int number_of_processes;

int timeSlice[5];
int queues[5];

int nextTime;

int globalTime = 0;
int number_of_queues;

int queue[5][1000];

int front[5];
int rear[5];

void insert(int X, int i)
{
	queue[i][rear[i]++] = X;
}

int TOP(int i)
{
	return queue[i][front[i]];	
}

void POP(int i)
{
	front[i]++;
}

void delay()
{
	int i=0;
	for(i=0;i<1e8;i++);
}

int quantum[100];
int curr;

int turnAround[100];

void func(int idx, int i)
{
	printf("Process %d, Time %d,%d\n",idx+1, processes[idx].totaltimeLeft, processes[idx].time_left_in_queue);
	delay();
	processes[idx].totaltimeLeft--;
	if(processes[idx].totaltimeLeft == 0)
		turnAround[idx] = globalTime - processes[idx].arrivalTime;
	processes[idx].time_left_in_queue--;
	processes[idx].round_robin_left--;
	globalTime ++;
}

int cmpfunc(const void *a, const void *b)
{
	return ((*(struct PROCESS *)a).arrivalTime - (*(struct PROCESS *)b).arrivalTime);
}

int main()
{
	
	memset(rear,0,sizeof(rear));
	scanf("%d",&number_of_queues);
	
	int i=0;
	
	for(i=0;i<number_of_queues-1;i++)
		scanf("%d",&timeSlice[i]);
	timeSlice[number_of_queues-1] = 1000000;
	scanf("%d",&PBT);
	scanf("%d",&number_of_processes);
	for(i=0;i<number_of_queues;i++)
		scanf("%d",&quantum[i]);
	// printf("HER\n");
	for(i=0;i<number_of_processes;i++)
	{
		scanf("%d%d",&processes[i].totaltimeLeft, &processes[i].arrivalTime);
		processes[i].time_left_in_queue = timeSlice[0];
	}
	// for(i=0;i<number_of_processes;i++)
	// 	printf("%d %d\n",processes[i].arrivalTime, processes[i].totaltimeLeft );

	qsort(processes, number_of_processes, sizeof(struct PROCESS), cmpfunc);
	// printf("Input Done %d\n",processes[0].arrivalTime);

	int idx = 0;
	while(true)
	{
		bool flag = false;
		// printf("%d\n",processes[0].arrivalTime);
		while(idx < number_of_processes && processes[idx].arrivalTime == globalTime)
		{
			// printf("HERE\n");
			insert(idx, 0);
			processes[idx].round_robin_left = quantum[0];
			idx++;
			flag = true;
		}
		// printf("%d %d\n", idx, processes[idx].arrivalTime);
		// printf("Queue edited\n");		
		if(idx < number_of_processes)
			nextTime = processes[idx].arrivalTime;
		else
			nextTime = 1000000;

		if(idx == number_of_processes)
		{
			int x;
			bool flag = false;
			for(x=0;x<number_of_queues;x++)
			{
				if(front[x] < rear[x])
					flag = true;
			}
			if(!flag)
				break;
		}


		for(i=0;i<number_of_queues;i++)
		{
			bool brk = false;
			if(front[i] < rear[i])
			{
				int x = TOP(i);
				int j;
				for(j=0;j<quantum[i];j++)
				{
					if(processes[x].round_robin_left == 0)
					{
						POP(i);
						if(processes[x].time_left_in_queue == 0)
						{
							processes[x].round_robin_left = quantum[i+1];
							insert(x,i+1);
						}
						else
						{
							processes[x].round_robin_left = quantum[i];
							insert(x,i);
						}
						brk = true;
						break;
					}
					func(x, i);
					// printf("Global Time: %d\n",globalTime);
					if(globalTime % PBT == 0)
					{
						int k;
						for(k=1;k<number_of_queues;k++)
						{
							front[i] = rear[i];
						}
						for(k=0;k<idx;k++)
						{
							if(processes[k].totaltimeLeft)
							{
								processes[k].round_robin_left = quantum[0];
								processes[k].time_left_in_queue = timeSlice[0];
								insert(k,0);
							}
						}
						brk = true;
						break;
					}
					if(processes[x].time_left_in_queue == 0)
					{
						POP(i);
						if(processes[x].totaltimeLeft)
						{
							processes[x].time_left_in_queue = timeSlice[i+1];
							processes[x].round_robin_left = quantum[i+1];
							insert(x, i+1);
							brk=true;
						}
						break;
					}
					if(globalTime == nextTime)
					{
						brk = true;
						break;
					}
					if(processes[x].totaltimeLeft == 0)
					{
						POP(i);
						break;
					}
				}
				if(brk)
					break;
				if(processes[x].totaltimeLeft)
				{
					POP(i);
					processes[x].round_robin_left = quantum[i];
					insert(x,i);
					break;
				}
			}
		}
	}
	printf("TurnAround Time:\n");
	for(i=0;i<number_of_processes;i++)
		printf("%d ",turnAround[i]);
	printf("\n");
	return 0;
}