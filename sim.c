#include<stdio.h>
#include<stdlib.h>

#define MAX_PROCESSES 8
#define QUANTUM 4
#define MAX_SERVICO 8
#define DISCO 0
#define TDISCO 2
#define IMPRESSORA 1
#define TIMPRESSORA 4
#define FITA 2
#define TFITA 3

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

typedef struct Process{
    int id;//id do processo
    int tempo_chegada;
    int tempo_servico;//unidades de tempo totais para concluir sevico
    int tempo_io;//unidades de tempo totais para concluir io
    int inicio_io;//tempo de inicio do io
    int tipo_io;
    int resta_cpu;//unidades de tempo restantes para cocluir servico
    int resta_io;//unidades de tempo restantes para concluir io
}processo;

processo* getProcess(int id, int tempo_chegada, int hasIo){
    processo *p = (processo*)malloc(sizeof(processo));

    p->id = id;
    p->tempo_chegada = tempo_chegada;

    int tempo_servico = rand()%(MAX_SERVICO+1);
    while(tempo_servico == 0){
        int tempo_servico = rand()%(MAX_SERVICO+1);
    }
    p->tempo_servico = tempo_servico;
    p->resta_cpu = tempo_servico;
    int hasIO = rand()%2;
    if(hasIO){
        int tipoIo = rand()%3;
        p->tipo_io = tipoIo;
        if(tipoIo == DISCO){
            p->resta_io = TDISCO;
            p->tempo_io = TDISCO;
        }else if(tipoIo == IMPRESSORA){
            p->resta_io = TIMPRESSORA;
            p->tempo_io = TIMPRESSORA;
        }else if(tipoIo == FITA){
            p->resta_io = TFITA;
            p->tempo_io = TFITA;
        }
        p->inicio_io = rand()%(p->tempo_servico +1);
    }else{
        p->tempo_io = -1;
        p->tipo_io = -1;
        p->inicio_io = -1;
        p->resta_io = -1;
    }

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