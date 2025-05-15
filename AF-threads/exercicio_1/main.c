#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// main thread (após término das threads filhas): "Contador: %d\n"
// main thread (após término das threads filhas): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global (operador ++) n_loops vezes
// - pai deve esperar pelas worker threads  antes de imprimir!


int contador_global = 0;

void* loop(void *arg) {
    int n_loops = * ((int*)arg);
    for (int i = 0; i < n_loops; i++) 
        ++contador_global;
    pthread_exit(NULL);    
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);
    
    pthread_t thread[n_threads];
    for (int i = 0; i < n_threads; i++)
        pthread_create(&thread[i], NULL, loop, &n_loops);
    
    for (int i = 0; i < n_threads; i++)
        pthread_join(thread[i],NULL);

    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", n_threads*n_loops);
    return 0;
}

/*
1- O aumento de número de threads também expressa maior probabilidade de múltiplas threads tentem acessar o mesmo contador,
 que pode ocasionar em condições de corrida. Podendo causar uma diferença entre o "valor contado" e o "valor esperado", 
 especialmente se não houver mecanismos de sincronização adequados como mutexes. 

2- O número de loops  se refere à quantidade de vezes que cada thread vai executar uma operação, 
se aumentarmos os loops maior será as condições de corridas caso as threads estiverem concorrendo pelo mesmo recurso. 
Dessa forma, amplificar-se a diferença entre o "valor contado" e o "esperado",
pois pequenas falhas de sincronização podem se somar a cada iteração.

3- A diferença entre "contador" e "esperado" geralmente segue um padrão caótico, 
pois depende da ordem em que as threads acessam o recurso compartilhado, em situações sem sincronização, 
a diferença tende a aumentar conforme o número de threads e loops cresce, 
mas não segue um padrão fixo devido à natureza não determinística desse tipo de concorrência. 
Caso executemos o programa sem mecanismos de sincronização adequados, 
o algoritmo tornando-se refém do hardware e do sistema operacional, 
já que diferentes mecanismos de escalonamento de threads podem afetar o tempo de execução.
*/