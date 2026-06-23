// tetris.c                                                   // Comentário original identificando o arquivo.
// Toda a lógica: peças, colisão, rotação, pontuação          // Comentário original listando as responsabilidades deste arquivo.

#define _POSIX_C_SOURCE 200809L                               // Define um macro para o pré-processador habilitando funções e comportamentos da norma POSIX (útil para portabilidade).
#include <stdio.h>                                            // Biblioteca padrão de entrada e saída.
#include <stdlib.h>                                           // Biblioteca padrão (necessária para malloc, calloc, free, rand e srand).
#include <string.h>                                           // Biblioteca para manipulação de blocos de memória (memcpy, memset).
#include <time.h>                                             // Biblioteca de manipulação de tempo (necessária para a semente de geração aleatória).
#include "tetris.h"                                           // Inclui o cabeçalho que define as estruturas (Peca, EstadoJogo) e protótipos deste jogo.

// Formato visual dos 7 Tetrominós em grade 4x4               // Comentário original.
// 1 = bloco presente, 0 = vazio                              // Comentário original.
static const int FORMAS[NUM_PECAS][4][4] = {                  // Declaração de um array estático 3D constante (7 peças x 4 linhas x 4 colunas) guardando a "geometria" de cada bloco.
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // I           // Matriz 4x4 representando a peça 'I' (reta horizontal de 4 blocos).
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // J           // Matriz 4x4 representando a peça 'J' (um bloco isolado acima, três embaixo).
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // L           // Matriz 4x4 representando a peça 'L'.
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // O           // Matriz 4x4 representando a peça 'O' (o quadrado 2x2).
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // S           // Matriz 4x4 representando a peça 'S'.
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // T           // Matriz 4x4 representando a peça 'T' (formato de triângulo/T curto).
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // Z           // Matriz 4x4 representando a peça 'Z'.
};                                                            // Fim da inicialização do array das formas.

// Cor ANSI de cada peça (31=vermelho, 32=verde, 33=amarelo...) // Comentário original.
static const int CORES[NUM_PECAS] = {36, 34, 33, 33, 32, 35, 31}; // Array constante vinculando um código de cor do terminal (ANSI) para cada tipo de peça.

// Cria uma peça aleatória no topo central do tabuleiro       // Comentário original.
Peca gerarPeca(void) {                                        // Função que cria e retorna uma nova estrutura do tipo 'Peca'.
    Peca p;                                                   // Declara uma variável local 'p' da estrutura Peca.
    p.tipo = (TipoPeca)(rand() % NUM_PECAS);                  // Gera um número aleatório, pega o resto da divisão por 7 (NUM_PECAS) para dar de 0 a 6, converte para enum e define o tipo.
    p.cor  = CORES[p.tipo];                                   // Define a cor da peça puxando o código correto do array constante de CORES usando o tipo sorteado.
    memcpy(p.matriz, FORMAS[p.tipo], sizeof(p.matriz));       // Copia todos os 16 inteiros da matriz de formato padrão para a matriz interna desta nova peça instanciada.
    p.x = COLS / 2 - 2;  // centraliza horizontalmente        // Posição X (coluna): pega o centro do tabuleiro (largura/2) e subtrai 2 para que a matriz 4x4 fique alinhada no meio.
    p.y = 0;              // começa no topo                   // Posição Y (linha): Inicia no índice 0, que é a linha mais alta do cenário.
    return p;                                                 // Retorna a nova peça montada.
}                                                             // Fim da função 'gerarPeca'.

// Aloca o tabuleiro com malloc e gera as primeiras peças     // Comentário original.
void inicializarJogo(EstadoJogo *e) {                         // Função que recebe o ponteiro do estado do jogo e o prepara para uma partida do zero.
    int i;                                                    // Declara um iterador 'i'.
    srand((unsigned int)time(NULL));  // semente para rand()  // Alimenta o algoritmo gerador de números aleatórios com a hora/data exata do sistema para não repetir a sequência de peças.

    // malloc: aloca array de ponteiros (uma linha = um int*) // Comentário original.
    e->tabuleiro = (int **)malloc(ROWS * sizeof(int *));      // Aloca a espinha dorsal do tabuleiro: um vetor onde cada posição é um ponteiro para uma linha de blocos.
    for (i = 0; i < ROWS; i++)                                // Inicia um laço que percorrerá todas as linhas (altura do jogo).
        e->tabuleiro[i] = (int *)calloc(COLS, sizeof(int)); // calloc = malloc + zera // Aloca memória para as colunas de cada linha e já preenche tudo com zero (poço vazio).

    e->pontuacao = 0;                                         // Zera os pontos iniciais do jogador.
    e->nivel     = 1;                                         // Define o nível do jogo iniciando no nível 1.
    e->linhas    = 0;                                         // Zera a contagem de linhas quebradas.
    e->gameOver  = 0;                                         // Define a flag gameOver como 0 (o jogo não acabou).
    e->pecaAtual = gerarPeca();                               // Sorteia a primeira peça que vai cair no tabuleiro.
    e->proxima   = gerarPeca();                               // Sorteia a peça que vai ficar guardada no display de "Próxima Peça".
}                                                             // Fim da função 'inicializarJogo'.

// Libera a memória que foi alocada com malloc                // Comentário original.
void liberarJogo(EstadoJogo *e) {                             // Função que limpa a memória da RAM após o jogo terminar (evita "Memory Leak").
    int i;                                                    // Iterador.
    for (i = 0; i < ROWS; i++)                                // Laço percorrendo todas as linhas da matriz.
        free(e->tabuleiro[i]);                                // Libera a memória ocupada pelas colunas daquela linha específica.
    free(e->tabuleiro);                                       // Por fim, libera o vetor principal de ponteiros de linhas.
}                                                             // Fim da função 'liberarJogo'.

// Verifica se a peça está colidindo com parede ou outra peça // Comentário original.
// Recebe int** (ponteiro para ponteiro) — uso de ponteiros conforme exigido // Comentário original.
int verificarColisao(int **tabuleiro, Peca *p) {              // Função que checa sobreposição. Retorna 1 se bater e 0 se caminho estiver livre.
    int l, c;                                                 // Iteradores de linha e coluna da matriz local 4x4 da peça.
    for (l = 0; l < 4; l++) {                                 // Laço iterando sobre as 4 linhas do "quadrado" da peça.
        for (c = 0; c < 4; c++) {                             // Laço iterando sobre as 4 colunas do "quadrado" da peça.
            if (!p->matriz[l][c]) continue;  // célula vazia, pula // Se a parte da matriz da peça for 0, não há bloco sólido nela. Pula o resto e vai pra próxima célula.
            int tx = p->x + c;                                // tx: Calcula a coordenada X global no tabuleiro somando a posição X geral da peça com a coluna local da matriz 4x4.
            int ty = p->y + l;                                // ty: Calcula a coordenada Y global no tabuleiro somando a posição Y geral da peça com a linha local da matriz 4x4.
            if (tx < 0 || tx >= COLS) return 1;  // saiu pela lateral // Verifica se bateu nas paredes: X negativo (passou da esquerda) ou X maior que largura (passou da direita). Retorna 1 (colisão).
            if (ty >= ROWS)           return 1;  // saiu pelo fundo // Verifica se bateu no chão: Y passou da profundidade máxima do poço. Retorna 1 (colisão).
            if (ty < 0)               continue;  // ainda acima do topo // Se a peça estiver nascendo e parte dela ainda estiver em "coordenada negativa" acima da tela, ignora choque ali.
            if (tabuleiro[ty][tx])    return 1;  // bateu em outra peça // Olha dentro da matriz principal do tabuleiro. Se tiver algo diferente de 0 ali, chocou com bloco morto. Retorna 1.
        }                                                     // Fim do laço interno de colunas.
    }                                                         // Fim do laço externo de linhas.
    return 0;                                                 // Se analisou as 16 células e não encostou em nada, o movimento é seguro. Retorna 0.
}                                                             // Fim da função 'verificarColisao'.

// Grava a peça no tabuleiro (quando ela não pode mais descer)// Comentário original.
void fixarPeca(int **tabuleiro, Peca *p) {                    // Pinta os blocos de uma peça na matriz física do tabuleiro quando ela encosta em algo.
    int l, c;                                                 // Iteradores 4x4.
    for (l = 0; l < 4; l++)                                   // Laço das linhas da peça.
        for (c = 0; c < 4; c++)                               // Laço das colunas da peça.
            if (p->matriz[l][c]) {                            // Onde a peça atual for sólida (valor 1):
                int ty = p->y + l, tx = p->x + c;             // Descobre o ponto global X e Y exato do poço no qual ela deve ser decalcada.
                if (ty >= 0) tabuleiro[ty][tx] = p->cor;      // Evitando travar caso um bloco fixe na "linha fantasma" (-1) acima da tela. Pinta na matriz com o código da cor (que é > 0).
            }                                                 // Fim do if de fixação de bloco unitário.
}                                                             // Fim da função 'fixarPeca'.

// Apaga as linhas completas e empurra o resto para baixo     // Comentário original.
// Retorna quantas linhas foram eliminadas                    // Comentário original.
int eliminarLinhas(int **tabuleiro) {                         // Varre a matriz, destrói as linhas lotadas de blocos e aplica a gravidade nos blocos flutuantes.
    int l, c, i, eliminadas = 0;                              // Iteradores 'l' e 'c' de matriz, iterador 'i' auxiliar de "gravidade", e acumulador de linhas destruídas.
    for (l = ROWS - 1; l >= 0; l--) {                         // Loop reverso: do chão (ROWS-1) até o topo (0), linha por linha.
        int completa = 1;                                     // Variável "flag". Assume (verdadeiro/1) que a linha analisada está totalmente preenchida.
        for (c = 0; c < COLS; c++)                            // Varre todas as colunas (da esquerda para direita) desta linha 'l'.
            if (!tabuleiro[l][c]) { completa = 0; break; }    // Se achar qualquer "buraco" (valor 0), marca a flag como falso (0) e para de olhar esta linha imediatamente (break).
        if (completa) {                                       // Chegou ao fim das colunas. Se a flag 'completa' continuar 1, temos um Tetris!
            eliminadas++;                                     // Soma +1 no placar de linhas queimadas nesta jogada.
            for (i = l; i > 0; i--)                           // Loop de Gravidade: Partindo da linha eliminada subindo de baixo pra cima até encostar no teto.
                memcpy(tabuleiro[i], tabuleiro[i-1], COLS * sizeof(int)); // Copia toda a linha física de cima, por cima da linha atual de baixo (puxa os blocos pendurados).
            memset(tabuleiro[0], 0, COLS * sizeof(int));      // Como as linhas desceram, preenche a linha 0 (o "teto") inteira com zeros, esvaziando o topo.
            l++;  // reavalia a linha (que agora tem conteúdo novo) // A linha atual 'l' recebeu dados novos da linha de cima. Aumenta 'l' para o laço não pular esta linha e testá-la na próxima iteração.
        }                                                     // Fim da queima de uma linha.
    }                                                         // Fim da varredura geral do tabuleiro.
    return eliminadas;                                        // Devolve ao sistema o total de linhas apagadas de uma só vez (1, 2, 3 ou 4).
}                                                             // Fim da função 'eliminarLinhas'.

// Soma pontos e sobe o nível a cada 10 linhas                // Comentário original.
void atualizarPontuacao(EstadoJogo *e, int n) {               // Recebe o estado atual e 'n' linhas destruídas para atualizar a GUI e dificuldade.
    int tabela[] = {0, 100, 300, 500, 800};                   // Tabela de base: quebrar 1 linha = 100pts; 2 linhas = 300pts; 3 linhas = 500pts; 4 linhas ("Tetris") = 800pts.
    if (n >= 1 && n <= 4) e->pontuacao += tabela[n] * e->nivel; // Checagem de segurança; multiplica os pontos base pelo nível para dar recompensas maiores a jogadores avançados.
    e->linhas += n;                                           // Soma o número do combo de linhas eliminadas no total de linhas geral da partida.
    e->nivel = (e->linhas / 10) + 1;                          // Fórmula do nível: A cada 10 linhas totais o jogador sobe 1 de nível. (ex: 25 linhas = nível 3).
}                                                             // Fim da função 'atualizarPontuacao'.

// Desce a peça 1 linha. Se colidir, fixa e gera nova peça.   // Comentário original.
int descerPeca(EstadoJogo *e) {                               // Executa o "tick" gravitacional do jogo ou da tecla 'baixo'. Retorna 1 se desceu com sucesso ou 0 se ela encostou de vez no chão.
    e->pecaAtual.y++;                                         // Artificialmente movimenta a peça para baixo (aumenta o Y no eixo).
    if (verificarColisao(e->tabuleiro, &e->pecaAtual)) {      // Submete a nova posição ao teste de física. Bateu em algo?
        e->pecaAtual.y--;                                     // A colisão ocorreu, então desfaz o movimento artificial da peça 1 Y para cima (posição válida).
        fixarPeca(e->tabuleiro, &e->pecaAtual);               // Pinta os bloquinhos 1 por 1 dela de forma permanente no tabuleiro de fundo.
        int n = eliminarLinhas(e->tabuleiro);                 // Chama a função da faxina, pegando quantas linhas ela por acaso conseguiu destruir.
        atualizarPontuacao(e, n);                             // Soma os pontos das linhas que ela destruiu (ou soma 0 caso não destrua nenhuma).
        e->pecaAtual = e->proxima;                            // A peça da fila de "Próxima" se materializa e vira a peça de controle "Atual" do jogador no topo da tela.
        e->proxima   = gerarPeca();                           // O jogo rola uma roleta e escolhe aleatoriamente a nova peça substituta que ficará esperando como "Próxima".
        if (verificarColisao(e->tabuleiro, &e->pecaAtual))    // Teste fatídico: A peça novinha acabou de nascer no Y=0. Ela já está colidindo no nascimento?
            e->gameOver = 1;                                  // Sim, o poço está lotado até a tampa. Decreta o Game Over (estado=1) que vai parar o loop principal do main().
        return 0;                                             // Retorna 0 para quem a chamou saber que a descida atingiu um obstáculo definitivo.
    }                                                         // Fim da mecânica de colisão vertical.
    return 1;                                                 // Se ela não colidiu durante a queda inicial do if, a descida foi aprovada. Retorna 1.
}                                                             // Fim da função 'descerPeca'.

// Move a peça para esquerda (-1) ou direita (+1)             // Comentário original.
void moverPeca(EstadoJogo *e, int dir) {                      // É chamada por A e D. Altera o X para o jogador ir pro lado.
    e->pecaAtual.x += dir;                                    // Desloca o x virtualmente adicionando 'dir' (-1 ou +1).
    if (verificarColisao(e->tabuleiro, &e->pecaAtual))        // Verifica se essa passada pro lado não está quebrando a lei da física empurrando tijolos ou varando a tela.
        e->pecaAtual.x -= dir;  // desfaz se colidiu          // Se for irregular, desfaz o passo da variável X subitamente para que o usuário sinta a parede.
}                                                             // Fim da função 'moverPeca'.

// Rotaciona a peça 90° girando a matriz 4x4                  // Comentário original.
// Fórmula: nova[col][3-linha] = original[linha][col]         // Comentário original com a lógica matricial.
void rotacionarPeca(EstadoJogo *e) {                          // Função principal acionada pelo W. Gira o bloquinho sentido horário.
    int temp[4][4], orig[4][4];                               // Instancia duas matrizes na RAM em escopo local: temp (onde o giro vai parar) e orig (um backup caso o giro seja bloqueado).
    int l, c;                                                 // Nossos leais iteradores de linhas e colunas.
    memcpy(orig, e->pecaAtual.matriz, sizeof(orig));          // Salva os dados atuais da matriz da peça na 'orig' preventivamente.
    for (l = 0; l < 4; l++)                                   // Começa a percorrer as linhas 4x4.
        for (c = 0; c < 4; c++)                               // Começa a percorrer as colunas 4x4.
            temp[c][3-l] = e->pecaAtual.matriz[l][c];         // Operação geométrica espelhada: Move o dado de [L][C] invertendo eixos L/C para forçar um "tombo" e armazenando no 'temp'.
    memcpy(e->pecaAtual.matriz, temp, sizeof(temp));          // Substitui a estrutura visual da peça ativa pela cópia "virada de lado" da temp.
    // Se colidir depois de rotar, tenta empurrar lateralmente// Comentário original.
    if (verificarColisao(e->tabuleiro, &e->pecaAtual)) {      // Opa! Girou e uma quina entalou na parede ou numa estalactite?
        int kicks[] = {1, -1, 2, -2}, k;                      // Sistema de "Wall Kick" do Tetris: Um array define empurrões forçados em X. (1 dir, 1 esq, 2 dir, 2 esq).
        int ok = 0;                                           // Flag que vai monitorar se conseguimos desenroscar o bloco.
        for (k = 0; k < 4 && !ok; k++) {                      // Tenta as 4 pancadas laterais do array consecutivamente até que 'ok' seja verdade.
            e->pecaAtual.x += kicks[k];                       // Modifica a posição de X com a força atual de empurrão lateral.
            if (!verificarColisao(e->tabuleiro, &e->pecaAtual)) ok = 1; // Testa o estado. Desentalou e a física está respeitada? Excelente, seta a flag OK pra sair do loop de empurrões.
            else e->pecaAtual.x -= kicks[k];                  // O empurrão também resultou em colisão, então remove esse kick para poder iterar na próxima opção do array 'kicks'.
        }                                                     // Fim do laço de tentativas de kick.
        if (!ok) memcpy(e->pecaAtual.matriz, orig, sizeof(orig)); // Nem depois de chutar todas as paredes deu pra girar a peça? Aborta a rotação inteira transferindo os blocos salvos do backup de volta pra ela.
    }                                                         // Fim da tratativa de colisão pós-giro.
}                                                             // Fim da função 'rotacionarPeca'.

// Desce a peça até o fundo de uma vez (hard drop)            // Comentário original.
void dropPeca(EstadoJogo *e) {                                // A temida descida forçada, convocada pela barra de ESPAÇO.
    while (1) {                                               // Laço infinito! Vai acontecer sem pausas.
        e->pecaAtual.y++;                                     // Joga a peça pro próximo Y.
        if (verificarColisao(e->tabuleiro, &e->pecaAtual)) {  // Checa se acertou chão/cimento.
            e->pecaAtual.y--;                                 // Como já penetrou e causou dano estrutural, ela deve recuar o excesso -1 casa pro Y que é legal.
            break;                                            // Quebra o loop eterno instantaneamente, pois o bloquinho chegou ao repouso.
        }                                                     // Fim do If de colisão.
        e->pontuacao += 2;  // 2 pontos por linha no hard drop// Bônus pela audácia: a cada laço que desceu livre ela acumula mais 2 pontinhos pro placar.
    }                                                         // Repete descida instantânea.
    descerPeca(e);                                            // Aproveita a própria rotina de carimbar bloco, verificar linha queimada e spawnar peça nova do 'descerPeca' como se nada tivesse ocorrido.
}                                                             // Fim da função 'dropPeca'.