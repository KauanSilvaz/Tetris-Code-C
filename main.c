// main.c - versão Windows                                   // Comentário original identificando o arquivo principal e a plataforma (Windows).
#include <stdio.h>                                           // Inclui a biblioteca padrão de entrada e saída (usada para printf, scanf, manipulação de arquivos).
#include <stdlib.h>                                          // Inclui a biblioteca padrão do C.
#include <windows.h>    // Sleep(), GetTickCount()           // Inclui a API do Windows, necessária para as funções de gerenciamento de tempo (Sleep e GetTickCount).
#include "tetris.h"                                          // Inclui o cabeçalho com a lógica principal, definições e estruturas do Tetris.
#include "interface.h"                                       // Inclui o cabeçalho com os protótipos das funções que desenham a interface visual.

static void salvarRanking(int pontuacao) {                   // Define a função interna 'salvarRanking' que recebe a pontuação final do jogador para salvar no disco.
    FILE *arq;                                               // Declara um ponteiro de arquivo chamado 'arq' para manipular o arquivo de texto "ranking.txt".
    char nome[50];                                           // Declara um vetor de caracteres (string) com espaço para até 49 letras (+1 nulo) para armazenar o nome do jogador.
    restaurarTerminal();                                     // Chama a função que devolve o terminal ao seu estado normal de exibição.
    mostrarCursor();                                         // Chama a função que torna o cursor piscante visível novamente para que o usuário saiba onde está digitando.
    printf("\nSeu nome: ");                                  // Imprime na tela o texto pedindo o nome do jogador, pulando uma linha antes.
    fflush(stdout);                                          // Força a exibição imediata do texto impresso no buffer do terminal.
    scanf("%49s", nome);                                     // Lê a entrada do teclado limitando a 49 caracteres (para evitar erro de memória) e guarda na variável 'nome'.
    while (getchar() != '\n');                               // Limpa o buffer do teclado, consumindo caracteres extras digitados até encontrar uma quebra de linha (ENTER).
    arq = fopen("ranking.txt", "a");                         // Abre o arquivo "ranking.txt" em modo "a" (append/anexar). Se não existir, é criado; se existir, o conteúdo novo vai pro final.
    if (arq) {                                               // Verifica se o arquivo foi aberto com sucesso (se o ponteiro 'arq' não é Nulo).
        fprintf(arq, "%s %d\n", nome, pontuacao);            // Escreve no arquivo o nome digitado seguido por um espaço, a pontuação obtida e pula uma linha.
        fclose(arq);                                         // Fecha o arquivo de texto, salvando as alterações permanentemente no disco.
    }                                                        // Fim do bloco condicional if.
    printf("Salvo! ENTER...");                               // Imprime na tela a mensagem de confirmação de que o recorde foi gravado.
    while (getchar() != '\n');                               // Trava a execução e espera o jogador pressionar ENTER para continuar.
}                                                            // Fim da função 'salvarRanking'.

static void loopJogo(void) {                                 // Define a função interna 'loopJogo', que é o laço de repetição principal (coração) de uma partida.
    EstadoJogo e;                                            // Declara uma variável 'e' do tipo 'EstadoJogo' (definido em tetris.h) para armazenar todos os dados da partida.
    DWORD agora, ultimo;    // GetTickCount() retorna milissegundos // Declara variáveis do tipo DWORD (inteiro longo sem sinal da API do Windows) para armazenar os tempos atuais e anteriores.
    int intervalo, tecla;                                    // Declara variáveis inteiras para controlar a velocidade de queda ('intervalo') e armazenar a tecla pressionada ('tecla').
    long ms;                                                 // Declara uma variável longa para calcular os milissegundos passados entre cada atualização de tela.

    inicializarJogo(&e);                                     // Chama a função (de tetris.c) que aloca memória e prepara a matriz e as peças iniciais, passando a referência de 'e'.
    configurarTerminal();                                    // Prepara o terminal do Windows para exibir o jogo corretamente (habilitando cores ANSI).
    ocultarCursor();                                         // Esconde o cursor piscante do console para não atrapalhar os gráficos do jogo.
    limparTela();                                            // Limpa completamente qualquer texto anterior que estivesse no terminal.

    ultimo = GetTickCount();                                 // Registra o tempo exato (em milissegundos desde que o Windows iniciou) em que o jogo começou, salvando em 'ultimo'.

    while (!e.gameOver) {                                    // Inicia o loop principal do jogo, que continua rodando repetidamente enquanto a flag 'gameOver' for falsa (0).
        agora = GetTickCount();                              // Captura o tempo atual exato neste ciclo do loop.
        ms = (long)(agora - ultimo);                         // Calcula a diferença de tempo (delta) em milissegundos desde a última vez que a peça caiu sozinha.

        intervalo = 600 - (e.nivel-1)*50;                    // Calcula o tempo que a peça demora para cair. Começa em 600ms e fica 50ms mais rápido a cada nível.
        if (intervalo < 100) intervalo = 100;                // Impede que o jogo fique instantâneo e impossível, limitando a velocidade máxima a 1 queda a cada 100 milissegundos.

        tecla = lerTeclaNaoBloqueante();                     // Verifica se o jogador apertou alguma tecla no momento, sem travar o jogo, e salva o caractere em 'tecla'.
        switch (tecla) {                                     // Inicia uma estrutura condicional para checar qual tecla específica foi pressionada.
            case 'a': moverPeca(&e, -1);  break;             // Se a tecla foi 'a', chama a função para mover a peça 1 posição para a esquerda (-1).
            case 'd': moverPeca(&e,  1);  break;             // Se a tecla foi 'd', chama a função para mover a peça 1 posição para a direita (+1).
            case 'w': rotacionarPeca(&e); break;             // Se a tecla foi 'w', chama a função que tenta rotacionar a peça em 90 graus.
            case 's': descerPeca(&e);     break;             // Se a tecla foi 's', acelera a descida movendo a peça 1 posição para baixo manualmente.
            case ' ': dropPeca(&e);       break;             // Se a tecla foi 'ESPAÇO', executa um "hard drop", derrubando a peça instantaneamente até o fundo.
            case 'q': e.gameOver = 1;     break;             // Se a tecla foi 'q', sinaliza ao loop que o jogo acabou (força a saída).
        }                                                    // Fim do bloco condicional switch.

        if (ms >= intervalo) {                               // Verifica se o tempo passado ('ms') já alcançou ou passou o tempo calculado para a gravidade agir ('intervalo').
            descerPeca(&e);                                  // A gravidade age: empurra a peça uma linha para baixo automaticamente.
            ultimo = agora;                                  // Atualiza a marcação de tempo base ('ultimo') para iniciar a contagem da próxima queda.
        }                                                    // Fim do bloco if que controla a gravidade.

        desenharJogo(&e);                                    // Atualiza a interface gráfica do console redesenhando o tabuleiro, as peças e o placar.
        Sleep(10);          // 10ms — equivalente ao nanosleep do Linux // Interrompe o processamento por 10 milissegundos para poupar o processador e não queimar 100% de CPU.
    }                                                        // Fim do loop principal da partida (só chega aqui e repete se não deu game over).

    desenharJogo(&e);                                        // Redesenha o último estado do jogo para o jogador ver a peça exata que causou a sua derrota.
    desenharGameOver(&e);                                    // Desenha a caixa de texto "GAME OVER" flutuando por cima do tabuleiro.
    restaurarTerminal();                                     // Restaura as configurações originais do terminal do Windows.
    mostrarCursor();                                         // Torna o cursor de texto visível novamente para as próximas telas.
    while (getchar() != '\n');                               // Limpa o buffer caso o usuário tenha digitado coisas a mais, esperando ele apertar ENTER para prosseguir.
    salvarRanking(e.pontuacao);                              // Chama o processo de registrar o nome do jogador e os pontos obtidos na partida.
    liberarJogo(&e);                                         // Chama a função para liberar toda a memória (matriz do tabuleiro) que havia sido alocada no início.
}                                                            // Fim da função 'loopJogo'.

int main(void) {                                             // Define a função 'main', que é o ponto de entrada principal e início de execução do programa no C.
    char op;                                                 // Declara uma variável caractere 'op' para armazenar a opção que o usuário escolherá no menu.
    do {                                                     // Inicia um laço de repetição "faça-enquanto" (do-while) para o menu, garantindo que rode pelo menos a primeira vez.
        desenharMenu();                                      // Chama a função (de interface.c) que desenha o menu visual principal do jogo.
        op = getchar();                                      // Lê um único caractere que o usuário digitou no terminal e o armazena em 'op'.
        while (getchar() != '\n');                           // Limpa o buffer do teclado (qualquer lixo digitado após a primeira letra) até o ENTER, evitando leitura fantasma.
        if (op == '1') loopJogo();                           // Se a opção digitada for '1', chama a função 'loopJogo()' que inicia a partida e fica lá até o fim.
        if (op == '2') desenharRanking();                    // Se a opção digitada for '2', chama a função que mostra as pontuações gravadas na tela.
    } while (op != '3');                                     // Avalia a condição do laço: se a opção não for '3', repete o menu novamente. Se for '3', sai do laço.
    limparTela();                                            // Limpa o terminal para que a tela final fique preta.
    printf("Ate logo!\n");                                   // Imprime uma mensagem de despedida após o encerramento do jogo.
    return 0;                                                // Retorna o valor 0 ao sistema operacional, código universal que indica que o programa terminou sem erros.
}                                                            // Fim do arquivo e da função principal 'main'.