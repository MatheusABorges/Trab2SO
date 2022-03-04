#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAX_PROCESSES 3
#define QUANTUM 4
#define MAX_SERVICO 8
#define IO_CHANCE 7
#define DISCO 0
#define TDISCO 2
#define IMPRESSORA 1
#define TIMPRESSORA 4
#define FITA 2
#define TFITA 3
#define TEMPO_PROX 5

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

void finalizaFila(fila *f){
    free(f->primeiro);
    free(f->ultimo);
    free(f);
}

typedef struct Process{
    int id;//id do processo
    int tempo_chegada;
    int tempo_servico;//unidades de tempo totais para concluir sevico
    int tempo_io;//unidades de tempo totais para concluir io
    int inicio_io;//tempo de inicio do io
    int tipo_io;
    int usou_cpu;//unidades de tempo utilizadas ate o momento em cpu
    int usou_io;//unidades de tempo utilizadas ate o momento em io
}processo;

processo* getProcess(int id, int tempo_chegada){
    processo *p = (processo*)malloc(sizeof(processo));

    p->id = id;
    p->tempo_chegada = tempo_chegada;
    printf("Processo de id %d foi criado\n", id);
    printf("Chegou no instante de tempo: %d\n", tempo_chegada);

    int tempo_servico = rand()%(MAX_SERVICO+1);
    while(tempo_servico == 0){
        tempo_servico = rand()%(MAX_SERVICO+1);
    }
    p->tempo_servico = tempo_servico;
    p->usou_cpu = 0;

    printf("Possui %d unidades de tempo de servico\n", tempo_servico);

    int hasIO = rand()%10;
    if(hasIO<=IO_CHANCE){
        int tipoIo = rand()%3;
        p->tipo_io = tipoIo;
        if(tipoIo == DISCO){
            printf("Possui IO do tipo DISCO com %d u.t. de duracao", TDISCO);
            p->usou_io = 0;
            p->tempo_io = TDISCO;
        }else if(tipoIo == IMPRESSORA){
            printf("Possui IO do tipo IMPRESSORA com %d u.t. de duracao", TIMPRESSORA);
            p->usou_io = 0;
            p->tempo_io = TIMPRESSORA;
        }else if(tipoIo == FITA){
            printf("Possui IO do tipo FITA com %d u.t. de duracao", TFITA);
            p->usou_io = 0;
            p->tempo_io = TFITA;
        }
        p->inicio_io = rand()%(p->tempo_servico +1);
        printf(" que e executado apos %d u.t. do inicio do processo\n", p->inicio_io);
    }else{
        printf("Nao possui IO\n");
        p->tempo_io = -1;
        p->tipo_io = -1;
        p->inicio_io = -1;
        p->usou_io = -1;
    }
    return p;
}

int getTempoIo(int tipoIo){
    switch (tipoIo)
    {
    case IMPRESSORA: return TIMPRESSORA;break;
    case FITA: return TFITA;break;
    case DISCO: return TDISCO;break;

    default:return -1;break;
    }
}

int main(){

    srand(time(NULL));

    int instante_tempo = 0;
    int processos_terminados = 0;
    int prox_processo_tempo = 0;
    int fatia_tempo_restante;
    int pid = 0;
    int processo_atual = -1;
    int io_atual = -1;
    int fatia_io_restante;

    processo *processos = (processo*)malloc(sizeof(processo)*MAX_PROCESSES);

    fila *alta_prioridade = getQueue();
    fila *baixa_prioridade = getQueue();
    fila *fila_io = getQueue();
    
    while(processos_terminados < MAX_PROCESSES){
        //printf("instante de tempo: %d\n", instante_tempo);
        if(instante_tempo == prox_processo_tempo && pid < MAX_PROCESSES){
            printf("\n\n+++++++++++++++++++++++++++++++++++++++++++\n");
            processos[pid] = *getProcess(pid, instante_tempo);
            inserir(pid, alta_prioridade);
            pid++;
            printf("\n");
            int prox = rand()%TEMPO_PROX;
            if(prox == 0){
                prox++;
            }
            prox_processo_tempo = instante_tempo + prox;
            printf("+++++++++++++++++++++++++++++++++++++++++++\n\n");
        }

        //Caso a CPU esteja "vazia"
        if(processo_atual == -1){
            //caso existam processos na fila de alta prioridade
            if(alta_prioridade->size > 0){
                processo_atual = remover(alta_prioridade);
                //caso o processo precise de IO
                if(processos[processo_atual].inicio_io == processos[processo_atual].usou_cpu){
                    processos[processo_atual].inicio_io = -1;
                    inserir(processo_atual, fila_io);
                    printf("Processo %d foi para fila de IO\n", processo_atual);
                    processo_atual = -1;
                    
                }else{
                    printf("Processo %d ganhou CPU\n", processo_atual);                   
                    int tempoRestante = processos[processo_atual].tempo_servico - processos[processo_atual].usou_cpu;
                    if(tempoRestante >= QUANTUM){
                        fatia_tempo_restante = QUANTUM;
                    }else{
                        fatia_tempo_restante = tempoRestante;
                    }
                }
            //caso existam processos na fila de baixa prioridade
            }else if(baixa_prioridade->size > 0){
                processo_atual = remover(baixa_prioridade);
                //caso o processo precise de IO
                if(processos[processo_atual].inicio_io == processos[processo_atual].usou_cpu){
                    processos[processo_atual].inicio_io = -1;
                    inserir(processo_atual, fila_io);
                    printf("Processo %d foi para fila de IO\n", processo_atual);
                    processo_atual = -1;
                    
                }else{
                    printf("Processo %d ganhou CPU\n", processo_atual);
                    int tempoRestante = processos[processo_atual].tempo_servico - processos[processo_atual].usou_cpu;
                    if(tempoRestante >= QUANTUM){
                        fatia_tempo_restante = QUANTUM;
                    }else{
                        fatia_tempo_restante = tempoRestante;
                    }
                }
            //caso nao existam processos em nenhuma das filas
            }else{
                printf("CPU Livre, sem processos em fila\n");
            }
        }

        else if(processo_atual != -1){
            fatia_tempo_restante--;
            processos[processo_atual].usou_cpu++;
            
            int tempoRestante = processos[processo_atual].tempo_servico - processos[processo_atual].usou_cpu;
            printf("Faltam %d u.t. para o processo %d terminar seu uso de CPU\n", tempoRestante, processo_atual);

            if(processos[processo_atual].inicio_io == processos[processo_atual].usou_cpu){
                inserir(processo_atual, fila_io);
                processos[processo_atual].inicio_io = -1;
                printf("Processo %d foi para fila de IO\n", processo_atual);
                processo_atual = -1;
            }

            if(fatia_tempo_restante == 0 && processo_atual != -1){
                if(processos[processo_atual].inicio_io == -1 && processos[processo_atual].usou_cpu == processos[processo_atual].tempo_servico){
                    printf("O processo %d terminou sua execucao e foi finalizado no instante de tempo %d\n", processo_atual, instante_tempo);
                    processos_terminados++;
                }else{
                    inserir(processo_atual, baixa_prioridade);
                    printf("O processo %d sofreu preempcao e foi movido para a fila de baixa prioridade\n", processo_atual);
                }
                processo_atual = -1;
            }

        }
    
        if(io_atual == -1){
            if(fila_io->size > 0){
                io_atual = remover(fila_io);
                printf("O processo %d agora esta em IO", io_atual);
                fatia_io_restante = getTempoIo(processos[io_atual].tipo_io);
                printf(" com %d de fatias de io\n", fatia_io_restante);
            }
        }

        else{
            fatia_io_restante--;
            printf("O processo %d possui %d u.t. de IO restantes\n", io_atual, fatia_io_restante);

            if(fatia_io_restante == 0) {
                int cpuRestante = processos[io_atual].tempo_servico - processos[io_atual].usou_cpu;
                if(cpuRestante != 0){
                    if(processos[io_atual].tipo_io == DISCO){
                        inserir(io_atual, baixa_prioridade);
                        printf("O processo %d foi para a fila de baixa prioridade\n", io_atual);
                    }else{
                        inserir(io_atual, alta_prioridade);
                        printf("O processo %d foi para a fila de alta prioridade\n", io_atual);
                    }
                }else{
                    processos_terminados ++;
                    printf("O processo %d foi finalizado apos concluir IO no instante de tempo %d\n", io_atual, instante_tempo);
                }
                io_atual = -1;
            }
        }
        instante_tempo++;
    }

    free(processos);
    finalizaFila(alta_prioridade);
    finalizaFila(baixa_prioridade);
    finalizaFila(fila_io);
    return 0;
}