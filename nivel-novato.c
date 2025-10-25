#include <stdio.h>
#include <string.h>

// Definicao da struct Territorio
struct Territorio {
    char nome[30];   // Nome do territorio
    char cor[10];    // Cor do exercito
    int tropas;      // Quantidade de tropas
};

int main() {
    struct Territorio territorios[5]; // Vetor para armazenar 5 territorios

    // Entrada de dados
    for(int i = 0; i < 5; i++) {
        printf("Cadastro do territorio %d\n", i + 1);

        printf("Nome: ");
        scanf("%s", territorios[i].nome); // Lendo nome do territorio

        printf("Cor do exercito: ");
        scanf("%s", territorios[i].cor); // Lendo cor do exercito

        printf("Quantidade de tropas: ");
        scanf("%d", &territorios[i].tropas); // Lendo quantidade de tropas

        printf("\n");
    }

    // Exibicao dos dados
    printf("=== Dados dos territorios cadastrados ===\n");
    for(int i = 0; i < 5; i++) {
        printf("Territorio %d:\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor do exercito: %s\n", territorios[i].cor);
        printf("Quantidade de tropas: %d\n", territorios[i].tropas);
        printf("\n");
    }

    return 0;
}

