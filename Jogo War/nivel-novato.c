#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura que representa cada território
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Funções (prototipos)
Territorio* cadastrarTerritorios(int n);
void exibirMapa(Territorio* mapa, int n);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMemoria(Territorio* mapa);
int lerInteiro(const char* msg, int min, int max);

// Programa principal
int main() {
    srand((unsigned) time(NULL)); // inicializa gerador de números aleatórios

    printf("=== WAR Estruturado - Simulação de Ataques entre Territ\xf3rios ===\n\n");

    int n = lerInteiro("Quantos territ\xf3rios deseja cadastrar? (min 2): ", 2, 1000);

    // Aloca dinamicamente o vetor de territórios
    Territorio* mapa = cadastrarTerritorios(n);
    if (!mapa) {
        fprintf(stderr, "Erro: falha na aloca\xe7\xe3o de mem\xf3ria.\n");
        return 1;
    }

    int opcao = 0;
    while (1) {
        printf("\n--- Menu ---\n");
        printf("1 - Exibir mapa\n");
        printf("2 - Fazer ataque\n");
        printf("3 - Sair\n");
        opcao = lerInteiro("Escolha uma opcao: ", 1, 3);

        if (opcao == 1) {
            exibirMapa(mapa, n);
        } else if (opcao == 2) {
            exibirMapa(mapa, n);
            int idAtacante = lerInteiro("Indice do territorio atacante: ", 0, n-1);
            int idDefensor = lerInteiro("Indice do territorio defensor: ", 0, n-1);

            // Validacoes
            if (idAtacante == idDefensor) {
                printf("Erro: o atacante e o defensor devem ser territorios diferentes.\n");
                continue;
            }

            Territorio* atacante = &mapa[idAtacante];
            Territorio* defensor = &mapa[idDefensor];

            if (strcmp(atacante->cor, defensor->cor) == 0) {
                printf("Erro: nao e permitido atacar territorio da mesma cor (mesmo jogador).\n");
                continue;
            }

            if (atacante->tropas < 2) {
                printf("Erro: o territorio atacante precisa ter pelo menos 2 tropas para iniciar um ataque.\n");
                continue;
            }

            // Realiza o ataque usando ponteiros
            atacar(atacante, defensor);

            // Exibe resultado atualizado
            printf("\nMapa atualizado ap\xf3s o ataque:\n");
            exibirMapa(mapa, n);

        } else if (opcao == 3) {
            printf("Saindo... liberando memoria.\n");
            break;
        }
    }

    liberarMemoria(mapa);
    return 0;
}

// Lê um inteiro do usuário entre min e max (inclusive). Repetir até entrada válida.
int lerInteiro(const char* msg, int min, int max) {
    int val;
    char buf[64];
    while (1) {
        printf("%s", msg);
        if (!fgets(buf, sizeof(buf), stdin)) {
            clearerr(stdin);
            continue;
        }
        if (sscanf(buf, "%d", &val) != 1) {
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }
        if (val < min || val > max) {
            printf("Valor fora do intervalo [%d - %d]. Tente novamente.\n", min, max);
            continue;
        }
        return val;
    }
}

// Cadastra n territorios dinamicamente (usando calloc) e retorna o ponteiro para o vetor
Territorio* cadastrarTerritorios(int n) {
    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    if (!mapa) return NULL;

    char buffer[128];
    for (int i = 0; i < n; ++i) {
        printf("\n--- Cadastro do territorio %d ---\n", i);
        printf("Nome: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            // Caso fgets falhe, preencher com nome padrao
            strncpy(mapa[i].nome, "Territorio", sizeof(mapa[i].nome)-1);
            mapa[i].nome[sizeof(mapa[i].nome)-1] = '\0';
        } else {
            // remove newline final
            buffer[strcspn(buffer, "\n")] = '\0';
            strncpy(mapa[i].nome, buffer, sizeof(mapa[i].nome)-1);
            mapa[i].nome[sizeof(mapa[i].nome)-1] = '\0';
        }

        printf("Cor (ex: vermelho, azul, verde): ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            strncpy(mapa[i].cor, "semcor", sizeof(mapa[i].cor)-1);
            mapa[i].cor[sizeof(mapa[i].cor)-1] = '\0';
        } else {
            buffer[strcspn(buffer, "\n")] = '\0';
            strncpy(mapa[i].cor, buffer, sizeof(mapa[i].cor)-1);
            mapa[i].cor[sizeof(mapa[i].cor)-1] = '\0';
        }

        mapa[i].tropas = lerInteiro("Quantidade de tropas (>0): ", 1, 1000000);
    }

    return mapa;
}

// Exibe o mapa atual com indices para selecao
void exibirMapa(Territorio* mapa, int n) {
    printf("\n--- Mapa de Territorios ---\n");
    printf("Indice | Nome                         | Cor       | Tropas\n");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < n; ++i) {
        printf("%5d  | %-27s | %-8s | %6d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Função que simula um ataque entre dois territories.
// Regras implementadas:
// - Cada lado rola um dado (1..6)
// - Se atacante vencer (dado maior): defensor muda de cor para a cor do atacante
//   e recebe metade das tropas do atacante (arredondado para baixo). O atacante perde
//   essa quantidade transferida.
// - Se atacante perder ou empatar: atacante perde uma tropa (se tiver >0)
void atacar(Territorio* atacante, Territorio* defensor) {
    if (!atacante || !defensor) return;

    printf("\nIniciando ataque: %s (%s, %d tropas) -> %s (%s, %d tropas)\n",
           atacante->nome, atacante->cor, atacante->tropas,
           defensor->nome, defensor->cor, defensor->tropas);

    int dadoA = (rand() % 6) + 1; // 1 a 6
    int dadoD = (rand() % 6) + 1; // 1 a 6

    printf("Dado do atacante: %d\n", dadoA);
    printf("Dado do defensor : %d\n", dadoD);

    if (dadoA > dadoD) {
        // atacante vence
        int transferencia = atacante->tropas / 2; // metade das tropas do atacante
        if (transferencia < 1) transferencia = 1; // garante transferencia minima

        printf("Atacante vence! Transferindo %d tropas e mudando o dono.\n", transferencia);

        // Atualiza campos DO DEFENSOR via ponteiro
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor)-1);
        defensor->cor[sizeof(defensor->cor)-1] = '\0';

        defensor->tropas = transferencia; // defensor recebe as tropas transferidas

        // Atualiza atacante (perde as tropas transferidas)
        atacante->tropas -= transferencia;
        if (atacante->tropas < 0) atacante->tropas = 0;

    } else {
        // atacante perde (inclui empate)
        printf("Atacante perde o confronto (ou empate). Perdendo 1 tropa.\n");
        if (atacante->tropas > 0) atacante->tropas -= 1;
    }
}

// Libera a memoria alocada para o mapa (por conveniencia usa free sobre o ponteiro)
void liberarMemoria(Territorio* mapa) {
    if (!mapa) return;
    free(mapa);
}
