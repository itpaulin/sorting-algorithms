#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Variáveis globais
const int SIZE = 250000;    // Tamanho dos arrays
const int NUM_BUCKETS = 10; // Número de baldes

// Função de comparação para o qsort
int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void bucketSort(int arr[])
{
    const int num_buckets = SIZE / NUM_BUCKETS + 1;

    int **buckets = (int **)malloc(num_buckets * sizeof(int *));
    int *bucketSizes = (int *)calloc(num_buckets, sizeof(int));

    for (int i = 0; i < num_buckets; ++i)
    {
        buckets[i] = (int *)malloc(SIZE * sizeof(int));
    }

    // Distribua os elementos nos baldes
    for (int i = 0; i < SIZE; ++i)
    {
        int bucket_index = arr[i] / NUM_BUCKETS;
        buckets[bucket_index][bucketSizes[bucket_index]++] = arr[i];
    }

    // Ordene cada balde usando o quicksort
    for (int i = 0; i < num_buckets; ++i)
    {
        qsort(buckets[i], bucketSizes[i], sizeof(int), compare);
    }

    // Concatene os baldes ordenados para obter a lista final
    int index = 0;
    for (int i = 0; i < num_buckets; ++i)
    {
        for (int j = 0; j < bucketSizes[i]; ++j)
        {
            arr[index++] = buckets[i][j];
        }
    }

    // Liberar memória alocada dinamicamente
    for (int i = 0; i < num_buckets; ++i)
    {
        free(buckets[i]);
    }
    free(buckets);
    free(bucketSizes);
}

void parallelBucketSort(int arr[], int num_threads)
{
    const int num_buckets = SIZE / NUM_BUCKETS + 1;

    int **buckets = (int **)malloc(num_buckets * sizeof(int *));
    int *bucketSizes = (int *)calloc(num_buckets, sizeof(int));

    for (int i = 0; i < num_buckets; ++i)
    {
        buckets[i] = (int *)malloc(SIZE * sizeof(int));
    }

    // Distribua os elementos nos baldes
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < SIZE; ++i)
    {
        int bucket_index = (arr[i] * NUM_BUCKETS) / 1000;
        int index;
// 2.3 - A) REGIAO CRITICA
#pragma omp critical
        {
            index = bucketSizes[bucket_index]++;
        }
        // 2.3 - B) Potencial dependência de dados
        buckets[bucket_index][index] = arr[i];
    }

    // Ordene cada balde usando o quicksort (paralelizado)
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < num_buckets; ++i)
    {
        qsort(buckets[i], bucketSizes[i], sizeof(int), compare);
    }

    // Concatene os baldes ordenados para obter a lista final
    int index = 0;
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < num_buckets; ++i)
    {
        for (int j = 0; j < bucketSizes[i]; ++j)
        {
            arr[index++] = buckets[i][j];
        }
    }

    // Liberar memória alocada dinamicamente
    for (int i = 0; i < num_buckets; ++i)
    {
        free(buckets[i]);
    }
    free(buckets);
    free(bucketSizes);
}

// Exibir array, usado para conferir se estava funcionando.
void printArray(int array[], int apartir)
{
    for (int i = apartir; i < (apartir + 10); ++i)
    {
        printf("%d  ", array[i]);
    }
    printf("\n");
}

int main()
{
    int *arraySerial = (int *)malloc(SIZE * sizeof(int));
    int *arrayParallel2 = (int *)malloc(SIZE * sizeof(int));
    int *arrayParallel4 = (int *)malloc(SIZE * sizeof(int));

    for (int i = 0; i < SIZE; ++i)
    {
        arraySerial[i] = rand() % SIZE;
        arrayParallel2[i] = arraySerial[i];
        arrayParallel4[i] = arraySerial[i];
    }

    // Medir tempo de execução para versão serial
    double startSerial = omp_get_wtime();
    bucketSort(arraySerial);
    double endSerial = omp_get_wtime();
    double serialTime = endSerial - startSerial;

    // Medir tempo de execução para versão paralela com 2 threads
    double startParallel2 = omp_get_wtime();
    parallelBucketSort(arrayParallel2, 2);
    double endParallel2 = omp_get_wtime();
    double parallelTime2 = endParallel2 - startParallel2;

    // Medir tempo de execução para versão paralela com 4 threads
    double startParallel4 = omp_get_wtime();
    parallelBucketSort(arrayParallel4, 4);
    double endParallel4 = omp_get_wtime();
    double parallelTime4 = endParallel4 - startParallel4;

    // Calcular Speedup e Eficiência
    double speedup2 = serialTime / parallelTime2;
    double efficiency2 = speedup2 / 2;

    double speedup4 = serialTime / parallelTime4;
    double efficiency4 = speedup4 / 4;

    printf("Tempo (Serial): %.6f segundos\n", serialTime);
    printf("Tempo (Paralelo 2 threads): %.6f segundos\n", parallelTime2);
    printf("Tempo (Paralelo 4 threads): %.6f segundos\n", parallelTime4);

    printf("Speedup (2 threads): %.2f\n", speedup2);
    printf("Eficiência (2 threads): %.2f\n", efficiency2);

    printf("Speedup (4 threads): %.2f\n", speedup4);
    printf("Eficiência (4 threads): %.2f\n", efficiency4);

    free(arraySerial);
    free(arrayParallel2);
    free(arrayParallel4);

    return 0;
}
