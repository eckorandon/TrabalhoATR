#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		// _getch
int ExibicaoAlarmes();

int main() {
    SetConsoleTitle(L"TERMINAL B");
    ExibicaoAlarmes();
}

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE EXIBICAO DE ALARMES*/
/*  XXXX*/
/*
    [] FALTA TUDO
*/

int ExibicaoAlarmes() {
    /*  DECLARACAO DE VARIAVEIS LOCAIS*/


    /*  PREENCHENDO VALORES GENERICOS DOS DADOS QUE SERAO EXIBIDOS NO TERMINAL A*/
    char    PIMS[54] = { 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', ' ', 'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', '#', '#',
                        ' ', 'I', 'D', ' ', 'A', 'L', 'A', 'R', 'M', 'E', ':', '#', '#', '#', '#', ' ',
                        'G', 'R', 'A', 'U', ':', '#', '#', ' ', 'P', 'R', 'E', 'V', ':', '#', '#', '#', '#', '#' };

    while (true) {

        /*      IMPRIME A MENSAGEM do PIMS*/
        for (int j = 0; j < 52; j++) {
            printf("%c", PIMS[j]);
        }

        printf("\n");
    }

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (0);
}