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


// INSERE O PRIMEIRO
//
//
//
// SE INSERIDO.start <= CORRENTE.start
//
// 	SE INSERIDO.start == CORRENTE.start && INSERIDO.TEMPO < CORRENTE.tempo
// 		INSERE ANTES DO CORRENTE
// 	SENAO
// 		INSERE DEPOIS DO CORRENTE
// 	FIM
//
// SENAO SE INSERIDO.start < CORRENTE.end
// 	//ORDENA COM TODOS ENTRE 1 ATÉ 7s
// SENAO
// 	tentaProximo
// FIM

void recalcFrom(TProcess* process){
  while(process){
    if(process->next){
      process->next->waitTime = (process->endTime - process->next->startTime);
      process->next->endTime = (process->next->startTime + process->processTime);
    }
    process = process->next;
  }
}

char orderFrom(TProcess* process, TProcess* current, int threshold){
  while(current){
    if(current->startTime < threshold){
      if(current->processTime < process->processTime){
        current=current->next;
      }else{
        process->last = current->last;
        process->next = current;
        current->last = current->last->next = process;
        return 0;
      }
    }else{
      current = null;
    }
  }

  return 1;
}

char insertSJF(TProcessPool* pool, TProcess *current, TProcess* process){
  TProcess *last;
  while(current){
    if(process->startTime <= current->startTime){
      if(process->startTime == current->startTime && process->processTime < current->processTime){

        process->last = current->last;
        process->next = current;


        if(current->last){
          current->last->next = process;
        }else{
          pool->first = process;
          process->endTime = (process->processTime+process->startTime);
        }

        current->last = process;

        recalcFrom(process);
        return 0;

      }else{

        process->next = current->next;
        process->last = current;
        current->next = process;
        recalcFrom(current);

        return 0;
      }
    }else if(process->startTime < current->endTime){
      orderFrom(process ,current->next, current->endTime);
      recalcFrom(current);
      return 2;
    }else{
      last = current;
      current = current->next;
    }
  }

  process->next = last->next;
  last->next = process;

  recalcFrom(current);

  return 1;
}

void insertProcess(struct t_processPool *pool, int start, int time, int pid){
  TProcess *current = pool->first;
  TProcess *process = malloc(sizeof(TProcess));

  process->processTime=time;
  process->startTime=start;
  process->pid=pid;

  if(!current){

    process->waitTime=0;
    process->endTime=(time+start);
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
