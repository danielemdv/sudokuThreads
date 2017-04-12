#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int matSudoku[9][9]; /* this data is shared by the thread(s) */
int resSudoku = 0; //this will have a value of 27 when all threads are done working
int acumRes = 0;

pthread_mutex_t mutexres,mutexcheck; //Variable to be able to mutex lock critical sections of threads.

/*
Help hint!!

we use the lock with:
	pthread_mutex_lock (&mutexres);
		//critical section, accesing common memory etc etc
	pthread_mutex_unlock (&mutexres);

*/

int semaphore = 1; //only one thread will be able to be in the critical section at a time. so start at one.

void *runner(void *param); /* threads call this function */

typedef struct {
	int id; //estoy agregando id...
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

	pthread_mutex_init(&mutexres, NULL); //Inicializamos correctamente la variable mutex
	pthread_mutex_init(&mutexcheck, NULL);

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
		p[i].id = i;
		p[i].row = i;
		p[i].column = 0;
		p[i].function = 0;
	}

	//columnChecker
	for(i = 9; i < 18; i+=1){
		p[i].id = i;
		p[i].row = 0;
		p[i].column = i-9;
		p[i].function = 1;
	}

	//squareCheckers
	for(i = 18; i < 27; i+=1){
		p[i].id = i;
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


	pthread_t tid[27]; /* the thread identifiers */
	pthread_attr_t attr; /* set of thread attributes */

	/* get the default attributes */
	pthread_attr_init(&attr);
	/* create the thread */


	//Crear nuestros threads!!!!
	for(i = 0; i < 27; i+=1){
		pthread_create(&tid[i],&attr,runner,&p[i]); //crear cada thread con su 'parametros' respectivos
	}


	//Aquí esta el join viejo...
/*
	for(i = 0; i < 27; i+=1){
		pthread_join(tid[i],NULL); //esperar a todos los threads...
	}
*/


//--------EN VEZ DEL JOIN!--------

//Implementando que espere a que los threads acaben sin join (lo queria hacer con conditionals y signals pero no parece ad hoc)

	int flag = 1;
	while(flag){
		pthread_mutex_lock (&mutexcheck);
			if(resSudoku == 27){
				pthread_mutex_unlock (&mutexcheck);
				flag = 0;
				printf("%s\n", "Ya acabaron los threads, podemos salir!!");
			}else{
				pthread_mutex_unlock (&mutexcheck);
				printf("%s\n", "Todavia no acaban los threads...");
			}
	}
	//Si salimos de este loop significa que todos los threads, aunque no necesariamente hayan hecho exit todavia, ya hicieron su trabajo.

	if(acumRes == 27){
		printf("%s\n", "El sudoku es valido!!");
	}else{
		printf("%s\n", "El sudoku NO es valido!!");
	}

	//Destruimos memoria que ya no utilizamos
	pthread_mutex_destroy(&mutexres);
	pthread_mutex_destroy(&mutexcheck);
	pthread_attr_destroy(&attr);

	return 0; //exit normally
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
	/*El row checker checara que esten todos los numeros del 1 al 9*/

	int arr[9]; //array to hold true values (1) for the numbers we have found.
	int i = 0;

	for(i = 0; i < 9; i = i+1 ){
		//leer del sudoku no deberia de ser bloqueado.
		int lec = matSudoku[p->row][i] - 1; //-1 porque estamos leyendo nums de 1 a 9 del sudoku y necesitamos de 0 a 8.
		arr[lec] = 1; //switch the entry of the array to true. This will crack if we read something that's not a number between 1 and 9 (which makes an index of 0 to 8)
	}

	int flag = 1;
	for(i = 0; i < 9; i = i+1){
		if(arr[i] == 0){
			flag = 0; //if any number is missing, flag is set to zero
		}
	}



	//Proteccion de seccion critica por acceso a variable comun entre threads.
	if(flag){
		pthread_mutex_lock (&mutexres);
			acumRes += 1;
		pthread_mutex_unlock (&mutexres);
	}

	//register completion
	pthread_mutex_lock (&mutexcheck);
		resSudoku  += 1;
	pthread_mutex_unlock (&mutexcheck);
}

//funcion que sera 1
void columnChecker(parametros *p){


	int arr[9]; //array to hold true values (1) for the numbers we have found.
	int i = 0;

	for(i = 0; i < 9; i = i+1 ){
		//leer del sudoku no deberia de ser bloqueado.
		int lec = matSudoku[i][p->column] - 1; //-1 porque estamos leyendo nums de 1 a 9 del sudoku y necesitamos de 0 a 8.
		arr[lec] = 1; //switch the entry of the array to true
	}

	int flag = 1;
	for(i = 0; i < 9; i = i+1){
		if(arr[i] == 0){
			flag = 0; //if any number is missing, flag is set to zero
		}
	}

	//Proteccion de seccion critica por acceso a variable comun entre threads.
	if(flag){
		pthread_mutex_lock (&mutexres);
			acumRes += 1;
		pthread_mutex_unlock (&mutexres);
	}

	//register completion
	pthread_mutex_lock (&mutexcheck);
		resSudoku  += 1;
	pthread_mutex_unlock (&mutexcheck);

}



//funcion que sera 2
void squareChecker(parametros *p){

	int arr[9]; //array to hold true values (1) for the numbers we have found.
	int i = 0;
	int j = 0;
	int row,column;
	row = p->row;
	column = p->column;

	int lec = -1;


	for(i = row; i < row + 3; i += 1 ){
		//leer del sudoku no deberia de ser bloqueado.
		for(j = column; j < column + 3; j += 1){
			lec = matSudoku[i][j] - 1; //-1 porque estamos leyendo nums de 1 a 9 del sudoku y necesitamos de 0 a 8.
			arr[lec] = 1; //switch the entry of the array to true
		}
	}


	int flag = 1;
	for(i = 0; i < 9; i = i+1){
		if(arr[i] != 1){
			flag = 0; //if any number is missing, flag is set to zero
		}
	}

	//Proteccion de seccion critica por acceso a variable comun entre threads.
	if(flag){
		pthread_mutex_lock (&mutexres);
			acumRes += 1;
		pthread_mutex_unlock (&mutexres);
	}

	//register completion
	pthread_mutex_lock (&mutexcheck);
		resSudoku  += 1;
	pthread_mutex_unlock (&mutexcheck);
}
