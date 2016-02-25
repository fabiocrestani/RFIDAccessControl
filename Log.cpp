#include <conio.h>
#include "Log.h"
#include "TempoData.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// LOG (construtor).
/// Propósito: Tenta abrir o arquivo para salvar logs.<br>
/// O atributo arquivo_ok serve para o caso em que o programa continua sem a funcionalidade de 
/// registrar logs<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LOG::LOG(){
	arquivo_ok = false;

	arquivo.open("LOG.txt", std::fstream::out | std::fstream::app);

	if(arquivo.is_open()){									// Se arquivo está aberto
		arquivo.seekp(ios::end);							// Posiciona no final do arquivo
		arquivo_ok = true;									//
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ~LOG (destrutor).
/// Propósito: 
/// Entradas: 
/// Saídas: 
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LOG::~LOG(){
	arquivo.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Inicio.
/// Propósito: Registra momento de abertura do programa no arquivo de logs<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se teve sucesso ou false caso contrário
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
		cout << endl << "Não foi possível abrir o arquivo de logs." << endl << "Deseja continuar assim mesmo?" << endl << endl
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
/// Propósito: Registra tentativa de acesso que teve sucesso, além dos dados do usuário<br>
/// Entradas: Objeto CARTAO com os dados do usuário<br>
/// Saída: Retorna true se teve sucesso ou false caso contrário<br>
///
/// \param [in] cartao Objeto da classe CARTAO com os dados do usuário
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::AcessoOK(CARTAO cartao){
	char nomeUsuario[100];
	char timeStamp[50];

	if(!arquivo_ok) return false;							// Se usuário não quer registrar logs
	if(!arquivo.is_open()) return false;					// Se arquivo não está ok

	cartao.GetUsuario(nomeUsuario);
	cartao.GetTimeStamp(timeStamp);

	arquivo << "> Usuário " << nomeUsuario << " acessou com " << cartao.GetCreditos() + 1
		    << " créditos em " << timeStamp << endl;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// AcessoOK.
/// Propósito: Registra tentativa de acesso que teve sucesso, além dos dados do usuário<br>
/// Entradas: Objeto CARTAO com os dados do usuário<br>
/// Saída: Retorna true se teve sucesso ou false caso contrário<br>
///
/// \param [in] cartao Objeto da classe CARTAO com os dados do usuário
/// \param [in] *mensagem ponteiro para mensagem de erro a ser exibida
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::AcessoNaoOK(CARTAO cartao, char *mensagem){
	char nomeUsuario[100];
	char timeStamp[50];

	if(!arquivo_ok) return false;							// Se usuário não quer registrar logs
	if(!arquivo.is_open()) return false;					// Se arquivo não está ok

	cartao.GetUsuario(nomeUsuario);
	cartao.GetTimeStamp(timeStamp);

	arquivo << "> Usuário " << nomeUsuario << " tentou acessar com " << cartao.GetCreditos() 
		    << " créditos em " << timeStamp;
	arquivo << ". Mensagem de erro: " << mensagem << endl;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// AcessoNaoOK2.
/// Propósito: Semelhante a AcessoNaoOK, porém sem informar dados do usuário<br>
/// Entradas: string com mensagem a ser exibida<br>
/// Saída: Retorna true se teve sucesso ou false caso contrário<br>
///
/// \param [in] *mensagem ponteiro para mensagem de erro a ser exibida
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LOG::AcessoNaoOK2(char *mensagem){
	TEMPODATA timestamp;
	char timestampchar[100];
	timestamp.RetornaDataEHoraAtualizada(timestampchar);

	if(!arquivo_ok) return false;							// Se usuário não quer registrar logs
	if(!arquivo.is_open()) return false;					// Se arquivo não está ok

	arquivo << "> Tentativa de acesso em " << timestampchar << " (data do computador)" << 
			   ". Mensagem de erro: " << mensagem << endl;

	return true;
}