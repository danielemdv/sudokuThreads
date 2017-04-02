#include <pthread.h> 
#include <stdio.h>

int matSudoku[9][9]; /* this data is shared by the thread(s) */

void *runner(void *param); /* threads call this function */

typedef struct {
	int fila;
	int columna;
	int funcion; //que funcion va a correr el hilo
} parametros;




int main(int argc, char *argv[])
{ 
	int i = 0;
	int j = 0;

	parametros p[27]; //creamos un arreglo de estructuras tipo 'parametros'

	for(i = 0; i < 9; i+=1){
		for(j = 0; j < 9; j+=1){
			scanf("%d",&matSudoku[i][j]); //Ir leyendo stdin
		}
	}

	for(i = 0; i < 9; i+=1){
		for(j = 0; j < 9; j+=1){
			printf("%d ",matSudoku[i][j]); //imprimirlo porque si
		}
		printf("\n");
	}

	//vamos a asignar la estructura que se le pasara a cada thread.
	/*
	Los primeros 9 (0 al 8) seran rowCheckers.
	Los segundos 9 (9 al 17) seran columnChecker
	Los terceros 9 (18 al 26) seran squareCheckers
	*/
	for(i = 0 i < 27; i+=1){
		p[i].fila = ;
		p[i].columna = ;
		p[i].funcion = ;

	}




	pthread_t tid; /* the thread identifier */
	pthread_attr_t attr; /* set of thread attributes */

	/* get the default attributes */
	pthread_attr_init(&attr);
	/* create the thread */

	//Crear nuestros threads!!!!
	for(i = 0; i < 27; i+=1){
		pthread_create(&tid,&attr,runner,p[i]); //crear cada thread con su 'parametros' respectivos
	}



	pthread_create(&tid,&attr,runner,argv[1]); /* wait for the thread to exit */
	pthread_join(tid,NULL); 

}


void *runner(void *param)
{ 
	//ejecutar funcion segun param pasado

	pthread_exit(0);
}

//funcion que sera 0
void rowChecker(parametros *p){
	

}

//funcion que sera 1
void columnChecker(parametros *p){


}



//funcion que sera 2
void squareChecker(parametros *p){
	

}