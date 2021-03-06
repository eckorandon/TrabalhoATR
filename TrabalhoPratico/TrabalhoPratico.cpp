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

    Data: Agosto de 2021

    Aplicacao de software multithread responsavel pela leitura de dados tanto de de um Sistema Digital de Controle Distribuido 
    (SDCD) quanto de um Plant Information Management System (PIMS) ficticios. Os mesmos serao apresentados em dois terminais
    de video. O primeiro ira exibir os dados do processo de fabricacao de celulose aos operadores (TERMINAL A) e o segundo
    apresentara previsoes de falhas operacionais geradas pelo PIMS (TERMINAL B).
*/

/* ======================================================================================================================== */
/*  INTRUCOES PARA UTILIZACAO DA BIBLIOTECA PTHREAD*/
/*
    Para que a biblioteca Pthreads funcione corretamente e necessario tomar alguns cuidados
  
    1.  A biblioteca Pthreads 2.9.1 deve estar instalada no seu computador
        em C:\Program Files\pthreads-w32-2-9-1-release
 
    2.  No Visual studio Comunity Edition selecione
        Project -> Properties -> Configuration Properties -> C/C++ -> General
        e defina em "Additional Include Directories" o diret?rio onde encontram-se os
        "header files" da distribuicao pthreads em
        C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\include

    3.  Agora selecione Project -> Properties -> Configuration Properties -> Linker -> General
        e defina em "Additional Library Directories" o diretorio onde se encontra
        a biblioteca Pthreads (extens?o .LIB) em
        C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\lib\x86
 
    4.  Depois selecione Project -> Properties -> Configuration Properties -> Linker -> Input
        e declare a biblioteca "pthreadVC2.lib" em "Additional Dependencies"
 
    5.  Por fim, selecione Project -> Properties -> Configuration Properties -> Debugging 
        e entao preencha o item "Environment" com
        PATH=C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\dll\x86 

    6. Certifique-se de que o seu projeto esta como x86. Do lado de Debug ? possivel alterar
       este parametro.
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

#define HAVE_STRUCT_TIMESPEC
#define WIN32_LEAN_AND_MEAN

#define _CHECKERROR	    1				                                        /*Ativa funcao CheckForError*/

#define RAM             100                                                     /*Tamanho da lista circular em memoria ram*/
#define	ESC_KEY			27                                                      /*Codigo ASCII para a tecla esc*/
#define FILE_SIZE       200                                                     /*Tamanho do arquivo em disco*/

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <conio.h>		                                                        /*_getch()*/
#include <math.h>                                                               /*pow()*/
#include <time.h>
#include <string.h>
#include "CheckForError.h"                                                      /*CheckForError()*/

/* ======================================================================================================================== */
/*  DECLARACAO DO PROTOTIPO DE FUNCAO DAS THREADS SECUNDARIAS*/

void* LeituraSDCD(void* arg);
void* LeituraPIMS(void* arg);
void* CapturaDados(void* arg);
void* CapturaAlarmes(void* arg);

/* ======================================================================================================================== */
/*  DECLARACAO DAS VARIAVEIS GLOBAIS*/

/*Espaco destinado a lista circular na memoria RAM e tecla de input*/
char    RamBuffer[RAM][52], key;

/*Variaveis de controle das posicoes na lista circular*/
int     p_ocup = 0, p_livre = 0;

/*Variavel de controle da posicao no arquivo em disco*/
int     n_mensagem = 0;

/* ======================================================================================================================== */
/*  HANDLE MUTEX*/

HANDLE hMutexBuffer;

/* ======================================================================================================================== */
/*  HANDLE SEMAFOROS*/

HANDLE hSemLivre, hSemOcupado, hArquivo;

/* ======================================================================================================================== */
/*  HANDLE EVENTOS*/

HANDLE hEventKeyS, hEventKeyP, hEventKeyD, hEventKeyA, hEventKeyO, hEventKeyC, hEventKeyEsc, hEventMailslotAlarmeA, hTimeOut;
HANDLE hArquivoCheio;

/* ======================================================================================================================== */
/*  HANDLE MAILSLOT*/

HANDLE hMailslotClienteAlarmeA;

/* ======================================================================================================================== */
/*  HANDLE ARQUIVO EM DISCO*/

HANDLE hFile;

/* ======================================================================================================================== */
/*  THREAD PRIMARIA*/
/*  CRIACAO OBJETOS, LISTA EM DISCO, THREADS SECUNDARIAS E PROCESSOS FILHOS*/ 
/*  TAREFA DE LEITURA DO TECLADO*/

int main() {
    /*------------------------------------------------------------------------------*/
    /*Declarando variaveis locais da main()*/
    int     i, status;

    DWORD   ret;
    
    /*------------------------------------------------------------------------------*/
    /*Criando objetos do tipo mutex*/
    hMutexBuffer = CreateMutex(NULL, FALSE, L"MutexBuffer");
    CheckForError(hMutexBuffer);

    /*------------------------------------------------------------------------------*/
    /*Criando objetos do tipo semaforo*/
    hSemLivre = CreateSemaphore(NULL, RAM, RAM, L"SemLivre");
    CheckForError(hSemLivre);

    hSemOcupado = CreateSemaphore(NULL, 0, RAM, L"SemOcupado");
    CheckForError(hSemOcupado);

    hArquivo = CreateSemaphore(NULL, FILE_SIZE, FILE_SIZE, L"SemArquivo");
    CheckForError(hArquivo);

    /*------------------------------------------------------------------------------*/
    /*Criando objetos do tipo eventos*/
    hEventKeyS = CreateEvent(NULL, FALSE, FALSE, L"KeyS");
    CheckForError(hEventKeyS);

    hEventKeyP = CreateEvent(NULL, FALSE, FALSE, L"KeyP");
    CheckForError(hEventKeyP);

    hEventKeyD = CreateEvent(NULL, FALSE, FALSE, L"KeyD");
    CheckForError(hEventKeyD);

    hEventKeyA = CreateEvent(NULL, FALSE, FALSE, L"KeyA");
    CheckForError(hEventKeyA);

    hEventKeyO = CreateEvent(NULL, FALSE, FALSE, L"KeyO");
    CheckForError(hEventKeyO);

    hEventKeyC = CreateEvent(NULL, FALSE, FALSE, L"KeyC");
    CheckForError(hEventKeyC);

    hEventKeyEsc = CreateEvent(NULL, TRUE, FALSE, L"KeyEsc");
    CheckForError(hEventKeyEsc);

    hEventMailslotAlarmeA = CreateEvent(NULL, FALSE, FALSE, L"MailslotAlarme");
    CheckForError(hEventMailslotAlarmeA);

    hTimeOut = CreateEvent(NULL, FALSE, FALSE, L"TimeOut");
    CheckForError(hTimeOut);

    hArquivoCheio = CreateEvent(NULL, FALSE, TRUE, L"ArquivoCheio");
    CheckForError(hArquivoCheio);

    /*------------------------------------------------------------------------------*/
    /*Threads*/

    /*Handles threads*/
    pthread_t hLeituraSDCD, hLeituraPIMS, hCapturaDados, hCapturaAlarmes;

    /*Criando threads secundarias*/
    i = 1;
    status = pthread_create(&hLeituraSDCD, NULL, LeituraSDCD, (void*)i);
    if (!status) printf("Thread %d - Leitura SDCD - criada com Id= %0x \n", i, (int)&hLeituraSDCD);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    i = 2;
    status = pthread_create(&hLeituraPIMS, NULL, LeituraPIMS, (void*)i);
    if (!status) printf("Thread %d - Leitura PIMS - criada com Id= %0x \n", i, (int)&hLeituraPIMS);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    i = 3;
    status = pthread_create(&hCapturaDados, NULL, CapturaDados, (void*)i);
    if (!status) printf("Thread %d - Captura de dados do processo - criada com Id= %0x \n", i, (int)&hCapturaDados);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    i = 4;
    status = pthread_create(&hCapturaAlarmes, NULL, CapturaAlarmes, (void*)i);
    if (!status) printf("Thread %d - Captura de alarmes - criada com Id= %0x \n\n", i, (int)&hCapturaAlarmes);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    /*------------------------------------------------------------------------------*/
    /*Processos*/

    /*Nomeando terminal da thread primaria*/
    SetConsoleTitle(L"TERMINAL PRINCIPAL");                                    

    /*Criando processos filhos*/
    STARTUPINFO si;				                                               /*StartUpInformation para novo processo*/
    PROCESS_INFORMATION NewProcess;	                                           /*Informacoes sobre novo processo criado*/

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);	                                                       /*Tamanho da estrutura em bytes*/

    /*Processo de exibicao de dados - TERMINAL A*/
    status = CreateProcess(
        L"..\\Debug\\ExibicaoDados.exe",                                       /*Caminho relativo do arquivo executavel*/
        NULL,                                                                  /*Apontador para parametros de linha de comando*/
        NULL,                                                                  /*Apontador para descritor de seguran?a*/
        NULL,                                                                  /*Apontador para threads do processo*/
        FALSE,	                                                               /*Heranca de handles*/
        CREATE_NEW_CONSOLE,	                                                   /*Flags de cria??o - Criar novo console no caso*/
        NULL,	                                                               /*Heranca do ambiente de execucao*/
        L"..\\Debug",                                                          /*Diretorio do arquivo executavel*/
        &si,			                                                       /*lpStartUpInfo*/
        &NewProcess);	                                                       /*lpProcessInformation*/
    if (!status) { 
        printf("Erro na criacao do Terminal A! Codigo = %d\n", GetLastError()); 
    }
    else {
        printf("Processo de exibicao de dados e TERMINAL A criados\n", GetLastError());
    }

    /*Processo de exibicao de alarmes - TERMINAL B*/
    status = CreateProcess(
        L"..\\Debug\\ExibicaoAlarmes.exe",                                     /*Caminho relativo do arquivo executavel*/
        NULL,                                                                  /*Apontador para parametros de linha de comando*/
        NULL,                                                                  /*Apontador para descritor de seguran?a*/
        NULL,                                                                  /*Apontador para threads do processo*/
        FALSE,	                                                               /*Heranca de handles*/
        CREATE_NEW_CONSOLE,	                                                   /*Flags de cria??o - Criar novo console no caso*/
        NULL,	                                                               /*Heranca do ambiente de execucao*/
        L"..\\Debug",                                                          /*Diretorio do arquivo executavel*/
        &si,			                                                       /*lpStartUpInfo*/
        &NewProcess);	                                                       /*lpProcessInformation*/
    if (!status) {
        printf("Erro na criacao do Terminal B! Codigo = %d\n", GetLastError());
    }
    else {
        printf("Processo de exibicao de alarmes e TERMINAL B criados\n\n", GetLastError());
    }
    
    /*------------------------------------------------------------------------------*/
    /*Criacao de arquivo em disco*/
    hFile = CreateFile(
        L"..\\DataLogger.txt",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        (LPSECURITY_ATTRIBUTES)NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);

    if (hFile == INVALID_HANDLE_VALUE){
        printf("Falha ao abrir ao abrir o arquivo. Codigo %d. \n", GetLastError());
    }
    else{
        printf("Arquivo para memoria em disco criado\n\n");
    }

    /*------------------------------------------------------------------------------*/
    /*Tratando inputs do teclado*/
    while (key != ESC_KEY) {
        key = _getch();
        switch (key) {
        case 's':
        case 'S':
            SetEvent(hEventKeyS);
            GetLastError();
            printf("Voce digitou a tecla S\n");
            break;
        case 'p':
        case 'P':
            SetEvent(hEventKeyP);
            GetLastError();
            printf("Voce digitou a tecla P\n");
            break;
        case 'd':
        case 'D':
            SetEvent(hEventKeyD);
            GetLastError();
            printf("Voce digitou a tecla D\n");
            break;
        case 'a':
        case 'A':
            SetEvent(hEventKeyA);
            GetLastError();
            printf("Voce digitou a tecla A\n");
            break;
        case 'o':
        case 'O':
            SetEvent(hEventKeyO);
            GetLastError();
            printf("Voce digitou a tecla O\n");
            printf("Alterando estado processo de exibicao de dados\n");
            break;
        case 'c':
        case 'C':
            SetEvent(hEventKeyC);
            GetLastError();
            printf("Voce digitou a tecla C\n");
            printf("Alterando estado processo de exibicao de alarmes\n");
            break;
        case ESC_KEY:
            printf("\n");
            SetEvent(hEventKeyEsc);
            GetLastError();
            break;
        default:
            printf("Voce digitou uma tecla sem funcao!\n");
            break;
        } /*fim do switch*/
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Aguardando o fim dos processos e threads antes de encerrar a main*/
    WaitForSingleObject(hTimeOut, 5000);
    GetLastError();

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(hTimeOut);
    CloseHandle(hEventMailslotAlarmeA);
    CloseHandle(hEventKeyEsc);
    CloseHandle(hEventKeyC);
    CloseHandle(hEventKeyO);
    CloseHandle(hEventKeyA);
    CloseHandle(hEventKeyD);
    CloseHandle(hEventKeyP);
    CloseHandle(hEventKeyS);
    CloseHandle(hArquivoCheio);
    CloseHandle(hSemOcupado);
    CloseHandle(hSemLivre);
    CloseHandle(hArquivo);
    CloseHandle(hMutexBuffer);
    CloseHandle(hFile);
    
    /*------------------------------------------------------------------------------*/
    printf("Finalizando - Inputs do teclado\n");
    return EXIT_SUCCESS;

} /*fim da funcao main*/

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA SDCD*/
/*  GERACAO DE VALORES DE MENSAGENS DE DADOS DO SDCD*/
/*  GRAVACAO DOS MESMOS NA LISTA CIRCULAR EM MEMORIA*/

void* LeituraSDCD(void* arg) {
    /*------------------------------------------------------------------------------*/
    /*Declarando variaveis locais do LeituraSDCD()*/
    int     index = (int)arg, status, nTipoEvento,
            k = 0, i = 0, l = 0;

    char    SDCD[52], UE[9] = "        ", AM[3] = "AM",
            CaracterAleatorio[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
            Unidade[11][9]{ "kg      ", "kg/m^2  ", "A       ", "V       ", "T       ", "Nm      ",
                            "m       ", "kgf     ", "N       ", "m/s     ", "m/s^2   " },
            Hora[3], Minuto[3], Segundo[3], MiliSegundo[4];
    
    DWORD   ret;

    BOOL    Memory;

    /*------------------------------------------------------------------------------*/
    /*Vetor com handles da tarefa*/
    HANDLE  Events[2]        = { hEventKeyS, hEventKeyEsc },
            SemLivre[2]      = { hSemLivre, hEventKeyEsc },
            MutexBuffer[2]   = { hMutexBuffer, hEventKeyEsc };

    /*------------------------------------------------------------------------------*/
    /*Loop de execucao*/
    while (key != ESC_KEY) {
        for (i = 1; i < 1000000; ++i) {
            /*------------------------------------------------------------------------------*/
            /*Condicao para termino da thread*/
            if (key == ESC_KEY) break;

            /*------------------------------------------------------------------------------*/
            /*Bloqueio e desbloqueio da thread LeituraSDCD*/
            ret = WaitForMultipleObjects(2, Events, FALSE, 1);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            if (nTipoEvento == 0) {
                printf("\x1b[31m""BLOQUEADO""\x1b[0m"" - Thread Leitura SDCD\n");

                ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
                GetLastError();

                nTipoEvento = ret - WAIT_OBJECT_0;

                printf("\x1b[32m""DESBLOQUEADO""\x1b[0m"" - Thread Leitura SDCD\n");
            }

            /*Condicao para termino do processo*/
            if (nTipoEvento == 1) {
                key = ESC_KEY;
                break;
            }

            /*------------------------------------------------------------------------------*/
            /*Gerando valores aleatorios para os campos referentes ao SDCD*/

            /*Valores de NSEQ - Numero sequencial de 1 ate 999999*/
            for (int j = 0; j < 6; j++) {
                k = i / pow(10, (5 - j));
                k = k % 10;
                SDCD[j] = k + '0';
            }
            SDCD[6] = '|';

            /*Valores de TIPO - Sempre 1*/
            SDCD[7] = '1';
            SDCD[8] = '|';

            /*Valores de TAG - Identificador da variavel do processo - Valores alfanumericos*/
            for (int j = 9; j < 19; j++) {
                if (j == 12 || j == 15) {
                    SDCD[j] = '-';
                }
                else {
                    SDCD[j] = CaracterAleatorio[(rand()%36)];
                } 
            }
            SDCD[19] = '|';

            /*Valores de VALOR - Valor aleatorio da variavel de processo*/
            for (int j = 20; j < 28; j++) {
                if (j == 25) {
                    SDCD[j] = '.';
                }
                else {
                    SDCD[j] = (rand() % 10)+'0';
                }
            }
            SDCD[28] = '|';

            /*Valores de UE - Unidade de engenharia escolhida aleatoriamente*/
            k = (rand() % 11);
            for (int j = 0; j < 9; j++) {
                UE[j] = Unidade[k][j];
            }

            k = 0;
            for (int j = 29; j < 37; j++) {
                SDCD[j] = UE[k];
                k++;
            }
            SDCD[37] = '|';

            /*Valores de MODO - Pode ser (A)utomatico ou (M)anual*/
            SDCD[38] = AM[(rand() % 2)];
            SDCD[39] = '|';

            /*Valores de TIMESTAMP - Com precisao de milisegundos*/
            SYSTEMTIME st;
            GetLocalTime(&st);

            /*Hora*/
            k = 0;
            l = 40;
            for (int j = 0; j < 2; j++) {
                k = st.wHour / pow(10, (1 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }
            SDCD[42] = ':';

            /*Minuto*/
            k = 0;
            l = 43;
            for (int j = 0; j < 2; j++) {
                k = st.wMinute / pow(10, (1 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }
            SDCD[45] = ':';

            /*Segundo*/
            k = 0;
            l = 46;
            for (int j = 0; j < 2; j++) {
                k = st.wSecond / pow(10, (1 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }
            SDCD[48] = '.';

            /*Milisegundo*/
            k = 0;
            l = 49;
            for (int j = 0; j < 3; j++) {
                k = st.wMilliseconds / pow(10, (2 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }

            /*------------------------------------------------------------------------------*/
            /*Condicao para termino da thread*/
            if (key == ESC_KEY) break;

            /*------------------------------------------------------------------------------*/
            /*Temporizador - Mensagens do SDCD se repetem de 500 em 500 ms*/
            /*475 pois o tempo medio da conquista de mutex, semaforo, criacao e gravacao dos dados na lista demora 025 ms*/
            WaitForSingleObject(hTimeOut, 475);
            GetLastError();

            /*------------------------------------------------------------------------------*/
            /*Gravacao dos dados gerados em memoria*/
            /*Caso memoria esteja cheia a tarefa se bloqueia*/

            /*Esperando o semaforo de espacos livres*/
            ret = WaitForMultipleObjects(2, SemLivre, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            /*Condi??o para termino do processo*/
            if (nTipoEvento == 1) {
                key = ESC_KEY;
                break;
            }
            else if (nTipoEvento == 0) {
                /*Conquistando o mutex da secao critica*/
                ret = WaitForMultipleObjects(2, MutexBuffer, FALSE, INFINITE);
                GetLastError();

                nTipoEvento = ret - WAIT_OBJECT_0;

                if (nTipoEvento == 1) {
                    key = ESC_KEY;
                    break;
                }
                else if (nTipoEvento == 0) {
                    /*Gravando dados em memoria RAM*/
                    for (int j = 0; j < 52; j++) {
                        RamBuffer[p_livre][j] = SDCD[j];
                    }

                    /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                    p_livre = (p_livre + 1) % RAM;

                    Memory = (p_livre == p_ocup);

                    /*Liberando o mutex da secao critica*/
                    status = ReleaseMutex(hMutexBuffer);
                    GetLastError();

                    /*Liberando o semaforo de espacos ocupados*/
                    ReleaseSemaphore(hSemOcupado, 1, NULL);
                    GetLastError();
                    
                    /*Quando a memoria estiver cheia a gravacao de dados e interrompida
                    ate que uma posicao livre apareca - os dados nao escritos sao descartados*/
                    if (Memory) {
                        printf("MEMORIA CHEIA\n");

                        printf("\x1b[31m""BLOQUEADO""\x1b[0m"" - Thread Leitura SDCD\n");

                        ret = WaitForMultipleObjects(2, SemLivre, FALSE, INFINITE);
                        GetLastError();

                        nTipoEvento = ret - WAIT_OBJECT_0;

                        if (nTipoEvento == 0) {
                            /*Liberando o semaforo de espacos livres*/
                            ReleaseSemaphore(hSemLivre, 1, NULL);
                            GetLastError();
                        }

                        printf("\x1b[32m""DESBLOQUEADO""\x1b[0m"" - Thread Leitura SDCD\n");
                    }
                }
            
                nTipoEvento = -1;
            }
        } /*fim do for*/
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(MutexBuffer);
    CloseHandle(SemLivre);
    CloseHandle(Events);

    /*------------------------------------------------------------------------------*/
    /*Finalizando a thread leitura do SDCD*/
    printf("Finalizando - Leitura do SDCD\n");
    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA PIMS*/
/*  GERACAO DE VALORES DE MENSAGENS DO PIMS*/
/*  GRAVACAO DOS ALARMES NAO CRITICOS NA LISTA CIRCULAR EM MEMORIA*/
/*  REPASSAGEM DOS ALARMES CRITICOS PARA A TAREFA DE EXIBICAO DE ALARMES*/

void* LeituraPIMS(void* arg) {
    /*------------------------------------------------------------------------------*/
    /*Declarando variaveis locais da funcao LeituraPIMS()*/
    int     index = (int)arg, status, nTipoEvento,
            k = 0, i = 0, l = 0, randon = 0, critico = 0,
            deadline2 = 5000, deadline9 = 8000, time2 = 0, time9 = 0;

    char    PIMS[31], CriticoNaoCritico[3] = "29",
            Hora[3], Minuto[3], Segundo[3];

    DWORD   ret, ticks1, ticks2, ticksA, ticksB, dwBytesLidos;

    BOOL    Memory;

    /*------------------------------------------------------------------------------*/
    /*Vetor com handles da tarefa*/
    HANDLE  Events[2]       = { hEventKeyP, hEventKeyEsc },
            SemLivre[2]     = { hSemLivre, hEventKeyEsc },
            MutexBuffer[2]  = { hMutexBuffer, hEventKeyEsc };

    /*------------------------------------------------------------------------------*/
    /*Criando arquivo para cliente mailslot*/
    WaitForSingleObject(hEventMailslotAlarmeA, INFINITE);
    GetLastError();

    hMailslotClienteAlarmeA = CreateFile(L"\\\\.\\mailslot\\MailslotAlarmeA", GENERIC_WRITE, FILE_SHARE_READ, 
                                        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    CheckForError(hMailslotClienteAlarmeA != INVALID_HANDLE_VALUE);

    /*------------------------------------------------------------------------------*/
    /*Tempo de inicio*/
    ticks1 = GetTickCount();
    ticksA = GetTickCount();

    /*------------------------------------------------------------------------------*/
    /*Loop de execucao*/
    while (key != ESC_KEY) {
        for (i = 1; i < 1000000; ++i) {
            /*------------------------------------------------------------------------------*/
            /*Condicao para termino da thread*/
            if (key == ESC_KEY) break;

            /*------------------------------------------------------------------------------*/
            /*Bloqueio e desbloqueio da thread LeituraPIMS*/
            ret = WaitForMultipleObjects(2, Events, FALSE, 1);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            if (nTipoEvento == 0) {
                printf("\x1b[31m""BLOQUEADO""\x1b[0m"" - Thread Leitura PIMS\n");

                ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
                GetLastError();

                nTipoEvento = ret - WAIT_OBJECT_0;

                printf("\x1b[32m""DESBLOQUEADO""\x1b[0m"" - Thread Leitura PIMS\n");
            }

            /*Condicao para termino do processo*/
            if (nTipoEvento == 1) {
                key = ESC_KEY;
                break;
            }

            /*------------------------------------------------------------------------------*/
            /*Gerando valores aleatorios para os campos referentes ao PIMS*/

            /*Valores de NSEQ - Numero sequencial de 1 ate 999999*/
            for (int j = 0; j < 6; j++) {
                k = i / pow(10, (5 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[6] = '|';

            /*Valores de ID ALARME - Numero aleatorio de 1 ate 9999 - Identificador da condicao anormal*/
            randon = rand() % 10000;
            k = 0;
            for (int j = 9; j < 13; j++) {
                k = randon / pow(10, (12 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[13] = '|';

            /*Valores de GRAU - Numero aleatorio de 1 ate 99 - Grau de impacto da condicao anormal*/
            randon = rand() % 100;
            k = 0;
            for (int j = 14; j < 16; j++) {
                k = randon / pow(10, (15 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[16] = '|';

            /*Valores de PREV - Numero aleatorio em minutos de 1 ate 14440 ate a ocorrencia da condicao*/
            randon = rand() % 14441;
            k = 0;
            for (int j = 17; j < 22; j++) {
                k = randon / pow(10, (21 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[22] = '|';

            /*Valores de TIMESTAMP*/
            SYSTEMTIME st;
            GetLocalTime(&st);

            /*Hora*/
            k = 0;
            l = 23;
            for (int j = 0; j < 2; j++) {
                k = st.wHour / pow(10, (1 - j));
                k = k % 10;
                PIMS[l] = k + '0';
                l++;
            }
            PIMS[25] = ':';

            /*Minuto*/
            k = 0;
            l = 26;
            for (int j = 0; j < 2; j++) {
                k = st.wMinute / pow(10, (1 - j));
                k = k % 10;
                PIMS[l] = k + '0';
                l++;
            }
            PIMS[28] = ':';

            /*Segundos*/
            k = 0;
            l = 29;
            for (int j = 0; j < 2; j++) {
                k = st.wSecond / pow(10, (1 - j));
                k = k % 10;
                PIMS[l] = k + '0';
                l++;
            }

            /*Valores de TIPO - 2 = nao critico e 9 = critico*/
            /*O tipo que tem o menor tempo ate o deadline e escolhido*/

            ticks2 = GetTickCount();
            ticksB = GetTickCount();

            time2 = deadline2 - (ticks2 - ticks1);
            time9 = deadline9 - (ticksB - ticksA);

            if (time9 >= time2) {
                PIMS[7] = CriticoNaoCritico[0];

                if (time2 <= 4000) {
                    randon = (rand() % time2);
                }
                else {
                    randon = (time2 - 4000) + (rand() % 4000);
                }
            }
            else {
                PIMS[7] = CriticoNaoCritico[1];

                if (time9 <= 5000) {
                    randon = (rand() % time9);
                }
                else {
                    randon = (time9 - 5000) + (rand() % 5000);
                }
            }
            critico = PIMS[7] - '0';
            PIMS[8] = '|';

            /*------------------------------------------------------------------------------*/
            /*Condicao para termino da thread*/
            if (key == ESC_KEY) break;

            /*------------------------------------------------------------------------------*/
            /*Gravacao dos dados de tipo 2 gerados em memoria ou*/
            /*Passagem de alarmes criticos para a tarefa de exibicao de alarmes*/

            /*Esperando o semaforo de espacos livres, temporizador e mutex*/
            if (critico == 2) {
                ret = WaitForMultipleObjects(2, SemLivre, FALSE, INFINITE);
                GetLastError();

                nTipoEvento = ret - WAIT_OBJECT_0;

                /*Condi??o para termino do processo*/
                if (nTipoEvento == 1) {
                    key = ESC_KEY;
                    break;
                }
                else if (nTipoEvento == 0) {
                    /*Temporizador - Mensagens do PIMS nao-criticas se repetem de 1 a 5 s*/

                    ticks1 = GetTickCount();

                    WaitForSingleObject(hTimeOut, (randon));
                    GetLastError();

                    /*Conquistando o mutex da secao critica*/
                    ret = WaitForMultipleObjects(2, MutexBuffer, FALSE, INFINITE);
                    GetLastError();

                    nTipoEvento = ret - WAIT_OBJECT_0;

                    if (nTipoEvento == 1) {
                        key = ESC_KEY;
                        break;
                    }
                    else if (nTipoEvento == 0) {
                        /*Gravando dados em memoria RAM*/
                        for (int j = 0; j < 31; j++) {
                            RamBuffer[p_livre][j] = PIMS[j];
                        }

                        /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                        p_livre = (p_livre + 1) % RAM;

                        Memory = (p_livre == p_ocup);
                    }

                    /*Liberando o mutex da secao critica*/
                    status = ReleaseMutex(hMutexBuffer);
                    GetLastError();

                    /*Liberando o semaforo de espacos ocupados*/
                    ReleaseSemaphore(hSemOcupado, 1, NULL);
                    GetLastError();

                    /*Quando a memoria estiver cheia a gravacao de dados e interrompida
                    ate que uma posicao livre apareca*/
                    if (Memory) {
                        printf("MEMORIA CHEIA\n");

                        printf("\x1b[31m""BLOQUEADO""\x1b[0m"" - Thread Leitura PIMS\n");

                        ret = WaitForMultipleObjects(2, SemLivre, FALSE, INFINITE);
                        GetLastError();

                        nTipoEvento = ret - WAIT_OBJECT_0;

                        if (nTipoEvento == 0) {
                            /*Liberando o semaforo de espacos livres*/
                            ReleaseSemaphore(hSemLivre, 1, NULL);
                            GetLastError();
                        }

                        printf("\x1b[32m""DESBLOQUEADO""\x1b[0m"" - Thread Leitura PIMS\n");
                    }
                }
            } 
            else if (critico == 9) {
                /*Temporizador - Mensagens do PIMS criticas se repetem de 3 a 8 s*/

                ticksA = GetTickCount();

                WaitForSingleObject(hTimeOut, (randon));
                GetLastError();

                /*Passar alarmes criticos para a tarefa de exibicao de alarmes*/
                WriteFile(hMailslotClienteAlarmeA, &PIMS, sizeof(PIMS), &dwBytesLidos, NULL);
                GetLastError();
            }

            nTipoEvento = -1;
        } /*fim do for*/
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(hMailslotClienteAlarmeA);
    CloseHandle(MutexBuffer);
    CloseHandle(SemLivre);
    CloseHandle(Events);

    /*------------------------------------------------------------------------------*/
    /*Finalizando a thread leitura do PIMS*/
    printf("Finalizando - Leitura do PIMS\n");
    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
} /*fim do LeituraPIMS*/

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE CAPTURA DE DADOS DO PROCESSO*/
/*  CAPTURA DE DADOS EM MEMORIA PARA GRAVACAO EM ARQUIVO*/
/*  SINALIZACAO DA GRAVACAO A TAREFA DE EXIBICAO DE DADOS DE PROCESSO*/

void* CapturaDados(void* arg) {
    /*------------------------------------------------------------------------------*/
    /*Declarando variaveis locais da funcao de CapturaDados()*/
    int     index = (int)arg, status, i, nTipoEvento;

    char    SDCD[52];

    DWORD   ret, dwBytesToWrite, dwBytesWritten, dwPos;
    
    /*------------------------------------------------------------------------------*/
    /*Vetor com handles da tarefa*/
    HANDLE Events[2]        = { hEventKeyD, hEventKeyEsc },
           SemOcupado[2]    = { hSemOcupado, hEventKeyEsc },
           MutexBuffer[2]   = { hMutexBuffer, hEventKeyEsc };

    /*------------------------------------------------------------------------------*/
    /*Loop de execucao*/
    while (key != ESC_KEY) {        
        nTipoEvento = -1;
        
        /*------------------------------------------------------------------------------*/
        /*Bloqueio e desbloqueio da thread CapturaDados*/
        ret = WaitForMultipleObjects(2, Events, FALSE, 1);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        if (nTipoEvento == 0) {
            printf("\x1b[31m""BLOQUEADO""\x1b[0m"" - Thread Captura de dados do processo\n");

            ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            printf("\x1b[32m""DESBLOQUEADO""\x1b[0m"" - Thread Captura de dados do processo\n");
        }

        /*Condicao para termino do processo*/
        if (nTipoEvento == 1) {
            key = ESC_KEY;
        }

        /*------------------------------------------------------------------------------*/
        /*Leitura dos dados gerados e gravados em memoria do SDCD*/

        /*Esperando o semaforo de espacos ocupados*/
        ret = WaitForMultipleObjects(2, SemOcupado, FALSE, INFINITE);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        /*Condi??o para termino do processo*/
        if (nTipoEvento == 1) {
            key = ESC_KEY;
        }
        else if (nTipoEvento == 0) {
            /*Conquistando o mutex da secao critica*/
            ret = WaitForMultipleObjects(2, MutexBuffer, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            if (nTipoEvento == 1) {
                key = ESC_KEY;
            }
            else if (nTipoEvento == 0) {
                /*Selecao dos dados apenas de tipo 1*/
                if (RamBuffer[p_ocup][7] == '1') {
                    /*Lendo dados gravados em memoria*/
                    char aux[54];
                    for (int j = 0; j < 54; j++) {
                        if (j < 52) 
                        {
                            SDCD[j] = RamBuffer[p_ocup][j];
                            aux[j] = SDCD[j];
                        }
                        else if(j == 52)
                        {
                            aux[j] = '\r';
                        }
                        else if (j == 53)
                        {
                            aux[j] = '\n';
                        }
                    }

                    /*Setando valores dos parametros das funcoes de tratamento de arquivo*/
                    dwBytesToWrite = (DWORD)strlen(aux) * sizeof(char) - 11;
                    dwBytesWritten = 0;
                    dwPos = SetFilePointer(hFile, n_mensagem * dwBytesToWrite, NULL, FILE_BEGIN);

                    /*Semaforo que espera o arquivo de disco*/
                    status = WaitForSingleObject(hArquivo, 0);
                    if (status == WAIT_TIMEOUT) {
                        printf("\n\t[Arquivo Cheio] captura de dados bloqueada\n");
                        WaitForSingleObject(hArquivoCheio, INFINITE);
                    }
                    else {
                        ResetEvent(hArquivoCheio);
                    }

                    /*Bloqueado acesso ao arquivo*/
                    LockFile(hFile, 0, NULL, 52 * FILE_SIZE, NULL);
                    
                    /*Escrita em arquivo*/
                    status = WriteFile(hFile, aux, dwBytesToWrite, &dwBytesWritten, NULL);
                    if (status != 0){
                        n_mensagem = (n_mensagem + 1) % FILE_SIZE;
                    }

                    /*Desbloqueando acesso ao arquivo*/
                    UnlockFile(hFile, 0, NULL, 52 * FILE_SIZE, NULL);

                    /*Tratamento de erros de abertura de arquivo*/
                    if (dwBytesWritten != dwBytesToWrite) {
                            printf("Erro: Numero de dados de escrita ? diferente dos dados escritos\n");
                    }

                    /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                    p_ocup = (p_ocup + 1) % RAM;

                    /*Liberando o semaforo de espacos livres*/
                    ReleaseSemaphore(hSemLivre, 1, NULL);
                    GetLastError();
                }
                else {
                    /*Liberando o semaforo de espacos ocupados*/
                    ReleaseSemaphore(hSemOcupado, 1, NULL);
                    GetLastError();
                }

                /*Liberando o mutex da secao critica*/
                status = ReleaseMutex(hMutexBuffer);
                GetLastError();
            }
        }
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(MutexBuffer);
    CloseHandle(SemOcupado);
    CloseHandle(Events);

    /*------------------------------------------------------------------------------*/
    /*Finalizando a thread de captura de dados do processo*/
    printf("Finalizando - Captura de dados do processo\n");
    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE CAPTURA DE ALARMES*/
/*  RETIRA AS MENSAGENS DE ALARMES NAO CRITICOS DA MEMORIA*/
/*  REPASSAGEM DAS MESMAS PARA A TAREFA DE EXIBICAO DE ALARMES*/

void* CapturaAlarmes(void* arg) {
    /*------------------------------------------------------------------------------*/
    /*Declarando variaveis locais da funcao CapturaDados()*/
    int     index = (int)arg, status, i, nTipoEvento;

    char    PIMS[31];

    DWORD   ret, dwBytesLidos, dwBytesToWrite, dwBytesWritten, dwPos;
    
    /*------------------------------------------------------------------------------*/
    /*Vetor com handles da tarefa*/
    HANDLE  Events[2]       = { hEventKeyA, hEventKeyEsc }, 
            SemOcupado[2]   = { hSemOcupado, hEventKeyEsc },
            MutexBuffer[2]  = { hMutexBuffer, hEventKeyEsc };

    /*------------------------------------------------------------------------------*/
    /*Loop de execucao*/
    while (key != ESC_KEY) {
        /*------------------------------------------------------------------------------*/
        /*Bloqueio e desbloqueio da thread CapturaAlarmes*/
        ret = WaitForMultipleObjects(2, Events, FALSE, 1);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        if (nTipoEvento == 0) {
            printf("\x1b[31m""BLOQUEADO""\x1b[0m"" - Thread Captura de alarmes\n");

            ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            printf("\x1b[32m""DESBLOQUEADO""\x1b[0m"" - Thread Captura alarmes\n");
        }

        /*Condicao para termino do processo*/
        if (nTipoEvento == 1) {
            key = ESC_KEY;
        }

        /*------------------------------------------------------------------------------*/
        /*Leitura dos dados gerados em memoria*/
        
        /*Esperando o semaforo de espacos ocupados*/
        ret = WaitForMultipleObjects(2, SemOcupado, FALSE, INFINITE);
        GetLastError();

        nTipoEvento = ret - WAIT_OBJECT_0;

        /*Condi??o para termino do processo*/
        if (nTipoEvento == 1) {
            key = ESC_KEY;
        }
        else if (nTipoEvento == 0) {
            /*Conquistando o mutex da secao critica*/
            ret = WaitForMultipleObjects(2, MutexBuffer, FALSE, INFINITE);
            GetLastError();

            nTipoEvento = ret - WAIT_OBJECT_0;

            if (nTipoEvento == 1) {
                key = ESC_KEY;
            }
            else if (nTipoEvento == 0) {
                /*Selecao dos dados apenas de tipo 1*/
                if (RamBuffer[p_ocup][7] == '2') {
                    /*Lendo dados gravados em memoria*/
                    for (int j = 0; j < 31; j++) {
                         PIMS[j] = RamBuffer[p_ocup][j];
                    }

                    /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                    p_ocup = (p_ocup + 1) % RAM;

                    /*Passar alarmes nao-criticos para a tarefa de exibicao de alarmes*/
                    WriteFile(hMailslotClienteAlarmeA, &PIMS, sizeof(PIMS), &dwBytesLidos, NULL);

                    /*Liberando o semaforo de espacos livres*/
                    ReleaseSemaphore(hSemLivre, 1, NULL);
                    GetLastError();
                }
                else {
                    /*Liberando o semaforo de espacos ocupados*/
                    ReleaseSemaphore(hSemOcupado, 1, NULL);
                    GetLastError();
                }

                /*Liberando o mutex da secao critica*/
                status = ReleaseMutex(hMutexBuffer);
                GetLastError();
            }
        }
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(MutexBuffer);
    CloseHandle(SemOcupado);
    CloseHandle(Events);

    /*------------------------------------------------------------------------------*/
    /*Finalizando a thread de captura de alarmes*/
    printf("Finalizando - Captura de alarmes\n");
    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}