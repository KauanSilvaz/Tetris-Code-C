// interface.h                           // Comentário original identificando o nome do arquivo de cabeçalho (header file).
#ifndef INTERFACE_H                      // Diretiva de pré-processador ("If Not Defined"). Verifica se INTERFACE_H ainda não foi definido, evitando que o arquivo seja incluído várias vezes durante a compilação.
#define INTERFACE_H                      // Define o macro INTERFACE_H. Ao lado do #ifndef, isso cria um "Include Guard", protegendo o código de inclusões duplicadas.

#include "tetris.h"                      // Inclui o cabeçalho "tetris.h" para que este arquivo conheça as estruturas de dados necessárias, como o tipo 'EstadoJogo'.

void limparTela(void);                   // Protótipo da função que limpa o texto da tela do terminal. Não recebe argumentos e não retorna valor.
void moverCursor(int linha, int coluna); // Protótipo da função que posiciona o cursor em coordenadas X (coluna) e Y (linha) específicas na tela.
void ocultarCursor(void);                // Protótipo da função que esconde o cursor piscante do terminal para manter o visual do jogo mais limpo.
void mostrarCursor(void);                // Protótipo da função que volta a exibir o cursor piscante no terminal.
void configurarTerminal(void);           // Protótipo da função que inicializa o console (especialmente no Windows) para suportar códigos de escape de cores ANSI.
void restaurarTerminal(void);            // Protótipo da função que devolve o terminal ao seu estado padrão quando o jogo é encerrado.
int  lerTeclaNaoBloqueante(void);        // Protótipo da função que captura a entrada do teclado sem interromper/pausar a execução do laço do jogo. Retorna um inteiro (o código da tecla).
void desenharJogo(EstadoJogo *e);        // Protótipo da função principal de renderização da partida. Recebe um ponteiro para a estrutura que guarda o estado do jogo.
void desenharGameOver(EstadoJogo *e);    // Protótipo da função que exibe a caixa de texto de Fim de Jogo e a pontuação alcançada.
void desenharMenu(void);                 // Protótipo da função responsável por mostrar a tela inicial com as opções de jogar, recordes e sair.
void desenharRanking(void);              // Protótipo da função que faz a leitura do arquivo de recordes e imprime a tabela de pontuações na tela.

#endif                                   // Fecha a diretiva condicional iniciada pelo #ifndef lá no topo. Marca o fim do conteúdo do arquivo de cabeçalho.