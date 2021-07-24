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
*/

/* ======================================================================================================================== */
/*  DEFINE AREA*/

#define HAVE_STRUCT_TIMESPEC
#define WIN32_LEAN_AND_MEAN

#define _CHECKERROR	    1				                                       /*Ativa funcao CheckForError*/

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
#include <conio.h>		                                                       /*_getch()*/
#include <math.h>                                                              /*pow()*/
#include <time.h>
#include "CheckForError.h"                                                     /*CheckForError()*/

/* ======================================================================================================================== */
/*  DECLARACAO DO PROTOTIPO DE FUNCAO DAS THREADS SECUNDARIAS*/

void* LeituraSDCD(void* arg);
void* LeituraPIMS(void* arg);
void* CapturaDados(void* arg);
void* CapturaAlarmes(void* arg);

/* ======================================================================================================================== */
/*  DECLARACAO DAS VARIAVEIS GLOBAIS*/

/*Espaco destinado a lista circular na memoria RAM*/
char RamBuffer[RAM][52];

/*Variaveis de controle das posicoes na lista circular*/
int p_ocup = 0, p_livre = 0;

/* ======================================================================================================================== */
/*  THREAD PRIMARIA*/
/*  CRIACAO DAS THREADS SECUNDARIAS, PROCESSOS FILHOS E TRATAMENTO DAS ENTRADAS DO TECLADO*/

int main() {

    /*Declarando handles das threads*/
    pthread_t hLeituraSDCD, hLeituraPIMS, hCapturaDados, hCapturaAlarmes;

    /*Declarando variaveis locais main()*/
    int i;
    char key;
    BOOL status;

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

    /*Criando processos filhos*/
    STARTUPINFO si;				                                               /*StartUpInformation para novo processo*/
    PROCESS_INFORMATION NewProcess;	                                           /*Informacoes sobre novo processo criado*/

    SetConsoleTitle(L"TERMINAL PRINCIPAL");                                    /*Nomeando o terminal da thread primaria*/

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

    /*Tratando inputs do teclado*/
    while (TRUE) {
        key = _getch();
        switch (key) {
        case 's':
        case 'S':
            printf("Voce digitou a tecla S\n");
            break;
        case 'p':
        case 'P':
            printf("Voce digitou a tecla P\n");
            break;
        case 'd':
        case 'D':
            printf("Voce digitou a tecla D\n");
            break;
        case 'a':
        case 'A':
            printf("Voce digitou a tecla A\n");
            break;
        case 'o':
        case 'O':
            printf("Voce digitou a tecla O\n");
            break;
        case 'c':
        case 'C':
            printf("Voce digitou a tecla C\n");
            break;
        case ESC_KEY:
            printf("Voce digitou a tecla esc\n");
            break;
        default:
            printf("Voce digitou uma tecla sem funcao\n");
            break;
        }
    } /*fim do while*/

    return EXIT_SUCCESS;
} /*fim da funcao main*/















/* =================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA SDCD*/
/*  GERACAO DE VALORES E GRAVACAO DOS MESMOS NA LISTA CIRCULAR EM MEMORIA RAM*/
/*
    [] FALTA GRAVACAO DE VALORES NA MEMORIA CIRCULAR COM USO DE MUTEX
    [] FALTA COMENTAR NO PADRAO
*/

void* LeituraSDCD(void* arg) {
    
    int     index = (int)arg,
            k = 0, i = 0, l = 0,
            NSEQ, TIPO;

    char    SDCD[52], UE[9] = "        ", MODO[1], AM[3] = "AM",
            CaracterAleatorio[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
            Unidade[11][9]{ "kg      ", "kg/m^2  ", "A       ", "V       ", "T       ", "Nm      ",
                            "m       ", "kgf     ", "N       ", "m/s     ", "m/s^2   " },
            Hora[3], Minuto[3], Segundo[3], MiliSegundo[4];

    while (true)
    {
        for (i = 1; i < 1000000; ++i) {

            //NSEQ
            NSEQ = i;
            for (int j = 0; j < 6; j++)
            {
                k = i / pow(10, (5 - j));
                k = k % 10;
                SDCD[j] = k + '0';
            }
            SDCD[6] = '|';

            //TIPO
            TIPO = 1;
            SDCD[7] = TIPO + '0';
            SDCD[8] = '|';

            //TAG
            for (int j = 9; j < 19; j++)
            {
                if (j == 12 || j == 15)
                {
                    SDCD[j] = '-';
                }
                else {
                    SDCD[j] = CaracterAleatorio[(rand()%36)];
                }
                
            }
            SDCD[19] = '|';

            //VALOR
            for (int j = 20; j < 28; j++)
            {
                if (j == 25)
                {
                    SDCD[j] = '.';
                }
                else {
                    SDCD[j] = (rand() % 10)+'0';
                }

            }
            SDCD[28] = '|';

            //UE
            k = (rand() % 11);
            for (int j = 0; j < 9; j++)
            {
                UE[j] = Unidade[k][j];
            }

            k = 0;
            for (int j = 29; j < 37; j++)
            {
                SDCD[j] = UE[k];
                k++;
            }
            SDCD[37] = '|';

            //MODO
            SDCD[38] = AM[(rand() % 2)];
            SDCD[39] = '|';

            //TIMESTAMP
            SYSTEMTIME st;
            GetLocalTime(&st);

            //HORA
            k = 0;
            l = 40;
            for (int j = 0; j < 2; j++)
            {
                k = st.wHour / pow(10, (1 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }
            SDCD[42] = ':';

            //MINUTO
            k = 0;
            l = 43;
            for (int j = 0; j < 2; j++)
            {
                k = st.wMinute / pow(10, (1 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }
            SDCD[45] = ':';

            //SEGUNDO
            k = 0;
            l = 46;
            for (int j = 0; j < 2; j++)
            {
                k = st.wSecond / pow(10, (1 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }
            SDCD[48] = '.';

            //MILISEGUNDO
            k = 0;
            l = 49;
            for (int j = 0; j < 3; j++)
            {
                k = st.wMilliseconds / pow(10, (2 - j));
                k = k % 10;
                SDCD[l] = k + '0';
                l++;
            }

            //IMPRIME A MENSAGEM do SDCD
            printf("SDCD\n");
            for (int j = 0; j < 52; j++) {
                printf("%c", SDCD[j]);
                RamBuffer[p_livre][j] = SDCD[j];
            }
            printf("\nRAM -> p_livre = %d\n", p_livre);
            for (int j = 0; j < 52; j++) {
                printf("%c", RamBuffer[p_livre][j]);
            }

            p_livre = (p_livre + 1) % RAM;

            printf("\n");

            //printf("%d ", index);
            //Sleep(1);	// delay de 1000 ms
        }
    }

    pthread_exit((void*)index);

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)index;
}

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA PIMS*/
/*  GERACAO DE VALORES E GRAVACAO DOS MESMOS NA LISTA CIRCULAR EM MEMORIA RAM*/
/*
    [] FALTA GRAVACAO DE VALORES NA MEMORIA CIRCULAR COM USO DE MUTEX
    [] FALTA COMENTAR NO PADRAO
*/

void* LeituraPIMS(void* arg) {
    int     index = (int)arg,
            k = 0, i = 0, l = 0,
            NSEQ, TIPO, IDALARME, GRAU, PREV, TIMESTAMP;

    char    PIMS[31], CriticoNaoCritico[3] = "29",
            Hora[3], Minuto[3], Segundo[3];

    while (true)
    {
        for (i = 1; i < 1000000; ++i) {

            //NSEQ
            NSEQ = i;
            for (int j = 0; j < 6; j++)
            {
                k = i / pow(10, (5 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[6] = '|';

            //TIPO
            PIMS[7] = CriticoNaoCritico[(rand() % 2)];
            TIPO = PIMS[7] - '0';
            PIMS[8] = '|';

            //ID ALARME
            IDALARME = rand() % 10000;
            k = 0;
            for (int j = 9; j < 13; j++)
            {
                k = IDALARME / pow(10, (12 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[13] = '|';

            //GRAU
            GRAU = rand() % 100;
            k = 0;
            for (int j = 14; j < 16; j++)
            {
                k = GRAU / pow(10, (15 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[16] = '|';

            //PREV
            PREV = rand() % 14441;
            k = 0;
            for (int j = 17; j < 22; j++)
            {
                k = PREV / pow(10, (21 - j));
                k = k % 10;
                PIMS[j] = k + '0';
            }
            PIMS[22] = '|';

            //TIMESTAMP
            SYSTEMTIME st;
            GetLocalTime(&st);

            //HORA
            k = 0;
            l = 23;
            for (int j = 0; j < 2; j++)
            {
                k = st.wHour / pow(10, (1 - j));
                k = k % 10;
                PIMS[l] = k + '0';
                l++;
            }
            PIMS[25] = ':';

            //MINUTO
            k = 0;
            l = 26;
            for (int j = 0; j < 2; j++)
            {
                k = st.wMinute / pow(10, (1 - j));
                k = k % 10;
                PIMS[l] = k + '0';
                l++;
            }
            PIMS[28] = ':';

            //SEGUNDO
            k = 0;
            l = 29;
            for (int j = 0; j < 2; j++)
            {
                k = st.wSecond / pow(10, (1 - j));
                k = k % 10;
                PIMS[l] = k + '0';
                l++;
            }

            //IMPRIME A MENSAGEM do SDCD
            for (int j = 0; j < 31; j++) {
                printf("%c", PIMS[j]);
            }

            printf("\n");

            //printf("%d ", index);
            //Sleep(1);	// delay de 1000 ms
        }
    }
    pthread_exit((void*)index);

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)index;
}

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE CAPTURA DE DADOS DO PROCESSO*/
/*  XXXX*/
/*
    [] FALTA TUDO
*/

void* CapturaDados(void* arg) {
    int index, i;

    index = (int)arg;

    for (i = 0; i < 100000; ++i) {
        printf("%d ", index);
        Sleep(1);	// delay de 1 ms
    }

    pthread_exit((void*)index);

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)index;
}

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE CAPTURA DE ALARMES
/*  XXXX*/
/*
    [] FALTA TUDO
*/

void* CapturaAlarmes(void* arg) {
    int index, i;

    index = (int)arg;

    for (i = 0; i < 100000; ++i) {
        printf("%d ", index);
        Sleep(1);	// delay de 1 ms
    }

    pthread_exit((void*)index);

    // O comando "return" abaixo é desnecessário, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)index;
}