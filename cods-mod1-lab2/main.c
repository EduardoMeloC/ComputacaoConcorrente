/* Prof.: Silvana Rosseto */
/* Módulo 1 - Laboratóio: 2 */
/* Aluno.: Eduardo Melo */
/* Código: Multiplicacao Matriz x Matriz (considerando matriz quadradada)

/* 
 * Comentários:
 *      Eu escolhi alternar as threads para cada linha. 
 *      Mudei pouco o código base. Acabei me perdendo mais com índice de matriz, mesmo.
 *      Multiplicacao de matriz é uma coisa que me embola até no papel
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *mat1; //matriz1 de entrada
float *mat2; //matriz2 de entrada
float *saida; //vetor de saida
int nthreads; //numero de threads

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int dim; //dimensao das estruturas de entrada
} tArgs;

//funcao que as threads executarao
void* tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;
   int id = args->id;
   int dim = args->dim;
    
   for(int i = id; i < dim; i += nthreads){
       for(int j = 0; j < dim; j ++){
          /* printf("thread_%d preenche (i, j) = (%d, %d)\n", id, i, j); */
          for(int k=0; k < dim; k++){
             saida[i*dim + j] += mat1[i*dim + k] * mat2[k*dim + j];;
          }
       }
   }

   pthread_exit(NULL);
}

void print_matrix(float* mat, int dim){
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            printf("%.1f, ", mat[i*dim + j]);
        }
        puts("");
    }
}

int main(int argc, char* argv[]) {
   int dim; //dimensao da matriz de entrada
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <dimensao das matrizes> <numero de threads>\n", argv[0]);
      return 1;
   }
   dim = atoi(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > dim) nthreads=dim;

   //alocacao de memoria para as estruturas de dados
   mat1 = (float *) malloc(sizeof(float) * dim * dim);
   if (mat1 == NULL) {printf("ERRO--malloc\n"); return 2;}
   mat2 = (float *) malloc(sizeof(float) * dim * dim);
   if (mat2 == NULL) {printf("ERRO--malloc\n"); return 2;}
   saida = (float *) malloc(sizeof(float) * dim * dim);
   if (saida == NULL) {printf("ERRO--malloc\n"); return 2;}

   //inicializacao das estruturas de dados de entrada e saida
   for(int i=0; i<dim; i++) {
      for(int j=0; j<dim; j++){
          mat1[i*dim+j] = 1;    //equivalente mat[i][j]
          mat2[i*dim+j] = 1; 
          saida[i*dim+j] = 0;
      }
   }
   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo inicializacao:%lf\n", delta);

   //multiplicacao da matriz pelo vetor
   GET_TIME(inicio);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = dim;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao:%lf\n", delta);

    //exibicao dos resultados
    /* puts("Matriz de saida:"); */
    /* print_matrix(saida, dim); */
    /* puts(""); */

   //liberacao da memoria
   GET_TIME(inicio);
   free(mat1);
   free(mat2);
   free(saida);
   free(args);
   free(tid);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);

   return 0;
}

