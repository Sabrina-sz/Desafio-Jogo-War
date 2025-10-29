/* war_missoes.c
   Implementação das missões estratégicas para cada jogador no estilo "War".
   Requisitos: modularização, ponteiros, malloc/free, uso de strcpy para atribuir missões.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 30
#define MAX_COR 10

typedef struct {
    char nome[30];
    char cor[10]; // cor do dono (ex: "Vermelho", "Azul", "Verde", "Amarelo")
    int tropas;
} Territorio;

/* ----------------------- Assinaturas das funções ----------------------- */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador);
void exibirMissao(const char* missao); // passagem por valor para exibir (const char*)
void atacar(Territorio* atacante, Territorio* defensor);
void exibirMapa(Territorio* mapa, int tamanho);
void liberarMemoria(Territorio* mapa, int tamanho, char** missoesJogadores, int numJogadores);
int contarTerritoriosDoJogador(Territorio* mapa, int tamanho, const char* corJogador);
int somarTropasDoJogador(Territorio* mapa, int tamanho, const char* corJogador);
int existeTerritorioComNome(Territorio* mapa, int tamanho, const char* nome);

/* ----------------------- Implementação ----------------------- */

/* Sorteia uma missão do vetor missoes[] e copia para destino usando strcpy.
   Supõe-se que 'destino' já foi alocado com espaço suficiente. */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

/* Verifica se a missão (string com formato "id|descrição") foi cumprida.
   Retorna 1 se cumprida, 0 caso contrário.
   Recebe também a cor do jogador para verificar objetivos relacionados ao jogador. */
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador) {
    int id = 0;
    char descricao[200];
    // separa id e descrição (ex: "1|Conquistar 3 territorios")
    char* sep = strchr(missao, '|');
    if (!sep) return 0;
    int len_id = sep - missao;
    char id_str[10] = {0};
    strncpy(id_str, missao, len_id);
    id = atoi(id_str);
    strcpy(descricao, sep + 1);

    // Implementação simples para cada id de missão
    switch (id) {
        case 1: { // Conquistar 3 territórios (ter >= 3 territórios)
            int cnt = contarTerritoriosDoJogador(mapa, tamanho, corJogador);
            if (cnt >= 3) return 1;
            return 0;
        }
        case 2: { // Eliminar todas as tropas da cor Vermelha (exemplo: 'Vermelho')
            int existe = 0;
            for (int i = 0; i < tamanho; ++i) {
                if (strcmp(mapa[i].cor, "Vermelho") == 0 && mapa[i].tropas > 0) {
                    existe = 1; break;
                }
            }
            return !existe; // true se não existir tropas vermelhas
        }
        case 3: { // Controlar todos os territórios (vencer o jogo)
            int cnt = contarTerritoriosDoJogador(mapa, tamanho, corJogador);
            return (cnt == tamanho);
        }
        case 4: { // Ter ao menos 10 tropas no total
            int total = somarTropasDoJogador(mapa, tamanho, corJogador);
            return (total >= 10);
        }
        case 5: { // Conquistar o território "Território X" (exemplo)
            // vamos extrair um nome esperado depois do texto, mas para simplicidade,
            // assumimos que missão 5 pede um território fixo "Fortaleza"
            const char* alvo = "Fortaleza";
            for (int i = 0; i < tamanho; ++i) {
                if (strcmp(mapa[i].nome, alvo) == 0 && strcmp(mapa[i].cor, corJogador) == 0)
                    return 1;
            }
            return 0;
        }
        default:
            return 0;
    }
}

/* Exibe a missão (passagem por valor do ponteiro const char*). */
void exibirMissao(const char* missao) {
    char* sep = strchr(missao, '|');
    if (sep) {
        printf("Sua missão: %s\n", sep + 1);
    } else {
        printf("Sua missão: %s\n", missao);
    }
}

/* Simulação do ataque entre territorios.
   Usa rand() para rolagem (1..6) para atacante e defensor.
   Se atacante vence (soma maior), transfere cor e metade das tropas ao defensor.
   Caso perca, atacante perde 1 tropa. */
void atacar(Territorio* atacante, Territorio* defensor) {
    if (!atacante || !defensor) return;
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Ataque inválido: mesmo dono.\n");
        return;
    }
    if (atacante->tropas <= 0) {
        printf("%s não tem tropas suficientes para atacar.\n", atacante->nome);
        return;
    }

    int rollA = (rand() % 6) + 1;
    int rollD = (rand() % 6) + 1;
    printf("Rolagem atacante (%s) = %d | defensor (%s) = %d\n", atacante->nome, rollA, defensor->nome, rollD);
    if (rollA > rollD) {
        // atacante vence: transfere cor e metade das tropas (arredonda para baixo)
        int transfer = atacante->tropas / 2;
        if (transfer == 0) transfer = 1; // garante ao menos 1 tropa transferida
        printf("Atacante vence! Transferindo %d tropas e mudando cor de %s para %s\n",
               transfer, defensor->nome, atacante->cor);
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = transfer;
        atacante->tropas -= transfer;
        if (atacante->tropas < 0) atacante->tropas = 0;
    } else {
        // atacante perde 1 tropa
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
        printf("Defensor resiste! %s perde 1 tropa (agora tem %d).\n", atacante->nome, atacante->tropas);
    }
}

/* Exibe o mapa com nome, cor e tropas de cada território. */
void exibirMapa(Territorio* mapa, int tamanho) {
    printf("\nMapa atual:\n");
    printf("Idx | Nome                 | Cor       | Tropas\n");
    printf("----+----------------------+-----------+-------\n");
    for (int i = 0; i < tamanho; ++i) {
        printf("%3d | %-20s | %-9s | %5d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("\n");
}

/* Libera a memória das missões dos jogadores (vetor de strings alocadas) e do mapa */
void liberarMemoria(Territorio* mapa, int tamanho, char** missoesJogadores, int numJogadores) {
    if (mapa) free(mapa);
    if (missoesJogadores) {
        for (int i = 0; i < numJogadores; ++i) {
            if (missoesJogadores[i]) free(missoesJogadores[i]);
        }
        free(missoesJogadores);
    }
}

/* Contadores auxiliares */
int contarTerritoriosDoJogador(Territorio* mapa, int tamanho, const char* corJogador) {
    int cnt = 0;
    for (int i = 0; i < tamanho; ++i) {
        if (strcmp(mapa[i].cor, corJogador) == 0) cnt++;
    }
    return cnt;
}
int somarTropasDoJogador(Territorio* mapa, int tamanho, const char* corJogador) {
    int soma = 0;
    for (int i = 0; i < tamanho; ++i) {
        if (strcmp(mapa[i].cor, corJogador) == 0) soma += mapa[i].tropas;
    }
    return soma;
}
int existeTerritorioComNome(Territorio* mapa, int tamanho, const char* nome) {
    for (int i = 0; i < tamanho; ++i) {
        if (strcmp(mapa[i].nome, nome) == 0) return 1;
    }
    return 0;
}

/* ----------------------- Função main (demonstração) ----------------------- */
int main() {
    srand((unsigned int)time(NULL));

    /* Definição de missões (cada string começa com um id e '|' para facilitar verificação) */
    char* missoesDisponiveis[] = {
        "1|Conquistar 3 territorios",                       // id 1
        "2|Eliminar todas as tropas da cor Vermelho",       // id 2 (note: cor usada "Vermelho")
        "3|Controlar todos os territorios",                 // id 3
        "4|Ter ao menos 10 tropas no total",                // id 4
        "5|Conquistar o territorio Fortaleza"               // id 5 (alvo: "Fortaleza")
    };
    int totalMissoes = sizeof(missoesDisponiveis) / sizeof(missoesDisponiveis[0]);

    /* Configuração do mapa: cria dinamicamente N territorios */
    int numTerritorios = 6;
    Territorio* mapa = (Territorio*) calloc(numTerritorios, sizeof(Territorio));
    if (!mapa) {
        perror("malloc mapa");
        return 1;
    }
    /* Inicializa nomes, cores e tropas - exemplo inicial */
    strcpy(mapa[0].nome, "Fortaleza"); strcpy(mapa[0].cor, "Vermelho"); mapa[0].tropas = 3;
    strcpy(mapa[1].nome, "Colina");     strcpy(mapa[1].cor, "Azul");     mapa[1].tropas = 2;
    strcpy(mapa[2].nome, "Planalto");   strcpy(mapa[2].cor, "Vermelho"); mapa[2].tropas = 4;
    strcpy(mapa[3].nome, "Vale");       strcpy(mapa[3].cor, "Verde");    mapa[3].tropas = 1;
    strcpy(mapa[4].nome, "Costa");      strcpy(mapa[4].cor, "Azul");     mapa[4].tropas = 3;
    strcpy(mapa[5].nome, "Pântano");    strcpy(mapa[5].cor, "Amarelo");  mapa[5].tropas = 2;

    /* Jogadores (exemplo com 2 jogadores): suas cores e missões alocadas dinamicamente */
    int numJogadores = 2;
    const char* coresJogadores[] = {"Azul", "Verde"}; // cores dos jogadores
    char** missoesJogadores = (char**) malloc(sizeof(char*) * numJogadores);
    if (!missoesJogadores) {
        perror("malloc missoesJogadores");
        free(mapa);
        return 1;
    }
    for (int i = 0; i < numJogadores; ++i) {
        missoesJogadores[i] = (char*) malloc(200 * sizeof(char)); // espaço para a string da missão
        if (!missoesJogadores[i]) {
            perror("malloc missao jogador");
            // liberar já alocado
            for (int j = 0; j < i; ++j) free(missoesJogadores[j]);
            free(missoesJogadores);
            free(mapa);
            return 1;
        }
        atribuirMissao(missoesJogadores[i], missoesDisponiveis, totalMissoes);
    }

    /* Exibe missão apenas uma vez no início para cada jogador (interface intuitiva) */
    printf("=== Missões atribuídas (exibidas uma vez) ===\n");
    for (int i = 0; i < numJogadores; ++i) {
        printf("Jogador %d (cor %s): ", i+1, coresJogadores[i]);
        exibirMissao(missoesJogadores[i]); // passagem por valor
    }
    printf("============================================\n\n");

    /* Loop de turnos simplificado: cada jogador pode atacar até que alguém cumpra a missão.
       A cada final de turno, verifica-se silenciosamente se a missão foi cumprida. */
    int vencedor = -1;
    int turno = 0;
    char input[100];

    while (1) {
        int jogadorAtual = turno % numJogadores;
        printf("=== Turno do Jogador %d (cor %s) ===\n", jogadorAtual + 1, coresJogadores[jogadorAtual]);
        exibirMapa(mapa, numTerritorios);

        // interação: jogador escolhe atacar (entre índices) ou passar
        printf("Digite o índice do território atacante (ou -1 para passar / s para sair): ");
        if (!fgets(input, sizeof(input), stdin)) break;
        if (input[0] == 's' || input[0] == 'S') break;
        int idxAtq = atoi(input);
        if (idxAtq == -1) {
            printf("Jogador %d passou o turno.\n", jogadorAtual + 1);
        } else if (idxAtq < 0 || idxAtq >= numTerritorios) {
            printf("Índice inválido.\n");
        } else {
            // verifica se jogador é dono do território atacante
            if (strcmp(mapa[idxAtq].cor, coresJogadores[jogadorAtual]) != 0) {
                printf("Você só pode atacar de territórios de sua cor.\n");
            } else {
                printf("Digite o índice do território defensor: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int idxDef = atoi(input);
                if (idxDef < 0 || idxDef >= numTerritorios) {
                    printf("Índice inválido.\n");
                } else if (idxDef == idxAtq) {
                    printf("Não pode atacar o mesmo território.\n");
                } else {
                    // valida que defensor é de cor diferente
                    if (strcmp(mapa[idxDef].cor, mapa[idxAtq].cor) == 0) {
                        printf("O defensor pertence ao mesmo jogador. Escolha outro alvo.\n");
                    } else {
                        atacar(&mapa[idxAtq], &mapa[idxDef]);
                    }
                }
            }
        }

        // Ao final do turno, verificar silenciosamente se missão foi cumprida
        for (int j = 0; j < numJogadores; ++j) {
            if (verificarMissao(missoesJogadores[j], mapa, numTerritorios, coresJogadores[j])) {
                vencedor = j;
                break;
            }
        }
        if (vencedor != -1) {
            printf("\n=== Missão cumprida! ===\n");
            printf("Jogador %d (cor %s) cumpriu sua missão: ", vencedor + 1, coresJogadores[vencedor]);
            exibirMissao(missoesJogadores[vencedor]);
            break;
        }

        turno++;
        printf("\n");
    }

    /* Limpeza e fim */
    liberarMemoria(mapa, numTerritorios, missoesJogadores, numJogadores);
    printf("Jogo finalizado. Memória liberada.\n");
    return 0;
}
