#ifndef _FUNCOES_BUSCA_
#define _FUNCOES_BUSCA_

#include <string>
#include "listabusca.h"

typedef char** args;
typedef void (*PFuncao) ( ListaB * lista, int tamanho );

int gerarTabela(std::string origem);
bool Ler_Buscas(int argc, args argv);
bool buscarNaTabela(int argc, args argv, std::string arquivo, ListaB& listaBusca, string dataHora);
bool buscaPorArquivo(int argc, args argv);
bool buscaBOR(ListaB* lista, int tamanho, int argc, args argv);
bool buscaBAND(ListaB* lista, int tamanho, int numerodepalavras, int argc, args argv);
bool listaLinhas(string arquivo, char * linhas,ListaB &listaBusca, char* chave, string dataHora);
int inicioPalavrasBusca(int argc, args argv);

#endif