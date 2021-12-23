#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define TABACO 0
#define PAPEL 1
#define CERILLAS 2

#define BOOL int


/*
    MÍRATE LA SOLUCIÓN DEL PROFESOR DE NUEVO, QUE ES MÁS SENCILLA
    Fecha: 14 de diciembre de 2020
*/


// recurso compartido
int ingredientes[3] = {0, 0, 0};

// semáforo para escritura y lectura sobre 'ingredientes'
sem_t semaforo;

// semáforo para que el agente espere a que se acaben los ingredientes
sem_t agent_sem;


// semáforo de espera (para cuando no se cumple una condición)
sem_t smokers_sem; // si solo usas semaforos, lo necesitas
                    // si usas tambén mutex no es necesario

// CORRECCIÓN: mejor ten una variable con el número de fumadores bloqueados
int nBloqueados = 0;

void fumar()
{
    sleep(5);
}

// sería parecido al resto de fumadores
// no lo voy a probar por pereza xd lol
void fumadorConPapel(void)
{
    while (1 == 1)
    {
        lock(mutex);

        if (ingredientes[TABACO] == ingredientes[CERILLAS] == 1)
        {
            // cojo el tabaco
            ingredientes[TABACO] = 0;
            // cojo las cerillas
            ingredientes[CERILLAS] = 0;

            sem_post(&agent_sem);
        }
        else // encima de la mesa no hay ingredientes
        {
            nBloqueados++;
            unlock(mutex);
            wait(semaforo)
        }

        unlock(mutex);

        // y fumamos, que nos lo merecemos
        fumar();
    }
}

void agente(void)
{
    int waiting_smokers;

    while (1 == 1)
    {

        // Conseguir 2 ingredientes al azar
        int i = rand() % 3;
        int j;
        do
        {
            j = rand() % 3;
        } while (i == j);

        lock(mutex);

        // esperamos a que se acaben los ingredientes
        while (ingredientes[CERILLAS] || ingredientes[TABACO] || ingredientes[PAPEL])
        {
            // dejamos que otro hilo acceda
            sem_post(&ingrs_sem);
            // hacemos que espere el agente
            sem_wait(&agent_sem);
            // y cuando haya terminado de esperar, bloqueamos
            // el semáforo de nuevo
            sem_wait(&ingrs_sem);
        }

        // añadimos los ingredientes
        ingredientes[i] = 1;
        ingredientes[j] = 1;

        // despertamos a los fumadores
        // sem_getvalue(&smokers_sem, &waiting_smokers);   // no es una función que hayamos dado, aunque vale
        /*for (int i = 0; i < waiting_smokers; i++)
        {
            sem_post(&smokers_sem);
        }*/
        while (nBloqueados > 0)
        {
            sem_post(semaforo);
            nBloqueados--;
        }

        unlock(mutex);
    }
}

int main(void)
{
    sem_init(&ingrs_sem, 0, 1);
    sem_init(&smokers_sem, 0, 0);
    sem_init(&agent_sem, 0, 0);

    sem_destroy(&ingrs_sem);
    sem_destroy(&smokers_sem);
    sem_destroy(&agent_sem);
}