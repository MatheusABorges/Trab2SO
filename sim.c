#include<stdio.h>
#include<stdlib.h>

#define MAX_PROCESSES 8

typedef struct Element{
    int valor;
    struct Element *prox;
}elemento;

typedef struct Queue{
    elemento *primeiro;
    elemento *ultimo;
    int size;
}fila;

fila* getQueue(){
    fila *aux = (fila*)malloc(sizeof(fila));
    elemento *primeiro = (elemento*)malloc(sizeof(elemento));
    aux->primeiro = primeiro;
    aux->ultimo = primeiro;
    aux->primeiro->valor = -1;
    aux->size = 0;

    return aux;
}

void inserir(int n, fila* f){
    if(f->size >= MAX_PROCESSES) {
        printf("fila estourada\n");
    }

    elemento *aux = (elemento*)malloc(sizeof(elemento));

    aux->valor = n;
    f->ultimo->prox = aux;
    f->ultimo = aux;

    if(f->primeiro->valor == -1){
        f->primeiro = aux;
    }
    f->size++;
}

int remover(fila *f){
    int aux = f->primeiro->valor;
    elemento *temp = f->primeiro;

    f->primeiro = f->primeiro->prox;
    free(temp);
    
    if(f->size == 1){
        elemento *primeiro = (elemento*)malloc(sizeof(elemento));
        f->primeiro = primeiro;
        f->ultimo = primeiro;
        f->primeiro->valor = -1;
    }
    f->size--;
    
    return aux;
}

int main(){

    fila *f = getQueue();
    fila *g = getQueue();
    for(int i=0; i<MAX_PROCESSES*2; i++){
        if(i%2 == 0){
            inserir(i,f);
        }else{
            inserir(i,g);
        }
    }

    for(int i=0; i<MAX_PROCESSES; i++){
        printf("f: %d\n", remover(f));
        printf("g: %d\n", remover(g));
    }

    return 0;
}