#include <semaphore.h>
#include <pthread.h>

const int num_sillas = 10;
// clientes en sala de espera
int num_clientes = 0;
int barbero_dormido = 0;

sem_t barbero_sem;
sem_t clientes_sem;

pthread_mutex_t mutex;

void Barbero()
{
    while (1 == 1)
    {
        lock(mutex);

        if (num_clientes == 0)
        {
            // duerme el barbero, esperando a un cliente
            barbero_dormido = 1;
            unlock(mutex);
            sem_wait(&barbero_sem);
            // tras despertar, bloqueamos el mutex de nuevo
            lock(mutex);
        }
        num_clientes--;

        unlock(mutex);

        // "llama" a un cliente para cortarle el pelo
        signal(clientes_sem);
        cortarPelo();
    }
}

void Cliente()
{
    lock(mutex);
    if (num_clientes < num_sillas)
    {
        num_clientes++;

        if (barbero_dormido)
        {
            barbero_dormido = 0;
            sem_post(&barbero_sem);
        }
        unlock(mutex);

        // espera a que le "llame" el barbero
        wait(clientes_sem);
        recibirCorteDePelo();
    }
    else
    {
        // hay que liberar el mutex que usamos para leer
        // 'num_clientes'
        unlock(mutex);
    }
}