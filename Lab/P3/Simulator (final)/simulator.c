#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "so_utils.h"
#include "terminal_colors.h"

#define N_PARADAS 5		// numero de paradas de la ruta
#define EN_RUTA 0		// autobus en ruta
#define EN_PARADA 1		// autobus en la parada
#define MAX_USUARIOS 40 // capacidad del autobus
#define USUARIOS 4		// numero de usuarios

#define TRUE 1
#define FALSE 0

// Si es TRUE, muestra información adicional sobre los ocupantes del autobús y
// los usuarios que esperan a subir o bajar.
int debug_mode = FALSE;

// "constantes"
int num_paradas = N_PARADAS;
int capacidad_autobus = MAX_USUARIOS;
int total_usuarios = USUARIOS;

// estado inicial
int estado = EN_RUTA;
// parada en la que se encuentra el autobus
int parada_actual = 0;
// ocupantes que tiene el autobus
int n_ocupantes = 0;

// personas que desean subir en cada parada
int *esperando_subir; //= {0,0,...0};
// personas que desean bajar en cada parada
int *esperando_bajar; //= {0,0,...0};

// Otras definiciones globales (comunicacion y sincronizacion)
pthread_mutex_t mutex; // Mutex global: se usa para cualquier acceso a un recurso compartido
pthread_cond_t usuario_esperando, autobus_en_espera;

// Sleeps for a random amount of seconds. This value is returned by the function.
// If debug_mode is TRUE, doesn't sleep (0 seconds).
// If debug_mode is FALSE, sleeps a random time between 0 and 5 seconds.
int random_sleep()
{
	int random_time = debug_mode ? 0 : rand() % 5;
	sleep(random_time);
	return random_time;
}

void mostrar_estado(void)
{
	if (debug_mode)
		printf("Ocupantes: %d\t Esperan subir: %d\t Esperan bajar: %d\n",
			   n_ocupantes, esperando_subir[parada_actual], esperando_bajar[parada_actual]);
}

/* Ajustar el estado y bloquear al autobus hasta que no haya pasajeros que
quieran bajar y/o subir la parada actual. Despues se pone en marcha */
void autobus_en_parada()
{
	lock(&mutex);
	estado = EN_PARADA;
	printf(YEL "Autobus ha llegado a la parada %d\n" RESET, parada_actual);
	mostrar_estado();

	while ((esperando_subir[parada_actual] != 0 && n_ocupantes < capacidad_autobus) ||
		   esperando_bajar[parada_actual] != 0)
	{
		broadcast(&usuario_esperando);
		wait(&autobus_en_espera, &mutex);
		mostrar_estado();
	}

	unlock(&mutex);
}

/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
void conducir_hasta_siguiente_parada()
{
	lock(&mutex);
	estado = EN_RUTA;
	printf(YEL "Autobus sale de la parada %d (ocupantes: %d)\n\n" RESET, parada_actual, n_ocupantes);
	unlock(&mutex);

	random_sleep();

	lock(&mutex);
	parada_actual = (parada_actual + 1) % num_paradas;
	assert(parada_actual >= 0 && parada_actual < num_paradas);
	unlock(&mutex);
}

/* El usuario indicara que quiere subir en la parada ’origen’, esperara a que
el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
proporcionar informacion de depuracion */
void subir_autobus(int id_usuario, int origen)
{
	lock(&mutex);

	printf(CYN "Usuario %d espera al autobús en la parada %d\n" RESET, id_usuario, origen);

	esperando_subir[origen]++;
	while (estado == EN_RUTA || parada_actual != origen || n_ocupantes >= capacidad_autobus)
	{
		wait(&usuario_esperando, &mutex);
	}
	esperando_subir[origen]--;
	n_ocupantes++;

	printf(CYN "Usuario %d ha subido al autobús\n" RESET, id_usuario);
	mostrar_estado();

	// Avisa al autobús de que no puede subir más gente
	if (esperando_subir[parada_actual] == 0 || n_ocupantes >= capacidad_autobus)
		signal(&autobus_en_espera);

	unlock(&mutex);
}

/* El usuario indicara que quiere bajar en la parada ’destino’, esperara a que
el autobus se pare en dicha parada y bajara. El id_usuario puede utilizarse para
proporcionar informacion de depuracion */
void bajar_autobus(int id_usuario, int destino)
{
	lock(&mutex);

	printf(BLU "Usuario %d espera a llegar a la parada %d\n" RESET, id_usuario, destino);

	esperando_bajar[destino]++;
	while (estado == EN_RUTA || parada_actual != destino)
	{
		wait(&usuario_esperando, &mutex);
	}
	esperando_bajar[destino]--;
	n_ocupantes--;

	printf(BLU "Usuario %d ha bajado del autobús\n" RESET, id_usuario);
	mostrar_estado();

	// Avisa al autobús para que pueda subir más gente o pueda marcharse.
	if (esperando_bajar[parada_actual] == 0)
		signal(&autobus_en_espera);

	unlock(&mutex);
}

void *thread_autobus(void *args)
{
	while (TRUE)
	{
		// esperar a que los viajeros suban y bajen
		autobus_en_parada();
		// conducir hasta siguiente parada
		conducir_hasta_siguiente_parada();
	}
}
void usuario(int id_usuario, int origen, int destino)
{
	// Esperar a que el autobus este en parada origen para subir
	subir_autobus(id_usuario, origen);
	// Bajarme en estacion destino
	bajar_autobus(id_usuario, destino);
}
void *thread_usuario(void *args)
{
	// obtener el id del usario
	int id_usuario;
	static int sig_id_usuario = 0;

	lock(&mutex);
	id_usuario = sig_id_usuario++;
	unlock(&mutex);

	while (TRUE)
	{
		int a, b;
		a = rand() % num_paradas;
		do
		{
			b = rand() % num_paradas;
		} while (a == b);
		usuario(id_usuario, a, b);
	}
}

// Inicializa mutexes y variables condicionales
void setup_mutexes_y_vc(void)
{
	init_mut(&mutex);
	init_vc(&autobus_en_espera);
	init_vc(&usuario_esperando);
}

void parse_args(int argc, char *argv[])
{
	static char *usage = "Uso: %s [-d] [-h] [-a capacidad_autobus] [-u total_usuarios] [-p num_paradas]\n";
	extern char *optarg;
	extern int optind, opterr, optopt;
	int opt, arg_as_int, err = 0;

	while ((opt = getopt(argc, argv, "da:u:p:h")) != -1)
	{
		switch (opt)
		{
		case 'd':
			debug_mode = TRUE;
			break;
		case 'a':
			arg_as_int = atoi(optarg);
			capacidad_autobus = arg_as_int;
			break;
		case 'u':
			arg_as_int = atoi(optarg);
			total_usuarios = arg_as_int;
			break;
		case 'p':
			arg_as_int = atoi(optarg);
			num_paradas = arg_as_int;
			break;
		case 'h':
			fprintf(stderr, usage, argv[0]);
			exit(1);
			break;
		default:
			err = 1;
			break;
		}
	}
	if (err)
	{
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	// Definicion de variables locales a main
	pthread_t pthr_autobus;
	//pthread_t usuarios[USUARIOS];
	pthread_t *pthr_arr_usuarios;

	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	parse_args(argc, argv);

	// Iniciar mutexes y variables de condición, y alocar memoria para los arrays globales
	setup_mutexes_y_vc();
	esperando_subir = (int *)calloc(num_paradas, sizeof(int));
	esperando_bajar = (int *)calloc(num_paradas, sizeof(int));
	pthr_arr_usuarios = (pthread_t *)calloc(total_usuarios, sizeof(pthread_t));

	// Crear el thread Autobus
	pthread_create(&pthr_autobus, NULL, thread_autobus, NULL);

	// Crear thread para el usuario i
	for (int i = 0; i < total_usuarios; i++)
		pthread_create(&pthr_arr_usuarios[i], NULL, thread_usuario, NULL);

	// Esperar terminacion de los hilos
	for (int i = 0; i < total_usuarios; i++)
		pthread_join(pthr_arr_usuarios[i], NULL);

	pthread_join(&pthr_autobus, NULL);

	free(esperando_subir);
	free(esperando_bajar);
	free(pthr_arr_usuarios);

	return 0;
}
