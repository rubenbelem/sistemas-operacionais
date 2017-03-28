#include <stdio.h>
#include <stdlib.h>

#define null NULL
typedef struct t_process TProcess;

typedef struct t_process{
  unsigned int pid;
  unsigned int startTime;
  unsigned int processTime;
  unsigned int waitTime;
  unsigned int endTime;
  TProcess *next;
  TProcess *last;
}TProcess;

typedef struct t_processPool{
  TProcess *first;
  unsigned int amount;
}TProcessPool;

TProcessPool* createProcessPool(){
  TProcessPool* pool = malloc(sizeof(TProcessPool));

  pool->first=null;
  pool->amount=0;

  return pool;
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
      process->next->waitTime = (process->endTime>process->next->startTime?process->endTime-process->next->startTime:0);
      process->next->endTime = process->next->waitTime>0? process->endTime+process->next->processTime : process->next->startTime+process->next->processTime;//(process->endTime+(process->next->startTime>process->endTime?process->next->startTime>process->endTime:0)+process->next->processTime);
      printf("{%d}",process->next->endTime);
    }
    process = process->next;
  }
}

char orderFrom(TProcess* process, TProcess* current, TProcess *last, int threshold){
  while(current){
    if(current->startTime <= threshold){
      if(current->processTime <= process->processTime){
        last = current;
        current = current->next;
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

  process->next = last->next;
  last->next = process;
  process->last = last;

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
          recalcFrom(process->last);
        }else{
          pool->first = process;
          process->endTime = (process->processTime+process->startTime);
          recalcFrom(process);
        }

        current->last = process;

        return 'A';

      }else{

        process->next = current->next;
        process->last = current;
        current->next = process;
        recalcFrom(current);

        return 'A';
      }
    }else if(process->startTime < current->endTime){
      orderFrom(process ,current->next, current, current->endTime);
      recalcFrom(current);
      return 'C';
    }else{
      last = current;
      current = current->next;
    }
  }

  process->next = last->next;
  last->next = process;
  process->last = last;

  recalcFrom(last);
  return 'B';
}

char insertFCFS(TProcessPool* pool, TProcess *current, TProcess* process){
  TProcess *last;
  while(current){
    // THERE IS SOMEONE BIGGER THAN THIS ONE
    if(current->startTime > process->startTime){
      process->waitTime=(current->last->endTime>process->startTime?(current->last->endTime-process->startTime):0);
      process->endTime=(process->startTime<current->last->endTime?current->last->endTime+process->processTime:process->startTime+process->processTime);

      process->next = current->last->next;
      current->last->next = process;

      return 'A';
    }else{
      last = current;
      current = current->next;
    }
  }
  // THERE WAS NO ONE BIGGER THAN THIS ONE
  process->waitTime=(last->endTime>process->startTime?(last->endTime-process->startTime):0);
  process->endTime=(process->startTime<last->endTime?last->endTime+process->processTime:process->startTime+process->processTime);

  process->next = last->next;
  last->next = process;

  return 'B';
}


void insertProcess(TProcessPool *pool, int start, int time, int pid){
  TProcess *current = pool->first;
  TProcess *process = malloc(sizeof(TProcess));
  process->last=process->next=NULL;

  process->processTime=time;
  process->startTime=start;
  process->pid=pid;

  if(!current){

    process->waitTime=0;
    process->endTime=(time+start);
    process->next=process->last=null;

    pool->first = process;

  }
  else{
    insertSJF(pool, current, process);
  }

  TProcess *p = pool->first;
  while(p){
    p = p->next;
  }

}

void printPool(TProcessPool *pool, int test, int exit){
  TProcess *process = pool->first, *toFree;
  int *exitBuffer = malloc(sizeof(int)*exit);
  int execSum=0;
  int waitSum=0;
  int order=0;
  printf("Teste %d\n", test);

  while(process){
    exitBuffer[order] = process->pid;
    execSum += process->endTime - process->startTime;
    waitSum += process->waitTime;
    toFree = process;
    process = process->next;
    free(toFree);
    order++;
  }
  printf("Tempo medio de execucao: %f\nTempo medio de espera: %f \n", ((float) execSum / (float)pool->amount), (float) waitSum / (float)pool->amount);

  for(int i=0;i<exit;i++){
    printf("P%d ",exitBuffer[i]);
  }

  free(exitBuffer);
}

int main(){
  int pNumber, pStart, pTime, pid=1, test=1;
  TProcessPool *pool=createProcessPool();

  while(scanf("%d", &pNumber)==1 && pNumber){
    pool->amount=pNumber;
    while(pid <= pNumber){
      scanf("%d", &pStart);
      scanf("%d",&pTime);
      insertProcess(pool, pStart, pTime, pid);
      pid++;
    }
    printPool(pool, test++, pNumber);
    printf("\n\n");
    pool->amount=0;
    pool->first=NULL;
    pid = 1;

  }
}
