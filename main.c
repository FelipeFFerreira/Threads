#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"
#include <omp.h>

#define rand1() (rand() / (tipoDado)RAND_MAX)

#define N 500 //DEFINA O TAMANO DA MATRIZ AQUI
#define threads 6 //DEFINA A QUANTIDADE DE THREADS AQUI
#define WAIT 0
#define READY 1
#define EXECUTED -1

typedef struct {
	bool state;

}semaphore;
semaphore sem_control;

typedef int tipoDado;
tipoDado matrizResultante[N][N], matriz_1[N][N];
int matrizIndentidade[N][N];

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
	clock_t tempo;
	tempo = clock();
	int i = 0; 
	int j, pronto = N;
	ptrArgsArq _argssArq = (ptrArgsArq) argsArq;

	while (i < N) {
		if (_argssArq->statusArq[i] == READY) {
			for (j = 0; j < N; j++) {
				while(sem_control.state);
				fprintf(_argssArq->arq, "%d  ", matrizResultante[i][j]);
				//fprintf(_argssArq->arq, "%d\n", i); 
			}
			fprintf(_argssArq->arq, "%c", '\n'); 
			_argssArq->statusArq[i] = EXECUTED;
			i += 1;
		}
	}
	
	fclose(_argssArq->arq);
	printf("\n>>>>Tempo Total Do Processo De Escrita no Arquivo: %gs\n",(float) (clock() - tempo)  / CLOCKS_PER_SEC);

	//printf("\nAcabei de escrever\n");
	pthread_exit( (void*) 0 );		//Legado do retorno

}

void * multiplicacao(void * argss)
{
	
    args * _argss = (args *)argss;
    lst_ptr p;
	int i, j;
	p = _argss->lista;
	
	while(p != NULL) {
		for(i = 0;  i < N; i++) {
			for(j = 0; j < N; j++) {
				matrizResultante[p->dado][i] += matriz_1[p->dado][j] * matrizIndentidade[j][i];
			}
			_argss->ptrArq->statusArq[p->dado] = READY;
		}
		p = p->prox;
	}

	pthread_exit( (void*) 0 );//Legado do retorno
}


void criarMatrizIdentidade()
{
	int i;
	for(i = 0; i < N; i++) {
		matrizIndentidade[i][i] = 1;
	}
}

void criarMatrizRandomica()
{
	
	int i, j, cont = 0;
	for(i = 0; i < N; i++) {
		for(j = 0 ; j < N; j++) {
			matriz_1[i][j] = j + 1; //rand1();
		}
	}
}

void imprimirMatriz(tipoDado matriz[][N])
{	
	FILE * arq; // Arquivo lógico     
    if ((arq = fopen("result_mult_matriz.txt", "w")) == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo %s\n", "filename");
     }

	int i, j;
	printf("\n");
	for(i = 0; i < N; i++) {
		for(j = 0 ; j < N; j++) {
			printf("%d  ", matriz[i][j]);
			fprintf(arq, "%d", matriz[i][j]);
		}
		printf("\n");
		fprintf(arq, "\n");
	}
	printf("\n");
}

FILE *open_arquivo(char * c) {

    FILE * arq; // Arquivo lógico     
    if ((arq = fopen("result_mult_matriz_th.txt", c)) == NULL) {
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
	int i, j, status;
	clock_t tempo;
	args _args[threads - 1]; //numero de args por threads de CPU
	struct argsArq _argsArq; //thread responsavel pelo arquivo

	criarMatrizIdentidade();
	criarMatrizRandomica();

	_argsArq.arq = open_arquivo("w");

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

	fprintf(_argsArq.arq, "\n\n[Tempo Total Do Processo: %gs]", (float) (clock() - tempo)  / CLOCKS_PER_SEC);
	fclose(_argsArq.arq);

	
	print_responsabilidade_thread(_args);
	

	imprimirMatriz(matrizResultante);

	printf("\nTerminando processo ...\n");
	printf("Tempo Total Do Processo: %gs\n\n",(float) (clock() - tempo)  / CLOCKS_PER_SEC);

	return EXIT_SUCCESS;
}
