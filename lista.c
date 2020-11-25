
/* lista.c
 *
 * Implementação das operações sobre o TAD lista ordenada implementada
 * de forma encadeada.
 *
 *
 */

#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void lst_init(lst_ptr * l) {
    *l = NULL;
}

void lst_ins(lst_ptr * l, lst_info val) {

    lst_ptr n;
    if ((n = (lst_ptr) malloc(sizeof(struct lst_no))) == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria!\n");
        exit(1);
    }
    n->dado = val;
    if (*l == NULL) {
        n->prox = *l;
        *l = n;
        return;
    }
    else {
        lst_ptr p = *l;
         while (p->prox != NULL) {
            p = p->prox;
         }
         n->prox = p->prox;
         p->prox = n;
         return;
    }
}

void lst_print(lst_ptr l) {
    printf("[ ");
    while (l != NULL) {
        printf("%d ,", l->dado);
        l = l->prox;
    }
    printf("\b ]\n");
}
