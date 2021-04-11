#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>

#define N 1000 //DEFINA O TAMANO DA MATRIZES A SEREM GERADAS AQUI


typedef unsigned long long int tipoDado;


tipoDado matriz[N][N];

FILE *open_arquivo(char * str, char * modo) {

    FILE * arq; // Arquivo lógico     
    if ((arq = fopen(str, modo)) == NULL) {
        fprintf(stderr, "Erro na abertura do arquivo %s\n", "filename");
     }

    return arq;
}

void criar_matriz(char * nome_arq, char * modo)
{	
	tipoDado i, j;

	FILE * fptr;
	fptr = open_arquivo(nome_arq, modo);

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)
			fprintf(fptr, "%lu ",  j + 1);
		fprintf(fptr, "\n");
	}

	fclose(fptr);
}

int main()
{
	criar_matriz("matriz_1.txt", "w");
	criar_matriz("matriz_2.txt", "w");

	return 0;
}