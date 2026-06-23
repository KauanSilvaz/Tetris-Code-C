#include <stdio.h>       // Inclui a biblioteca padrão de entrada e saída (printf, fopen, etc.).
#include <stdlib.h>      // Inclui a biblioteca padrão do C (system).
#include <string.h>      // Inclui a biblioteca de manipulação de strings.
#include <conio.h>       // Inclui a biblioteca do Windows para capturar teclas sem bloquear (_kbhit, _getch).
#include <windows.h>     // Inclui a API do Windows para manipular o terminal (cursor, cores).
#include "tetris.h"      // [Puxa de: tetris.h] Inclui as definições estruturais do jogo (EstadoJogo, Peca, ROWS, COLS).
#include "interface.h"   // [Puxa de: interface.h] Inclui as assinaturas das funções deste próprio arquivo.

static HANDLE hConsole;  // Declara uma variável estática (global apenas para este arquivo) para controlar o terminal do Windows.

void limparTela(void) {  // Função que limpa a tela do terminal.
    system("cls");       // Executa o comando "cls" do Windows no terminal para apagar tudo.
}

void moverCursor(int linha, int coluna) { // Função que move o cursor do terminal para uma coordenada específica.
    COORD pos;                            // Declara a estrutura COORD da API do Windows para guardar coordenadas X e Y.
    pos.X = (SHORT)(coluna - 1);          // Define a coluna (X). Subtrai 1 pois a API do Windows começa a contar do 0.
    pos.Y = (SHORT)(linha - 1);           // Define a linha (Y). Subtrai 1 pelo mesmo motivo.
    SetConsoleCursorPosition(hConsole, pos); // Move o cursor físico do terminal para a posição calculada.
}

void ocultarCursor(void) {               // Função para esconder o cursor piscante de digitação.
    CONSOLE_CURSOR_INFO info;            // Declara a estrutura da API do Windows para propriedades do cursor.
    info.dwSize = 1;                     // Define o tamanho do cursor para o mínimo possível (1).
    info.bVisible = FALSE;               // Altera o status de visibilidade para falso (invisível).
    SetConsoleCursorInfo(hConsole, &info); // Aplica a configuração no console atual.
}

void mostrarCursor(void) {               // Função para reexibir o cursor piscante.
    CONSOLE_CURSOR_INFO info;            // Declara a estrutura de propriedades do cursor.
    info.dwSize = 100;                   // Restaura o tamanho do cursor para o padrão máximo (100).
    info.bVisible = TRUE;                // Altera o status de visibilidade para verdadeiro.
    SetConsoleCursorInfo(hConsole, &info); // Aplica a configuração no console.
}

void configurarTerminal(void) {          // Função que prepara o terminal para aceitar cores ANSI.
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Pega a referência (handle) da janela atual do terminal e salva na variável global.
    DWORD modo;                          // Declara uma variável para guardar a configuração atual do terminal.
    GetConsoleMode(hConsole, &modo);     // Lê como o terminal está configurado agora.
    SetConsoleMode(hConsole, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING); // Ativa a flag que permite usar códigos de cores ANSI no CMD/Powershell.
}

void restaurarTerminal(void) {           // Função para normalizar o terminal ao fechar o jogo.
    mostrarCursor();                     // Chama a função interna para garantir que o cursor volte a aparecer.
}

int lerTeclaNaoBloqueante(void) {        // Função que lê o teclado sem pausar a execução da gravidade do jogo.
    if (!_kbhit()) return -1;            // Se nenhuma tecla foi apertada no buffer, sai imediatamente retornando -1.
    int c = _getch();                    // Se apertou algo, captura a tecla pressionada sem mostrá-la na tela.
    
    if (c == 0 || c == 224) {            // Se o código for 0 ou 224, significa que é uma tecla especial de dois bytes (como as setinhas).
        int c2 = _getch();               // Lê o segundo byte que identifica exatamente qual foi a setinha apertada.
        if (c2 == 72) return 'w';        // Seta para cima mapeada para retornar 'w' (rotacionar).
        if (c2 == 80) return 's';        // Seta para baixo mapeada para retornar 's' (descer rápido).
        if (c2 == 75) return 'a';        // Seta para a esquerda mapeada para retornar 'a'.
        if (c2 == 77) return 'd';        // Seta para a direita mapeada para retornar 'd'.
    }
    return c;                            // Retorna o código da tecla normal apertada.
}

static void setCor(int cor)  { printf("\033[%dm", cor); } // Altera a cor da fonte do terminal enviando um código ANSI de escape.
static void resetCor(void)   { printf("\033[0m"); }       // Restaura a cor da fonte do terminal para o padrão de fábrica.

static void desenharTabuleiro(EstadoJogo *e) { // [Puxa de: tetris.h] Recebe o EstadoJogo atual para desenhar.
    int l, c;                                  // Variáveis para iterar as matrizes em linhas e colunas.
    int vis[ROWS][COLS];                       // [Puxa de: tetris.h] Cria matriz visual temporária usando as constantes ROWS e COLS.
    
    for (l = 0; l < ROWS; l++)                 // Percorre todas as linhas do tabuleiro.
        for (c = 0; c < COLS; c++)             // Percorre todas as colunas do tabuleiro.
            vis[l][c] = e->tabuleiro[l][c];    // Copia as cores dos blocos que já estão fixados no fundo para a matriz visual.

    Peca sombra = e->pecaAtual;                // [Puxa de: tetris.h] Instancia uma cópia da peça em queda para simular onde ela vai parar (Ghost Piece).
    while (!verificarColisao(e->tabuleiro, &sombra)) sombra.y++; // [Puxa de: tetris.c] Empurra a sombra para baixo no loop até a função de colisão gritar.
    sombra.y--;                                // Desfaz a última passada pois o while parou dentro do obstáculo.
    
    for (l = 0; l < 4; l++)                    // Percorre as linhas da matriz 4x4 da sombra.
        for (c = 0; c < 4; c++)                // Percorre as colunas da matriz 4x4 da sombra.
            if (sombra.matriz[l][c]) {         // Se a matriz da peça for sólida neste ponto (não vazia):
                int ty = sombra.y+l, tx = sombra.x+c; // Converte as coordenadas 4x4 para a posição absoluta X e Y na tela.
                if (ty>=0 && ty<ROWS && tx>=0 && tx<COLS && !vis[ty][tx]) // Verifica se a coordenada não vazou da tela e se a casa visual está vazia.
                    vis[ty][tx] = -1;          // Marca a casa na matriz visual com -1 (indicativo de que ali será desenhado o pontilhado da sombra).
            }

    for (l = 0; l < 4; l++)                    // Percorre as linhas da matriz 4x4 da peça real que está ativamente caindo.
        for (c = 0; c < 4; c++)                // Percorre as colunas da matriz 4x4 da peça real.
            if (e->pecaAtual.matriz[l][c]) {   // Se a peça real for sólida neste ponto:
                int ty = e->pecaAtual.y+l, tx = e->pecaAtual.x+c; // Calcula a coordenada X e Y absoluta da peça ativa.
                if (ty>=0 && ty<ROWS && tx>=0 && tx<COLS) // Verifica se essa parte da peça está visível dentro dos limites do tabuleiro.
                    vis[ty][tx] = e->pecaAtual.cor; // Sobrepõe a casa na matriz visual com o código da cor exata da peça.
            }

    moverCursor(1,1); printf("+"); for(c=0;c<COLS;c++) printf("--"); printf("+"); // Posiciona o cursor no canto superior esquerdo e imprime a borda do teto.
    
    for (l = 0; l < ROWS; l++) {               // Percorre as linhas da matriz visual gerada acima para fazer o print definitivo na tela.
        moverCursor(l+2, 1); printf("|");      // Move o cursor para o começo da linha e imprime a borda da parede esquerda.
        for (c = 0; c < COLS; c++) {           // Percorre as colunas da respectiva linha.
            if      (vis[l][c] > 0) { setCor(vis[l][c]); printf("##"); resetCor(); } // Se o valor for > 0 (é bloco), aplica a cor ANSI, imprime '##' e reseta a cor.
            else if (vis[l][c] < 0)   printf("..");   // Se o valor for < 0 (é a sombra), imprime '..' em cinza.
            else                      printf("  ");   // Se o valor for 0 (espaço vazio), imprime um vão em branco "  ".
        }
        printf("|");                           // Fecha a linha imprimindo a borda da parede direita.
    }
    moverCursor(ROWS+2,1); printf("+"); for(c=0;c<COLS;c++) printf("--"); printf("+"); // Posiciona o cursor debaixo da última linha e imprime o piso do poço.
}

static void desenharPainel(EstadoJogo *e) {    // [Puxa de: tetris.h] Recebe a referência do jogo para imprimir o HUD na interface lateral.
    int px = COLS*2+4, l, c;                   // [Puxa de: tetris.h] Calcula o recuo X do painel (px) se baseando de forma responsiva na largura de COLS.
    moverCursor(1,px);  printf("TETRIS");      // Imprime o Logo no topo do painel lateral.
    moverCursor(3,px);  printf("PROXIMA:");    // Imprime o cabeçalho do display que mostrará o próximo bloco.
    
    for (l=0; l<4; l++) {                      // Itera sobre as 4 linhas da área reservada para mostrar a próxima peça.
        moverCursor(4+l, px);                  // Vai descendo linha por linha.
        for (c=0; c<4; c++) {                  // Itera as colunas da próxima peça.
            if (e->proxima.matriz[l][c]) { setCor(e->proxima.cor); printf("##"); resetCor(); } // Se for sólido, muda pra cor da peça, imprime os blocos e reseta.
            else printf("  ");                 // Onde for vazio, imprime espaço em branco pra não poluir.
        }
    }
    moverCursor(9,px);  printf("Pontos: %d  ", e->pontuacao); // Formata e imprime a pontuação puxando do EstadoJogo (espaços evitam rebarba de strings anteriores).
    moverCursor(10,px); printf("Nivel:  %d  ", e->nivel);     // Imprime a variável do nível atual.
    moverCursor(11,px); printf("Linhas: %d  ", e->linhas);    // Imprime o contador de linhas brutas quebradas.
    
    moverCursor(13,px); printf("Controles:");                 // Imprime o título da área de tutorial.
    moverCursor(14,px); printf("a/d  mover    ");             // Descreve eixo X.
    moverCursor(15,px); printf("w    rotar    ");             // Descreve Giro.
    moverCursor(16,px); printf("s    descer   ");             // Descreve Soft Drop.
    moverCursor(17,px); printf("spc  drop     ");             // Descreve Hard Drop.
    moverCursor(18,px); printf("q    sair     ");             // Descreve Quit.
}

void desenharJogo(EstadoJogo *e) {             // [Puxa de: tetris.h] Função mestra de atualização contínua de tela no gameloop.
    desenharTabuleiro(e);                      // Chama sub-rotina para desenhar física e blocos.
    desenharPainel(e);                         // Chama sub-rotina para dados de texto laterais.
    fflush(stdout);                            // Força a descarga imediata do buffer de saída da biblioteca C para o terminal atualizar sem lag.
}

void desenharGameOver(EstadoJogo *e) {         // [Puxa de: tetris.h] Função chamada só quando o usuário perde e o tabuleiro transborda.
    int cx = COLS+2;                           // [Puxa de: tetris.h] Define o meio aproximado da tela somando a constante COLS.
    moverCursor(9,cx-4);  printf("+-----------------+"); // Teto do Pop-up flutuante.
    moverCursor(10,cx-4); printf("|   GAME  OVER    |"); // Corpo superior do Pop-up.
    moverCursor(11,cx-4); printf("|  Pts: %7d   |", e->pontuacao); // Corpo central imprimindo a pontuação usando máscara numérica de 7 dígitos de pad.
    moverCursor(12,cx-4); printf("+-----------------+"); // Piso do Pop-up.
    moverCursor(14,cx-4); printf("Pressione ENTER");     // Instrução para travamento da GUI.
    fflush(stdout);                            // Manda empurrar pra tela imediatamente.
}

void desenharMenu(void) {                      // Função isolada do loop que apresenta a tela de Início (Main Menu).
    limparTela();                              // Remove sujeiras anteriores do terminal do usuário.
    printf("\n\n");                            // Margem de respiro no topo.
    printf("  +==========================+\n"); // Borda de cima do menu.
    printf("  |        TETRIS            |\n"); // Header principal.
    printf("  |  Tecnicas de Prog. I     |\n"); // Header secundário (disciplina/projeto).
    printf("  +==========================+\n"); // Divisória de opções.
    printf("  |  [1] Novo Jogo           |\n"); // Mostra atalho Iniciar.
    printf("  |  [2] Ver Recordes        |\n"); // Mostra atalho de Scores.
    printf("  |  [3] Sair                |\n"); // Mostra atalho Encerrar.
    printf("  +==========================+\n"); // Borda de baixo do menu.
    printf("\n  Escolha: ");                   // Prompt para aguardar a decisão (1, 2 ou 3).
    fflush(stdout);                            // Impede que o cursor pisque antes das opções serem totalmente escritas.
}

void desenharRanking(void) {                   // Função responsável por extrair e ranquear os recordes guardados no arquivo local .txt.
    FILE *arq;                                 // Declara uma struct nativa de ponteiro de manipulação de arquivo do C.
    
    typedef struct {                           // Cria uma tipagem de dado temporária existindo só neste escopo funcional.
        char nome[50];                         // String de C para até 49 chars pra segurar o nickname lido.
        int pontos;                            // Inteiro bruto para a pontuação puxada do txt.
    } Recorde;                                 // Nome dado a essa estrutura criada.
    
    Recorde recs[100];                         // Instancia no array "recs" um limite de absorção em lote para até 100 jogadores lidos do txt.
    int total = 0;                             // Variável de controle real de quantos objetos válidos foram instanciados pelo arquivo.

    limparTela();                              // Limpa a tela que tinha o Menu Inicial.
    printf("\n  === RECORDES ===\n\n");        // Imprime título da Leaderboard.
    
    arq = fopen("ranking.txt", "r");           // Realiza a abertura do arquivo TXT somente para leitura ("r" de read).
    if (!arq) {                                // Segurança: Se o arquivo estiver ausente ou dar fail em permissões (arq será NULL).
        printf("  Nenhum recorde ainda.\n");   // Pula todo o processamento e avisa que o arquivo de BD está limpo.
    } else {
        while (total < 100 && fscanf(arq, "%49s %d", recs[total].nome, &recs[total].pontos) == 2) { // Tenta ler linhas que tenham padrão String e Int do TXT num while de teto 100 itens. Retorna 2 quando é sucessivo em ler duas variáveis.
            total++;                           // Soma mais 1 item preenchido nos objetos da memória e avança o loop.
        }
        fclose(arq);                           // Destrói a ponte de acesso com o TXT já que os dados que importam subiram pra matriz da RAM.

        for (int i = 0; i < total - 1; i++) {  // Inicia ordenação pesada (Bubble Sort). Loop lento para garantir que cada casa desça devidamente pro buraco correto.
            for (int j = 0; j < total - i - 1; j++) { // Sub-loop comparando lado a lado.
                if (recs[j].pontos < recs[j+1].pontos) { // Condição do Bubble: A posição atual é menor em score que o cara no índice da frente? Se sim...
                    Recorde temp = recs[j];    // Tira os dados do perdedor e enfia numa variável genérica temporária.
                    recs[j] = recs[j+1];       // Transporta os dados do cara com placar maior pro primeiro slot vazio.
                    recs[j+1] = temp;          // Devolve os dados do perdedor que tavam em backup e rebaixa ele um degrau no array "recs".
                }
            }
        }

        int limite = (total < 10) ? total : 10; // C - Ternary Operator. Checa: Total extraído é menor que 10? Se sim, a var limite vira o total. Se for falso (há centenas de registros), a var limite trava cravada em 10.
        for (int i = 0; i < limite; i++) {      // Percorre os dados formatados usando o "limite" de teto (Top 10 max).
            printf("  %2d. %-15s %d\n", i + 1, recs[i].nome, recs[i].pontos); // Imprime o rank numérico compensado (i+1 para não existir ranking 0), o Nome forçado alinhar -15 slots esquerdos e os inteiros de pontuação.
        }
    }
    
    printf("\n  ENTER para voltar...");        // Imprime mensagem para retornar ao menu.
    fflush(stdout);                            // Descarrega saídas pendentes da string acima para a GUI.
    while (getchar() != '\n');                 // Laço infinito travado segurando a tela que captura botões descartando no vento, até ele achar um 'ENTER' ('\n').
}