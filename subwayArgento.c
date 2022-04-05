#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50
#define LONGITUD_MAXIMA_CADENA 1000

//creo estructura de semaforos 
struct semaforos {
	sem_t sem_cortar;
	sem_t sem_mezclar;
	//poner demas semaforos aqui
	sem_t sem_agregarSal;
	sem_t sem_cocinar;
	sem_t sem_hornearPan;
	sem_t sem_prepararEnsalada;
	sem_t sem_armarSandwichA;
	sem_t sem_armarSandwichB;
};

//creo los pasos con los ingredientes
struct paso {
	char accion [LIMITE];
     	char ingredientes[4][LIMITE];
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

sem_t sem_hornoCompartido;
sem_t sem_salCompartida;
sem_t sem_sartenCompartida;

void escribirEnLog(char *mensaje, char *modo) {
	const char* filename = "logSubwayArgento.txt";
	size_t resultado;

	FILE *fichero = fopen(filename, modo);
	if (fichero == NULL) {
		printf("\tErrror al escribir\n");
		return;
	}

	resultado = fwrite(mensaje, sizeof(char), strlen(mensaje), fichero);

	if(fclose(fichero) != 0) {
		printf("No se pudo cerrar archivo");
		return;
	}
}


//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
			//printf("mydata->pasos_param[i].accion %s \n", mydata->pasos_param[i].accion);
			//printf("Accion %s \n", accionIn);
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("\t-----------------------------------------------------------------------------------------Equipo %d - accion %s COMENZO \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
}

void* imprimirFin(void *data, char *accionIn, int finalizo) {	
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	int i;
	for(i = 0; i < sizeArray; i ++){
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0)
			printf("\t-----------------------------------------------------------------------------------------Equipo %d - accion %s FINALIZO \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
	}

	if(finalizo == 1) {
		printf("\t-----------------------------------------------------------------------------------------Equipo %d - Termino el circuito!! \n " , mydata->equipo_param);
	}
}


//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;


    	sem_wait(&mydata->semaforos_param.sem_cortar);

	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 500000 );
	imprimirFin(mydata,accion, 0);

	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    	sem_post(&mydata->semaforos_param.sem_mezclar);
	
    	pthread_exit(NULL);
}


void* mezclar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "mezclar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;

	
    	sem_wait(&mydata->semaforos_param.sem_mezclar);

	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 500000 );
	imprimirFin(mydata,accion, 0);

	//doy la señal a la siguiente accion (mezclar me habilita agregarSal)
    	sem_post(&mydata->semaforos_param.sem_agregarSal);

    	pthread_exit(NULL);
}


void* agregarSal(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "agregarSal";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;

	
    	sem_wait(&mydata->semaforos_param.sem_agregarSal);
	sem_wait(&sem_salCompartida);

	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 500000 );
	imprimirFin(mydata,accion, 0);

	//doy la señal a la siguiente accion (agregar sal me habilita a cocinar)
	sem_post(&mydata->semaforos_param.sem_cocinar);

	//Libero la sal compartida
	sem_post(&sem_salCompartida);
    	pthread_exit(NULL);
}


void* cocinar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cocinar";
	struct parametro *mydata = data;

    	sem_wait(&mydata->semaforos_param.sem_cocinar);
	sem_wait(&sem_sartenCompartida);

	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	usleep( 500000 );
	imprimirFin(mydata,accion, 0);
	
	//doy la señal a la siguiente accion (cocinar me habilita armarSandwichA)
	sem_post(&mydata->semaforos_param.sem_armarSandwichA);
	
	//Libero sarte compartida
	sem_post(&sem_sartenCompartida);
    	pthread_exit(NULL);
}


void* hornearPan(void *data) {
	//printf("Horneando \n");
	//creo el nombre de la accion de la funcion 
	char *accion = "hornearPan";
	struct parametro *mydata = data;

    	sem_wait(&mydata->semaforos_param.sem_hornearPan);
    	sem_wait(&sem_hornoCompartido);


	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	usleep( 500000 );
	imprimirFin(mydata,accion, 0);

	//doy la señal a la siguiente accion (hornearPan me habilita prepararEnsalada)
	sem_post(&mydata->semaforos_param.sem_prepararEnsalada);
	

	//Libero el horno compartido
	sem_post(&sem_hornoCompartido);
    	pthread_exit(NULL);
}

void* prepararEnsalada(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "prepararEnsalada";
	struct parametro *mydata = data;

    	sem_wait(&mydata->semaforos_param.sem_prepararEnsalada);

	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	usleep( 500000 );
	imprimirFin(mydata,accion, 0);

	sem_post(&mydata->semaforos_param.sem_armarSandwichB);
	
    	pthread_exit(NULL);
}

void* armarSandwich(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "armarSandwich";
	struct parametro *mydata = data;

    	sem_wait(&mydata->semaforos_param.sem_armarSandwichA);
    	sem_wait(&mydata->semaforos_param.sem_armarSandwichB);
	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	usleep( 500000 );
	imprimirFin(mydata,accion, 1);

    	pthread_exit(NULL);
}


void* ejecutarReceta(void *i) {
	
	//DECLARO SEMAFOROS
	//variables semaforos
	sem_t sem_cortar;
	sem_t sem_mezclar;
	//crear variables semaforos aqui
	sem_t sem_agregarSal;
	sem_t sem_cocinar;
	sem_t sem_hornearPan;
	sem_t sem_prepararEnsalada;
	sem_t sem_armarSandwichA;
	sem_t sem_armarSandwichB;
	
	//variables hilos
	pthread_t p1; 
	//crear variables hilos aqui
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	

	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos	
	pthread_data->semaforos_param.sem_cortar = sem_cortar;
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	//setear demas semaforos al struct aqui	
	pthread_data->semaforos_param.sem_agregarSal = sem_agregarSal;
	pthread_data->semaforos_param.sem_cocinar = sem_cocinar;	
	pthread_data->semaforos_param.sem_hornearPan = sem_hornearPan;
	pthread_data->semaforos_param.sem_prepararEnsalada = sem_prepararEnsalada;
	pthread_data->semaforos_param.sem_armarSandwichA = sem_armarSandwichA;
	pthread_data->semaforos_param.sem_armarSandwichB = sem_armarSandwichB;

	

	//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
	//Listado de todas las acciones a realizar
///
	const char* filename="receta.txt";

	FILE *archivo = fopen(filename, "r");
	char bufer[LONGITUD_MAXIMA_CADENA];
	int numAccion=0;

	while(fgets(bufer, LONGITUD_MAXIMA_CADENA, archivo)) {
		strtok(bufer, "\n");

		int numIngrediente=0;
		char *token;
		token=strtok(bufer, "|");
		//printf("accion %d : %s \n", numAccion, token);	
	     	strcpy(pthread_data->pasos_param[numAccion].accion, token);

		while(token != NULL){
			token=strtok(NULL, "|");
			if(token != NULL){
				//printf("ingredientes %d : %s \n", numIngrediente, token);
				strcpy(pthread_data->pasos_param[numAccion].ingredientes[numIngrediente], token);
				numIngrediente++;
			}
		}
		numAccion++;

	}
	fclose(archivo);
///

/*
     	strcpy(pthread_data->pasos_param[0].accion, "cortar");
	strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo 1");
	strcpy(pthread_data->pasos_param[0].ingredientes[1], "ajo 2");
	strcpy(pthread_data->pasos_param[0].ingredientes[2], "puñado de perejil");

	strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
	strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[1].ingredientes[2], "huevo");
	strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne");

	strcpy(pthread_data->pasos_param[2].accion, "agregarSal");
	strcpy(pthread_data->pasos_param[2].ingredientes[0], "sal");

	strcpy(pthread_data->pasos_param[3].accion, "cocinar");
	strcpy(pthread_data->pasos_param[3].ingredientes[0], "Milanesa en sarten");
	
	strcpy(pthread_data->pasos_param[4].accion, "hornearPan");
	strcpy(pthread_data->pasos_param[4].ingredientes[0], "pan");
*/
	
	
	//INICIALIZO SEMAFOROS -> REFERENCIA A SEMAFORO, INDICA COMPARTIDO ENTRE PROCESOS, INICIALIZACION DEL SEMAFORO
	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_cortar),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	//inicializar demas semaforos aqui	
	sem_init(&(pthread_data->semaforos_param.sem_agregarSal),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar),0,0);

	sem_init(&(pthread_data->semaforos_param.sem_hornearPan),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_prepararEnsalada),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarSandwichA),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarSandwichB),0,0);



	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    	int rc;
    	rc = pthread_create(&p1,                           //identificador unico
                     	NULL,                          //atributos del thread
                        cortar,             //funcion a ejecutar
                        pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui
	
	//Creo hilo para mezclar
	rc = pthread_create(&p2, NULL, mezclar, pthread_data);
	
	//Creo hilo para AgregarSal	
	rc = pthread_create(&p3, NULL, agregarSal, pthread_data);
	
	//Creo hilo para Cocinar
	rc = pthread_create(&p4, NULL, cocinar, pthread_data);

	//Creo hilo para hornear pan
	rc = pthread_create(&p5, NULL, hornearPan, pthread_data);

	//Creo hilo para prepara ensalada
	rc = pthread_create(&p6, NULL, prepararEnsalada, pthread_data);

	//Creo hilo para armar sandwich
	rc = pthread_create(&p7, NULL, armarSandwich, pthread_data);
	
	
	//join de todos los hilos
	pthread_join(p1,NULL);
	//crear join de demas hilos
	pthread_join(p2,NULL);

	pthread_join(p3, NULL);

	pthread_join(p4, NULL);

	pthread_join(p5, NULL);

	pthread_join(p6, NULL);

	pthread_join(p7, NULL);


	//valido que el hilo se alla creado bien 
 	if (rc){
       		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	}

	 
	//destruccion de los semaforos
	sem_destroy(&sem_cortar);
	sem_destroy(&sem_mezclar);
	//destruir demas semaforos 	
	sem_destroy(&sem_agregarSal);
	sem_destroy(&sem_cocinar);
	sem_destroy(&sem_hornearPan);
	sem_destroy(&sem_prepararEnsalada);
	sem_destroy(&sem_armarSandwichA);
	sem_destroy(&sem_armarSandwichB);
	
	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	escribirEnLog("Comienzo log subway argento\n", "w+");


	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	int *equipoNombre4 =malloc(sizeof(*equipoNombre4));
	//faltan equipos
  
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;
	*equipoNombre4 = 4;


	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	//faltan hilos
	pthread_t equipo3;
	pthread_t equipo4;
	
	//Inicializo semaforos para utensillos compartidos
	sem_init(&sem_hornoCompartido, 0, 1);
	sem_init(&sem_salCompartida, 0, 1);
	sem_init(&sem_sartenCompartida, 0, 1);
  
	//inicializo los hilos de los equipos
    	rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    	rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);
  	//faltn inicializaciones
    	rc = pthread_create(&equipo3,NULL,ejecutarReceta,equipoNombre3);
    	rc = pthread_create(&equipo4,NULL,ejecutarReceta,equipoNombre4);


   	if (rc){
       		printf("Error:unable to create thread, %d \n", rc);
	        exit(-1);
     	} 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);
	pthread_join (equipo4,NULL);
	//.. faltan joins


    	escribirEnLog("Finalizo log subway argento\n", "a");
    	pthread_exit(NULL);
}


//Para compilar:   gcc subwayArgento.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
