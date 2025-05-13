#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ALUNOS 200000

typedef struct {
    int ano;
    char nome[12];
    char disciplina[20];
    float nota;
} Aluno;

const char* disciplinas[] = {"Matemática", "Português", "Geografia"};
const int num_disciplinas = 3;
const char* nomes_base[] = {"João", "Maria", "Carlos", "Julia", "Pedro", "Gabriela"};
const int num_nomes_base = 6;

// Geração de entrada aleatória
void gerarEntradaTXT(int quantidade) {
    FILE *arquivo = fopen("entrada.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo entrada.txt\n");
        return;
    }

    for (int i = 0; i < quantidade; i++) {
        int ano = 2010 + rand() % (2023 - 2010 + 1);
        char nome[50];
        const char* base = nomes_base[rand() % num_nomes_base];
        int sufixo = rand() % 1000;
        sprintf(nome, "%s%d", base, sufixo);

        const char* disciplina = disciplinas[rand() % num_disciplinas];
        float nota = (float)(rand() % 101) / 10.0f;

        fprintf(arquivo, "%d,%s,%s,%.1f\n", ano, nome, disciplina, nota);
    }

    fclose(arquivo);
    printf("Arquivo entrada.txt gerado com sucesso com %d linhas.\n", quantidade);
}

// Leitura do arquivo de entrada
int lerTXT(const char *nomeArquivo, Aluno alunos[]) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return 0;
    }

    char linha[256];
    int totalAlunos = 0;

    while (fgets(linha, sizeof(linha), file) && totalAlunos < MAX_ALUNOS) {
        sscanf(linha, "%d,%99[^,],%49[^,],%f",
               &alunos[totalAlunos].ano,
               alunos[totalAlunos].nome,
               alunos[totalAlunos].disciplina,
               &alunos[totalAlunos].nota);
        totalAlunos++;
    }

    fclose(file);
    return totalAlunos;
}

// Ordenação por Bubble Sort
void bubbleSortPorNota(Aluno array[], int total, int* comparacoes, double* tempo_execucao) {
    clock_t start_time = clock();

    *comparacoes = 0;
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - i - 1; j++) {
            (*comparacoes)++;
            if (array[j].nota < array[j + 1].nota) {
                Aluno temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }

    clock_t end_time = clock();
    *tempo_execucao = (double)(end_time - start_time) / CLOCKS_PER_SEC;
}

// Merge com alocação dinâmica (heap)
void merge(Aluno array[], int left, int mid, int right, int* comparacoes) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Aluno *leftArray = malloc(n1 * sizeof(Aluno));
    Aluno *rightArray = malloc(n2 * sizeof(Aluno));

    if (!leftArray || !rightArray) {
        printf("Erro de alocação de memória no merge.\n");
        exit(1);
    }

    for (int i = 0; i < n1; i++)
        leftArray[i] = array[left + i];
    for (int j = 0; j < n2; j++)
        rightArray[j] = array[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        (*comparacoes)++;
        if (leftArray[i].nota >= rightArray[j].nota) {
            array[k++] = leftArray[i++];
        } else {
            array[k++] = rightArray[j++];
        }
    }

    while (i < n1) {
        array[k++] = leftArray[i++];
    }
    while (j < n2) {
        array[k++] = rightArray[j++];
    }

    free(leftArray);
    free(rightArray);
}

// Merge Sort principal
void mergeSortPorNota(Aluno array[], int left, int right, int* comparacoes) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortPorNota(array, left, mid, comparacoes);
        mergeSortPorNota(array, mid + 1, right, comparacoes);
        merge(array, left, mid, right, comparacoes);
    }
}

// Função auxiliar com tempo para Merge Sort
void ordenarMergeSort(Aluno array[], int total, int* comparacoes, double* tempo_execucao) {
    clock_t start = clock();
    *comparacoes = 0;
    mergeSortPorNota(array, 0, total - 1, comparacoes);
    clock_t end = clock();
    *tempo_execucao = (double)(end - start) / CLOCKS_PER_SEC;
}

// Salvar o resultado ordenado
void salvarOrdenado(const char *nomeArquivo, Aluno alunos[], int total) {
    FILE *file = fopen(nomeArquivo, "w");
    if (!file) {
        printf("Erro ao salvar o arquivo %s\n", nomeArquivo);
        return;
    }

    for (int i = 0; i < total; i++) {
        fprintf(file, "%d,%s,%s,%.1f\n",
                alunos[i].ano,
                alunos[i].nome,
                alunos[i].disciplina,
                alunos[i].nota);
    }

    fclose(file);
    printf("Arquivo %s gerado com sucesso com os dados ordenados por nota.\n", nomeArquivo);
}

// Programa principal
int main() {
    srand(time(NULL));
    Aluno alunos[MAX_ALUNOS];
    int totalAlunos;
    int comparacoes = 0;
    double tempo_execucao = 0.0;
    int opcao;

    printf("\n==== MENU ====\n");
    printf("1 - Gerar arquivo entrada.txt\n");
    printf("2 - Ordenar por nota (Bubble Sort)\n");
    printf("3 - Ordenar por nota (Merge Sort)\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
        case 1: {
            int qtd;
            printf("Digite a quantidade de alunos a serem gerados: ");
            scanf("%d", &qtd);
            gerarEntradaTXT(qtd);
            break;
        }
        case 2:
            totalAlunos = lerTXT("entrada.txt", alunos);
            printf("Total de alunos lidos: %d\n", totalAlunos);
            if (totalAlunos > 0) {
                bubbleSortPorNota(alunos, totalAlunos, &comparacoes, &tempo_execucao);
                salvarOrdenado("saida_bubble_sort.txt", alunos, totalAlunos);
                printf("\nAlgoritmo: Bubble Sort\nTamanho Entrada: %d\nTempo execução: %.2f segundos\nComparações (passos): %d\n", totalAlunos, tempo_execucao, comparacoes);
            }
            break;
        case 3:
            totalAlunos = lerTXT("entrada.txt", alunos);
            printf("Total de alunos lidos: %d\n", totalAlunos);
            if (totalAlunos > 0) {
                ordenarMergeSort(alunos, totalAlunos, &comparacoes, &tempo_execucao);
                salvarOrdenado("saida_merge_sort.txt", alunos, totalAlunos);
                printf("\nAlgoritmo: Merge Sort\nTamanho Entrada: %d\nTempo execução: %.2f segundos\nComparações (passos): %d\n", totalAlunos, tempo_execucao, comparacoes);
            }
            break;
        default:
            printf("Opção inválida!\n");
    }

    return 0;
}
