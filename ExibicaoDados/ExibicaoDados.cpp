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

#define FILE_SIZE       200                                                    /*Tamanho do arquivo*/

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>		                                                       /*_getch()*/
#include "CheckForError.h"                                                     /*CheckForError()*/

/* ======================================================================================================================== */
/*  HANDLE EVENTOS*/

HANDLE hEventKeyO, hEventKeyEsc, hArquivo, hArquivoCheio, hFile;

/* ======================================================================================================================== */
/*  HANDLE COR*/

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

/* ======================================================================================================================== */
/*  VARIAVEIS GLOBAIS*/

int n_mensagem = 0;

/* ======================================================================================================================== */
/*  TAREFA DE EXIBICAO DE DADOS DO PROCESSO*/
/*  QUANDO SINALIZADA PELA TAREFA DE CAPTURA DE DADOS RETIRA MENSSAGENS DE DADOS DE PROCESSO DO ARQUIVO*/
/*  EXIBE AS MESMAS NO TERMINAL*/
/*  EXIBE O ESTADO BLOQUEADO OU DESBLOQUEADO DO PROCESSO*/

int main() {
    /*Nomeando o terminal do processo*/
    SetConsoleTitle(L"TERMINAL A - Exibicao de dados");                                            
    
    /*Declarando variaveis locais main()*/
    /*Valores genericos para fins de formatacao*/
    int     nTipoEvento = 2, key = 0, cont = 0;

    char NSEQ[6], HORA[12], TAG[10], VALOR[8], UE[8], MODO;

    char    SDCD[76] = { 'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', ' ', 
                         'H', 'O', 'R', 'A', ':', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', '.', 'M', 'S', 'S', ' ',
                         'T', 'A', 'G', ':', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                         'V', 'A', 'L', 'O', 'R', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                         'U', 'E', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ', 
                         'M', 'O', 'D', 'O', ':', '#' };

    DWORD   ret, status, dwPos, dwBytesToWrite, dwBytesWritten;

    /*------------------------------------------------------------------------------*/
    /*Abrindo eventos*/
    hEventKeyO = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyO");
    CheckForError(hEventKeyO);

    hEventKeyEsc = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"KeyEsc");
    CheckForError(hEventKeyEsc);

    hArquivoCheio = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"ArquivoCheio");
    CheckForError(hArquivoCheio);

    /*------------------------------------------------------------------------------*/
    /*Abrindo Semaforo*/
    hArquivo = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"SemArquivo");
    CheckForError(hArquivo);

    /*------------------------------------------------------------------------------*/
    /*Abrindo arquivo*/
    hFile = CreateFile(
        L"..\\DataLogger.txt",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        (LPSECURITY_ATTRIBUTES)NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Falha ao abrir ao abrir o arquivo - Codigo %d. \n", GetLastError());
    }
    else {
        printf("Arquivo aberto com sucesso\n");
    }

    /*------------------------------------------------------------------------------*/
    /*Vetor com handles da tarefa*/
    HANDLE Events[2] = { hEventKeyO, hEventKeyEsc };

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
            printf(" - Processo de exibicao de alarmes\n");
            
            ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            if (nTipoEvento == 0) {
                nTipoEvento = 2;
            }

            SetConsoleTextAttribute(hConsole, 10);
            printf("DESBLOQUEADO");
            SetConsoleTextAttribute(hConsole, 15);
            printf(" - Processo de exibicao alarmes\n");
        }

        /*Condicao para termino do processo*/
        if (nTipoEvento == 1) {
            key = ESC_KEY;
        }

        if (nTipoEvento == 2) {
            Sleep(300);
            char aux[54] = "000000000000000000000000000000000000000000000000000";
            char aux_2[54] = "000000000000000000000000000000000000000000000000000";

            cont = cont % FILE_SIZE;
            dwPos = cont * 54;

            /*Leitura de arquivo*/
            LockFile(hFile, 0, NULL, 52 * FILE_SIZE, NULL);
            dwBytesToWrite = (DWORD)strlen(aux) * sizeof(char);
            dwBytesWritten = 0;
            dwPos = SetFilePointer(hFile, dwPos, NULL, FILE_BEGIN);
            status = ReadFile(hFile,aux, dwBytesToWrite, &dwBytesWritten, NULL);
            
            if (FALSE == status) {
                printf("Nao foi possivel habilitar o arquivo para escrita. Codigo %d\n", GetLastError());
            }
            
            UnlockFile(hFile, 0, NULL, 52 * FILE_SIZE, NULL);

            if(strcmp(aux, aux_2) != 0) {   
                 /*Separacao de dados capturados do arquivo*/
                 int cont_aux = 0;
                 for (int i = 0; i < 54; i++)
                 {
                    if (i < 6){
                      NSEQ[cont_aux] = aux[i];
                      cont_aux++;
                    }
                    else if (i == 6 || i == 7 || i == 8 || i == 19 || i == 28 || i == 37 || i == 39) cont_aux = 0;
                    else if (i >= 9 && i <= 18) { 
                      TAG[cont_aux] = aux[i];
                      cont_aux++;
                    }
                    else if (i >= 20 && i <= 27) { 
                      VALOR[cont_aux] = aux[i];
                      cont_aux++;
                    }
                    else if (i >= 29 && i <= 36) { 
                      UE[cont_aux] = aux[i];
                      cont_aux++;
                    }
                    else if (i == 38) { 
                      MODO = aux[i];
                    }
                    else if (i >= 40 && i < 52) {
                      HORA[cont_aux] = aux[i]; 
                      cont_aux++;
                    }
                 }
                
                /*Impressao de dados em console*/
                printf("NSEQ:");
                for (int i = 0; i < 6; i++) printf("%c", NSEQ[i]);
                printf(" HORA:");
                for (int i = 0; i < 12; i++) printf("%c", HORA[i]);
                printf("TAG:");
                for (int i = 0; i < 10; i++) printf("%c", TAG[i]);
                printf(" VALOR:");
                for (int i = 0; i < 8; i++) printf("%c", VALOR[i]);
                printf(" HORA:");
                for (int i = 0; i < 8; i++) printf("%c", UE[i]);
                printf(" MODO:%c\r\n", MODO);

                cont++;
                ReleaseSemaphore(hArquivo, 1, NULL);
                SetEvent(hArquivoCheio);
            }
        }
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(Events);
    CloseHandle(hEventKeyEsc);
    CloseHandle(hEventKeyO);
    CloseHandle(hConsole);
    CloseHandle(hArquivo);
    CloseHandle(hArquivoCheio);
    CloseHandle(hFile);

    /*------------------------------------------------------------------------------*/
    /*Finalizando o processo de exibicao de dados*/
    return EXIT_SUCCESS;
}