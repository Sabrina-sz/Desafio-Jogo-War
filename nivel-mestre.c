/*
 * Implementação do Sistema de Missões Estratégicas para Jogo Estilo WAR.
 * * Este código demonstra a modularização, alocação dinâmica de memória,
 * uso de ponteiros e gerenciamento de missões de jogadores em C.
 */

#include <stdio.h>
#include <stdlib.h> // Para malloc, calloc, free, rand, srand
#include <string.h> // Para strcpy, strcmp
#include <time.h>   // Para time (usado no srand)

// --- Constantes Globais ---
#define MAX_NOME_TERRITORIO 30
#define MAX_COR 10
#define MAX_TAMANHO_MISSAO 256
#define TOTAL_MISSOES_PREDEFINIDAS 5

// --- Estruturas ---

/**
 * @brief Define a estrutura de um território no mapa.
 */
typedef struct Territorio {
    char nome[MAX_NOME_TERRITORIO];
    char cor[MAX_COR]; // Cor do jogador que domina
    int tropas;
} Territorio;


// --- Vetor Global de Missões (Conforme Requisito) ---

/**
 * @brief Armazena as descrições pré-definidas das missões.
 * Usado pela função atribuirMissao.
 */
const char* VETOR_MISSOES[TOTAL_MISSOES_PREDEFINIDAS] = {
    "Eliminar todas as tropas da cor Vermelha",
    "Conquistar o territorio 'Brasil'",
    "Possuir 3 territorios",
    "Possuir 10 tropas no territorio 'Canada'",
    "Eliminar todas as tropas da cor Azul"
};


// --- Protótipos das Funções (Modularização) ---

// Funções de Missão
void atribuirMissao(char* destino, const char* missoes[], int totalMissoes);
int verificarMissao(const char* missao, Territorio* mapa, int tamanhoMapa, const char* corJogador);
void exibirMissao(const char* missao); // Passagem por valor (do ponteiro)

// Funções do Jogo
Territorio* inicializarMapa(int* tamanhoMapa);
void exibirMapa(Territorio* mapa, int tamanhoMapa);
void atacar(Territorio* atacante, Territorio* defensor);

// Funções de Gerenciamento
void liberarMemoria(Territorio* mapa, char* missaoP1, char* missaoP2);


// --- Função Principal (main) ---

int main() {
    // 1. Inicializa o gerador de números aleatórios
    srand(time(NULL));

    int tamanhoMapa = 0;
    const char* JOGADOR1_COR = "Verde";
    const char* JOGADOR2_COR = "Amarelo";

    // 2. Alocação de Memória para o Mapa
    Territorio* mapa = inicializarMapa(&tamanhoMapa);

    // 3. Alocação de Memória Dinâmica para as Missões (Requisito)
    // Aloca espaço no heap para armazenar a string da missão de cada jogador.
    char* missaoJogador1 = (char*)malloc(MAX_TAMANHO_MISSAO * sizeof(char));
    char* missaoJogador2 = (char*)malloc(MAX_TAMANHO_MISSAO * sizeof(char));

    if (missaoJogador1 == NULL || missaoJogador2 == NULL) {
        printf("Erro critico: Falha ao alocar memoria para missoes.\n");
        // Libera o que pode ter sido alocado
        free(mapa);
        free(missaoJogador1); // free(NULL) é seguro
        free(missaoJogador2);
        return 1;
    }

    // 4. Sorteio e Atribuição das Missões (Passagem por "Referência")
    // Passamos o ponteiro 'missaoJogador1' para que a função POSSA ESCREVER
    // no endereço de memória que ele aponta.
    atribuirMissao(missaoJogador1, VETOR_MISSOES, TOTAL_MISSOES_PREDEFINIDAS);
    atribuirMissao(missaoJogador2, VETOR_MISSOES, TOTAL_MISSOES_PREDEFINIDAS);

    // 5. Exibição Condicional (Apenas uma vez no início)
    printf("--- JOGO INICIADO ---\n");
    printf("\n[Jogador 1 - %s]\n", JOGADOR1_COR);
    exibirMissao(missaoJogador1); // Passagem por Valor (do ponteiro)

    printf("\n[Jogador 2 - %s]\n", JOGADOR2_COR);
    exibirMissao(missaoJogador2);

    // --- Loop Principal do Jogo (Simulação) ---
    int jogoAtivo = 1;
    int turno = 1;
    
    // Loop de simulação (limitado a 5 turnos para exemplo)
    while (jogoAtivo && turno <= 5) {
        printf("\n----------------------------------\n");
        printf("--- TURNO %d ---\n", turno);
        printf("----------------------------------\n");
        exibirMapa(mapa, tamanhoMapa);

        // --- Turno Jogador 1 (Verde) ---
        printf("\nTurno do Jogador 1 (%s)\n", JOGADOR1_COR);
        // Simulação de ataque: P1 (Brasil) ataca P2 (Argentina)
        int idxAtacante = 0; // Brasil
        int idxDefensor = 1; // Argentina

        // Validação de ataque (Requisito): Só ataca territórios inimigos
        if (strcmp(mapa[idxAtacante].cor, JOGADOR1_COR) == 0 && 
            strcmp(mapa[idxDefensor].cor, JOGADOR1_COR) != 0) 
        {
            printf("%s (%s) ataca %s (%s)!\n", 
                mapa[idxAtacante].nome, mapa[idxAtacante].cor,
                mapa[idxDefensor].nome, mapa[idxDefensor].cor);
            
            atacar(&mapa[idxAtacante], &mapa[idxDefensor]);
        } else {
            printf("%s nao pode atacar %s.\n", mapa[idxAtacante].nome, mapa[idxDefensor].nome);
        }

        // Verificação de Vitória (Requisito)
        if (verificarMissao(missaoJogador1, mapa, tamanhoMapa, JOGADOR1_COR)) {
            printf("\n!!! JOGADOR 1 (%s) VENCEU O JOGO !!!\n", JOGADOR1_COR);
            exibirMissao(missaoJogador1);
            jogoAtivo = 0;
            continue; // Pula o resto do loop
        }


        // --- Turno Jogador 2 (Amarelo) ---
        printf("\nTurno do Jogador 2 (%s)\n", JOGADOR2_COR);
        // Simulação de ataque: P2 (Canada) ataca NPC (Alasca)
        idxAtacante = 2; // Canada
        idxDefensor = 3; // Alasca

        // Validação
        if (strcmp(mapa[idxAtacante].cor, JOGADOR2_COR) == 0 && 
            strcmp(mapa[idxDefensor].cor, JOGADOR2_COR) != 0) 
        {
             printf("%s (%s) ataca %s (%s)!\n", 
                mapa[idxAtacante].nome, mapa[idxAtacante].cor,
                mapa[idxDefensor].nome, mapa[idxDefensor].cor);
            
            atacar(&mapa[idxAtacante], &mapa[idxDefensor]);
        } else {
            printf("%s nao pode atacar %s.\n", mapa[idxAtacante].nome, mapa[idxDefensor].nome);
        }

        // Verificação de Vitória (Requisito)
        if (verificarMissao(missaoJogador2, mapa, tamanhoMapa, JOGADOR2_COR)) {
            printf("\n!!! JOGADOR 2 (%s) VENCEU O JOGO !!!\n", JOGADOR2_COR);
            exibirMissao(missaoJogador2);
            jogoAtivo = 0;
            continue;
        }

        turno++;
    }

    if (jogoAtivo) {
        printf("\n--- FIM DA SIMULACAO (Limite de turnos atingido) ---\n");
    }

    // 6. Liberação de Memória (Requisito)
    liberarMemoria(mapa, missaoJogador1, missaoJogador2);

    return 0;
}


// --- Implementação das Funções ---

/**
 * @brief Inicializa um mapa de jogo simples com 4 territórios.
 * Aloca memória dinamicamente para o vetor de territórios.
 * * @param tamanhoMapa Ponteiro para a variável que armazenará o tamanho do mapa.
 * @return Ponteiro para o vetor de territórios alocado.
 */
Territorio* inicializarMapa(int* tamanhoMapa) {
    *tamanhoMapa = 4;
    
    // Usa calloc para inicializar a memória com zeros (boa prática)
    Territorio* mapa = (Territorio*)calloc(*tamanhoMapa, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro critico: Falha ao alocar memoria para o mapa.\n");
        exit(1); // Encerra o programa se não houver memória
    }

    // Território Jogador 1
    strcpy(mapa[0].nome, "Brasil");
    strcpy(mapa[0].cor, "Verde");
    mapa[0].tropas = 5;

    // Território Jogador 2
    strcpy(mapa[1].nome, "Argentina");
    strcpy(mapa[1].cor, "Amarelo");
    mapa[1].tropas = 3;

    // Território Jogador 1 (para missão "Possuir 3 territórios")
    strcpy(mapa[2].nome, "Canada");
    strcpy(mapa[2].cor, "Verde");
    mapa[2].tropas = 8; // Para missão "Possuir 10 tropas"

    // Território Neutro (para missão "Eliminar Vermelha")
    strcpy(mapa[3].nome, "Alasca");
    strcpy(mapa[3].cor, "Vermelha");
    mapa[3].tropas = 2;

    return mapa;
}

/**
 * @brief Sorteia uma missão do vetor global e a copia para o destino.
 * (Requisito: void atribuirMissao(char* destino, char* missoes[], int totalMissoes))
 * * @param destino Ponteiro para a string de destino (memória já alocada).
 * @param missoes Vetor de strings contendo as missões pré-definidas.
 * @param totalMissoes Tamanho do vetor de missões.
 */
void atribuirMissao(char* destino, const char* missoes[], int totalMissoes) {
    int indiceSorteado = rand() % totalMissoes;
    
    // Copia a string da missão sorteada para a memória dinâmica do jogador
    strcpy(destino, missoes[indiceSorteado]);
}

/**
 * @brief Exibe a missão do jogador.
 * (Requisito: Passagem por valor para exibição)
 * O ponteiro 'missao' é passado por valor. A função recebe uma cópia
 * do endereço, mas não pode alterar para onde o 'missao' original (na main) aponta.
 * Ela pode, no entanto, alterar o *conteúdo* da memória (o que não fazemos aqui).
 * * @param missao A string (char*) contendo a descrição da missão.
 */
void exibirMissao(const char* missao) {
    printf("Sua missao: %s\n", missao);
}

/**
 * @brief Simula um ataque entre dois territórios.
 * (Requisito: void atacar(Territorio* atacante, Territorio* defensor))
 * * @param atacante Ponteiro para o território atacante.
 * @param defensor Ponteiro para o território defensor.
 */
void atacar(Territorio* atacante, Territorio* defensor) {
    // Validação básica de tropas
    if (atacante->tropas <= 1) {
        printf("Ataque falhou: %s precisa de mais de 1 tropa para atacar.\n", atacante->nome);
        return;
    }

    // Simulação de dados (Requisito)
    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;

    printf("  > Dados: Atacante (%d) vs Defensor (%d)\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        // Vitória do Atacante
        printf("  > VITORIA! %s conquistou %s.\n", atacante->nome, defensor->nome);
        
        // Transferência de cor (Requisito)
        strcpy(defensor->cor, atacante->cor);

        // Transferência de metade das tropas (Requisito)
        int tropasMovidas = atacante->tropas / 2;
        if (tropasMovidas == 0) {
            tropasMovidas = 1; // Garante que mova pelo menos 1 se tiver 2 ou 3 tropas
        }

        defensor->tropas = tropasMovidas;
        atacante->tropas -= tropasMovidas;

    } else {
        // Derrota do Atacante
        printf("  > DERROTA! %s perdeu 1 tropa.\n", atacante->nome);
        atacante->tropas--; // Perde uma tropa (Requisito)
    }
}

/**
 * @brief Exibe o estado atual de todos os territórios no mapa.
 * * @param mapa Ponteiro para o início do vetor de territórios.
 * @param tamanhoMapa O número de territórios no vetor.
 */
void exibirMapa(Territorio* mapa, int tamanhoMapa) {
    printf("\n--- ESTADO ATUAL DO MAPA ---\n");
    for (int i = 0; i < tamanhoMapa; i++) {
        printf("[%d] %-10s \t(Cor: %-8s, Tropas: %d)\n", 
               i, 
               mapa[i].nome, 
               mapa[i].cor, 
               mapa[i].tropas);
    }
}

/**
 * @brief Verifica se a condição de vitória da missão foi atingida.
 * (Requisito: int verificarMissao(char* missao, Territorio* mapa, int tamanho))
 * * @param missao A string da missão do jogador.
 * @param mapa O vetor de territórios.
 * @param tamanhoMapa O tamanho do mapa.
 * @param corJogador A cor do jogador sendo verificado.
 * @return 1 (true) se a missão foi cumprida, 0 (false) caso contrário.
 */
int verificarMissao(const char* missao, Territorio* mapa, int tamanhoMapa, const char* corJogador) {
    
    // Lógica simples de verificação baseada na string da missão
    
    if (strcmp(missao, "Eliminar todas as tropas da cor Vermelha") == 0) {
        for (int i = 0; i < tamanhoMapa; i++) {
            if (strcmp(mapa[i].cor, "Vermelha") == 0) {
                return 0; // Ainda existe território Vermelho
            }
        }
        return 1; // Nenhuma cor "Vermelha" encontrada
    }
    
    if (strcmp(missao, "Eliminar todas as tropas da cor Azul") == 0) {
         for (int i = 0; i < tamanhoMapa; i++) {
            if (strcmp(mapa[i].cor, "Azul") == 0) {
                return 0; 
            }
        }
        return 1;
    }

    if (strcmp(missao, "Conquistar o territorio 'Brasil'") == 0) {
        for (int i = 0; i < tamanhoMapa; i++) {
            if (strcmp(mapa[i].nome, "Brasil") == 0) {
                // Achou o território, verifica se a cor é do jogador
                return (strcmp(mapa[i].cor, corJogador) == 0);
            }
        }
        return 0; // Território "Brasil" não encontrado (improvável)
    }

    if (strcmp(missao, "Possuir 3 territorios") == 0) {
        int contagem = 0;
        for (int i = 0; i < tamanhoMapa; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                contagem++;
            }
        }
        return (contagem >= 3);
    }

    if (strcmp(missao, "Possuir 10 tropas no territorio 'Canada'") == 0) {
         for (int i = 0; i < tamanhoMapa; i++) {
            if (strcmp(mapa[i].nome, "Canada") == 0) {
                // Achou o território, verifica cor E tropas
                return (strcmp(mapa[i].cor, corJogador) == 0 && mapa[i].tropas >= 10);
            }
        }
        return 0;
    }

    // Missão desconhecida ou ainda não implementada
    return 0;
}


/**
 * @brief Libera toda a memória alocada dinamicamente.
 * (Requisito: void liberarMemoria(...))
 * * @param mapa Ponteiro para o vetor de territórios.
 * @param missaoP1 Ponteiro para a string de missão do Jogador 1.
 * @param missaoP2 Ponteiro para a string de missão do Jogador 2.
 */
void liberarMemoria(Territorio* mapa, char* missaoP1, char* missaoP2) {
    printf("\n--- Liberando memoria ---\n");
    
    // Libera o vetor de territórios
    free(mapa);
    mapa = NULL;
    
    // Libera as strings de missão
    free(missaoP1);
    missaoP1 = NULL;
    
    free(missaoP2);
    missaoP2 = NULL;
    
    printf("Memoria liberada com sucesso.\n");
}
