#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
double* load_vector(const char* filename, int* out_size);

typedef struct {
    double *a, *b;
    int inicio, fim;
} meu_novo_tipo;

void* paralelizar(void* arg) {
    meu_novo_tipo tipo = *(meu_novo_tipo*) arg;
    double *soma = malloc(sizeof(double));
    *soma = 0;

    for (int i = tipo.inicio; i < tipo.fim; i++) {
        *soma += (tipo.a[i] * tipo.b[i]);
    }
    pthread_exit((void *) soma);
}

// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Temos argumentos suficientes?
    if (argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n", 
               argv[0]);
        return 1;
    }

    // Quantas threads?
    int n_threads = atoi(argv[1]);
    if (n_threads <= 0) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }

    // Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }

    // Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b têm tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }

    // Criação das threads
    
    pthread_t thread[n_threads];
    meu_novo_tipo tipo[n_threads];

    int amostra = a_size / n_threads;
    if (n_threads <= a_size) {
        for (int j=0; j < n_threads;j++){
            tipo[j].a=a;
            tipo[j].b=b;
            tipo[j].inicio=j*amostra;

            if (j == n_threads-1) 
                tipo[j].fim=a_size;
            else 
                 tipo[j].fim=(j+1)*amostra;
            pthread_create(&thread[j], NULL, paralelizar, &tipo[j]);

            }
        } else {
            n_threads = a_size;
                for (int j=0; j < n_threads;j++){
                    tipo[j].a=a;
                    tipo[j].b=b;
                    tipo[j].inicio=j*amostra;

                    if (j == n_threads-1) 
                        tipo[j].fim=a_size; 
                    else 
                        tipo[j].fim=(j+1)*amostra;
            pthread_create(&thread[j], NULL, paralelizar, &tipo[j]);
        }
    }

    double resultado_total = 0;
    for (int i = 0; i < n_threads; i++) {
        double* parcial;
        pthread_join(thread[i], (void**)&parcial);
        resultado_total += *parcial;
        free(parcial);
    }

    // Avalia o resultado
    avaliar(a, b, a_size, resultado_total);

    // Libera memória
    free(a);
    free(b);

    return 0;
}
