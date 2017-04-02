#include <pthread.h> 
#include <stdio.h>

int matSudoku[9][9]; /* this data is shared by the thread(s) */
int resSudoku[27]; //this will hold boolean values depending of the threads' results.

void *runner(void *param); /* threads call this function */

typedef struct {
	int row;
	int column;
	int function; //que funcion va a correr el hilo (esto deberia ser un apuntador a la funcion si queremos ser muy tight)
} parametros;

//funciones que definire abajo
void rowChecker(parametros *p);
void columnChecker(parametros *p);
void squareChecker(parametros *p);




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
	Los primeros 9 [0 al 8] seran rowCheckers.
	Los segundos 9 [9 al 17] seran columnChecker
	Los terceros 9 [18 al 26] seran squareCheckers
	*/

	//rowCheckers
	for(i = 0; i < 9; i+=1){
		p[i].row = i;
		p[i].column = 0;
		p[i].function = 0;
	}
	
	//columnChecker	
	for(i = 9; i < 18; i+=1){
		p[i].row = 0;
		p[i].column = i;
		p[i].function = 1;
	}

	//squareCheckers
	for(i = 18; i < 27; i+=1){
		int j = i - 18; //to normalize things a little bit.
		
		if(j < 3)
		{
			p[i].row = 0;
			p[i].column = j*3;
		}
		else if(j < 6){
			p[i].row = 3;
			p[i].column = (j-3)*3;
		}
		else{
			p[i].row = 6;
			p[i].column = (j-6)*3;
		}
		
		p[i].function = 2;
	}




	pthread_t tid; /* the thread identifier */
	pthread_attr_t attr; /* set of thread attributes */

	/* get the default attributes */
	pthread_attr_init(&attr);
	/* create the thread */

	//Crear nuestros threads!!!!
	for(i = 0; i < 27; i+=1){
		pthread_create(&tid,&attr,runner,&p[i]); //crear cada thread con su 'parametros' respectivos
	}



	pthread_create(&tid,&attr,runner,argv[1]); /* wait for the thread to exit */
	pthread_join(tid,NULL); 

	return 0;
}


void *runner(void *param)
{ 
	//ejecutar funcion segun param pasado
	
	parametros* p = param; //This seems to work... le damos 'forma' al void*
	
	//Esto lo debo sustituir para ser mas elegante y usar apuntadores a funciones
	if(p->function == 0)
	{
		rowChecker(param);
	}
	else if(p->function == 1)
	{
		columnChecker(param);
	}
	else if(p->function == 2)
	{
		squareChecker(param);
	}
	else
	{
		printf("Something would be really wrong..");
	}
	

	pthread_exit(0);
}

//funcion que sera 0
void rowChecker(parametros *p){
	
	printf("Soy un rowChecker!!");

}

//funcion que sera 1
void columnChecker(parametros *p){

	printf("Soy un columnChecker!!");

}



//funcion que sera 2
void squareChecker(parametros *p){
	
	printf("Soy un squareChecker!!");
	
}
