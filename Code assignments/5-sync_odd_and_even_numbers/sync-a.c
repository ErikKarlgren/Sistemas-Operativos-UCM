// Using locks and conditional variables

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_NUM 50

// Ultimo numero mostrado por pantalla
int numero;

int done_pares = 0;
int done_impares = 0;

// turno = { IMPARES, PARES, CONSUMIDOR }
typedef enum
{
  IMPARES,
  PARES,
  CONSUMIDOR
} type_turno;

type_turno turno = IMPARES;

// MUTEX m
pthread_mutex_t m;
// VC vcImpares, vcPares, vcConsumidor
pthread_cond_t vcImpares;
pthread_cond_t vcPares;
pthread_cond_t vcConsumidor;

void *Impares(void *data)
{
  int i;

  for (i = 1; i <= 2 * MAX_NUM; i += 2)
  {
    pthread_mutex_lock(&m);

    while (turno != IMPARES)
      pthread_cond_wait(&vcImpares, &m);
    numero = i;
    pthread_cond_signal(&vcConsumidor);
    turno = CONSUMIDOR;

    pthread_mutex_unlock(&m);
  }
  done_impares = 1;
  pthread_exit(NULL);
}

void *Pares(void *data)
{
  int i;

  for (i = 2; i <= 2 * MAX_NUM; i += 2)
  {
    pthread_mutex_lock(&m);

    while (turno != PARES)
      pthread_cond_wait(&vcPares, &m);
    numero = i;
    pthread_cond_signal(&vcConsumidor);
    turno = CONSUMIDOR;

    pthread_mutex_unlock(&m);
  }
  done_pares = 1;
  pthread_exit(NULL);
}

void *Consumidor(void *data)
{
  while (!done_impares || !done_pares)
  {
    // We lock the mutex
    pthread_mutex_lock(&m);

    while (turno != CONSUMIDOR)
      pthread_cond_wait(&vcConsumidor, &m);

    printf("Dato: %d\n", numero);
    if (numero % 2 == 0) {
      turno = IMPARES;                 // make the "continue" condition for Impares true
      pthread_cond_signal(&vcImpares); // and then send a signal so Impares competes for the mutex when it becomes unlocked again
    }
    else {                             // similar explanation as above
      turno = PARES;
      pthread_cond_signal(&vcPares);
    }

    // We unlock the mutex
    pthread_mutex_unlock(&m);
  }
  printf("Consumidor done!!\n");
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  pthread_t pTh_par, pTh_impar, pTh_cons;

  // <by Erik>
  pthread_mutex_init(&m, NULL);
  pthread_cond_init(&vcImpares, NULL);
  pthread_cond_init(&vcPares, NULL);
  pthread_cond_init(&vcConsumidor, NULL);
  // </by Erik>

  pthread_create(&pTh_par, NULL, Pares, NULL);
  pthread_create(&pTh_impar, NULL, Impares, NULL);
  pthread_create(&pTh_cons, NULL, Consumidor, NULL);

  pthread_join(pTh_par, NULL);
  pthread_join(pTh_impar, NULL);
  pthread_join(pTh_cons, NULL);

  // <by Erik>
  pthread_mutex_destroy(&m);
  pthread_cond_destroy(&vcImpares);
  pthread_cond_destroy(&vcPares);
  pthread_cond_destroy(&vcConsumidor);
  // </by Erik>

  exit(0);
}
