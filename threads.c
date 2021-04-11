
/* threads.c
 *
 * Definição das operações sobre o TAD threads e implementação das funçoes especificas 
 * sobre o arquivo main.c.
 * esse TAD.
 */

#include "threads.h"
#include <stdlib.h>

void print_matriz(tipoDado matriz[][N])
{
	tipoDado i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)
			printf("%lu", matriz[i][j]);
	}
	printf("\n");
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

bool status_create(int status) 
{
	if (status) {
		printf("Erro criando thread, retornou codigo %d\n", status );
		return false;
	}

	return true;
}

FILE *open_arquivo(char * str, char * modo) {

    FILE * arq; // Arquivo lógico     
    if ((arq = fopen(str, modo)) == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo %s\n", "filename");
        exit(1);
     }

    return arq;
}
