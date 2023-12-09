#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

const int SIZE = 25000000;

void shellSortSerial(int arr[])
{
    for (int interval = SIZE / 2; interval > 0; interval /= 2)
    {
        for (int i = interval; i < SIZE; i += 1)
        {
            int temp = arr[i];
            int j;
            for (j = i; j >= interval && arr[j - interval] > temp; j -= interval)
            {
                arr[j] = arr[j - interval];
            }
            arr[j] = temp;
        }
    }
}

void shellSortParallel(int arr[], int num_threads)
{
    for (int interval = SIZE / 2; interval > 0; interval /= 2)
    {
#pragma omp parallel for num_threads(num_threads)
        for (int i = interval; i < SIZE; i += 1)
        {
            int temp = arr[i];
            int j;
            // 2.3 - B) Potencial dependência de dados neste loop interno
            for (j = i; j >= interval && arr[j - interval] > temp; j -= interval)
            {
                arr[j] = arr[j - interval];
            }
            arr[j] = temp;
        }
    }
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
    shellSortSerial(arraySerial);
    double endSerial = omp_get_wtime();
    double serialTime = endSerial - startSerial;

    // Medir tempo de execução para versão paralela
    double startParallel2 = omp_get_wtime();
    shellSortParallel(arrayParallel2, 2);
    double endParallel2 = omp_get_wtime();
    double parallelTime2 = endParallel2 - startParallel2;

    // Medir tempo de execução para versão paralela
    double startParallel4 = omp_get_wtime();
    shellSortParallel(arrayParallel4, 4);
    double endParallel4 = omp_get_wtime();
    double parallelTime4 = endParallel4 - startParallel4;

    // Calcular Speedup e Eficiência para 2 threads
    double speedup2 = serialTime / parallelTime2;
    double efficiency2 = speedup2 / 2;

    // Calcular Speedup e Eficiência para 4 threads
    double speedup4 = serialTime / parallelTime4;
    double efficiency4 = speedup4 / 4;

    // Imprimir resultados
    printf("Tempo (Serial): %.6f segundos\n", serialTime);
    printf("Tempo (Paralelo 2 threads): %.6f segundos\n", parallelTime2);
    printf("Speedup (2 threads): %.2f\n", speedup2);
    printf("Eficiência (2 threads): %.2f\n", efficiency2);
    printf("Tempo (Paralelo 4 threads): %.6f segundos\n", parallelTime4);
    printf("Speedup (4 threads): %.2f\n", speedup4);
    printf("Eficiência (4 threads): %.2f\n", efficiency4);

    free(arraySerial);
    free(arrayParallel2);
    free(arrayParallel4);

    return 0;
}
