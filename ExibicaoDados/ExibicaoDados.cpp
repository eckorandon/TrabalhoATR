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
/*  INTRUCOES PARA UTILIZACAO DA BIBLIOTECA CHECKFORERROR*/
/*
    Para que a biblioteca CheckForError funcione corretamente e necessario tomar alguns cuidados

    1.  No Visual studio Comunity Edition selecione
        Project -> Properties -> Configuration Properties -> C/C++ -> Language
        Em "Conformance Mode" selecione a opcao "No(/permissive)".

    2.  Repita o mesmo passo para todos os processos de uma mesma solucao.
*/

/* ======================================================================================================================== */
/*  DEFINE AREA*/

#define WIN32_LEAN_AND_MEAN

#define _CHECKERROR	    1				                                       /*Ativa funcao CheckForError*/

/*Codigo ASCII para a tecla esc*/
#define	ESC_KEY			27

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		                                                       /*_getch()*/
#include "CheckForError.h"                                                     /*CheckForError()*/

/* ======================================================================================================================== */
/*  HANDLE EVENTOS*/

HANDLE hEventKeyO, hEventKeyEsc;

/* ======================================================================================================================== */
/*  TAREFA DE EXIBICAO DE DADOS DO PROCESSO*/
/*  QUANDO SINALIZADA PELA TAREFA DE CAPTURA DE DADOS RETIRA MENSSAGENS DE DADOS DE PROCESSO DO ARQUIVO*/
/*  EXIBE AS MESMAS NO TERMINAL*/
/*
    TAREFAS
    [X] Imprimir estados
    [ ] Finalizar thread quando Esc é apertado
*/

int main() {
    /*Nomeando o terminal do processo*/
    SetConsoleTitle(L"TERMINAL A");                                            
    
    /*Declarando variaveis locais main()*/
    /*Valores genericos para fins de formatacao*/
    int     nTipoEvento, key = 0;

    char    SDCD[76] = { 'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', ' ', 
                         'H', 'O', 'R', 'A', ':', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', '.', 'M', 'S', 'S', ' ',
                         'T', 'A', 'G', ':', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                         'V', 'A', 'L', 'O', 'R', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                         'U', 'E', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ', 
                         'M', 'O', 'D', 'O', ':', '#' };

    DWORD   ret;

    /*------------------------------------------------------------------------------*/
    /*Abrindo eventos*/
    hEventKeyO = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyO");
    CheckForError(hEventKeyO);

    hEventKeyEsc = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyEsc");
    CheckForError(hEventKeyEsc);

    /*------------------------------------------------------------------------------*/
    while (key != ESC_KEY) {
        /*------------------------------------------------------------------------------*/
        /*Condição para termino do processo*/
        ret = WaitForSingleObject(hEventKeyEsc, 1);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        if (nTipoEvento == 0) {
            key = ESC_KEY;
        }

        /*------------------------------------------------------------------------------*/
        /*Bloqueio e desbloqueio do processo de exibicao de dados do processo*/
        ret = WaitForSingleObject(hEventKeyO, 1);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        if (nTipoEvento == 0) {
            printf("BLOQUEADO - Processo de exibicao de dados\n");
            
            ret = WaitForSingleObject(hEventKeyO, INFINITE);
            GetLastError();

            printf("DESBLOQUEADO - Processo de exibicao de dados\n");
        }

        /*PARA TESTES ============= Imprime as menssagems do SDCD ============= PARA TESTES*/
        /*
            for (int j = 0; j < 76; j++) {
                printf("%c", SDCD[j]);
            }

            printf("\n");
        */
    }

    /*------------------------------------------------------------------------------*/
    printf("Finalizando processo de exibicao de dados\n");
    system("PAUSE");
    return EXIT_SUCCESS;
}