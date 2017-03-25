#include <stdio.h>
#include <stdlib.h>

#define null NULL

typedef struct t_process{
  unsigned int pid;
  unsigned int startTime;
  unsigned int processTime;
  unsigned int waitTime;
  unsigned int endTime;
  struct t_process *next;
  struct t_process *last;
}TProcess;

typedef struct t_processPool{
  struct t_process *first;
  unsigned int waitSum;
  unsigned int execSum;
  unsigned int amount;
}TProcessPool;

void destroyProcessPool(struct t_processPool *pool){
  TProcess *process = pool->first;

  while(process){
    pool->first = process->next;
    free(process);
    process = process->next;
  }

  pool->first=null;
  pool->amount=0;
  pool->execSum=0;
  pool->waitSum=0;

}

TProcessPool* createProcessPool(){
  TProcessPool* pool = malloc(sizeof(TProcessPool));

  pool->first=null;
  pool->amount=0;
  pool->execSum=0;
  pool->waitSum=0;

  return pool;
}

char insertFCFS(TProcessPool* pool, TProcess *current, TProcess* process){
  TProcess *last;
  while(current){
    // THERE IS SOMEONE BIGGER THAN THIS ONE
    if(current->startTime > process->startTime){
      process->waitTime=(current->last->endTime>process->startTime?(current->last->endTime-process->startTime):0);
      process->endTime=(process->startTime<current->last->endTime?current->last->endTime+process->processTime:process->startTime+process->processTime);
      pool->execSum+=(process->endTime-process->startTime);
      pool->waitSum+=process->waitTime;

      process->next = current->last->next;
      current->last->next = process;

      return 0;
    }else{
      last = current;
      current = current->next;
    }
  }
  // THERE WAS NO ONE BIGGER THAN THIS ONE
  process->waitTime=(last->endTime>process->startTime?(last->endTime-process->startTime):0);
  process->endTime=(process->startTime<last->endTime?last->endTime+process->processTime:process->startTime+process->processTime);
  pool->execSum+=(process->endTime-process->startTime);
  pool->waitSum+=process->waitTime;

  process->next = last->next;
  last->next = process;

  return 1;
}

char insertSJF(TProcessPool* pool, TProcess *current, TProcess* process){
  TProcess *last;
  while(current){
    // THERE IS SOMEONE BIGGER THAN THIS ONE
    if(current->startTime <= process->startTime && process->processTime < current->processTime){
      process->next = current;
      process->last = current->last;
      if(current->last){
        current->last->next = process;
      }else{
        pool->first = process;
      }
      current->last = process;
      return 0;
    // }else if(current->startTime < process->startTime){
    //   process->next = current;
    //   process->last = current->last;
    //   if(current->last){
    //     current->last->next = process;
    //   }else{
    //     pool->first = process;
    //   }
    //   current->last = process;
    //   return 0;
    }else{
      last = current;
      current=current->next;
    }
  }

  process->next = null;
  process->last = last;
  last->next = process;


  return 1;
}

void insertProcess(struct t_processPool *pool, int start, int time, int pid){
  TProcess *current = pool->first;
  TProcess *process = malloc(sizeof(TProcess));

  process->processTime=time;
  process->startTime=start;
  process->pid=pid;

  //pool->amount++;

  if(!current){
    //printf("{inserted at first}");
    process->waitTime=0;
    process->endTime=time;
    process->next=process->last=null;

    pool->first = process;
    pool->execSum+=process->processTime;
  }
  else{
    insertSJF(pool, current, process);
  }

}

void printPool(TProcessPool *pool){
  TProcess *process = pool->first;
  printf("-=-=- ProcessPool %p -=-=- (amount : %u / execSum : %u / waitSum : %u)\n", pool, pool->amount, pool->execSum, pool->waitSum);
  printf("Bare informations : \n \t - Execution time average : %f \n\t - Waiting time average : %f \nst ex wa en\n", ((float) pool->execSum / (float)pool->amount), (float) pool->waitSum / (float)pool->amount);
  while(process){
    printf("[PID : %d] %u %u %u %u-> %p\n", process->pid , process->startTime, process->processTime, process->waitTime, process->endTime, process->next);
    process = process->next;
  }
}

int main(){
  int pNumber, pStart, pTime, pid=1;
  TProcessPool *pool;

  while(scanf("%u", &pNumber)==1 && pNumber){
    pool = createProcessPool();
    pool->amount=pNumber;
    while(pid <= pNumber){
      scanf("%u", &pStart);
      scanf("%u",&pTime);
      insertProcess(pool, pStart, pTime, pid);
      pid++;
    }

    printPool(pool);

    pid = 1;
    destroyProcessPool(pool);
  }
}
