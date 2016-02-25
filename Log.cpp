#include <conio.h>
#include "Log.h"
#include "TempoData.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// LOG (construtor).
/// Prop�sito: Tenta abrir o arquivo para salvar logs.<br>
/// O atributo arquivo_ok serve para o caso em que o programa continua sem a funcionalidade de 
/// registrar logs<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LOG::LOG(){
	arquivo_ok = false;

	arquivo.open("LOG.txt", std::fstream::out | std::fstream::app);

	if(arquivo.is_open()){									// Se arquivo est� aberto
		arquivo.seekp(ios::end);							// Posiciona no final do arquivo
		arquivo_ok = true;									//
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ~LOG (destrutor).
/// Prop�sito: 
/// Entradas: 
/// Sa�das: 
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LOG::~LOG(){
	arquivo.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Inicio.
/// Prop�sito: Registra momento de abertura do programa no arquivo de logs<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se teve sucesso ou false caso contr�rio
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::Inicio(){
	int opcao_digitada;
	TEMPODATA timestamp;
	char timestampchar[100];

	if(arquivo.is_open()){
		arquivo_ok = true;
		timestamp.RetornaDataEHoraAtualizada(timestampchar);
		arquivo << 
"\n*********************************************************************************\n\
Programa iniciou em:" << timestampchar << 
"\n*********************************************************************************\n";
		cout << " LOGS:   OK " << endl;
		return true;
	}
	else{
		arquivo_ok = false;
		cout << endl << "N�o foi poss�vel abrir o arquivo de logs." << endl << "Deseja continuar assim mesmo?" << endl << endl
			 << "Digite 1 para continuar ou qualquer tecla para cancelar e sair do programa" << endl;

		opcao_digitada = getch();
		cout << endl << endl;
		if(opcao_digitada == '1')
			return true;
		else 
			return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// AcessoOK.
/// Prop�sito: Registra tentativa de acesso que teve sucesso, al�m dos dados do usu�rio<br>
/// Entradas: Objeto CARTAO com os dados do usu�rio<br>
/// Sa�da: Retorna true se teve sucesso ou false caso contr�rio<br>
///
/// \param [in] cartao Objeto da classe CARTAO com os dados do usu�rio
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::AcessoOK(CARTAO cartao){
	char nomeUsuario[100];
	char timeStamp[50];

	if(!arquivo_ok) return false;							// Se usu�rio n�o quer registrar logs
	if(!arquivo.is_open()) return false;					// Se arquivo n�o est� ok

	cartao.GetUsuario(nomeUsuario);
	cartao.GetTimeStamp(timeStamp);

	arquivo << "> Usu�rio " << nomeUsuario << " acessou com " << cartao.GetCreditos() + 1
		    << " cr�ditos em " << timeStamp << endl;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// AcessoOK.
/// Prop�sito: Registra tentativa de acesso que teve sucesso, al�m dos dados do usu�rio<br>
/// Entradas: Objeto CARTAO com os dados do usu�rio<br>
/// Sa�da: Retorna true se teve sucesso ou false caso contr�rio<br>
///
/// \param [in] cartao Objeto da classe CARTAO com os dados do usu�rio
/// \param [in] *mensagem ponteiro para mensagem de erro a ser exibida
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::AcessoNaoOK(CARTAO cartao, char *mensagem){
	char nomeUsuario[100];
	char timeStamp[50];

	if(!arquivo_ok) return false;							// Se usu�rio n�o quer registrar logs
	if(!arquivo.is_open()) return false;					// Se arquivo n�o est� ok

	cartao.GetUsuario(nomeUsuario);
	cartao.GetTimeStamp(timeStamp);

	arquivo << "> Usu�rio " << nomeUsuario << " tentou acessar com " << cartao.GetCreditos() 
		    << " cr�ditos em " << timeStamp;
	arquivo << ". Mensagem de erro: " << mensagem << endl;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// AcessoNaoOK2.
/// Prop�sito: Semelhante a AcessoNaoOK, por�m sem informar dados do usu�rio<br>
/// Entradas: string com mensagem a ser exibida<br>
/// Sa�da: Retorna true se teve sucesso ou false caso contr�rio<br>
///
/// \param [in] *mensagem ponteiro para mensagem de erro a ser exibida
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::AcessoNaoOK2(char *mensagem){
	TEMPODATA timestamp;
	char timestampchar[100];
	timestamp.RetornaDataEHoraAtualizada(timestampchar);

	if(!arquivo_ok) return false;							// Se usu�rio n�o quer registrar logs
	if(!arquivo.is_open()) return false;					// Se arquivo n�o est� ok

	arquivo << "> Tentativa de acesso em " << timestampchar << " (data do computador)" << 
			   ". Mensagem de erro: " << mensagem << endl;

	return true;
}