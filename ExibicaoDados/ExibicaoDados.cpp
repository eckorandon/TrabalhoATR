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

/* ======================================================================================================================== */
/*  TAREFA DE EXIBICAO DE DADOS DO PROCESSO*/
/*  QUANDO SINALIZADA PELA TAREFA DE CAPTURA DE DADOS RETIRA MENSSAGENS DE DADOS DE PROCESSO DO ARQUIVO*/
/*  EXIBE AS MESMAS NO TERMINAL*/
/*
    TAREFAS
    [ ] Mostrar estados bloqueio/desbloqueio
*/

int main() {
    /*Nomeando o terminal do processo*/
    SetConsoleTitle(L"TERMINAL A");                                            
    
    /*Declarando variaveis locais main()*/
    /*Valores genericos para fins de formatacao*/
    char    SDCD[76] = { 'N', 'S', 'E', 'Q', ':', '#', '#', '#', '#', ' ', 
                         'H', 'O', 'R', 'A', ':', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S', '.', 'M', 'S', 'S', ' ',
                         'T', 'A', 'G', ':', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                         'V', 'A', 'L', 'O', 'R', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ',
                         'U', 'E', ':', '#', '#', '#', '#', '#', '#', '#', '#', ' ', 
                         'M', 'O', 'D', 'O', ':', '#' };

    while (true) {

        /*PARA TESTES ============= Imprime as menssagems do SDCD ============= PARA TESTES*/
        /*
            for (int j = 0; j < 76; j++) {
                printf("%c", SDCD[j]);
            }

            printf("\n");
        */
    }

    /*Comando nao utilizado, esta aqui apenas para compatibilidade com o Visual Studio da Microsoft*/
    return (0);
}