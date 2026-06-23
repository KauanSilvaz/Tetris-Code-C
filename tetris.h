// tetris.h - Arquivo de cabeçalho principal contendo as regras de estrutura do jogo.

#ifndef TETRIS_H                 // Include Guard: Verifica se TETRIS_H não foi definido ainda para evitar dupla inclusão.
#define TETRIS_H                 // Define a macro TETRIS_H, ativando a proteção.

#define COLS 10                  // Define a constante de largura do tabuleiro (10 colunas). Usado amplamente por tetris.c e interface.c.
#define ROWS 20                  // Define a constante de altura do tabuleiro (20 linhas). Usado por tetris.c e interface.c.

typedef enum {                   // Cria um tipo enumerado (enum) para representar as 7 peças do Tetris como números.
    PECA_I, PECA_J, PECA_L, PECA_O, // Valores de 0 a 3 mapeados para as respectivas peças.
    PECA_S, PECA_T, PECA_Z,      // Valores de 4 a 6 mapeados para as respectivas peças.
    NUM_PECAS                    // Recebe o valor 7. Por ser o último da lista, serve magicamente para definir a quantidade total de peças.
} TipoPeca;                      // Nomeia esse novo tipo de dado como 'TipoPeca'.

typedef struct {                 // Define a estrutura que molda os dados de uma Peça em jogo.
    int matriz[4][4];            // Matriz 4x4 que guarda o desenho espacial da peça (0 para vazio, 1 para bloco sólido).
    int x, y;                    // Coordenadas globais da peça dentro da matriz do tabuleiro principal.
    TipoPeca tipo;               // Usa o enum criado acima para identificar a forma da peça.
    int cor;                     // Guarda o código numérico ANSI da cor da peça.
} Peca;                          // Nomeia esta estrutura como 'Peca'.

typedef struct {                 // Define a super-estrutura que guarda o "Save State" da partida em andamento.
    int **tabuleiro;             // Ponteiro duplo para alocar o grid 2D do tabuleiro dinamicamente na memória RAM (heap).
    Peca pecaAtual;              // Guarda os dados da peça que o jogador está ativamente controlando.
    Peca proxima;                // Guarda os dados da peça reserva que aparece no display lateral.
    int pontuacao;               // O score acumulado do jogador.
    int nivel;                   // A dificuldade atual (que afeta diretamente a velocidade da gravidade no main.c).
    int linhas;                  // Contador bruto de quantas linhas foram destruídas na partida.
    int gameOver;                // Flag booleana (0 ou 1) que serve para avisar o loop do main.c se o jogador perdeu.
} EstadoJogo;                    // Nomeia esta estrutura principal como 'EstadoJogo'.

// Protótipos (assinaturas) das funções lógicas que estão escritas de fato dentro de 'tetris.c':
void inicializarJogo(EstadoJogo *e);                // Prepara a memória dinâmica e as variáveis iniciais para uma nova partida.
void liberarJogo(EstadoJogo *e);                    // Limpa a memória RAM alocada para o tabuleiro ao fechar o jogo.
Peca gerarPeca(void);                               // Sorteia e monta uma peça aleatória com sua respectiva cor e formato original.
int  verificarColisao(int **tabuleiro, Peca *p);    // Checa se a peça está sobrepondo paredes ou blocos acumulados (retorna 0 para livre, 1 para colisão).
void fixarPeca(int **tabuleiro, Peca *p);           // Transfere a cor da peça para a matriz do tabuleiro quando ela atinge o chão de vez.
int  eliminarLinhas(int **tabuleiro);               // Varre o tabuleiro, deleta linhas cheias, puxa os blocos de cima para baixo e retorna o combo quebrado.
void atualizarPontuacao(EstadoJogo *e, int linhas); // Calcula o score ganho baseado no combo de linhas e atualiza o nível a cada 10 linhas.
int  descerPeca(EstadoJogo *e);                     // Aplica o "tick" de gravidade natural empurrando a peça 1 bloco para baixo.
void moverPeca(EstadoJogo *e, int dir);             // Tenta mover a peça horizontalmente (para a esquerda ou direita).
void rotacionarPeca(EstadoJogo *e);                 // Gira a matriz 4x4 da peça em 90 graus, acionando o sistema de empurrão se bater na parede (Wall Kick).
void dropPeca(EstadoJogo *e);                       // Derruba a peça instantaneamente até o repouso absoluto (Hard Drop).

#endif                           // Fecha o bloco de proteção do Include Guard iniciado na linha 3.