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

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		                                                       /*_getch()*/
#include "CheckForError.h"                                                     /*CheckForError()*/

/* ======================================================================================================================== */
/*  HANDLE EVENTOS*/

HANDLE hEventKeyC, hEventKeyEsc;

/* ======================================================================================================================== */
/*  TAREFA DE EXIBICAO DE ALARMES*/
/*  RECEBE MENSSAGENS DE ALARMES CRITICOS (9) DA TAREFA DE CAPTURA DE ALARMESE*/
/*  RECEBE MENSSAGENS DE ALARMES NAO CRITICOS (2) DA TAREFA DE LEITURA DO PIMS*/
/*  EXIBE AS MESMAS NO TERMINAL*/
/*
    TAREFAS
    [X] Imprimir estados
    [ ] Finalizar thread quando Esc � apertado
*/

int main() {
    /*Nomeando o terminal do processo*/
    SetConsoleTitle(L"TERMINAL B");
    
    /*Declarando variaveis locais main()*/
    /*Valores genericos para fins de formatacao*/
    int     nTipoEvento;

    char    PIMS[54] = { 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', ' ', 
                         'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', '#', '#', ' ', 
                         'I', 'D', ' ', 'A', 'L', 'A', 'R', 'M', 'E', ':', '#', '#', '#', '#', ' ',
                         'G', 'R', 'A', 'U', ':', '#', '#', ' ', 
                         'P', 'R', 'E', 'V', ':', '#', '#', '#', '#', '#' };

    DWORD   ret;

    /*------------------------------------------------------------------------------*/
    /*Abrindo eventos*/
    hEventKeyC = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyC");
    CheckForError(hEventKeyC);

    hEventKeyEsc = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyEsc");
    CheckForError(hEventKeyEsc);

    while (true) {

        /*Bloqueio e desbloqueio do processo de exibicao de alarmes*/
        ret = WaitForSingleObject(hEventKeyC, 1);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        if (nTipoEvento == 0) {
            printf("BLOQUEADO - Processo de exibicao de alarmes\n");

            ret = WaitForSingleObject(hEventKeyC, INFINITE);
            GetLastError();

            printf("DESBLOQUEADO - Processo de exibicao de alarmes\n");
        }

        /*PARA TESTES ============= Imprime as menssagems do PIMS ============= PARA TESTES*/
        /*
            for (int j = 0; j < 52; j++) {
                printf("%c", PIMS[j]);
            }

            printf("\n");
        */
    }

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (0);
}