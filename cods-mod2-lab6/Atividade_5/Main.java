/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 2 - Laboratório: 6 */
/* Codigo: Incremento em um vetor em Java */

/*
 * Comentarios: 
 *      > Em Java, ao instanciar um int[], ele já vem com os valores zerados.
 *        Então eu escolhi não alterar esse valor, durante a inicialização.
*/

import java.util.Arrays;

class T extends Thread {
    private int id;
    private int nthreads;
    // Vetor compartilhado entre as threads
    int[] arr;

    // Construtor
    public T(int id, int nthreads, int[] arr){
        this.id = id;
        this.nthreads = nthreads;
        this.arr = arr;
    }

    // Fluxo principal da thread
    public void run() {
        for (int i = id; i < arr.length; i += nthreads){
            this.arr[i]++;
        }
    }
}

class Main {
    // Numero de threads
    static final int nthreads = 2;
    // Tamanho do vetor
    static final int arrLength = 32;

    public static void main(String[] args){
        Thread[] threads = new Thread[nthreads];
        int[] arr = new int[arrLength];

        // Imprime o vetor
        System.out.println(Arrays.toString(arr));

        // Cria as threads da aplicaçao
        for (int i=0; i < threads.length; i++){
            threads[i] = new T(i, nthreads, arr);
        }

        // Inicia as threads
        for (int i=0; i < threads.length; i++){
            threads[i].start();
        }

        // Espera o termino de todas as threads
        for (int i=0; i < threads.length; i++){
            try{
                threads[i].join();
            }
            catch(InterruptedException e){
                return;
            }
        }

        // Imprime o vetor
        System.out.println(Arrays.toString(arr));
    }
}

