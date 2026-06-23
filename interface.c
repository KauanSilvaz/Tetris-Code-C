#include <stdio.h>                           // Inclui a biblioteca padrão de entrada e saída (usada para printf, fopen, etc.).
#include <stdlib.h>                          // Inclui a biblioteca padrão do C (usada para a função system).
#include <string.h>                          // Inclui a biblioteca de manipulação de strings.
#include <conio.h>                           // Inclui biblioteca do Windows para entrada de console (fornece _kbhit e _getch).
#include <windows.h>                         // Inclui a API do Windows (necessária para manipular o terminal, cores e cursor).
#include "tetris.h"                          // Inclui as definições e estruturas principais do jogo Tetris (EstadoJogo, Peca, etc.).
#include "interface.h"                       // Inclui os protótipos das funções definidas neste próprio arquivo.

static HANDLE hConsole;                      // Declara uma variável estática (global apenas para este arquivo) para controlar o console do Windows.
 
                                             // Limpa a tela                            
void limparTela(void) {                      // Define a função 'limparTela', que não recebe argumentos nem retorna valores.
    system("cls");                           // Chama o comando do sistema operacional ("cls" no Windows) para limpar o texto do terminal.
}                                            // Fim da função 'limparTela'.

// Move o cursor para linha/coluna usando API do Windows // Comentário original descrevendo a função.
void moverCursor(int linha, int coluna) { // Define a função 'moverCursor', recebendo as coordenadas de linha e coluna.
    COORD pos;                           // Declara uma estrutura 'COORD' (da API do Windows) para armazenar coordenadas X e Y.
    pos.X = (SHORT)(coluna - 1);         // Define a coordenada X (coluna). Subtrai 1 porque o console no Windows começa no índice 0.
       SetConsoleCursorPosition(hConsole, pos); // Move o cursor físico do terminal para a posição especificada em 'pos'.
}                                        // Fim da função 'moverCursor'.

// Esconde o cursor (sem piscar durante o jogo) // Comentário original.
void ocultarCursor(void) {               // Define a função 'ocultarCursor' para esconder o cursor piscante de texto.
    CONSOLE_CURSOR_INFO info;            // Declara uma estrutura 'CONSOLE_CURSOR_INFO' para configurar as propriedades do cursor.
    info.dwSize = 1;                     // Define o tamanho do cursor como 1 (o valor mínimo permitido).
    info.bVisible = FALSE;               // Define a propriedade de visibilidade do cursor como Falsa (invisível).
    SetConsoleCursorInfo(hConsole, &info); // Aplica as novas configurações de cursor ao console atual.
}                                        // Fim da função 'ocultarCursor'.

// Mostra o cursor de volta              // Comentário original.
void mostrarCursor(void) {               // Define a função 'mostrarCursor' para voltar a exibir o cursor de texto.
    CONSOLE_CURSOR_INFO info;            // Declara a mesma estrutura para configurar o cursor.
    info.dwSize = 100 ;                  // Define o tamanho do cursor como 100 (tamanho máximo/padrão).
    info.bVisible = TRUE;                // Define a propriedade de visibilidade do cursor como Verdadeira (visível).
    SetConsoleCursorInfo(hConsole, &info); // Aplica as novas configurações para exibir o cursor no terminal.
}                                        // Fim da função 'mostrarCursor'.

// Configura o terminal Windows para aceitar cores ANSI // Comentário original.
void configurarTerminal(void) {          // Define a função que inicializa o terminal para suportar cores modernas.
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Obtém o "handle" (identificador) do dispositivo de saída padrão (o terminal atual) e salva em 'hConsole'.
    DWORD modo;                          // Declara uma variável do tipo DWORD para armazenar o modo atual do console.
    GetConsoleMode(hConsole, &modo);     // Lê o modo de operação atual do console e salva na variável 'modo'.
    SetConsoleMode(hConsole, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING); // Atualiza o console para habilitar o processamento de sequências de escape ANSI (para cores).
}                                        // Fim da função 'configurarTerminal'.

// No Windows não precisa restaurar nada especial // Comentário original.
void restaurarTerminal(void) {           // Define a função para restaurar o terminal ao estado normal antes de sair do jogo.
    mostrarCursor();                     // Chama a função interna para garantir que o cursor volte a ficar visível.
}                                        // Fim da função 'restaurarTerminal'.

// Lê tecla sem travar — usa _kbhit() e _getch() do conio.h // Comentário original.
// _kbhit() retorna 1 se tem tecla esperando, sem bloquear  // Comentário original.
int lerTeclaNaoBloqueante(void) {        // Define a função para capturar teclas sem pausar o jogo.
    if (!_kbhit()) return -1;            // Verifica se alguma tecla foi pressionada. Se não foi (_kbhit retorna 0), sai da função retornando -1.
    int c = _getch();                    // Se uma tecla foi pressionada, lê o caractere do buffer sem exibi-lo na tela e salva em 'c'.
    // Setas chegam como dois bytes: 0 ou 224, depois o código // Comentário original explicando teclas especiais.
    if (c == 0 || c == 224) {            // Verifica se o primeiro byte lido indica que é uma tecla especial (como as setas do teclado).
        int c2 = _getch();               // Lê o segundo byte que identifica qual tecla especial foi pressionada.
        if (c2 == 72) return 'w';        // Se o segundo byte for 72 (Seta para Cima), retorna o caractere 'w' (usado para rotacionar).
        if (c2 == 80) return 's';        // Se o segundo byte for 80 (Seta para Baixo), retorna o caractere 's' (usado para descer mais rápido).
        if (c2 == 75) return 'a';        // Se o segundo byte for 75 (Seta para a Esquerda), retorna o caractere 'a' (mover para a esquerda).
        if (c2 == 77) return 'd';        // Se o segundo byte for 77 (Seta para a Direita), retorna o caractere 'd' (mover para a direita).
    }                                    // Fim do bloco de verificação de teclas especiais.
    return c;                            // Se não for uma tecla especial, apenas retorna o código ASCII da tecla pressionada.
}                                        // Fim da função 'lerTeclaNaoBloqueante'.

// ---- daqui pra baixo é igual ao original ---- // Comentário original separando as funções dependentes do sistema.

static void setCor(int cor)  { printf("\033[%dm", cor); } // Função auxiliar para mudar a cor do texto do terminal usando código de escape ANSI.
static void resetCor(void)   { printf("\033[0m"); }       // Função auxiliar para resetar a cor do texto do terminal para o padrão.

static void desenharTabuleiro(EstadoJogo *e) {   // Define a função para desenhar a grade do jogo, recebe um ponteiro para o estado do jogo.
    int l, c;                                    // Declara variáveis inteiras para iterar sobre linhas (l) e colunas (c).
    int vis[ROWS][COLS];                         // Cria uma matriz bidimensional temporária 'vis' para preparar o que será impresso na tela.
    for (l = 0; l < ROWS; l++)                   // Inicia um laço que percorre todas as linhas do tabuleiro.
        for (c = 0; c < COLS; c++)               // Inicia um laço aninhado que percorre todas as colunas do tabuleiro.
            vis[l][c] = e->tabuleiro[l][c];      // Copia a cor do bloco do tabuleiro real para a nossa matriz visual temporária 'vis'.

    Peca sombra = e->pecaAtual;                  // Cria uma cópia da peça atual chamada 'sombra' para calcular onde ela vai cair.
    while (!verificarColisao(e->tabuleiro, &sombra)) sombra.y++; // Faz a 'sombra' descer uma linha de cada vez até detectar uma colisão.
    sombra.y--;                                  // Como o laço parou após a colisão, sobe a 'sombra' 1 linha para ficar na posição válida final.
    for (l = 0; l < 4; l++)                      // Laço para percorrer as 4 linhas da matriz 4x4 da peça 'sombra'.
        for (c = 0; c < 4; c++)                  // Laço para percorrer as 4 colunas da matriz 4x4 da peça 'sombra'.
            if (sombra.matriz[l][c]) {           // Verifica se existe um bloco na coordenada atual da peça 'sombra'.
                int ty = sombra.y+l, tx = sombra.x+c; // Calcula a posição Y (ty) e X (tx) absolutas da sombra no tabuleiro.
                if (ty>=0 && ty<ROWS && tx>=0 && tx<COLS && !vis[ty][tx]) // Garante que as coordenadas estão dentro do tabuleiro e o espaço visual está vazio.
                    vis[ty][tx] = -1;            // Marca a posição na matriz visual com -1 (indicador visual para desenhar a textura de "sombra").
            }                                    // Fim do if e dos laços da sombra.

    for (l = 0; l < 4; l++)                      // Laço para percorrer as 4 linhas da matriz 4x4 da peça real sendo controlada.
        for (c = 0; c < 4; c++)                  // Laço para percorrer as 4 colunas da matriz 4x4 da peça real sendo controlada.
            if (e->pecaAtual.matriz[l][c]) {     // Verifica se existe um bloco na coordenada atual da matriz da peça.
                int ty = e->pecaAtual.y+l, tx = e->pecaAtual.x+c; // Calcula as coordenadas Y (ty) e X (tx) absolutas da peça no tabuleiro.
                if (ty>=0 && ty<ROWS && tx>=0 && tx<COLS) // Verifica se a parte da peça atual está visível dentro dos limites do tabuleiro.
                    vis[ty][tx] = e->pecaAtual.cor; // Define a posição visual com a cor exata da peça atual.
            }                                    // Fim do if e dos laços da peça ativa.

    moverCursor(1,1); printf("+"); for(c=0;c<COLS;c++) printf("--"); printf("+"); // Posiciona o cursor no topo esquerdo e imprime a borda superior do tabuleiro.
    for (l = 0; l < ROWS; l++) {                 // Laço que percorre cada linha do tabuleiro visual para imprimir na tela.
        moverCursor(l+2, 1); printf("|");        // Move o cursor para o início da próxima linha na tela e imprime a borda lateral esquerda.
        for (c = 0; c < COLS; c++) {             // Laço que percorre cada coluna da linha atual.
            if      (vis[l][c] > 0) { setCor(vis[l][c]); printf("##"); resetCor(); } // Se o valor for > 0 (cor da peça), muda a cor, imprime blocos cheios "##" e reseta a cor.
            else if (vis[l][c] < 0)   printf(".."); // Se o valor for < 0 (é a sombra), imprime pontilhados ".." simbolizando onde a peça vai cair.
            else                      printf("  "); // Se o valor for 0 (vazio), imprime espaços em branco "  ".
        }                                        // Fim do laço das colunas.
        printf("|");                             // Imprime a borda lateral direita da linha atual.
    }                                            // Fim do laço das linhas do tabuleiro.
    moverCursor(ROWS+2,1); printf("+"); for(c=0;c<COLS;c++) printf("--"); printf("+"); // Posiciona o cursor na base e imprime a borda inferior do tabuleiro.
}                                                // Fim da função 'desenharTabuleiro'.

static void desenharPainel(EstadoJogo *e) {      // Define a função auxiliar para desenhar o painel lateral com placar e dicas.
    int px = COLS*2+4, l, c;                     // Calcula a coordenada X (px) do painel para ficar ao lado direito do tabuleiro; declara iteradores.
    moverCursor(1,px);  printf("TETRIS");        // Move o cursor para o topo do painel e imprime o título do jogo.
    moverCursor(3,px);  printf("PROXIMA:");      // Move o cursor e imprime o cabeçalho para indicar a próxima peça.
    for (l=0; l<4; l++) {                        // Inicia um laço para as 4 linhas da matriz da próxima peça.
        moverCursor(4+l, px);                    // Move o cursor descendo linha por linha na área dedicada à próxima peça.
        for (c=0; c<4; c++) {                    // Inicia um laço para as 4 colunas da matriz da próxima peça.
            if (e->proxima.matriz[l][c]) { setCor(e->proxima.cor); printf("##"); resetCor(); } // Se houver bloco, aplica a cor da próxima peça, desenha "##" e reseta a cor.
            else printf("  ");                   // Se não houver bloco ali, imprime espaço em branco.
        }                                        // Fim do laço das colunas.
    }                                            // Fim do laço das linhas.
    moverCursor(9,px);  printf("Pontos: %d  ", e->pontuacao); // Posiciona e imprime a pontuação atual. Os espaços extras no final evitam lixo residual na tela.
    moverCursor(10,px); printf("Nivel:  %d  ", e->nivel);     // Posiciona e imprime o nível atual do jogador.
    moverCursor(11,px); printf("Linhas: %d  ", e->linhas);    // Posiciona e imprime a quantidade de linhas eliminadas.
    moverCursor(13,px); printf("Controles:");                 // Posiciona e imprime o título "Controles:".
    moverCursor(14,px); printf("a/d  mover    ");             // Imprime instrução para mover horizontalmente.
    moverCursor(15,px); printf("w    rotar    ");             // Imprime instrução para rotacionar a peça.
    moverCursor(16,px); printf("s    descer   ");             // Imprime instrução para descida rápida da peça.
    moverCursor(17,px); printf("spc  drop     ");             // Imprime instrução para hard drop (cair tudo de uma vez).
    moverCursor(18,px); printf("q    sair     ");             // Imprime instrução para sair e finalizar a partida.
}                                                // Fim da função 'desenharPainel'.

void desenharJogo(EstadoJogo *e) {               // Define a função principal de atualização de tela do jogo em andamento.
    desenharTabuleiro(e);                        // Chama a função interna para desenhar o tabuleiro e as peças.
    desenharPainel(e);                           // Chama a função interna para desenhar a interface lateral.
    fflush(stdout);                              // Força a saída de tudo que foi impresso para aparecer instantaneamente no terminal (evita buffer delay).
}                                                // Fim da função 'desenharJogo'.

void desenharGameOver(EstadoJogo *e) {           // Define a função para exibir a tela de Fim de Jogo.
    int cx = COLS+2;                             // Calcula uma posição horizontal 'cx' mais ou menos centralizada.
    moverCursor(9,cx-4);  printf("+-----------------+"); // Posiciona e imprime o topo da caixa de Game Over no centro da tela.
    moverCursor(10,cx-4); printf("|   GAME  OVER    |"); // Posiciona e imprime o texto "GAME OVER".
    moverCursor(11,cx-4); printf("|  Pts: %7d   |", e->pontuacao); // Posiciona e imprime a pontuação final formatada dentro da caixa.
    moverCursor(12,cx-4); printf("+-----------------+"); // Posiciona e imprime o rodapé da caixa de Game Over.
    moverCursor(14,cx-4); printf("Pressione ENTER");     // Avisa o usuário que ele precisa pressionar ENTER para continuar.
    fflush(stdout);                              // Força a exibição imediata do texto no terminal.
}                                                // Fim da função 'desenharGameOver'.

void desenharMenu(void) {                        // Define a função que desenha o Menu Inicial do jogo.
    limparTela();                                // Limpa o terminal completamente antes de desenhar.
    printf("\n\n");                              // Pula duas linhas no topo.
    printf("  +==========================+\n");  // Imprime a borda superior do cabeçalho do menu.
    printf("  |        TETRIS            |\n");  // Imprime o nome do jogo.
    printf("  |  Tecnicas de Prog. I     |\n");  // Imprime uma string adicional do cabeçalho (possivelmente o nome da disciplina escolar).
    printf("  +==========================+\n");  // Imprime a divisória do menu.
    printf("  |  [1] Novo Jogo           |\n");  // Imprime a primeira opção interativa: Jogar.
    printf("  |  [2] Ver Recordes        |\n");  // Imprime a segunda opção interativa: Ranking.
    printf("  |  [3] Sair                |\n");  // Imprime a terceira opção interativa: Fechar aplicativo.
    printf("  +==========================+\n");  // Imprime a borda inferior do menu.
    printf("\n  Escolha: ");                     // Imprime o prompt solicitando a digitação do usuário.
    fflush(stdout);                              // Força a exibição imediata da interface.
}                                                // Fim da função 'desenharMenu'.

void desenharRanking(void) {
    FILE *arq;
    
    // Cria uma estrutura temporária só para guardar os dados enquanto a gente ordena
    typedef struct {
        char nome[50];
        int pontos;
    } Recorde;
    
    Recorde recs[100]; // Suporta ler até 100 recordes do txt
    int total = 0;     // Vai contar quantos recordes achamos no arquivo

    limparTela();
    printf("\n  === RECORDES ===\n\n");
    
    arq = fopen("ranking.txt", "r");
    if (!arq) {
        printf("  Nenhum recorde ainda.\n");
    } else {
        // 1. LER TODOS OS RECORDES: Lê do txt e joga pro nosso vetor 'recs'
        while (total < 100 && fscanf(arq, "%49s %d", recs[total].nome, &recs[total].pontos) == 2) {
            total++;
        }
        fclose(arq);

        // 2. ORDENAR (Bubble Sort): Coloca do maior para o menor
        for (int i = 0; i < total - 1; i++) {
            for (int j = 0; j < total - i - 1; j++) {
                if (recs[j].pontos < recs[j+1].pontos) {
                    // Troca de lugar se o de baixo for maior que o de cima
                    Recorde temp = recs[j];
                    recs[j] = recs[j+1];
                    recs[j+1] = temp;
                }
            }
        }

        // 3. EXIBIR: Mostra só os 10 primeiros (ou menos, se não tiver 10 jogadas salvas)
        int limite = (total < 10) ? total : 10;
        for (int i = 0; i < limite; i++) {
            printf("  %2d. %-15s %d\n", i + 1, recs[i].nome, recs[i].pontos);
        }
    }
    
    printf("\n  ENTER para voltar...");
    fflush(stdout);
    while (getchar() != '\n');
}                                              // Fim da função 'desenharRanking'.