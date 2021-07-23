/* =================================================================================================== */
/*  CABECALHO*/
/*
    UNIVERSIDADE FEDERAL DE MINAS GERAIS
 
    Trabalho pratico
    Automacao em tempo real (ELT012)
  
    Professor: 
    Luiz Themystokliz S. Mendes
 
    Alunos:
    Estevao Coelho Kiel de Oliveira - 2016119416
    Italo Jose Dias - 2017002121 
*/

/* =================================================================================================== */
/*  INTRUCOES PARA UTILIZACAO DA BIBLIOTECA PTHREAD*/
/*
    Para que a biblioteca Pthreads funcione corretamente e necessario tomar alguns cuidados
  
    1.  A biblioteca Pthreads 2.9.1 deve estar instalada no seu computador
        em C:\Program Files\pthreads-w32-2-9-1-release
 
    2.  No Visual studio Comunity Edition selecione
        Project -> Properties -> Configuration Properties -> C/C++ -> General
        e defina em "Additional Include Directories" o diret�rio onde encontram-se os
        "header files" da distribuicao pthreads em
        C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\include

    3.  Agora selecione Project -> Properties -> Configuration Properties -> Linker -> General
        e defina em "Additional Library Directories" o diretorio onde se encontra
        a biblioteca Pthreads (extens�o .LIB) em
        C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\lib\x86
 
    4.  Depois selecione Project -> Properties -> Configuration Properties -> Linker -> Input
        e declare a biblioteca "pthreadVC2.lib" em "Additional Dependencies"
 
    5.  Por fim, selecione Project -> Properties -> Configuration Properties -> Debugging 
        e entao preencha o item "Environment" com
        PATH=C:\Program Files\pthreads-w32-2-9-1-release\Pre-built.2\dll\x86
*/

/* =================================================================================================== */
/*  DEFINE AREA*/

#define HAVE_STRUCT_TIMESPEC
#define WIN32_LEAN_AND_MEAN
#define _CHECKERROR	    1				                            // Ativa funcao CheckForError
#define RAM             100
#define DISC            200

#define	S_KEY			115
#define	P_KEY			112
#define	D_KEY			100
#define	A_KEY			97
#define	O_KEY			111
#define	C_KEY			99
#define	ESC_KEY			27

/* =================================================================================================== */
/*  INCLUDE AREA*/

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <conio.h>		                                          // _getch()
#include <math.h>                                                 // pow()
#include <string.h>                                               // strcpy()
#include <time.h>
#include "CheckForError.h"

/* =================================================================================================== */
/*  DECLARACAO DO PROTOTIPO DE FUNCAO DAS THREADS SECUNDARIAS*/

void* LeituraSDCD(void* arg);
void* LeituraPIMS(void* arg);
void* CapturaDados(void* arg);
void* CapturaAlarmes(void* arg);
void* ExibicaoDados(void* arg);
void* ExibicaoAlarmes(void* arg);

/* =================================================================================================== */
/*  DECLARACAO DE VARIAVEIS GLOBAIS*/

char RamBuffer[RAM][52];
char DiscBuffer[DISC][52];

/* =================================================================================================== */
/*  THREAD PRIMARIA*/
/*  CRIACAO DAS THREADS E TRATAMENTO DO TECLADO*/

int main() {

    pthread_t hLeituraSDCD, hLeituraPIMS, hCapturaDados, hCapturaAlarmes, hExibicaoDados, hExibicaoAlarmes;

/*  DECLARACAO VARIAVEIS LOCAIS DA MAIN*/
    int status, i, key;
    char ProcessA[] = "\\Debug\\ExibicaoDados.exe";
    char ProcessB[] = "\\Debug";


    

/*  CRIACAO DAS THREADS SECUNDARIAS*/
    i = 1;
    status = pthread_create(&hLeituraSDCD, NULL, LeituraSDCD, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hLeituraSDCD);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, status);
    /*
    i = 2;
    status = pthread_create(&hLeituraPIMS, NULL, LeituraPIMS, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hLeituraPIMS);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, status);
    
    i = 3;
    status = pthread_create(&hCapturaDados, NULL, CapturaDados, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hCapturaDados);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, status);

    i = 4;
    status = pthread_create(&hCapturaAlarmes, NULL, CapturaAlarmes, (void*)i);
    if (!status) printf("Thread %d criada com Id= %0x \n", i, (int)&hCapturaAlarmes);
    else printf("Erro na criacao da thread %d! Codigo = %d\n", i, status);*/

/*  CRIACAO DOS PROCESOS - TERMINAL A E B*/
    BOOL statusProcess;
    STARTUPINFO si;				    // StartUpInformation para novo processo
    PROCESS_INFORMATION NewProcess;	// Informa��es sobre novo processo criado

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);	// Tamanho da estrutura em bytes


    statusProcess = CreateProcess(
        ProcessA, // Caminho do arquivo execut�vel
        NULL,                       // Apontador p/ par�metros de linha de comando
        NULL,                       // Apontador p/ descritor de seguran�a
        NULL,                       // Idem, threads do processo
        FALSE,	                     // Heran�a de handles
        NORMAL_PRIORITY_CLASS,	     // Flags de cria��o
        NULL,	                     // Heran�a do ambiente de execu��o
        ProcessB,              // Diret�rio do arquivo execut�vel
        &si,			             // lpStartUpInfo
        &NewProcess);	             // lpProcessInformation
    if (!statusProcess) printf("Erro na criacao do Notepad! Codigo = %d\n", GetLastError());

/*  TRATAMENTO INPUTS DO TECLADO*/
    while (TRUE) {
        key = _getch();
        switch (key) {
        case S_KEY:
            printf("Voce digitou a tecla s, o numero dela e %d\n", key);
            break;
        case P_KEY:
            printf("Voce digitou a tecla p, o numero dela e %d\n", key);
            break;
        case D_KEY:
            printf("Voce digitou a tecla d, o numero dela e %d\n", key);
            break;
        case A_KEY:
            printf("Voce digitou a tecla a, o numero dela e %d\n", key);
            break;
        case O_KEY:
            printf("Voce digitou a tecla o, o numero dela e %d\n", key);
            break;
        case C_KEY:
            printf("Voce digitou a tecla c, o numero dela e %d\n", key);
            break;
        case ESC_KEY:
            printf("Voce digitou a tecla esc, o numero dela e %d\n", key);
            break;
        default:
            break;;
        }
    } /*fim do while*/

    return EXIT_SUCCESS;
} /*fim da funcao main*/

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA SDCD*/
/*  GERACAO DE VALORES E GRAVACAO DOS MESMOS EM MEMORIA RAM*/
/*
    [] FALTA FAZER AS VARIAVEIS COM OS VALORES DOS CAMPOS
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
            for (int j = 0; j < 52; j++) {
                printf("%c", SDCD[j]);
            }

            printf("\n");

            //printf("%d ", index);
            //Sleep(1);	// delay de 1000 ms
        }
    }

    pthread_exit((void*)index);

    // O comando "return" abaixo � desnecess�rio, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)index;
}

/* =================================================================================================== */
/*  THREAD SECUNDARIA DE LEITURA PIMS*/
/*  GERACAO DE VALORES E GRAVACAO DOS MESMOS EM MEMORIA RAM*/
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

    // O comando "return" abaixo � desnecess�rio, mas presente aqui para compatibilidade
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

    // O comando "return" abaixo � desnecess�rio, mas presente aqui para compatibilidade
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

    // O comando "return" abaixo � desnecess�rio, mas presente aqui para compatibilidade
    // com o Visual Studio da Microsoft
    return (void*)index;
}