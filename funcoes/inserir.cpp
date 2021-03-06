#include <fstream>
#include <cstring>
#include <locale.h>
#include <iostream>
#include "inserir.h"
#include "estruturas.h"
#include "buscar.h"
#include <ctime>

using namespace std;

/*
 Função que insere um arquivo da base de buscas um arquivo
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos do terminal
 @return true se inseriu com sucesso, false caso a opção escolhida não seja -i
 */
bool inserir(int argc, args argv){
	
	setlocale(LC_ALL,"pt_BR"); 
	
	
	// verifica se a função chamada é inserir
	if(strcmp(argv[1], "-i")  || (argc < 3)){
		
		return false;	
	}

	char * banco = (char*)"bancodedados";
	fstream file (banco , ios::out | ios::app); // faz abrir sempre no fim do arquivo
	
	if(!file.is_open()){
		cout << "Não foi possível abrir o arquivo bancodedados." << endl;
		return false;
	}

	file.close();

	for (int i = 2; i < argc; i++){
		bool validou = copiarArquivoParaPasta(argv[i]);
		if(atualiza(banco,argv[i])){			
			if(fazAtualizacao(banco,argv[i]) && validou){
				cout << "Arquivo " << argv[i] << " já estava na base de buscas.\n\tSeu registro foi atualizado." <<endl;
				
			}
			else{
				cout << "Não foi possível atualizar o arquivo " << argv[i] << "." << endl;
				
			}
		}
		else{
			if(apenasInsere(banco,argv[i],argc,i) && validou){
				cout << "Arquivo: " << argv[i] << " inserido na base de buscas." <<endl;
				
			}
			else{
				cout << "Não foi possível inserir o arquivo " << argv[i] << "." << endl;
				
			}

		}

	}

	return true;

}

/*
 Função que executa a atualização do arquivo, tanto no arquivo de log como na pasta /banco
 @param base - vetor de caracteres que possui o nome do arquivo da base de dados
 @param nomeArquivo - nome do arquivo que vai ser atualizado
 @return true se atualizou com sucesso, false caso não atualize
 */

bool fazAtualizacao(char * base, char * nomeArquivo){

	setlocale(LC_ALL,"pt_BR"); 
		
	Lista lista = LIS_Criar();

	string prefixo = nomeArquivo;

	string linha;

	ifstream arquivoBase;
	arquivoBase.open(base,ifstream::in);

	if(!arquivoBase.is_open()){
		return false;
	}

	//Construir a lista com cada nó sendo uma string do indice do arquivo referenciado
	while(!arquivoBase.eof()){

		getline(arquivoBase,linha);

		//copia os indices de arquivos diferentes de arquivinho para a lista de indices
		if(linha.substr(0, prefixo.size()) != nomeArquivo){
		
			LIS_InserirFim(lista,linha);

		}
		else{
			if(copiarArquivoParaPasta(nomeArquivo)){

				int tamNome = strlen(nomeArquivo);
				char caminho[6+tamNome];
				strcpy(caminho, "banco/");
				strcat(caminho, nomeArquivo);
				int palavras = gerarTabela(caminho); // Conta as palavras e gera tabela de dispersao
				
				std::time_t horadeinsercao;
				struct tm * timeinfo;
				std::time(&horadeinsercao);
				char horaData[80];
				// humanizando o tempo
				timeinfo = localtime(&horadeinsercao);
				strftime(horaData,80,"%Y%m%d%H%M%S", timeinfo);

				string pontoevirgula = ";";

				string linhaatualizada = nomeArquivo+pontoevirgula + horaData + pontoevirgula + std::to_string(palavras).c_str();
				
				//o que tem da posiçao 0 até tamanho da linha, é substituido pelo conteudo de linhaatualizada
				//linha.replace(0,linha.size(),linhaatualizada); 
				LIS_InserirFim(lista,linhaatualizada);
			}else{
				return false;
			}
		}
	}

	//Apaga o arquivo de log e insere os indices da lista encadeada que contem os indices atualizados em novo log.
	arquivoBase.close();
	remove(base);
	fstream novobanco (base , ios::out | ios::app);

    for(No i = lista->cabeca; i != lista->cauda; i = i->proximo){
        if(i != lista->cabeca){
        	if(i->proximo == lista->cauda){
        		novobanco << i->conteudo;
        	}        		
        	else{
        		novobanco << i->conteudo << '\n';
        	}
        }
    }
	    
	novobanco.close();
	
	return true;

}

/*
 Função que insere um novo arquivo, tanto no arquivo de log como na pasta /banco
 @param base - vetor de caracteres que possui o nome do arquivo da base de dados
 @param nomeArquivo - nome do arquivo que vai ser inserido
 @param argc - tamanho do vetor de argumentos
 @param posicao - usado para checar se a posicao do argumento é ultima
 @return true se inseriu com sucesso, false caso não insira
 */

bool apenasInsere(char * base, char * nomeArquivo, int argc, int posicao){

	fstream file (base , ios::out | ios::app);

	if(!file.is_open()){
		return false;
	}

	if(!copiarArquivoParaPasta(nomeArquivo)) return false;

	int tamNome = strlen(nomeArquivo);
	char caminho[6+tamNome];
	strcpy(caminho, "banco/");
	strcat(caminho, nomeArquivo);		

	int palavras = gerarTabela(caminho); // Conta as palavras e gera arquivo .dat com a tabela de dispersao

	std::time_t horadeinsercao;
	struct tm * timeinfo;
	std::time(&horadeinsercao);
	char horaData[80];
	// humanizando o tempo
	timeinfo = localtime(&horadeinsercao);
	strftime(horaData,80,"%Y%m%d%H%M%S", timeinfo);

	file << nomeArquivo << ";" << horaData << ";" << palavras << "\n";

	file.close();
	return true;

}


/*
 Função que verifica se o arquivo digitado pelo usuário já está na base de buscas
 @param base - vetor de caracteres que possui o nome do arquivo da base de dados
 @param nomeArquivo - nome do arquivo que vai ser verificado
 @return true se encotrar o arquivo na base de buscas, false caso não encontre
 */
bool atualiza(char* base, char* nomeArquivo){

	string linha;
	fstream arquivoBase (base, ios::in | ios::app);

	//Construir a lista com cada nó sendo uma string do indice do arquivo referenciado
	while(!arquivoBase.eof()){

		getline(arquivoBase,linha);

		//copia os indices de arquivos diferentes de arquivinho para a lista de indices
		if(linha.substr(0, strlen(nomeArquivo)) == nomeArquivo){
			arquivoBase.close();
			return true;
		}
	}
	arquivoBase.close();
	return false;

}

/*
  Função que copia o conteudo do arquivo que se deseja colocar ou atualizar na base de buscas
  gerando uma copia (.txt)
 @param nomeArquivo - nome do arquivo que vai ser atualizado/inserido
 @return true se inseriu a copia foi realizada com sucesso, false caso contrário
 */
		
bool copiarArquivoParaPasta(char* arquivo){
	
	int tamNome = strlen(arquivo);
	char caminho[6+tamNome];
	strcpy(caminho, "banco/");
	strcat(caminho, arquivo);

	gerarTabela(caminho);
	
	ifstream origem (arquivo, fstream::binary);

	if(origem){
		ofstream destino (caminho, fstream::trunc|fstream::binary);
		destino << origem.rdbuf ();
		return true;
	}
	return false;
}

/*
  Função que gera a Tabela de Dispersão e gera um arquivo .dat com o conteúdo da tabela.
  Além de retorna a quantidade de palavras do texto.
 @param origem - string com o endereço do arquivo de texto
 @return quantidade de palavras do texto
 */

int gerarTabela(std::string origem){
	Tabela tabela = TAB_CriarTabela(100);
	std::string palavra;
	char * aux = new char[origem.length() +1];
	std::strcpy(aux, origem.c_str());
	ifstream arquivo (aux, fstream::binary);
	
	if (arquivo.is_open()){
		
		std::string linha;
		int contadorLinha = 0;
		int contadorPalavras = 0;
			
		while(!arquivo.eof()){
			contadorLinha++;
			getline(arquivo, linha);
			char* termo = std::strtok((char *)linha.c_str() ," .,;!?()[]");
			
			while(termo != NULL){
				contadorPalavras++;
				Chave chave = TAB_CriarChave(termo);
				Valor valor = TAB_CriarValor(std::to_string(contadorLinha).c_str());
				TAB_Inserir(tabela, chave, valor);
				termo = strtok(NULL," .,;!?()[]");	
			}
		}arquivo.close();

		string arquivotabela = origem.erase(origem.length()-4,4) +".dat";
		char * auxTabela = new char[arquivotabela.length() +1];
		std::strcpy(auxTabela, arquivotabela.c_str());
		
		gerarArquivoTabela(auxTabela, tabela);		
		return contadorPalavras;
	} 	
	return -1;
}