/* ======================================================================================================================== */
/*  CABECALHO*/
/*
    UNIVERSIDADE FEDERAL DE MINAS GERAIS

    Trabalho pratico
    Automacao em tempo real (ELT012)

    Professor:
    Luiz Themystokliz S. Mendes

    Alunos:
    Estevao Coelho Kiel de Oliveira     - 2016119416
    Italo Jose Dias                     - 2017002121

    Data: Julho de 2021

    Aplicacao de software multithread responsavel pela leitura de dados tanto de de um Sistema Digital de Controle Distribuido
    (SDCD) quanto de um Plant Information Management System (PIMS) ficticios. Os mesmos serao apresentados em dois terminais
    de video. O primeiro ira exibir os dados do processo de fabricacao de celulose aos operadores (TERMINAL A) e o segundo
    apresentara previsoes de falhas operacionais geradas pelo PIMS (TERMINAL B).
*/

/* ======================================================================================================================== */
/*  DEFINE AREA*/

#define WIN32_LEAN_AND_MEAN

#define _CHECKERROR	    1				                                       /*Ativa funcao CheckForError*/

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		                                                       /*_getch()*/
#include "CheckForError.h"                                                     /*CheckForError()*/


int main() {
    SetConsoleTitle(L"TERMINAL A");                                            /*Nomeando o terminal do processo*/
    

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

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (0);
}