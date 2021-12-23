// Using semaphores

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_NUM 10

int numero;

int done_pares   = 0;
int done_impares = 0;

// turno = { IMPARES, PARES, CONSUMIDOR }
typedef enum {
  IMPARES, PARES, CONSUMIDOR
} type_turno;

type_turno turno = IMPARES;

// MUTEX m
// VC vcImpares, vcPares, vcConsumidor

void *Impares(void *data){
  int i;
    
  for( i=1; i <= 2*MAX_NUM; i+=2 ){
	  // lock( m )
	  // while turno != IMPARES 
	  //   wait( vcImpares, m )
    numero=i;
          // signal( vcConsumidor )
	  // turno = CONSUMIDOR
	  // unlock( m )
  }
  done_impares = 1;
}

void *Pares(void *data){
  int i;
    
  for( i=2; i <= 2*MAX_NUM; i+=2 ){
	  // lock( m )
	  // while turno != PARES 
	  //   wait( vcPares, m )
    numero=i;
          // signal( vcConsumidor )
	  // turno = CONSUMIDOR
	  // unlock( m )
  }
  done_pares = 1;
}

void *Consumidor(void *data){

  while(!done_impares || !done_pares){
	  // lock( m )
	  // while turno != CONSUMIDOR
	  //   wait( vcConsumidor, m )
    printf("Dato: %d\n",numero);
          // if numero % 2 == 0
	  //   turno = IMPARES
          //   signal( vcImpares )
	  // else
	  //   turno = PARES
	  //   signal( vcPares )
	  // unlock( m )
  }
  printf("Consumidor done!!\n");
}

int main(int argc, char *argv[]) {
  pthread_t pTh_par, pTh_impar, pTh_cons;

  pthread_create(&pTh_par,   NULL, Pares,      NULL);
  pthread_create(&pTh_impar, NULL, Impares,    NULL);
  pthread_create(&pTh_cons,  NULL, Consumidor, NULL);

  pthread_join(pTh_par,   NULL);
  pthread_join(pTh_impar, NULL);
  pthread_join(pTh_cons,  NULL);
  
  exit(0);
}
