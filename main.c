#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"

//#define INSTALL_OMP //Se for no linux INSTALL

#ifdef INSTALL_OMP
	#include <omp.h>
#endif

#define rand1() (rand() / (tipoDado)RAND_MAX)

#define N 1000 //DEFINA O TAMANO DA MATRIZ AQUI
#define threads 8 //DEFINA A QUANTIDADE DE THREADS AQUI

/*semaphoro controle de IO*/
#define WAIT 0
#define READY 1
#define EXECUTED -1

typedef struct {
	bool state;

}semaphore;
semaphore sem_control;

typedef unsigned long long int tipoDado;
tipoDado matrizResultante[N][N], matriz_1[N][N];
tipoDado matrizIndentidade[N][N];

typedef struct argsArq * ptrArgsArq;
struct argsArq {
	pthread_t thread;
    int statusArq[N];
    int id;
    FILE * arq;
};


typedef struct {
	pthread_t thread;
    lst_ptr lista;
    int id;
    ptrArgsArq ptrArq;
}args;


void * solicitacaoArquivo(void * argsArq)
{
	//clock_t tempo;
	//tempo = clock();
	tipoDado i = 0; 
	tipoDado j, pronto = N;
	ptrArgsArq _argssArq = (ptrArgsArq) argsArq;

	while (i < N) {
		if (_argssArq->statusArq[i] == READY) {
			for (j = 0; j < N; j++) {
				while(sem_control.state);
				fprintf(_argssArq->arq, "%lu  ", matrizResultante[i][j]);
			}
			fprintf(_argssArq->arq, "%c", '\n'); 
			_argssArq->statusArq[i] = EXECUTED;
			i += 1;
		}
	}
	
	fclose(_argssArq->arq);
	//printf("\n>>>>Tempo Total Do Processo De Escrita no Arquivo: %gs\n",(float) (clock() - tempo)  / CLOCKS_PER_SEC);

	//printf("\nAcabei de escrever\n");
	pthread_exit( (void*) 0 );		//Legado do retorno

}

void * multiplicacao(void * argss)
{
	
    args * _argss = (args *)argss;
    lst_ptr p;
	tipoDado j;
	p = _argss->lista;

	int k = 0;
	
	while (p != NULL) {
		#ifdef INSTALL_OMP
			#pragma omp parallel for
		#endif
		for (j = 0;  j < N; j++) {
			for (k = 0; k < N; k++) {
					matrizResultante[p->dado][j] = matrizResultante[p->dado][j] + matriz_1[p->dado][k] * matrizIndentidade[k][j];										
			}
		}
	
		_argss->ptrArq->statusArq[p->dado] = READY; //libera para escrita no arquivo
		p = p->prox;
	}
		

	pthread_exit( (void*) 0 );//Legado do retorno
}


void print_matriz(tipoDado matriz[][N])
{
	tipoDado i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)
			printf("%lu", matriz[i][j]);
	}
	printf("\n");
}

void * lerMatrizEntrada_1(void * fptr)
{
	FILE * fptr_ = (FILE *)fptr;
	tipoDado i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			fscanf(fptr_, "%d ", &matrizIndentidade[i][j]);
		}
	}
}

void * lerMatrizEntrada_2(void * fptr)
{
	FILE * fptr_ = (FILE *)fptr;
	tipoDado i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)
			fscanf(fptr_, "%lu", &matriz_1[i][j]);
	}
}

FILE *open_arquivo(char * str, char * modo) {

    FILE * arq; // Arquivo lógico     
    if ((arq = fopen(str, modo)) == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo %s\n", "filename");
     }

    return arq;
}


void print_responsabilidade_thread(args * _args)
{	
	int i;
	printf("\n____________________________________________________\n");
	for(i = 0; i < threads - 1; i++) {
		printf("\nMeu iDThread: %d\n", _args[i].id);
		printf("As linhas x colunas que fiquei responsavel :)\n");
		lst_print(_args[i].lista);
	}
	printf("\nJá eu iDThread: %d fiquei responsavel pelo Arquivo :)\n", i +1);
}

int main ()
{
	tipoDado i, j, status;
	clock_t tempo;
	args _args[threads - 1]; //numero de args por threads de CPU
	struct argsArq _argsArq; //thread responsavel pelo arquivo de saida

	pthread_t thread_1; //thread responsavel pelo arquivo de entrada
	pthread_t thread_2; //thread responsavel pelo arquivo de entrada

	FILE * fptr_1 = open_arquivo("/home/felipe/Faculdade/Programa Paralela/Threads/Matrizes/matriz_1.txt", "r");
	
	FILE * fptr_2 = open_arquivo("/home/felipe/Faculdade/Programa Paralela/Threads/Matrizes/matriz_2.txt", "r");
	

	status = pthread_create((&thread_1), NULL, lerMatrizEntrada_1, (void *)fptr_1);
	if ( status ) {
		printf("Erro criando thread %d, retornou codigo %d\n", i, status );
		return EXIT_FAILURE;
	}

	status = pthread_create((&thread_2), NULL, lerMatrizEntrada_2, (void *)fptr_2);
	if ( status ) {
		printf("Erro criando thread %d, retornou codigo %d\n", i, status );
		return EXIT_FAILURE;
	}

	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);

	fclose(fptr_1);
	fclose(fptr_2);

	_argsArq.arq = open_arquivo("matriz_resultante.csv", "w");

	//#pragma omp for
	for(i = 0; i < N; i++) _argsArq.statusArq[i] = WAIT;


	/*Repassa IdxThread*/
	for(i = 0; i < threads - 1; i++) {
	 lst_init(&_args[i].lista);
	 _args[i].id = i + 1;
	}
	_argsArq.id = i;

	/*Repassa linha de trabalho por thread*/
	//#pragma omp for
	for(i = 0; i < N; i++) lst_ins(&_args[i % (threads - 1)].lista, i);
	
	/*Repassa função de trabalho*/
	tempo = clock();

	for(i = 0; i < threads - 1; i++) {
		_args[i].ptrArq = &_argsArq;
		status = pthread_create((&_args[i].thread), NULL, multiplicacao, (void *)&_args[i]);
		if ( status ) {
			printf("Erro criando thread %d, retornou codigo %d\n", i, status );
			return EXIT_FAILURE;
		}
	}
	
	//Thered delegada para escrita do resulto no arquivo
	status = pthread_create((&_argsArq.thread), NULL, solicitacaoArquivo, (void *)&_argsArq);
	if ( status ) {
		printf("Erro criando thread %d, retornou codigo %d\n", i, status );
		return EXIT_FAILURE;
	}

	/*Thered principal aguarda todas as thredes de trabalhos finalizarem
	  para proseguir
	*/
	for(i = 0; i < threads - 1; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	pthread_join(_argsArq.thread, NULL);


	//fprintf(_argsArq.arq, "\n\n[Tempo Total Do Processo: %gs]", (float) (clock() - tempo)  / CLOCKS_PER_SEC);
	
	print_responsabilidade_thread(_args);
	
	fclose(_argsArq.arq);
	//imprimirMatriz(matrizResultante);

	printf("\nTerminando processo ...\n");
	printf("\n\n[Tempo Total Do Processo: %fs]\n", (float) (clock() - tempo)  / CLOCKS_PER_SEC);

	

	return EXIT_SUCCESS;
}
