#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"
#define rand1() (rand() / (tipoDado)RAND_MAX)

#define N 400 //DEFINA O TAMANO DA MATRIZ AQUI
#define threads 6 //DEFINA A QUANTIDADE DE THREADS AQUI



typedef float tipoDado;
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
	int i, j, pronto = N;
	ptrArgsArq _argssArq = (ptrArgsArq) argsArq;

	while(pronto > 0 ) {
		for(i = 0; _argssArq->statusArq[i] == 1 || _argssArq->statusArq[i] == -1; i++) {
			if(_argssArq->statusArq[i] != -1) {
				for(j = 0; j < N; j++) {
					fprintf(_argssArq->arq, "%.3f  ", matrizResultante[i][j]); 
				}
				fprintf(_argssArq->arq, "%c", '\n'); 
				_argssArq->statusArq[i] = -1;
				pronto--;
			}
		}
	}
	printf(">>>>Tempo Total Do Processo: %gs\n\n",(float) (clock() - tempo)  / CLOCKS_PER_SEC);

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
			//_argss->ptrArq->statusArq[p->dado] = 1;
		}
		p = p->prox;
	}

	pthread_exit( (void*) 0 );		//Legado do retorno
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
			matriz_1[i][j] = rand1();//j + 1;
		}
	}
}

void imprimirMatriz(tipoDado matriz[][N])
{
	int i, j;
	printf("\n");
	for(i = 0; i < N; i++) {
		for(j = 0 ; j < N; j++) {
			printf("%.3f  ", matriz[i][j]);
		}
		printf("\n");
	}
	printf("\n");

}

FILE *open_arquivo(char * c) {

    FILE * arq; // Arquivo lógico     
    if ((arq = fopen("arq_multi_threads", c)) == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo %s\n", "filename");
     }

    return arq;
}

int main ()
{

	int i, j, status;
	//srand(time(NULL));
	clock_t tempo;
	args _args[threads - 1]; //numero de args por threads de cpu
	struct argsArq _argsArq; //thread responsavel pelo arquivo

	_argsArq.arq = open_arquivo("w");
	for(i = 0; i < N; i++) _argsArq.statusArq[i] = 0;

	criarMatrizIdentidade();
	criarMatrizRandomica();

	/*Repassa IdxThread*/
	for(i = 0; i < threads - 1; i++) {
	 lst_init(&_args[i].lista);
	 _args[i].id = i + 1;
	}
	_argsArq.id = i;

	/*Repassa linha de trabalho por thread*/
	for(i = 0; i < N; i++) lst_ins(&_args[i % (threads - 1)].lista, i);
	
	tempo = clock();	
	for(i = 0; i < threads - 1; i++) {
		_args[i].ptrArq = &_argsArq;
		status = pthread_create((&_args[i].thread), NULL, multiplicacao, (void *)&_args[i]);
		if ( status ) {
			printf("Erro criando thread %d, retornou codigo %d\n", i, status );
			return EXIT_FAILURE;
		}
	}
	/*
	status = pthread_create((&_argsArq.thread), NULL, solicitacaoArquivo, (void *)&_argsArq);
	if ( status ) {
		printf("Erro criando thread %d, retornou codigo %d\n", i, status );
		return EXIT_FAILURE;
	}
*/
	for(i = 0; i < threads - 1; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	//pthread_join(_argsArq.thread, NULL);
	fprintf(_argsArq.arq, "\n\n[Tempo Total Do Processo: %gs]", (float) (clock() - tempo)  / CLOCKS_PER_SEC);
	fclose(_argsArq.arq);

	printf("\n____________________________________________________\n");
	for(i = 0; i < threads - 1; i++) {
		printf("\nMeu iDThread: %d\n", _args[i].id);
		printf("As linhas x colunas que fiquei responsavel :)\n");
		lst_print(_args[i].lista);
	}
	printf("\nJá eu iDThread: %d fiquei responsavel pelo Arquivo :)\n", i +1);

	
	imprimirMatriz(matriz_1);
	printf("\nX\n");
	imprimirMatriz((void *)matrizIndentidade);
	printf("\n=\n");
	imprimirMatriz(matrizResultante);

	printf("Terminando processo ...\n");
	printf("Tempo Total Do Processo: %gs\n\n",(float) (clock() - tempo)  / CLOCKS_PER_SEC);

	return EXIT_SUCCESS;
}
