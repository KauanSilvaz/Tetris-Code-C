// main.c - Ponto de partida do jogo compilado para Windows.
#include <stdio.h>       // Inclui a biblioteca padrão de entrada e saída (printf, scanf, manipulação de arquivos).
#include <stdlib.h>      // Inclui a biblioteca padrão do C.
#include <windows.h>     // Inclui a API do Windows para ter acesso às funções de tempo (Sleep, GetTickCount).
#include "tetris.h"      // [Puxa de: tetris.h] Permite que o main conheça a lógica do jogo, as peças e a struct EstadoJogo.
#include "interface.h"   // [Puxa de: interface.h] Permite que o main possa chamar as funções que desenham na tela.

static void salvarRanking(int pontuacao) {       // Função interna para salvar o score do jogador no disco.
    FILE *arq;                                   // Cria um ponteiro de arquivo para manipular o TXT.
    char nome[50];                               // Cria um vetor de caracteres para armazenar o nome digitado.
    
    restaurarTerminal();                         // [Puxa de: interface.c] Devolve as configurações normais do terminal.
    mostrarCursor();                             // [Puxa de: interface.c] Volta a exibir o cursor para o jogador ver onde está digitando.
    
    printf("\nSeu nome: ");                      // Pede o nome do jogador na tela.
    fflush(stdout);                              // Força a exibição imediata da string acima no terminal.
    scanf("%49s", nome);                         // Lê o que foi digitado (limitando a 49 caracteres para evitar overflow) e salva na variável 'nome'.
    while (getchar() != '\n');                   // Limpa o buffer de entrada jogando fora qualquer sujeira até encontrar o "ENTER".
    
    arq = fopen("ranking.txt", "a");             // Abre ou cria o arquivo "ranking.txt" em modo "Append" (adicionar ao final do arquivo).
    if (arq) {                                   // Se o arquivo foi aberto com sucesso (ponteiro válido):
        fprintf(arq, "%s %d\n", nome, pontuacao);// Grava a string com o nome, um espaço, os pontos e quebra a linha no TXT.
        fclose(arq);                             // Fecha o arquivo e libera ele para o sistema operacional.
    }
    printf("Salvo! ENTER...");                   // Confirmação visual para o usuário.
    while (getchar() != '\n');                   // Congela a tela esperando o usuário apertar ENTER para voltar ao menu.
}

static void loopJogo(void) {                     // O "coração" do jogo: o laço que roda a partida do começo ao fim.
    EstadoJogo e;                                // [Puxa de: tetris.h] Declara a variável mestre 'e' que guarda todo o status da partida atual.
    DWORD agora, ultimo;                         // Variáveis do Windows para guardar os milissegundos do relógio do sistema.
    int intervalo, tecla;                        // Variáveis para a matemática da velocidade da peça e para capturar botões.
    long ms;                                     // Variável para guardar o delta de tempo (diferença entre 'agora' e 'ultimo').

    inicializarJogo(&e);                         // [Puxa de: tetris.c] Monta o tabuleiro na memória RAM e sorteia a 1ª peça.
    configurarTerminal();                        // [Puxa de: interface.c] Ativa as cores do Windows.
    ocultarCursor();                             // [Puxa de: interface.c] Esconde o cursor piscante.
    limparTela();                                // [Puxa de: interface.c] Limpa o console para iniciar a renderização limpa.

    ultimo = GetTickCount();                     // Salva o momento exato em que a partida começou (em milissegundos).

    while (!e.gameOver) {                        // Loop principal: repete enquanto a flag gameOver dentro do EstadoJogo for 0.
        agora = GetTickCount();                  // Pega a hora exata deste frame.
        ms = (long)(agora - ultimo);             // Calcula quantos milissegundos se passaram desde que a última peça caiu um bloco.

        intervalo = 600 - (e.nivel-1)*50;        // Calcula o atraso da gravidade. Nível 1 = 600ms. Cada nível diminui 50ms.
        if (intervalo < 100) intervalo = 100;    // Trava a velocidade máxima em 100ms por bloco para não ficar impossível.

        tecla = lerTeclaNaoBloqueante();         // [Puxa de: interface.c] Captura o teclado invisivelmente sem travar o loop.
        switch (tecla) {                         // Analisa qual tecla foi pressionada:
            case 'a': moverPeca(&e, -1);  break; // [Puxa de: tetris.c] Move 1 bloco para a esquerda.
            case 'd': moverPeca(&e,  1);  break; // [Puxa de: tetris.c] Move 1 bloco para a direita.
            case 'w': rotacionarPeca(&e); break; // [Puxa de: tetris.c] Gira a peça em 90 graus.
            case 's': descerPeca(&e);     break; // [Puxa de: tetris.c] Soft Drop: puxa a peça pra baixo pontualmente.
            case ' ': dropPeca(&e);       break; // [Puxa de: tetris.c] Hard Drop: joga a peça pro fundo de uma vez.
            case 'q': e.gameOver = 1;     break; // Quit: Força o fim do loop alterando a variável gameOver para 1 (Verdadeiro).
        }

        if (ms >= intervalo) {                   // Se o tempo acumulado for maior que o delay do nível atual:
            descerPeca(&e);                      // [Puxa de: tetris.c] A gravidade natural age descendo a peça 1 Y.
            ultimo = agora;                      // Reseta o cronômetro para a próxima queda automática.
        }

        desenharJogo(&e);                        // [Puxa de: interface.c] Manda desenhar a matriz e as cores atualizadas na tela.
        Sleep(10);                               // Pausa o processador por 10ms (100 FPS limit) para não fritar a CPU rodando o loop loucamente.
    }

    desenharJogo(&e);                            // [Puxa de: interface.c] Atualiza a tela uma última vez para mostrar o momento exato em que perdeu.
    desenharGameOver(&e);                        // [Puxa de: interface.c] Desenha o painel pop-up avisando que perdeu.
    restaurarTerminal();                         // [Puxa de: interface.c]
    mostrarCursor();                             // [Puxa de: interface.c]
    while (getchar() != '\n');                   // Limpa o buffer até o ENTER caso o usuário tenha esbarrado no teclado morrendo.
    
    salvarRanking(e.pontuacao);                  // [Função interna] Chama a rotina para pedir o nome e gravar no arquivo de recordes.
    liberarJogo(&e);                             // [Puxa de: tetris.c] Dá free() no tabuleiro dinâmico e devolve a memória pro Windows.
}

int main(void) {                                 // Ponto de entrada do executável.
    char op;                                     // Variável para a escolha do menu.
    do {                                         // Loop infinito do menu:
        desenharMenu();                          // [Puxa de: interface.c] Mostra as opções (1- Jogar, 2- Ranking, 3- Sair).
        op = getchar();                          // Lê a decisão do usuário.
        while (getchar() != '\n');               // Limpa o buffer jogando o ENTER (ou letras a mais) no lixo.
        
        if (op == '1') loopJogo();               // [Função interna] Inicia a partida. O programa fica "preso" no loopJogo até morrer.
        if (op == '2') desenharRanking();        // [Puxa de: interface.c] Lê o TXT e renderiza a tela de melhores pontuações.
    } while (op != '3');                         // Se escolheu 3, quebra o loop e o programa se prepara para fechar.
    
    limparTela();                                // [Puxa de: interface.c] Zera o terminal para o usuário não ficar com "lixo" gráfico na tela após sair.
    printf("Ate logo!\n");                       // Dá tchau.
    return 0;                                    // Devolve código de sucesso (0) indicando que rodou liso sem crash.
}