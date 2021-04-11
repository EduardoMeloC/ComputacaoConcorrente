/* Prof.: Silvana Rosseto */
/* Módulo 1 - Laboratório: 3 */
/* Aluno.: Eduardo Melo */
/* Código: Aproximação de pi concorrente com a Fórmula de Leibniz */

/*
 * Comentários:
 *      > Com a finalidade de diminuir o erro, eu optei por dividir as iterações em blocos,
 *      para eveitar o deslocamento da mantissa. 
 *      > Eu me surpreendi porque, às vezes, mesmo com muitas iterações, o sequencial às vezes era mais rápido.
 *      > Foi engraçado perceber que o próprio M_PI é bem impreciso. Decobri que existe o M_PIl, mas não quis usar.
 *      > Eu escrevi um safe.c que chama as funçoes que printam no stderr direto com o if de erro.
 *      > Essa fórmula converge muito lentamente. Eu fiquei me perguntando se existe algum jeito melhor de chegar ao valor de pi.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "safe.c"
#include "timer.h"

// global variables
long long int g_niterations;
int g_nthreads;

// sequential version
double get_pi(){
    // implementation of Leibniz Formula for pi
    double seq = 0.;
    for (long long int i = 0; i < g_niterations; i++){
        seq += 1.0/(i*2 + 1) * (i % 2 == 0 ? 1 : -1);
    }
    return 4 * seq;
}

// thread's flow
void* pi_thread(void* arg){
    // each thread processes a block of Leibniz Formula's sequence
    long int id = (long int) arg;
    double* sum = (double*) safe_malloc(sizeof(double)); // local block sum
    long long int block_len = g_niterations / g_nthreads;
    long long int start = id * block_len;
    long long int end = (id == g_nthreads-1) ? g_niterations : start + block_len;

    for (long long int i = start; i < end; i++){
        *sum += 1.0/(i*2 + 1) * (i % 2 == 0 ? 1 : -1);
    }

    pthread_exit((void *) sum);
}

// multithreaded version
double multithreaded_get_pi(){
    // declare variables
    pthread_t *tid;
    double* retval;
    double seq = 0.;

    // allocate memory
    tid = (pthread_t *) safe_malloc(sizeof(pthread_t) * g_nthreads);

    // create threads
    for(long int i=0; i < g_nthreads; i++){
        safe_pthread_create(tid+i, NULL, pi_thread, (void*) i);
    }
    // wait for threads to end
    for(long int i=0; i < g_nthreads; i++){
        safe_pthread_join(*(tid+i), (void**) &retval);
        seq += *retval;
        free(retval);
    }

    // free memory
    free(tid);
    return 4 * seq;
}

int main(int argc, char* argv[]){
    // declare variables
    double timer_start, timer_end;

    // validate argv inputs
    if(argc < 3){
        printf("Usage: %s <niterations> <nthreads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    g_niterations = atoll(argv[1]);
    g_nthreads = atoi(argv[2]);
    
    // sequential version
    printf("\nSEQUENTIAL VERSION\n\n");

    GET_TIME(timer_start);
    double sequential_pi = get_pi();
    GET_TIME(timer_end);
    double sequential_deltatime = timer_end - timer_start; 
    double sequential_err = M_PI - sequential_pi;

    printf("M_PI:\t %.22lf\n", M_PI);
    printf("get_pi:\t %.22lf\n", sequential_pi);
    printf("err:\t %.22lf\n", sequential_err);
    printf("time:\t %.4lfs\n", sequential_deltatime);

    printf("\n-----------------\n");

    // concurrent version
    printf("\nMULTITHREADED VERSION\n\n");

    GET_TIME(timer_start);
    double threaded_pi = multithreaded_get_pi();
    GET_TIME(timer_end);
    double threaded_deltatime = timer_end - timer_start; 
    double threaded_err = M_PI - threaded_pi;

    printf("M_PI:\t %.22lf\n", M_PI);
    printf("get_pi:\t %.22lf\n", threaded_pi);
    printf("err:\t %.22lf\n", threaded_err);
    printf("time:\t %.4lfs\n", threaded_deltatime);

    // comparing results
    printf("\n-----------------\n");
    printf("\nCOMPARISON AND ANALYSIS\n\n");
    char* str_fastest = (threaded_deltatime < sequential_deltatime) ? "concurrent" : "sequential";
    char* str_smallesterr = (threaded_err < sequential_err) ? "concurrent" : "sequential";
    double fastest_time = (threaded_deltatime < sequential_deltatime) ? threaded_deltatime : sequential_deltatime;
    double slowest_time = (threaded_deltatime < sequential_deltatime) ? sequential_deltatime : threaded_deltatime;
    double smallest_err = (threaded_err < sequential_err) ? threaded_err : sequential_err;
    double biggest_err = (threaded_err < sequential_err) ? sequential_err : threaded_err; 

    printf("Fastest version:\t %s (%.2fx faster)\n", str_fastest, slowest_time / fastest_time);
    printf("Smallest error version:\t %s (%.10e error difference)\n", str_smallesterr, biggest_err - smallest_err);


    exit(EXIT_SUCCESS);
}
