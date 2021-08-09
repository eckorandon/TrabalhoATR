CABECALHO

    UNIVERSIDADE FEDERAL DE MINAS GERAIS
 
    Trabalho pratico
    Automacao em tempo real (ELT012)
  
    Professor: 
    Luiz Themystokliz S. Mendes
 
    Alunos:
    Estevao Coelho Kiel de Oliveira
    Italo Jose Dias

    Data: Agosto de 2021

    Aplicacao de software multithread responsavel pela leitura de dados tanto de de um Sistema Digital de Controle Distribuido 
    (SDCD) quanto de um Plant Information Management System (PIMS) ficticios. Os mesmos serao apresentados em dois terminais
    de video. O primeiro ira exibir os dados do processo de fabricacao de celulose aos operadores (TERMINAL A) e o segundo
    apresentara previsoes de falhas operacionais geradas pelo PIMS (TERMINAL B).


INTRUCOES PARA UTILIZACAO DA BIBLIOTECA PTHREAD

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


INTRUCOES PARA UTILIZACAO DA BIBLIOTECA CHECKFORERROR

    Para que a biblioteca CheckForError funcione corretamente e necessario tomar alguns cuidados

    1.  No Visual studio Comunity Edition selecione
        Project -> Properties -> Configuration Properties -> C/C++ -> Language
        Em "Conformance Mode" selecione a opcao "No(/permissive)".

    2.  O arquivo CheckForError.h deve se encontrar dentro da pasta do projeto.

    3.  Repita o mesmo passo para todos os projetos de uma mesma solucao.
