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

    2.  O arquivo CheckForError.h deve se encontrar dentro da pasta do projeto.

    3.  Repita o mesmo passo para todos os projetos de uma mesma solucao.
*/

/* ======================================================================================================================== */
/*  DEFINE AREA*/

#define WIN32_LEAN_AND_MEAN

#define _CHECKERROR	    1				                                       /*Ativa funcao CheckForError*/

#define	ESC_KEY			27                                                     /*Codigo ASCII para a tecla esc*/

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		                                                       /*_getch()*/
#include "CheckForError.h"                                                     /*CheckForError()*/

/* ======================================================================================================================== */
/*  HANDLE EVENTOS*/

HANDLE hEventKeyC, hEventKeyEsc, hEventMailslotAlarmeA;

/* ======================================================================================================================== */
/*  HANDLE MAILSLOT*/

HANDLE hMailslotServerAlarmeA;

/* ======================================================================================================================== */
/*  HANDLE COR*/

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

/* ======================================================================================================================== */
/*  TAREFA DE EXIBICAO DE ALARMES*/
/*  RECEBE E EXIBE MENSAGENS DE ALARMES NAO CRITICOS (2) DA TAREFA DE CAPTURA DE ALARMES E*/
/*  RECEBE E EXIBE MENSAGENS DE ALARMES CRITICOS (9) DA TAREFA DE LEITURA DO PIMS AMBAS VIA MAILSLOT*/
/*  EXIBE AS MESMAS NO TERMINAL*/
/*  EXIBE O ESTADO BLOQUEADO OU DESBLOQUEADO DO PROCESSO*/

int main() {
    /*Nomeando o terminal do processo*/
    SetConsoleTitle(L"TERMINAL B - Exibicao de alarmes");
    
    /*Declarando variaveis locais main()*/
    /*Valores genericos para fins de formatacao*/
    int     nTipoEvento = 2, key = 0;

    bool    status;

    char    PIMS[54] = { 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', ' ',
                         'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', '#', '#', ' ',
                         'I', 'D', ' ', 'A', 'L', 'A', 'R', 'M', 'E', ':', '#', '#', '#', '#', ' ',
                         'G', 'R', 'A', 'U', ':', '#', '#', ' ',
                         'P', 'R', 'E', 'V', ':', '#', '#', '#', '#', '#' },
            MsgBuffer[31];

    DWORD   ret, dwBytesLidos, MenssageCount;

    /*------------------------------------------------------------------------------*/
    /*Abrindo eventos*/
    hEventKeyC = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyC");
    CheckForError(hEventKeyC);

    hEventKeyEsc = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyEsc");
    CheckForError(hEventKeyEsc);

    hEventMailslotAlarmeA = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"MailslotAlarme");
    CheckForError(hEventMailslotAlarmeA);

    hMailslotServerAlarmeA = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"MailslotAlarme");
    CheckForError(hMailslotServerAlarmeA);

    /*------------------------------------------------------------------------------*/
    /*Vetor com handles da tarefa*/
    HANDLE Events[2] = { hEventKeyC, hEventKeyEsc };

    /*------------------------------------------------------------------------------*/
    /*Criando servidor mailslot*/
    hMailslotServerAlarmeA = CreateMailslot(L"\\\\.\\mailslot\\MailslotAlarmeA", 0, MAILSLOT_WAIT_FOREVER, NULL);
    CheckForError(hMailslotServerAlarmeA != INVALID_HANDLE_VALUE);
    SetEvent(hEventMailslotAlarmeA);
    GetLastError();

    /*------------------------------------------------------------------------------*/
    /*Loop de execucao*/
    while (key != ESC_KEY) {
        /*------------------------------------------------------------------------------*/
        /*Bloqueio e desbloqueio ou finalizacao do processo de exibicao de dados do processo*/
        ret = WaitForMultipleObjects(2, Events, FALSE, 1);
        GetLastError();

        if (ret != WAIT_TIMEOUT) {
            nTipoEvento = ret - WAIT_OBJECT_0;
        }
        
        if (nTipoEvento == 0) {
            SetConsoleTextAttribute(hConsole, 12);
            printf("BLOQUEADO");
            SetConsoleTextAttribute(hConsole, 15);
            printf(" - Processo de exibicao de dados\n");

            ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            if (nTipoEvento == 0) {
                nTipoEvento = 2;
            }

            SetConsoleTextAttribute(hConsole, 10);
            printf("DESBLOQUEADO");
            SetConsoleTextAttribute(hConsole, 15);
            printf(" - Processo de exibicao de dados\n");
        }

        /*Condicao para termino do processo*/
        if (nTipoEvento == 1) {
            key = ESC_KEY;
        }

        GetMailslotInfo(hMailslotServerAlarmeA, (LPDWORD)NULL, (LPDWORD)NULL, &MenssageCount, (LPDWORD)NULL);
        GetLastError();

        /*Caso nTipoEvento nao tenha sido alterado -> leitura do mailslot*/
        if (nTipoEvento == 2 && (int)MenssageCount > 0) {
            status = ReadFile(hMailslotServerAlarmeA, &MsgBuffer, sizeof(MsgBuffer), &dwBytesLidos, NULL);
            CheckForError(status);

            /*TIMESTAMP*/
            for (int j = 0; j < 8; j++) {
                PIMS[j] = MsgBuffer[(j + 23)];
            }

            /*NSEQ*/
            for (int j = 14; j < 20; j++) {
                PIMS[j] = MsgBuffer[(j - 14)];
            }

            /*ID ALARME*/
            for (int j = 31; j < 35; j++) {
                PIMS[j] = MsgBuffer[(j - 22)];
            }

            /*GRAU*/
            for (int j = 41; j < 43; j++) {
                PIMS[j] = MsgBuffer[(j - 27)];
            }

            /*PREV*/
            for (int j = 49; j < 54; j++) {
                PIMS[j] = MsgBuffer[(j - 32)];
            }

            if (MsgBuffer[7] == '9') {
                /*Exibe alarmes criticos em vermelho*/
                SetConsoleTextAttribute(hConsole, 12);
                for (int j = 0; j < 54; j++) {
                    printf("%c", PIMS[j]);
                }
                SetConsoleTextAttribute(hConsole, 15);
                printf("\n");
            }
            else if (MsgBuffer[7] == '2') {
                /*Exibe alarmes nao criticos*/
                for (int j = 0; j < 54; j++) {
                    printf("%c", PIMS[j]);
                }
                printf("\n");
            }   
        }
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(Events);
    CloseHandle(hMailslotServerAlarmeA);
    CloseHandle(hEventMailslotAlarmeA);
    CloseHandle(hEventKeyEsc);
    CloseHandle(hEventKeyC);
    CloseHandle(hConsole);

    /*------------------------------------------------------------------------------*/
    /*Finalizando o processo de exibicao de alarmes*/
    return EXIT_SUCCESS;
}