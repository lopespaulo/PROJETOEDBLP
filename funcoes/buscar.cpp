#include "buscar.h"
#include "estruturas.h"
#include "listar.h"
#include "listabusca.h"
#include "relogio.h"
#include <cstring>
#include <iostream>
#include <string>
#include <locale.h>
#include <fstream>
#include <limits>

using namespace std;

long int tempoInicial; // variavel global não precisa passar por parametro

//Função que imprime o resultado por ordem de ocorrência
void imprimepC( ListaB * lista, int tamanho ) { 
	
	LIS_OrdenarB(lista,tamanho,pC);

	for(int j=0; j<tamanho; j++){
		LIS_ImprimirB(lista[j]);
	} 
	
}
//Função que imprime o resultado por ordem de inserção
void imprimepI(  ListaB * lista, int tamanho ) {  
	
	LIS_OrdenarB(lista,tamanho,pI);
	for(int j=0; j<tamanho; j++){
		LIS_ImprimirB(lista[j]);
	}
	
}
//Função que imprime o resultado por ordem alfabética
void imprimepA( ListaB * lista, int tamanho ) { 

	LIS_OrdenarB(lista,tamanho,pA);
	for(int j=0; j<tamanho; j++){
		LIS_ImprimirB(lista[j]);
	}
}


//Funçãp  que trata a opção de impressão escolhida pelo usuário 
PFuncao selecionaImpressao (int argc, args argv){

	for (int i = 2; i < argc; i++){
		if(!strcmp(argv[i], "-pC")){
			return imprimepC;
		}else if(!strcmp(argv[i], "-pA")){
			return imprimepA;
		}
	}
	return imprimepI;
}

fstream& GoToLine(std::fstream& file, int num);
int contadorLinhas(ifstream& file);

/*
 Função que valida a entrada no terminal e escolhe a busca apropriada 
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos do terminal
 @return true se a busca for feita com sucesso, false caso a opção escolhida não seja -bAND ou -bOR, ou haja
 problemas na busca.
 */
bool Ler_Buscas(int argc, args argv){

	if((!strcmp(argv[1], "-bAND") || !strcmp(argv[1],"-bOR")) && (argc > 2)){
		return buscaPorArquivo(argc, argv);
	}else{
		return false;
	}
}

/*
 Função que inicia a busca em cada arquivo da base de buscas
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos do terminal
 @return true se a busca em todos os arquivos correr bem, false caso apresente algum problema
 */
bool buscaPorArquivo(int argc, args argv){
	
	setlocale(LC_ALL,"pt_BR"); 
	char * banco = (char *) "bancodedados";
	ifstream file;
	file.open(banco);
	if (!file.is_open()){
		cout << "Não foi possivel abrir " << banco << endl;
		return false;
	}

   int numeroLinhas = contadorLinhas(file);
	//Criação de um vetor de listas, em que cada lista corresponde a um arquivo da base de buscas
    ListaB listaBusca[numeroLinhas];
    for(int i=0; i<numeroLinhas;i++){
     	listaBusca[i] = LIS_CriarB();
    }
    int a=0;

	std::string linha;
	while(!file.eof()){
		getline(file, linha);
		string arquivo = "banco/";
		
		if(linha != "\0"){
			string dataHora = quebraLinha(linha, 2);
			for (int j = 0; j < (int)linha.size(); j++){
				if(linha[j] == ';'){ // o primeiro ; da linha
					arquivo = arquivo.erase(arquivo.length()-4,4) +".dat";
					buscarNaTabela(argc, argv, arquivo, listaBusca[a], dataHora); //mandar lista por referência
					break;						
				}
				arquivo+=linha[j];
			}
		}
		a++;
	}	
	file.close();

	//Calculo do numero de palavras digitadas para busca
	int qtdePalavrasBusca = argc - inicioPalavrasBusca(argc,argv);

	if(!strcmp(argv[1], "-bAND")){
		
		buscaBAND(listaBusca,numeroLinhas,qtdePalavrasBusca, argc, argv);
	}

	if(!strcmp(argv[1], "-bOR")){
		
		buscaBOR(listaBusca,numeroLinhas, argc, argv);
	}

	return true;
}

/*
 Função que organiza o vetor de listas com as linhas que contém palavras-chave de acordo com a estartégia OR
 @param lista - vetor de listas que contém o resultado da busca de cada palavra
 @param tamanho - tamanho do vetor de listas
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos
 @return true quando todos os tratamentos a lista são executados.
 */
bool buscaBOR(ListaB* lista, int tamanho, int argc, args argv){
	
	for(int j=0; j < tamanho; j++){

		EliminaLinhasIguais(lista[j]);
		LIS_OrdenarB(lista[j],numeroLinha);
		
	}
	void (*ponteiroFuncao) ( ListaB * lista, int tamanho );
	ponteiroFuncao = selecionaImpressao(argc, argv);
	ponteiroFuncao(lista,tamanho);
	
	// imprime o relogio
	for (int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-tT")){
			fimRelogio(tempoInicial);
			break;
		}
	}

	return true;
}

/*
 Função que organiza o vetor de listas com as linhas que contém palavras-chave de acordo com a estartégia AND
 @param lista - vetor de listas que contém o resultado da busca de cada palavra
 @param tamanho - tamanho do vetor de listas
 @param numerodepalavras - quantidade de palavras-chave
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos
 @return true quando todos so tratamentos a lista são executados.
 */
bool buscaBAND(ListaB* lista, int tamanho,int numerodepalavras, int argc, args argv){
	for(int j=0; j < tamanho; j++){

		LIS_OrdenarB(lista[j],numeroLinha);
		ListaB aux = EliminaLinhasSemTodasPalavras(lista[j],numerodepalavras);
		EliminaLinhasIguais(aux);
		lista[j]=aux;
	}

	void (*ponteiroFuncao) ( ListaB * lista, int tamanho );
	ponteiroFuncao = selecionaImpressao(argc, argv);
	ponteiroFuncao(lista,tamanho);
	// imprime o relogio
	for (int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-tT")){
			fimRelogio(tempoInicial);
			break;
		}
	}
	return true;
}
/*
 Função que não sei como se chama, quando descobrir coloco um nome melhor
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos do terminal
 @return true caso tenha aberto corretamento o arquivo e designado a busca, false caso o arquivo nao exista na base
 */

bool buscarNaTabela(int argc, args argv, string arquivo, ListaB& listaBusca, string dataHora){
	fstream file (arquivo);
	file.seekg(0);
	string Aux = arquivo;
	Aux = Aux.erase(Aux.length()-4,4) +".txt";



	if (!file.is_open()){
		cout << "Erro na abertura do arquivo" << endl;
		return false;
	}

	string linha;
	string nulo = "NULO";
	getline(file, linha);
	int tamanho = atoi(linha.c_str());


	for (int i = inicioPalavrasBusca(argc,argv); i < argc; i++){
		//cout << " > Procurando por: [" << argv[i] << "]" << endl;
 		Chave chave = TAB_CriarChave(argv[i]);
		int linhaIdeal = Hash(PreHash(chave), tamanho)+2;
		
		GoToLine(file, linhaIdeal);	
		getline(file,linha);

		
		char* auxi = strtok((char*)linha.c_str()," :");
		string coisinhas(auxi);

		std::size_t foundedWord = coisinhas.find(argv[i]);                
		std::size_t foundedNull = linha.find(nulo);                
		
		//Se não achar a chave e != de nulo prossiga
		while(foundedWord==string::npos && !file.eof()){
			if(foundedNull!=string::npos){
				string linhaFail = "\t - Não contem a palavra\n";
				LIS_InserirFimB(listaBusca,Aux,argv[i],dataHora,linhaFail,-10);
				break;
			}
			getline(file, linha);
			
			char* auxi2 = strtok((char*)linha.c_str()," :");
			string coisinhas2(auxi2);
	
			foundedWord = coisinhas2.find(argv[i]);                
			foundedNull = linha.find(nulo);
			
		}
		// Se achou a palavra
		if(foundedWord!=string::npos){
			// Descarto tudo antes do ':' 
			//strtok((char*)linha.c_str(),":");		
			char* auxLinhas = strtok(NULL,":");
			// Atribuo a linhas a uma string e envio a listagem				
			listaLinhas(arquivo, auxLinhas,listaBusca,argv[i], dataHora);
		}
		file.seekg(0);
	}
	file.close();
	
	return true;
}


/*
 Função que recebe um caminho para arquivo contido na base de buscas e as linhas a navegar nesses arquivos
 @param arquivo - localização do arquivo na base
 @param linhas - todas as linhas a buscar no arquivo
 @return true se o arquivo foi aberto e as linhas tratadas; false caso o arquivo nao exista
 */
bool listaLinhas(string arquivo, char * linhas,ListaB &listaBusca, char* chave, string dataHora){
	
	string arquivoAux = arquivo.erase(arquivo.length()-4,4) +".txt";

	char* nlinha = std::strtok(linhas ,"-");
	fstream arquivoTXT(arquivoAux);

	// Verifica se o arquivo foi aberto corretamente
	if(!arquivoTXT.is_open()){
		return false;
	}

	while(nlinha != NULL){	
		string linhaAux;
		GoToLine(arquivoTXT, atoi(nlinha));
		getline(arquivoTXT,linhaAux);
		LIS_InserirFimB(listaBusca,arquivoAux,chave,dataHora,linhaAux,atoi(nlinha));
		nlinha = strtok(NULL,"-");			
	}

	arquivoTXT.close();	
	return true;			
	


}

/*
 Função que recebe um arquivo contido na base de buscas, e a linha desejada nesse arquivo
 FONTE: //http://stackoverflow.com/questions/5207550/in-c-is-there-a-way-to-go-to-a-specific-line-in-a-text-file/5207600#5207600
 @param file - arquivo a percorrer
 @param num - linha desejada no arquivo
 @return file com o ponteiro armadao na linha desejada
 */
std::fstream& GoToLine(std::fstream& file, int num){
	file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

/*
 Função que conta o numero de linhas de um arquivo
 @param file - arquivo a percorrer
 @return quantidade de linhas
 */

int contadorLinhas(ifstream& file){
	int cont = 0;
	while(!file.eof()){
		file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
		cont++;
	}
	file.seekg(std::ios::beg);
	return cont-1;
}
/*
 Função que identifica se o usuário digitou opção de impressão e/ou opção de exibir tempo de busca
 @param argc - tamanho do vetor de argumentos
 @param argv - vetor de argumentos
 @return o indice em que inicia as palavras-chave no vetor de argumentos
 */
int inicioPalavrasBusca(int argc, args argv){
 	string tipodeImpressao = "-pC -pI -pA";
 	string tempo = "-tF -tT";
 	int inicio = 2;
 
	if(argc >2){
		size_t tipoImpressao2 = tipodeImpressao.find(argv[2]);
		size_t tipoTempo2 = tempo.find(argv[2]);
		if(tipoImpressao2!=string::npos) inicio++;
		if(tipoTempo2!=string::npos) inicio++;
	}
 	
	if(argc > 4){
		size_t tipoImpressao3 = tipodeImpressao.find(argv[3]);
		size_t tipoTempo3 = tempo.find(argv[3]);
		if(tipoImpressao3!=string::npos) inicio++;
		if(tipoTempo3!=string::npos) inicio++;
	}
	for (int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-tT")){
			tempoInicial = meuRelogio();
			break;
		}
	}

 	return inicio;

}



