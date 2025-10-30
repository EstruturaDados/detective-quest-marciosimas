#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constantes Globais ---
#define NOME_MAX 50
#define PISTA_MAX 50
#define SUSPEITO_MAX 50
#define TAM_HASH 10 // Tamanho da tabela hash

// ===================================================================
// NÍVEL NOVATO: Estruturas da Mansão (Árvore Binária)
// ===================================================================

/**
 * @brief Estrutura para um nó da Árvore Binária (Mapa da Mansão).
 * Também armazena a pista e o suspeito associado (Níveis Aventureiro/Mestre).
 */
typedef struct Sala {
    char nome[NOME_MAX];
    struct Sala *esquerda;
    struct Sala *direita;
    
    // Campos para Níveis Aventureiro e Mestre
    char pistaEncontrada[PISTA_MAX];
    char suspeitoAssociado[SUSPEITO_MAX];
} Sala;

/**
 * @brief Cria uma nova sala (nó da árvore) e a inicializa.
 */
Sala* criarSala(const char* nome, const char* pista, const char* suspeito, Sala* esq, Sala* dir) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    
    strcpy(novaSala->nome, nome);
    
    // Define pista e suspeito (pode ser "" se não houver)
    strcpy(novaSala->pistaEncontrada, pista ? pista : "");
    strcpy(novaSala->suspeitoAssociado, suspeito ? suspeito : "");

    novaSala->esquerda = esq;
    novaSala->direita = dir;
    return novaSala;
}

/**
 * @brief Libera a memória alocada para a árvore da mansão.
 */
void liberarMansao(Sala* raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    free(raiz);
}


// ===================================================================
// NÍVEL AVENTUREIRO: Estruturas de Pistas (Árvore de Busca - BST)
// ===================================================================

/**
 * @brief Estrutura para um nó da Árvore de Busca Binária (Pistas).
 */
typedef struct PistaNode {
    char pista[PISTA_MAX];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

/**
 * @brief Insere uma nova pista na BST em ordem alfabética.
 * Se a pista já existir, ela não é duplicada.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    // 1. Caso base: Árvore vazia, cria o primeiro nó.
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        if (novo == NULL) {
            printf("Erro de alocacao de memoria!\n");
            exit(1);
        }
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }
    
    // 2. Compara a nova pista com a raiz atual
    int cmp = strcmp(pista, raiz->pista);
    
    if (cmp < 0) {
        // 3a. Nova pista é "menor" (vem antes), insere à esquerda
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (cmp > 0) {
        // 3b. Nova pista é "maior" (vem depois), insere à direita
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // 3c. Se cmp == 0, a pista já existe, não faz nada.
    
    return raiz;
}

/**
 * @brief Exibe todas as pistas coletadas em ordem alfabética (percurso em-ordem).
 */
void mostrarPistasEmOrdem(PistaNode* raiz) {
    if (raiz != NULL) {
        mostrarPistasEmOrdem(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        mostrarPistasEmOrdem(raiz->direita);
    }
}

/**
 * @brief Libera a memória alocada para a BST de pistas.
 */
void liberarBST(PistaNode* raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarBST(raiz->esquerda);
    liberarBST(raiz->direita);
    free(raiz);
}

// ===================================================================
// NÍVEL MESTRE: Suspeitos (Tabela Hash com Encadeamento)
// ===================================================================

/**
 * @brief Estrutura para um nó da lista encadeada (tratamento de colisões).
 */
typedef struct HashNode {
    char pista[PISTA_MAX];
    char suspeito[SUSPEITO_MAX];
    struct HashNode *proximo;
} HashNode;

/**
 * @brief Função de hash simples (baseada na soma ASCII).
 */
unsigned int funcaoHash(const char* str) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++)) {
        hash += c;
    }
    return hash % TAM_HASH;
}

/**
 * @brief Inicializa a tabela hash com ponteiros NULL.
 */
void inicializarTabelaHash(HashNode* tabela[]) {
    for (int i = 0; i < TAM_HASH; i++) {
        tabela[i] = NULL;
    }
}

/**
 * @brief Insere uma associação Pista -> Suspeito na Tabela Hash.
 * Usa encadeamento (insere no início da lista) para tratar colisões.
 */
void inserirNaHash(HashNode* tabela[], const char* pista, const char* suspeito) {
    unsigned int indice = funcaoHash(pista);
    
    // Cria o novo nó
    HashNode* novoNode = (HashNode*)malloc(sizeof(HashNode));
    if (novoNode == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    strcpy(novoNode->pista, pista);
    strcpy(novoNode->suspeito, suspeito);
    
    // Insere no início da lista encadeada do índice
    novoNode->proximo = tabela[indice];
    tabela[indice] = novoNode;
}

/**
 * @brief Exibe todas as associações Pista -> Suspeito na tabela.
 */
void mostrarAssociacoes(HashNode* tabela[]) {
    printf("\n--- Relatorio de Evidencias (Pista -> Suspeito) ---\n");
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode* atual = tabela[i];
        if (atual) {
            printf("Indice [%d]:\n", i);
            while (atual != NULL) {
                printf("  Pista: %-20s | Suspeito: %s\n", atual->pista, atual->suspeito);
                atual = atual->proximo;
            }
        }
    }
}

/**
 * @brief Analisa a tabela hash e encontra o suspeito mais citado.
 */
void encontrarCulpado(HashNode* tabela[]) {
    // Estrutura auxiliar para contar citações
    struct ContadorSuspeito {
        char nome[SUSPEITO_MAX];
        int contagem;
    } contadores[20]; // Limite de 20 suspeitos únicos
    
    int numSuspeitosUnicos = 0;

    // Itera por toda a tabela hash
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode* atual = tabela[i];
        while (atual != NULL) {
            const char* suspeito = atual->suspeito;
            int encontrado = 0;
            
            // Procura se o suspeito já está na lista de contagem
            for (int j = 0; j < numSuspeitosUnicos; j++) {
                if (strcmp(contadores[j].nome, suspeito) == 0) {
                    contadores[j].contagem++;
                    encontrado = 1;
                    break;
                }
            }
            
            // Se não encontrou, adiciona como um novo suspeito
            if (!encontrado && numSuspeitosUnicos < 20) {
                strcpy(contadores[numSuspeitosUnicos].nome, suspeito);
                contadores[numSuspeitosUnicos].contagem = 1;
                numSuspeitosUnicos++;
            }
            
            atual = atual->proximo;
        }
    }

    // Encontra o máximo na lista de contadores
    if (numSuspeitosUnicos == 0) {
        printf("\nNenhuma pista foi ligada a um suspeito.\n");
        return;
    }

    int maxContagem = 0;
    char culpado[SUSPEITO_MAX];
    strcpy(culpado, contadores[0].nome);
    maxContagem = contadores[0].contagem;

    for (int i = 1; i < numSuspeitosUnicos; i++) {
        if (contadores[i].contagem > maxContagem) {
            maxContagem = contadores[i].contagem;
            strcpy(culpado, contadores[i].nome);
        }
    }
    
    printf("\n------------------------------------------------------\n");
    printf("VEREDITO DA INVESTIGACAO\n");
    printf("O suspeito mais associado as pistas (%d pistas) e: %s\n", maxContagem, culpado);
    printf("------------------------------------------------------\n");
}

/**
 * @brief Libera a memória alocada para a Tabela Hash.
 */
void liberarTabelaHash(HashNode* tabela[]) {
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode* atual = tabela[i];
        while (atual != NULL) {
            HashNode* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        tabela[i] = NULL;
    }
}


// ===================================================================
// LÓGICA PRINCIPAL DO JOGO (MAIN LOOP)
// ===================================================================

/**
 * @brief Inicia o loop principal de exploração do jogo.
 * Esta função implementa a lógica do Nível Novato (navegação)
 * e integra as lógicas dos níveis Aventureiro (coleta na BST)
 * e Mestre (coleta na Tabela Hash).
 */
void iniciarExploracao(Sala* raizMansao, PistaNode** raizPistas, HashNode* tabelaHash[]) {
    Sala* salaAtual = raizMansao;
    char escolha;

    printf("Bem-vindo ao Detective Quest!\n");
    printf("Voce esta no Hall de Entrada. Comece a explorar.\n");

    while (1) {
        printf("\n======================================================\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // --- Lógica Aventureiro/Mestre: Encontrar Pista ---
        if (strlen(salaAtual->pistaEncontrada) > 0) {
            printf("Voce encontrou uma pista: [ %s ]\n", salaAtual->pistaEncontrada);
            
            // Adiciona na BST de Pistas (Nível Aventureiro)
            *raizPistas = inserirPista(*raizPistas, salaAtual->pistaEncontrada);
            
            // Adiciona na Tabela Hash (Nível Mestre)
            if (strlen(salaAtual->suspeitoAssociado) > 0) {
                inserirNaHash(tabelaHash, salaAtual->pistaEncontrada, salaAtual->suspeitoAssociado);
                printf("Esta pista parece estar ligada ao... %s!\n", salaAtual->suspeitoAssociado);
            }
            
            // Limpa a pista da sala para não ser encontrada novamente
            strcpy(salaAtual->pistaEncontrada, ""); 
        }
        // --- Fim da Lógica ---

        // Lógica Novato: Fim do caminho (nó-folha)
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Este e o fim deste caminho. Voce decide voltar ao Hall de Entrada.\n");
            salaAtual = raizMansao; // Volta ao início
            continue; // Pula para a próxima iteração do loop
        }

        // Lógica Novato: Navegação
        printf("Escolha seu caminho:\n");
        if (salaAtual->esquerda) printf("  (e) -> %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita)  printf("  (d) -> %s\n", salaAtual->direita->nome);
        printf("  (s) -> Sair da mansao e analisar pistas\n");
        printf("Opcao: ");
        
        scanf(" %c", &escolha); // Espaço antes de %c para consumir newlines pendentes

        if (escolha == 's') {
            printf("\nVoce decide parar a exploracao e analisar suas pistas.\n");
            break; // Sai do loop principal
        
        } else if (escolha == 'e') {
            if (salaAtual->esquerda) {
                salaAtual = salaAtual->esquerda; // Move para a esquerda
            } else {
                printf("Nao ha caminho a esquerda.\n");
            }
        
        } else if (escolha == 'd') {
            if (salaAtual->direita) {
                salaAtual = salaAtual->direita; // Move para a direita
            } else {
                printf("Nao ha caminho a direita.\n");
            }
        
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }
}


// ===================================================================
// FUNÇÃO PRINCIPAL (main)
// ===================================================================

int main() {
    // 1. Inicializa as estruturas de dados
    PistaNode* raizPistas = NULL;
    HashNode* tabelaSuspeitos[TAM_HASH];
    inicializarTabelaHash(tabelaSuspeitos);

    // 2. Monta o mapa da mansão (Nível Novato)
    // (Pista, Suspeito) são adicionados para Níveis Aventureiro/Mestre
    
    // Folhas (fins de caminho)
    Sala* salaJantar = criarSala("Sala de Jantar", "Faca de Prata", "Cozinheiro", NULL, NULL);
    Sala* biblioteca = criarSala("Biblioteca", "Carta Amassada", "Mordomo", NULL, NULL);
    Sala* jardim = criarSala("Jardim de Inverno", "Terra Molhada", "Jardineiro", NULL, NULL);
    Sala* varanda = criarSala("Varanda", "Pegada de Bota", "Jardineiro", NULL, NULL);
    
    // Nós intermediários
    Sala* cozinha = criarSala("Cozinha", "Garrafa de Veneno", "Cozinheiro", salaJantar, NULL);
    Sala* escritorio = criarSala("Escritorio", NULL, NULL, biblioteca, cozinha);
    Sala* salaMusica = criarSala("Sala de Musica", "Corda de Piano Arrebentada", "Mordomo", jardim, varanda);
    
    // Raiz
    Sala* raizMansao = criarSala("Hall de Entrada", NULL, NULL, escritorio, salaMusica);

    // 3. Inicia o jogo
    iniciarExploracao(raizMansao, &raizPistas, tabelaSuspeitos);

    // 4. Exibe os relatórios finais
    
    // Relatório Nível Aventureiro
    printf("\n\n--- CADERNO DE PISTAS (Ordem Alfabetica) ---\n");
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        mostrarPistasEmOrdem(raizPistas);
    }
    
    // Relatórios Nível Mestre
    mostrarAssociacoes(tabelaSuspeitos);
    encontrarCulpado(tabelaSuspeitos);

    // 5. Libera toda a memória alocada
    liberarMansao(raizMansao);
    liberarBST(raizPistas);
    liberarTabelaHash(tabelaSuspeitos);

    printf("\nJogo Concluido. Obrigado por jogar Detective Quest!\n");

    return 0;
}