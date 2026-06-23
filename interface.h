#ifndef INTERFACE_H                      // Include Guard: Verifica se 'INTERFACE_H' ainda não foi definido. Impede que este arquivo seja lido duas vezes pelo compilador.
#define INTERFACE_H                      // Define a macro 'INTERFACE_H', ativando a proteção iniciada na linha acima.

#include "tetris.h"                      // [Puxa de: tetris.h] Importação vital. Traz as structs do jogo (como 'EstadoJogo') para que as funções abaixo saibam o que elas são.

void limparTela(void);                   // Assinatura da função que limpa o texto do terminal.
void moverCursor(int linha, int coluna); // Assinatura da função que altera a posição X e Y do cursor no console.
void ocultarCursor(void);                // Assinatura da função que esconde o cursor piscante de digitação para não poluir o jogo.
void mostrarCursor(void);                // Assinatura da função que devolve a visibilidade ao cursor piscante.
void configurarTerminal(void);           // Assinatura da função que habilita o suporte a cores ANSI no Windows.
void restaurarTerminal(void);            // Assinatura da função de limpeza que devolve o terminal ao estado normal antes do jogo fechar.
int  lerTeclaNaoBloqueante(void);        // Assinatura da função que captura inputs do teclado (w, a, s, d) em tempo real sem pausar o jogo.

void desenharJogo(EstadoJogo *e);        // [Puxa de: tetris.h] Assinatura da função que desenha a tela da partida. Exige um ponteiro da struct 'EstadoJogo' para ler matriz e placar.
void desenharGameOver(EstadoJogo *e);    // [Puxa de: tetris.h] Assinatura da função que exibe a pontuação final na tela de derrota. Também exige o 'EstadoJogo'.

void desenharMenu(void);                 // Assinatura da função que imprime o Menu Principal (Novo Jogo, Recordes, Sair).
void desenharRanking(void);              // Assinatura da função que lê e exibe o Top 10 salvo no txt.

#endif                                   // Fecha o bloco do Include Guard iniciado na primeira linha. Tudo que está entre o #ifndef e aqui pertence a este cabeçalho.