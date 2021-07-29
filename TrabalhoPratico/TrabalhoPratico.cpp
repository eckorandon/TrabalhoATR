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
/*  INTRUCOES PARA UTILIZACAO DA BIBLIOTECA PTHREAD*/
/*
    Para que a biblioteca Pthreads funcione corretamente e necessario tomar alguns cuidados
  
    1.  A biblioteca Pthreads 2.9.1 deve estar instalada no seu computador
        em C:\Program Files\pthreads-w32-2-9-1-release
 
    2.  No Visual studio Comunity Edition selecione
        Project -> Properties -> Configuration Properties -> C/C++ -> General
        e defina em "Additional Include Directories" o diretório onde encontram-se os
        "header files" da distribuicao pthreads em
        C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\include

    3.  Agora selecione Project -> Properties -> Configuration Properties -> Linker -> General
        e defina em "Additional Library Directories" o diretorio onde se encontra
        a biblioteca Pthreads (extensão .LIB) em
        C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\lib\x86
 
    4.  Depois selecione Project -> Properties -> Configuration Properties -> Linker -> Input
        e declare a biblioteca "pthreadVC2.lib" em "Additional Dependencies"
 
    5.  Por fim, selecione Project -> Properties -> Configuration Properties -> Debugging 
        e entao preencha o item "Environment" com
        PATH=C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\dll\x86

    6. Certifique-se de que o seu projeto esta como x86. Do lado de Debug é possivel alterar
       este parametro.
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

#define HAVE_STRUCT_TIMESPEC
#define WIN32_LEAN_AND_MEAN

#define _CHECKERROR	    1				                                        /*Ativa funcao CheckForError*/

/*Tamanho da lista circular em memoria ram*/
#define RAM             100

/*Codigo ASCII para a tecla esc*/
#define	ESC_KEY			27

/* ======================================================================================================================== */
/*  INCLUDE AREA*/

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <conio.h>		                                                        /*_getch()*/
#include <math.h>                                                               /*pow()*/
#include <time.h>
#include "CheckForError.h"                                                      /*CheckForError()*/

/* ======================================================================================================================== */
/*  DECLARACAO DO PROTOTIPO DE FUNCAO DAS THREADS SECUNDARIAS*/

void* LeituraSDCD(void* arg);
void* LeituraPIMS(void* arg);
void* CapturaDados(void* arg);
void* CapturaAlarmes(void* arg);

/* ======================================================================================================================== */
/*  DECLARACAO DAS VARIAVEIS GLOBAIS*/

/*Espaco destinado a lista circular na memoria RAM*/
char    RamBuffer[RAM][52];

/*Variaveis de controle das posicoes na lista circular*/
int     p_ocup = 0, p_livre = 0;

/* ======================================================================================================================== */
/*  HANDLE MUTEX*/

HANDLE hMutexBuffer;

/* ======================================================================================================================== */
/*  HANDLE EVENTOS*/

HANDLE hEventKeyS, hEventKeyP, hEventKeyD, hEventKeyA, hEventKeyO, hEventKeyC, hEventKeyEsc;

/* ======================================================================================================================== */
/*  THREAD PRIMARIA*/
/*  CRIACAO DAS THREADS SECUNDARIAS E PROCESSOS FILHOS*/ 
/*  TAREFA DE LEITURA DO TECLADO*/
/*
    TAREFAS
    [X] Criacao de threads
    [X] Criacao de processos
    [ ] Tratar inputs do teclado
    [ ] Fechar threads e processos
*/

int main() {
    /*------------------------------------------------------------------------------*/
    /*Declarando variaveis locais main()*/
    int i, status;
    char key;
    
    /*------------------------------------------------------------------------------*/
    /*Criando mutex*/
    hMutexBuffer = CreateMutex(NULL, FALSE, L"MutexBuffer");
    CheckForError(hMutexBuffer);

    /*------------------------------------------------------------------------------*/
    /*Criando eventos*/
    hEventKeyS      = CreateEvent(NULL, FALSE, FALSE, L"KeyS");
    CheckForError(hEventKeyS);

    hEventKeyP      = CreateEvent(NULL, FALSE, FALSE, L"KeyP");
    CheckForError(hEventKeyP);

    hEventKeyD      = CreateEvent(NULL, FALSE, FALSE, L"KeyD");
    CheckForError(hEventKeyD);

    hEventKeyA      = CreateEvent(NULL, FALSE, FALSE, L"KeyA");
    CheckForError(hEventKeyA);

    hEventKeyO      = CreateEvent(NULL, FALSE, FALSE, L"KeyO");
    CheckForError(hEventKeyO);

    hEventKeyC      = CreateEvent(NULL, FALSE, FALSE, L"KeyC");
    CheckForError(hEventKeyC);

    hEventKeyEsc    = CreateEvent(NULL, FALSE, FALSE, L"KeyEsc");
    CheckForError(hEventKeyEsc);

    /*------------------------------------------------------------------------------*/
    /*Handles threads*/
    pthread_t hLeituraSDCD, hLeituraPIMS, hCapturaDados, hCapturaAlarmes;

    /*Criando threads secundarias*/
    i = 1;
    status = pthread_create(&hLeituraSDCD, NULL, LeituraSDCD, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hLeituraSDCD);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    i = 2;
    status = pthread_create(&hLeituraPIMS, NULL, LeituraPIMS, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hLeituraPIMS);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    i = 3;
    status = pthread_create(&hCapturaDados, NULL, CapturaDados, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hCapturaDados);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    i = 4;
    status = pthread_create(&hCapturaAlarmes, NULL, CapturaAlarmes, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hCapturaAlarmes);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, GetLastError());
    
    /*------------------------------------------------------------------------------*/
    /*Criando processos filhos*/
    STARTUPINFO si;				                                               /*StartUpInformation para novo processo*/
    PROCESS_INFORMATION NewProcess;	                                           /*Informacoes sobre novo processo criado*/

    /*Nomeando o terminal da thread primaria*/
    SetConsoleTitle(L"TERMINAL PRINCIPAL");                                    

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);	                                                       /*Tamanho da estrutura em bytes*/

    /*Processo de exibicao de dados - TERMINAL A*/
    status = CreateProcess(
        L"..\\Debug\\ExibicaoDados.exe",                                       /*Caminho relativo do arquivo executavel*/
        NULL,                                                                  /*Apontador para parametros de linha de comando*/
        NULL,                                                                  /*Apontador para descritor de segurança*/
        NULL,                                                                  /*Apontador para threads do processo*/
        FALSE,	                                                               /*Heranca de handles*/
        CREATE_NEW_CONSOLE,	                                                   /*Flags de criação - Criar novo console no caso*/
        NULL,	                                                               /*Heranca do ambiente de execucao*/
        L"..\\Debug",                                                          /*Diretorio do arquivo executavel*/
        &si,			                                                       /*lpStartUpInfo*/
        &NewProcess);	                                                       /*lpProcessInformation*/
    if (!status) printf("Erro na criacao do Terminal A! Codigo = %d\n", GetLastError());

    /*Processo de exibicao de alarmes - TERMINAL B*/
    status = CreateProcess(
        L"..\\Debug\\ExibicaoAlarmes.exe",                                     /*Caminho relativo do arquivo executavel*/
        NULL,                                                                  /*Apontador para parametros de linha de comando*/
        NULL,                                                                  /*Apontador para descritor de segurança*/
        NULL,                                                                  /*Apontador para threads do processo*/
        FALSE,	                                                               /*Heranca de handles*/
        CREATE_NEW_CONSOLE,	                                                   /*Flags de criação - Criar novo console no caso*/
        NULL,	                                                               /*Heranca do ambiente de execucao*/
        L"..\\Debug",                                                          /*Diretorio do arquivo executavel*/
        &si,			                                                       /*lpStartUpInfo*/
        &NewProcess);	                                                       /*lpProcessInformation*/
    if (!status) printf("Erro na criacao do Terminal B! Codigo = %d\n", GetLastError());
    
    /*------------------------------------------------------------------------------*/
    /*Tratando inputs do teclado*/
    while (TRUE) {
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
            break;
        case 'c':
        case 'C':
            SetEvent(hEventKeyC);
            GetLastError();
            printf("Voce digitou a tecla C\n");
            break;
        case ESC_KEY:
            SetEvent(hEventKeyEsc);
            GetLastError();
            printf("Voce digitou a tecla esc\n");
            break;
        default:
            printf("Voce digitou uma tecla sem funcao\n");
            break;
        } /*fim do switch*/
    } /*fim do while*/

    /*------------------------------------------------------------------------------*/
    /*Fechando handles*/
    CloseHandle(hMutexBuffer);
    CloseHandle(hEventKeyS);
    CloseHandle(hEventKeyP);
    CloseHandle(hEventKeyD);
    CloseHandle(hEventKeyA);
    CloseHandle(hEventKeyO);
    CloseHandle(hEventKeyC);
    CloseHandle(hEventKeyEsc);

    /*------------------------------------------------------------------------------*/
    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return EXIT_SUCCESS;
} /*fim da funcao main*/

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA SDCD*/
/*  GERACAO DE VALORES/CAPTURA DE MENSSAGENS DE DADOS DO SDCD*/
/*  GRAVACAO DOS MESMOS NA LISTA CIRCULAR EM MEMORIA*/
/*
    TAREFAS
    [X] Geracao de valores aleatorio
    [X] Gravacao em memoria RAM
    [X] Mutex na RAM e p_livre
    [X] Imprimir estados
    [ ] Finalizar thread quando Esc e apertado
    [ ] Bloquear quando nao tiver posicao disponivel na RAM
    [ ] Alertar quando a memoria esta cheia  no terminal main
    [ ] Continuar quando tiver uma posicao disponivel na RAM
*/

void* LeituraSDCD(void* arg) {

    /*Declarando variaveis locais LeituraSDCD()*/
    int     index = (int)arg, status, nTipoEvento,
            k = 0, i = 0, l = 0;

    char    SDCD[52], UE[9] = "        ", AM[3] = "AM",
            CaracterAleatorio[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
            Unidade[11][9]{ "kg      ", "kg/m^2  ", "A       ", "V       ", "T       ", "Nm      ",
                            "m       ", "kgf     ", "N       ", "m/s     ", "m/s^2   " },
            Hora[3], Minuto[3], Segundo[3], MiliSegundo[4];
    
    DWORD   ret;

    while (true) {

        /*Gerando valores aleatorios para os campos referentes ao SDCD e gravando-os em memoria*/
        for (i = 1; i < 1000000; ++i) {

            /*Bloqueio e desbloqueio da thread LeituraSDCD*/
            ret = WaitForSingleObject(hEventKeyS, 1);
            nTipoEvento = ret - WAIT_OBJECT_0;
            if (nTipoEvento == 0) {
                printf("BLOQUEADO - Thread Leitura SDCD\n");
                ret = WaitForSingleObject(hEventKeyS, INFINITE);
                printf("DESBLOQUEADO - Thread Leitura SDCD\n");
            }

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

            /*Gravacao dos dados gerados em memoria*/
            status = WaitForSingleObject(hMutexBuffer, INFINITE);
            if (((p_livre+1) % RAM) == p_ocup) {
                printf("MEMORIA CHEIA\n");
                //esperar posicao livre
            }
     
            for (int j = 0; j < 52; j++) {
                RamBuffer[p_livre][j] = SDCD[j];
            }

            /*PARA TESTES ============= Imprime as menssagems ============= PARA TESTES*/
            /*
                printf("Thread %d ", index);

                printf("SDCD\n");
                for (int j = 0; j < 52; j++) {
                    printf("%c", SDCD[j]);
                }

                printf("\n");

                printf("RAM -> p_livre = %d\n", p_livre);
                for (int j = 0; j < 52; j++) {
                    printf("%c", RamBuffer[p_livre][j]);
                }

                printf("\n");
            */

            /*Movendo a posicao de livre para o proximo slot da memoria circular*/
            p_livre = (p_livre + 1) % RAM;

            /*Liberando o mutex da secao critica*/
            status = ReleaseMutex(hMutexBuffer);

            /*Delay em milisegundos antes do fim do laco for*/
            Sleep(1000);

        } /*fim do for*/
    } /*fim do while*/

    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA PIMS*/
/*  GERACAO DE VALORES/CAPTURA DE MENSSAGENS DO PIMS*/
/*  GRAVACAO DOS ALARMES NAO CRITICOS NA LISTA CIRCULAR EM MEMORIA*/
/*  REPASSAGEM DOS ALARMES CRITICOS PARA A TAREFA DE EXIBICAO DE ALARMES*/
/*
    TAREFAS
    [X] Geracao de valores aleatorio
    [X] Gravacao em memoria RAM
    [X] Mutex na RAM e p_livre
    [X] Imprimir estados
    [ ] Finalizar thread quando Esc é apertado
    [ ] Bloquear quando nao tiver posicao disponivel na RAM
    [ ] Alertar quando a memoria esta cheia no terminal main
    [ ] Continuar quando tiver uma posicao disponivel na RAM
*/

void* LeituraPIMS(void* arg) {
    int     index = (int)arg, status, nTipoEvento,
            k = 0, i = 0, l = 0, randon = 0, critico = 0;

    char    PIMS[31], CriticoNaoCritico[3] = "29",
            Hora[3], Minuto[3], Segundo[3];

    DWORD   ret;

    while (true) {

        for (i = 1; i < 1000000; ++i) {

            /*Bloqueio e desbloqueio da thread LeituraPIMS*/
            ret = WaitForSingleObject(hEventKeyP, 1);
            nTipoEvento = ret - WAIT_OBJECT_0;
            if (nTipoEvento == 0) {
                printf("BLOQUEADO - Thread Leitura PIMS\n");
                ret = WaitForSingleObject(hEventKeyP, INFINITE);
                printf("DESBLOQUEADO - Thread Leitura PIMS\n");
            }

            /*Valores de NSEQ - Numero sequencial de 1 ate 999999*/
            for (int j = 0; j < 6; j++) {
                k = i / pow(10, (5 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[6] = '|';

            //TIPO
            /*Valores de TIPO - 2 = nao critico e 9 = critico*/
            PIMS[7] = CriticoNaoCritico[(rand() % 2)];
            critico = PIMS[7] - '0';
            PIMS[8] = '|';

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

            /*Gravacao dos dados gerados em memoria*/
            status = WaitForSingleObject(hMutexBuffer, INFINITE);
            if (critico == 2) {
                if (((p_livre + 1) % RAM) == p_ocup) {
                    printf("MEMORIA CHEIA\n");
                    //esperar esvaziar
                }

                for (int j = 0; j < 31; j++) {
                    RamBuffer[p_livre][j] = PIMS[j];
                }

                /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                p_livre = (p_livre + 1) % RAM;
            }
            else {
                /*Passar alarmes criticos para a tarefa de exibicao de alarmes*/
                /*A ser implementado na Etapa 2 do trabalho*/
            }

            /*Liberando o mutex da secao critica*/
            status = ReleaseMutex(hMutexBuffer);

            /*PARA TESTES ============= Imprime as menssagems ============= PARA TESTES*/
            /*
                printf("Thread %d ", index);

                printf("PIMS\n");
                for (int j = 0; j < 31; j++) {
                   printf("%c", PIMS[j]);
                }

                printf("\n");
                if (critico == 2) {
                    printf("RAM -> p_livre = %d\n", p_livre);
                    for (int j = 0; j < 31; j++) {
                        printf("%c", RamBuffer[p_livre][j]);
                    }
                }
                printf("\n");
            */

            /*Delay em milisegundos antes do fim do laco for*/
            Sleep(1000);

        } /*fim do for*/
    } /*fim do while*/

    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE CAPTURA DE DADOS DO PROCESSO*/
/*  CAPTURA DE DADOS EM MEMORIA PARA GRAVACAO EM ARQUIVO*/
/*  SINALIZACAO DA GRAVACAO A TAREFA DE EXIBICAO DE DADOS DE PROCESSO*/
/*
    TAREFAS
    [X] Capturar dados da RAM e armazena temporariamente
    [X] Imprimir menssagens na console principal
    [X] Mutex na RAM e p_ocupado
    [X] Imprimir estados
    [ ] Finalizar thread quando Esc é apertado
*/

void* CapturaDados(void* arg) {

    /*Declarando variaveis locais CapturaDados()*/
    int     index = (int)arg, status, i, nTipoEvento;

    char    SDCD[52];

    DWORD   ret;

    while (true) {

        /*Bloqueio e desbloqueio da thread CapturaDados*/
        ret = WaitForSingleObject(hEventKeyD, 1);
        nTipoEvento = ret - WAIT_OBJECT_0;
        if (nTipoEvento == 0) {
            printf("BLOQUEADO - Thread Captura de dados do processo\n");
            ret = WaitForSingleObject(hEventKeyD, INFINITE);
            printf("DESBLOQUEADO - Thread Captura de dados do processo\n");
        }

        /*Leitura dos dados gerados em memoria*/
        status = WaitForSingleObject(hMutexBuffer, INFINITE);
        if (p_ocup != p_livre) {
            if (RamBuffer[p_ocup][7] == '1') {
                for (int j = 0; j < 52; j++) {
                    SDCD[j] = RamBuffer[p_ocup][j];
                }

                /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                p_ocup = (p_ocup + 1) % RAM;

                for (int j = 0; j < 52; j++) {
                    printf("%c", SDCD[j]);
                }
                printf("\n");
            }
        }

        /*Liberando o mutex da secao critica*/
        status = ReleaseMutex(hMutexBuffer);

    } /*fim do while*/

    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}

/* ======================================================================================================================== */
/*  THREAD SECUNDARIA DE CAPTURA DE ALARMES
/*  RETIRA AS MENSSAGENS DE ALARMES NAO CRITICOS EM MEMORIA*/
/*  REPASSAGEM DAS MESMAS PARA A TAREFA DE EXIBICAO DE ALARMES*/
/*
    TAREFAS
    [X] Capturar dados da RAM e armazena temporariamente
    [X] Imprimir menssagens na console principal
    [X] Mutex na RAM e p_ocupado
    [X] Imprimir estados
    [ ] Finalizar thread quando Esc é apertado
*/

void* CapturaAlarmes(void* arg) {

    /*Declarando variaveis locais CapturaDados()*/
    int     index = (int)arg, status, i, nTipoEvento;

    char    PIMS[31];

    DWORD   ret;

    while (true) {

        /*Bloqueio e desbloqueio da thread CapturaAlarmes*/
        ret = WaitForSingleObject(hEventKeyA, 1);
        nTipoEvento = ret - WAIT_OBJECT_0;
        if (nTipoEvento == 0) {
            printf("BLOQUEADO - Thread Captura de alarmes\n");
            ret = WaitForSingleObject(hEventKeyA, INFINITE);
            printf("DESBLOQUEADO - Thread Captura alarmes\n");
        }

        /*Leitura dos dados gerados em memoria*/
        status = WaitForSingleObject(hMutexBuffer, INFINITE);
        if (p_ocup != p_livre) {
            if (RamBuffer[p_ocup][7] == '2') {
                for (int j = 0; j < 31; j++) {
                    PIMS[j] = RamBuffer[p_ocup][j];
                }

                /*Movendo a posicao de livre para o proximo slot da memoria circular*/
                p_ocup = (p_ocup + 1) % RAM;

                for (int j = 0; j < 31; j++) {
                    printf("%c", PIMS[j]);
                }
                printf("\n");
            }
        }
        status = ReleaseMutex(hMutexBuffer);

    } /*fim do while*/

    pthread_exit((void*)index);

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (void*)index;
}