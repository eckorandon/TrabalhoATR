#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		// _getch
int ExibicaoDados();

int main() {
    SetConsoleTitle(L"TERMINAL A");
    ExibicaoDados();
}

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE EXIBICAO DE DADOS DO PROCESSO*/
/*  XXXX*/
/*
    [] FALTA TUDO
*/

int ExibicaoDados() {

    /*  DECLARACAO DE VARIAVEIS LOCAIS*/


    /*  PREENCHENDO VALORES GENERICOS DOS DADOS QUE SERAO EXIBIDOS NO TERMINAL A*/
    char    SDCD[76] = { 'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', ' ', 'H', 'O', 'R', 'A', ':',
                        'H', 'H', ':', 'M', 'M', ':', 'S', 'S', '.', 'M', 'S', 'S', ' ',
                        'T', 'A', 'G', ':', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                        'V', 'A', 'L', 'O', 'R', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                        'U', 'E', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ', 'M', 'O', 'D', 'O', ':', '#' };

    while (true) {

        /*      IMPRIME A MENSAGEM do SDCD*/
        for (int j = 0; j < 76; j++) {
            printf("%c", SDCD[j]);
        }

        printf("\n");
    }

    // O comando "return" abaixo � desnecess�rio, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (0);
}