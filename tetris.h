// tetris.h                                      // Comentário original identificando o arquivo de cabeçalho.
// Compilação: gcc main.c tetris.c interface.c -o tetris // Comentário original com a instrução de compilação via terminal.

#ifndef TETRIS_H                                 // Diretiva de pré-processador: "Se TETRIS_H não estiver definido". Evita múltiplas inclusões deste mesmo arquivo (Include Guard).
#define TETRIS_H                                 // Define a macro TETRIS_H, fechando o par de proteção para a próxima vez que o compilador ver este arquivo.

#define COLS 10   // largura do poço             // Define uma constante macro 'COLS' valendo 10, que determina a largura padrão da matriz do tabuleiro.
#define ROWS 20   // altura do poço              // Define uma constante macro 'ROWS' valendo 20, que determina a altura padrão do poço do Tetris.

// Os 7 tipos de peça do Tetris                  // Comentário original.
typedef enum {                                   // Inicia a definição de uma enumeração (enum) criando um novo tipo de dado para nomear valores sequenciais inteiros.
    PECA_I, PECA_J, PECA_L, PECA_O,              // Constantes enumeradas representando cada formato de peça (o C atribui automaticamente os valores 0, 1, 2, 3).
    PECA_S, PECA_T, PECA_Z,                      // Constantes enumeradas para o restante das peças (recebem valores 4, 5, 6).
    NUM_PECAS                                    // Truque do C: como é o último elemento da lista, receberá o valor 7, que magicamente representa o número total de peças do jogo.
} TipoPeca;                                      // Apelida essa estrutura de enumeração recém-criada como 'TipoPeca' para ser usada como tipo de variável.

// Representa a peça ativa                       // Comentário original.
typedef struct {                                 // Inicia a definição de uma estrutura (struct) para agrupar todas as variáveis que compõem uma peça.
    int matriz[4][4];  // formato da peça em grade 4x4 // Declara uma matriz 4x4 bidimensional para guardar o "desenho" físico/espacial da peça (onde tem bloco e onde é vazio).
    int x, y;          // posição no tabuleiro   // Declara variáveis inteiras para armazenar a coordenada X (coluna) e Y (linha) em que a peça se encontra no poço geral.
    TipoPeca tipo;                               // Declara uma variável do tipo 'TipoPeca' (nossa enumeração) para identificar rapidamente qual a forma original do bloco.
    int cor;           // código de cor ANSI     // Declara um inteiro para armazenar a cor da peça que será desenhada no terminal.
} Peca;                                          // Nomeia a estrutura como o tipo 'Peca'.

// Todo o estado de uma partida                  // Comentário original.
typedef struct {                                 // Inicia a definição de uma estrutura aglutinadora (o "Save State" do seu jogo).
    int **tabuleiro;   // matriz 2D alocada dinamicamente (ponteiro!) // Declara um ponteiro para ponteiro (int**). Ele servirá para mapear a nossa matriz [20][10] na memória dinâmica (heap).
    Peca pecaAtual;                              // Variável estruturada que guarda os dados exatos da peça que o jogador está controlando na queda.
    Peca proxima;                                // Variável estruturada que guarda a peça gerada em espera para ser exibida no visor lateral.
    int pontuacao;                               // Variável inteira que acumula o placar (score) do jogador durante a partida.
    int nivel;                                   // Variável inteira para determinar o nível de dificuldade atual (velocidade da gravidade).
    int linhas;                                  // Variável inteira que contabiliza o número bruto de linhas destruídas desde o início.
    int gameOver;                                // Variável inteira usada como flag (0 ou 1) para sinalizar se a tela lotou e o jogador perdeu.
} EstadoJogo;                                    // Nomeia essa super-estrutura como 'EstadoJogo', facilitando o trânsito dessas informações pelas funções.

// Protótipos das funções de tetris.c            // Comentário original indicando a seção de "sumário" das ações do jogo.
void inicializarJogo(EstadoJogo *e);             // Assinatura da função que prepara as variáveis da partida e aloca o tabuleiro na memória (recebe ponteiro do estado).
void liberarJogo(EstadoJogo *e);                 // Assinatura da função que limpa as matrizes dinâmicas da memória quando o programa é encerrado.
Peca gerarPeca(void);                            // Assinatura da função que constrói e retorna uma estrutura do tipo 'Peca' com formato e cor sorteados aleatoriamente.
int  verificarColisao(int **tabuleiro, Peca *p); // Assinatura da função que calcula se a matriz da peça bate nas paredes ou blocos antigos do tabuleiro (retorna 0 ou 1).
void fixarPeca(int **tabuleiro, Peca *p);        // Assinatura da função que "carimba" a matriz da peça no tabuleiro principal permanentemente quando ela cai no chão.
int  eliminarLinhas(int **tabuleiro);            // Assinatura da função que varre as linhas, elimina as que estão 100% cheias, desce as demais e retorna o total quebrado.
void atualizarPontuacao(EstadoJogo *e, int linhas); // Assinatura da função que soma os pontos com base no multiplicador de linhas apagadas juntas e atualiza o nível.
int  descerPeca(EstadoJogo *e);                  // Assinatura da função que força a peça para o próximo índice de Y; avalia se precisou fixá-la na nova posição.
void moverPeca(EstadoJogo *e, int dir);          // Assinatura da função que desloca a peça no eixo X (laterais) baseando-se no valor de 'dir' (-1 ou +1).
void rotacionarPeca(EstadoJogo *e);              // Assinatura da função que altera a matriz interna 4x4 da peça para espelhá-la 90 graus (e aplica empurrões para não bugar na parede).
void dropPeca(EstadoJogo *e);                    // Assinatura da função que laça a gravidade infinitamente, socando a peça até o fundo num piscar de olhos e adicionando bônus.

#endif                                           // Fim da área protegida pelo Include Guard iniciado no topo do arquivo.