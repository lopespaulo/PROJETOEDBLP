#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "estruturas.h"
#include "remover.h"

using namespace std;

/*
 Função que remove um arquivo da base de buscas um arquivo
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos do terminal
 @return true se removeu com sucesso, false caso a opção escolhida não seja -r
 */
bool remover(int argc, args argv){
	

	// verifica se a função chamada é remover
	if(strcmp(argv[1], "-r")  || (argc < 3)){

		return false;	
	} 


	for(int i=2; i < argc; i++){
		if(remocao(argv[i]))
			cout <<"Arquivo \"" << argv[i] << "\" removido da base de buscas." << endl;
		else
			cout <<"Arquivo \"" << argv[i] << "\" não estava na base de buscas." << endl;

	}

	return true;

}
/*
 Função que remove o arquivo da pasta
 @param arquivo - nome do arquivo a ser removido da pasta /banco
 @return true se removeu com sucesso, false caso contrário
 */
bool removerdaPasta(char* arquivo){
	
	int tamNome = strlen(arquivo);
	char caminho[6+tamNome];
	strcpy(caminho, "banco/");
	strcat(caminho, arquivo);	


	string arquivodat(caminho);

	arquivodat = arquivodat.erase(arquivodat.length()-4,4) +".dat";

	//remove retorna 0 quando remove com sucesso
	if(remove(caminho) == 0 && remove(arquivodat.c_str() ) ){

		return true;
	}
	return false;
}

/*
 Função que usa uma lista duplamente encadeada para remover o nome do arquivo do arquivo de log .txt da bade de busca
 @param nomedoarquivo - nome do arquivo a ser removido do log da base de busca
 @return true se removeu com sucesso, false caso contrário
 */
bool remocao(char* nomedoarquivo){
	
	Lista listaLog = LIS_Criar();
	string linhaLog;
	string prefixo = nomedoarquivo;
	bool verificaExistenciaArquivo = false;

	fstream arquivoLog;
	arquivoLog.open("bancodedados",ios::in);

	if(arquivoLog.is_open()){
		while(!arquivoLog.eof()){
			
			getline(arquivoLog,linhaLog);			
			LIS_InserirFim(listaLog, linhaLog);
			
		}
	}

	arquivoLog.close();

	int indice = LIS_Buscar(listaLog,nomedoarquivo);

	if(indice > 0){
		verificaExistenciaArquivo = true;
		LIS_Remover(listaLog,indice);
		removerdaPasta(nomedoarquivo);
		
	}

	if(verificaExistenciaArquivo){	

		remove("bancodedados");

		fstream novobanco ("bancodedados" , ios::out | ios::app);

	    for(No i = listaLog->cabeca; i != listaLog->cauda; i = i->proximo)
	    {
	        if(i != listaLog->cabeca && i->proximo != listaLog->cauda){
	        	novobanco << i->conteudo << endl;
	        }
	        else if(i != listaLog->cabeca){
	        	novobanco << i->conteudo;
	        }
	    }
	    
	    novobanco.close();
	    return true;
	}
	else{
		return false;
	}

}

