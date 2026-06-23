// tetris.c - Motor lógico e físico do jogo.

#define _POSIX_C_SOURCE 200809L                  // Habilita padrões POSIX para garantir que o código rode bem em diferentes sistemas operacionais.
#include <stdio.h>                               // Inclui a biblioteca padrão de entrada e saída (printf, etc.).
#include <stdlib.h>                              // Inclui funções de alocação de memória (malloc, calloc, free) e geração de números (rand, srand).
#include <string.h>                              // Inclui funções para manipulação direta de blocos de memória (memcpy, memset).
#include <time.h>                                // Inclui funções de tempo, usadas para gerar a semente aleatória das peças.
#include "tetris.h"                              // [Puxa de: tetris.h] Importação principal. Traz o conhecimento das structs (Peca, EstadoJogo) e macros (ROWS, COLS, NUM_PECAS).

// Declara um array 3D estático (privado a este arquivo) que guarda o desenho de cada peça.
// [Puxa de: tetris.h] Usa a macro NUM_PECAS (que vale 7) para definir o tamanho do array.
static const int FORMAS[NUM_PECAS][4][4] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // Peça I
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // Peça J
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // Peça L
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // Peça O (Quadrado)
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // Peça S
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // Peça T
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // Peça Z
};

// Array estático associando cada peça (de 0 a 6) a um código de cor ANSI específico.
// [Puxa de: tetris.h] Usa a macro NUM_PECAS.
static const int CORES[NUM_PECAS] = {36, 34, 33, 33, 32, 35, 31};

// Função que cria e retorna uma estrutura 'Peca' com formato e cor sorteados.
// [Puxa de: tetris.h] Retorna o tipo estruturado 'Peca'.
Peca gerarPeca(void) {
    Peca p;                                             // Declara a variável 'p' local do tipo Peca.
    p.tipo = (TipoPeca)(rand() % NUM_PECAS);            // [Puxa de: tetris.h] Sorteia um número de 0 a 6 e faz o cast para o enum 'TipoPeca'.
    p.cor  = CORES[p.tipo];                             // Busca a cor correspondente no array local usando o tipo sorteado como índice.
    memcpy(p.matriz, FORMAS[p.tipo], sizeof(p.matriz)); // Copia os 16 números da matriz de formato padrão para dentro da matriz da peça 'p'.
    p.x = COLS / 2 - 2;                                 // [Puxa de: tetris.h] Define o X inicial. Usa a constante COLS para centralizar a matriz 4x4 no topo.
    p.y = 0;                                            // Define o Y inicial como 0 (linha mais alta do tabuleiro).
    return p;                                           // Devolve a peça montada.
}

// Função que aloca as matrizes na memória e zera os placares para um novo jogo.
// [Puxa de: tetris.h] Recebe o ponteiro do 'EstadoJogo' para alterá-lo diretamente.
void inicializarJogo(EstadoJogo *e) {
    int i;                                              // Variável iteradora.
    srand((unsigned int)time(NULL));                    // Alimenta o gerador de números aleatórios com os segundos atuais do relógio do PC.

    // [Puxa de: tetris.h] Usa a constante ROWS para alocar o array principal de ponteiros (as linhas).
    e->tabuleiro = (int **)malloc(ROWS * sizeof(int *));
    for (i = 0; i < ROWS; i++)                          // Itera sobre as linhas criadas.
        // [Puxa de: tetris.h] Usa a constante COLS para alocar e zerar as colunas de cada linha (criando o grid bidimensional).
        e->tabuleiro[i] = (int *)calloc(COLS, sizeof(int));

    e->pontuacao = 0;                                   // Zera a pontuação na memória.
    e->nivel     = 1;                                   // Inicia a dificuldade no nível 1.
    e->linhas    = 0;                                   // Zera a contagem de linhas quebradas.
    e->gameOver  = 0;                                   // Define que o jogo está rodando (0).
    e->pecaAtual = gerarPeca();                         // [Função Interna] Chama a função de cima para criar a primeira peça que vai cair.
    e->proxima   = gerarPeca();                         // [Função Interna] Cria logo a segunda peça para ficar no display de "Próxima".
}

// Função que destrói a matriz dinâmica para não vazar memória RAM quando o jogo fechar.
// [Puxa de: tetris.h] Recebe o ponteiro do 'EstadoJogo'.
void liberarJogo(EstadoJogo *e) {
    int i;                                              // Iterador.
    for (i = 0; i < ROWS; i++)                          // [Puxa de: tetris.h] Itera sobre todas as linhas (ROWS).
        free(e->tabuleiro[i]);                          // Libera os arrays de colunas guardados em cada linha.
    free(e->tabuleiro);                                 // Libera o vetor raiz (espinha dorsal) que guardava os ponteiros das linhas.
}

// Função matemática que checa se a peça sobrepõe parede, chão ou blocos velhos.
// [Puxa de: tetris.h] Recebe a matriz dinâmica do tabuleiro e o ponteiro da 'Peca'.
int verificarColisao(int **tabuleiro, Peca *p) {
    int l, c;                                           // Iteradores para linha e coluna.
    for (l = 0; l < 4; l++) {                           // Itera nas 4 linhas da peça.
        for (c = 0; c < 4; c++) {                       // Itera nas 4 colunas da peça.
            if (!p->matriz[l][c]) continue;             // Se o valor na matriz local for 0 (vazio), pula a checagem desta célula.
            int tx = p->x + c;                          // Calcula em qual coluna (X) do tabuleiro global esta parte da peça está.
            int ty = p->y + l;                          // Calcula em qual linha (Y) do tabuleiro global esta parte da peça está.
            
            // [Puxa de: tetris.h] Usa COLS e ROWS para testar colisão com as bordas da matriz.
            if (tx < 0 || tx >= COLS) return 1;         // Bateu na parede esquerda (tx < 0) ou parede direita (tx >= COLS)? Retorna 1 (Colidiu).
            if (ty >= ROWS)           return 1;         // Bateu no chão (ty >= ROWS)? Retorna 1 (Colidiu).
            if (ty < 0)               continue;         // Se o bloco estiver "nascendo" acima da tela (ty negativo), ignora a colisão lá em cima.
            if (tabuleiro[ty][tx])    return 1;         // Olha no tabuleiro global. Se tiver um número maior que 0 lá, bateu em bloco morto. Retorna 1.
        }
    }
    return 0;                                           // Terminou de olhar a matriz 4x4 e não bateu em nada? Retorna 0 (Livre).
}

// Pinta permanentemente a cor da peça na matriz de fundo quando ela para de cair.
// [Puxa de: tetris.h] Recebe a matriz dinâmica do tabuleiro e o ponteiro da 'Peca'.
void fixarPeca(int **tabuleiro, Peca *p) {
    int l, c;                                           // Iteradores 4x4.
    for (l = 0; l < 4; l++)                             // Itera as 4 linhas da peça.
        for (c = 0; c < 4; c++)                         // Itera as 4 colunas da peça.
            if (p->matriz[l][c]) {                      // Acha um bloco sólido da peça...
                int ty = p->y + l, tx = p->x + c;       // Converte pra coordenada global...
                if (ty >= 0) tabuleiro[ty][tx] = p->cor; // Carimba o código da cor no array 2D do tabuleiro (se estiver dentro da tela visível).
            }
}

// Analisa a matriz de baixo pra cima, destrói linhas preenchidas e desce o resto.
int eliminarLinhas(int **tabuleiro) {
    int l, c, i, eliminadas = 0;                        // Variáveis de iteração e o contador de linhas quebradas neste turno.
    // [Puxa de: tetris.h] Usa ROWS e COLS para percorrer o poço.
    for (l = ROWS - 1; l >= 0; l--) {                   // Começa a ler a matriz de baixo (ROWS - 1) para cima (0).
        int completa = 1;                               // Cria uma flag assumindo que a linha atual está completamente preenchida.
        for (c = 0; c < COLS; c++)                      // Varre as colunas desta linha.
            if (!tabuleiro[l][c]) { completa = 0; break; } // Achou um 0 (buraco)? Quebra a flag pra falso e para de checar essa linha na mesma hora.
        
        if (completa) {                                 // Se a flag não foi quebrada (linha lotada de blocos)...
            eliminadas++;                               // Conta um ponto de linha eliminada.
            for (i = l; i > 0; i--)                     // Inicia um laço subindo a partir da linha apagada...
                memcpy(tabuleiro[i], tabuleiro[i-1], COLS * sizeof(int)); // Puxa (copia) a linha de cima por cima da linha atual, aplicando a gravidade nos blocos.
            memset(tabuleiro[0], 0, COLS * sizeof(int)); // Como tudo desceu, a linha 0 (teto absoluto) fica duplicada. Limpa o teto com zeros.
            l++;                                        // A linha atual recebeu blocos novos da linha de cima. Aumenta 'l' para o laço testar essa mesma altura de novo.
        }
    }
    return eliminadas;                                  // Retorna pro jogo quantas linhas sumiram.
}

// Atualiza a matemática do Score e aumenta o Nível do jogo.
// [Puxa de: tetris.h] Recebe a struct EstadoJogo.
void atualizarPontuacao(EstadoJogo *e, int n) {
    int tabela[] = {0, 100, 300, 500, 800};             // Define a base de pontos (índice = nº de linhas quebradas). Ex: quebrar 4 linhas de uma vez = 800pts.
    if (n >= 1 && n <= 4) e->pontuacao += tabela[n] * e->nivel; // Se quebrou de 1 a 4 linhas, multiplica o valor da tabela pelo nível atual e soma no placar.
    e->linhas += n;                                     // Soma as linhas apagadas ao total acumulado.
    e->nivel = (e->linhas / 10) + 1;                    // A cada 10 linhas acumuladas, sobe a dificuldade para o próximo inteiro.
}

// O motor da gravidade. Faz a peça tentar ir para Y+1.
// [Puxa de: tetris.h] Recebe o EstadoJogo completo.
int descerPeca(EstadoJogo *e) {
    e->pecaAtual.y++;                                   // Finge que a peça desceu 1 casa.
    if (verificarColisao(e->tabuleiro, &e->pecaAtual)) { // [Função Interna] Chama o teste de física pra ver se ela entrou na parede ou no chão.
        e->pecaAtual.y--;                               // Bateu! Então recua ela 1 casa pra cima (pra última posição segura).
        fixarPeca(e->tabuleiro, &e->pecaAtual);         // [Função Interna] Congela ela na matriz do fundo do tabuleiro.
        int n = eliminarLinhas(e->tabuleiro);           // [Função Interna] Vê se a fixação dela gerou alguma quebra de linha. Salva o total em 'n'.
        atualizarPontuacao(e, n);                       // [Função Interna] Manda a pontuação avaliar se o 'n' vai render pontos/níveis.
        e->pecaAtual = e->proxima;                      // Pega a peça que estava de reserva e transforma nela.
        e->proxima   = gerarPeca();                     // [Função Interna] Rola a roleta pra gerar uma nova peça de reserva.
        if (verificarColisao(e->tabuleiro, &e->pecaAtual)) // Testa se a nova peça acabou de nascer encavalada em uma montanha de blocos antigos.
            e->gameOver = 1;                            // Se sim, morreu. Levanta a bandeira de Game Over.
        return 0;                                       // Avisa quem chamou a função que a peça atingiu o repouso.
    }
    return 1;                                           // Se passou direto no 'if', a peça simplesmente caiu livre no ar. Retorna sucesso.
}

// Faz o deslocamento horizontal (teclas A e D).
// [Puxa de: tetris.h] Recebe o EstadoJogo.
void moverPeca(EstadoJogo *e, int dir) {
    e->pecaAtual.x += dir;                              // Adiciona a direção (-1 pra esquerda, +1 pra direita) na variável X da peça.
    if (verificarColisao(e->tabuleiro, &e->pecaAtual))  // [Função Interna] Testa se esse passo pra frente invadiu o cimento.
        e->pecaAtual.x -= dir;                          // Se sim, remove o deslocamento imediatamente. A peça não se move e o usuário sente a parede.
}


// Executa o giro da peça adaptando a "caixa de colisão" (Bounding Box) dependendo do formato dela.
// [Puxa de: tetris.h] A função recebe o ponteiro geral do EstadoJogo.
void rotacionarPeca(EstadoJogo *e) {
    
    // Cria matrizes temporárias na memória local. O `= {0}` é VITAL aqui:
    // Como algumas peças giram em 3x3, a 4ª linha/coluna ficaria com "lixo" de memória. O {0} garante a matriz limpa.
    int temp[4][4] = {0}, orig[4][4]; 
    
    // Declara os iteradores padrão de linhas e colunas.
    int l, c;
    
    // Faz um backup exato da matriz da peça antes de tentar o giro. Se der errado, usamos a 'orig' para reverter.
    memcpy(orig, e->pecaAtual.matriz, sizeof(orig));

    // [Puxa de: tetris.h] Avalia através do 'enum' se a peça atual é o Quadrado (O).
    if (e->pecaAtual.tipo == PECA_O) {
        return; // O quadrado não muda visualmente ao girar. Sai da função instantaneamente poupando CPU.
        
    // [Puxa de: tetris.h] Avalia se a peça atual é a Reta/Barra (I).
    } else if (e->pecaAtual.tipo == PECA_I) {
        // Giro 4x4 clássico apenas para a barra, pois ela usa toda a extensão da matriz.
        for (l = 0; l < 4; l++)
            for (c = 0; c < 4; c++)
                // Fórmula de rotação de matriz quadrada: a nova coluna recebe a antiga linha inversa.
                temp[c][3-l] = e->pecaAtual.matriz[l][c];
                
    // Se não for 'O' e não for 'I', restaram as peças 'T', 'L', 'J', 'S' e 'Z'.
    } else {
        // Giro 3x3 apertado. Elas rodam no próprio eixo para não bater atoa nas paredes (permite o T-Spin).
        for (l = 0; l < 3; l++) // IMPORTANTE: O laço de leitura só vai até o índice 2!
            for (c = 0; c < 3; c++)
                // A fórmula ajusta o limite superior da inversão de '3-l' para '2-l'.
                temp[c][2-l] = e->pecaAtual.matriz[l][c];
    }

    // Pega o desenho que foi montado dentro de 'temp' e injeta na matriz da peça que está em jogo.
    memcpy(e->pecaAtual.matriz, temp, sizeof(temp));
    
    // [Função Interna] Chama a física do tetris.c. Mesmo girando perfeito, a quina da peça recém-girada bateu na parede?
    if (verificarColisao(e->tabuleiro, &e->pecaAtual)) {
        
        // Wall Kick: Sistema que tenta "chutar" a peça para o lado se ela entalar durante o giro.
        int kicks[] = {1, -1, 2, -2}, k; // Array com as tentativas de empurrão em X: +1, -1, +2, -2.
        int ok = 0;                      // Flag de sucesso. Se virar 1, o empurrão funcionou.
        
        // Loop de testes. O '!ok' garante que o loop pare na mesma hora se algum chute der certo.
        for (k = 0; k < 4 && !ok; k++) {
            
            // Empurra o X da peça com a força do chute atual do array.
            e->pecaAtual.x += kicks[k];
            
            // [Função Interna] Se testar a colisão agora e der FALSO (0), o caminho está livre! Seta 'ok = 1'.
            if (!verificarColisao(e->tabuleiro, &e->pecaAtual)) ok = 1;
            
            // Se bateu, desfaz esse empurrão específico subtraindo a força, para tentar a próxima força do laço.
            else e->pecaAtual.x -= kicks[k];
        }
        
        // Fim das tentativas de Wall Kick. A flag 'ok' continua sendo 0 (Falso)?
        // Significa que não há espaço físico viável. Cancela tudo e devolve a matriz do backup 'orig' pra peça.
        if (!ok) memcpy(e->pecaAtual.matriz, orig, sizeof(orig));
    }
} // Fim da função.

// Desce a peça violentamente até onde der e carimba ela (Spacebar).
// [Puxa de: tetris.h] Recebe o EstadoJogo.
void dropPeca(EstadoJogo *e) {
    while (1) {                                         // Inicia um laço de repetição trancado (infinito).
        e->pecaAtual.y++;                               // Joga a peça 1 Y para baixo.
        if (verificarColisao(e->tabuleiro, &e->pecaAtual)) { // [Função Interna] Testa se entrou no chão.
            e->pecaAtual.y--;                           // Bateu! Recua o último passo.
            break;                                      // Estoura o laço infinito na hora.
        }
        e->pontuacao += 2;                              // Se caiu livre, dá +2 de score por cada bloco saltado (bônus de audácia).
    }
    descerPeca(e);                                      // [Função Interna] A peça já tá colada no fundo. Usa a descerPeca() pra carimbar ela, matar linha e spawnar a próxima, reaproveitando código.
}