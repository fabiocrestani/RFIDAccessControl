#include "Cartao.h"
#include <Windows.h>
#include "ConexaoMySQL.h"
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CARTAO (construtor).
/// Prop�sito: <br>
/// Entradas: Ponteiro para objeto da classe CONEXAOMYSQL<br>
/// Sa�das: Nenhuma
///
/// \param [in] *mysql Ponteiro para objeto da classe CONEXAOMYSQL
///////////////////////////////////////////////////////////////////////////////////////////////////
CARTAO::CARTAO(CONEXAOMYSQL *mysql){
	Cmysql = mysql;										 // Objeto da classe CONEXAOMYSQL agregado
	strcpy(timestamp, "");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CARTAO (destrutor).
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CARTAO::~CARTAO(){};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetDados.
/// Prop�sito: Retorna os dados do cart�o<br>
/// Entradas: Nenhuma<br>
/// Sa�das: n�m. serial, nome do usu�rio e n�mero de cr�ditos<br>
///
/// \param [out] *s ponteiro para vetor com n�mero serial
/// \param [out] *u ponteiro para vetor com nome de usu�rio
/// \param [out] *c ponteiro para inteiro com n�mero de cr�ditos
/// \param [out]  i ponteiro para inteiro com o ID do cart�o
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetDados(char *s, char *u, int *c, int *i){
	strcpy(s, serial);
	strcpy(u, usuario);
	*c = creditos;
	*i = id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetNumSerie.
/// Prop�sito: Retorna n�mero de s�rie<br>
/// Entradas: Ponteiro para receber o n�mero de s�rie<br>
/// Sa�das: Nenhuma<br>
///
/// \param [out] *s ponteiro para receber o n�mero de s�rie
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetNumSerie(char *s){
	strcpy(s, serial);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetUsuario.
/// Prop�sito: Retorna nome de usu�rio<br>
/// Entradas: Ponteiro para receber o nome de usu�rio<br>
/// Sa�das: Nenhuma<br>
///
/// \param [out] *u ponteiro para receber o nome de usu�rio
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetUsuario(char *u){
	strcpy(u, usuario);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetCreditos.
/// Prop�sito: Retorna n�mero de cr�ditos<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna valor inteiro com o n�mero de cr�ditos<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int CARTAO::GetCreditos(){
	return creditos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetID.
/// Prop�sito: Retorna o id<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna valor inteiro com o id<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int CARTAO::GetID(){
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetTimeStamp.
/// Prop�sito: Retorna string com data e hora de acesso armazenados neste objeto<br>
/// Entradas: Ponteiro para um vetor de char que conter� o timestamp<br>
/// Sa�das: Nenhuma<br>
///
/// \param [out] *t Ponteiro para um vetor de char que conter� o timestamp
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetTimeStamp(char *t){
	if(timestamp=="") strcpy(timestamp, "00/00/0000 00:00:00");
	strcpy(t, timestamp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetDados.
/// Prop�sito: Guarda os dados do cart�o neste objeto<br>
/// Entradas: n�m. serial, nome do usu�rio e n�mero de cr�ditos<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] *s vetor de char com o n�mero de s�rie
/// \param [in] *u vetor de char com o nome do usuario
/// \param [in] c inteiro com a quantidade de cr�ditos
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetDados(const char *s, const char *u, const int c){
	strcpy(serial, s);
	strcpy(usuario, u);
	creditos = c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetNumSerie.
/// Prop�sito: Guarda o n�mero de s�rie no objeto<br>
/// Entradas: N�mero de s�rie<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] *s vetor de char com o n�mero de s�rie
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetNumSerie(const char *s){
	strcpy(serial, s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetUsuario.
/// Prop�sito: Guarda o nome do usu�rio no objeto<br>
/// Entradas: Nome do usu�rio<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] *u vetor de char com o nome do usu�rio
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetUsuario(const char *u){
	strcpy(usuario, u);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetCreditos.
/// Prop�sito: Guarda o n�mero de cr�ditos no objeto<br>
/// Entradas: N�mero de cr�ditos<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] c inteiro com a quantidade de cr�ditos
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetCreditos(const int c){
	creditos = c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetID.
/// Prop�sito: Guarda o ID do cart�o no objeto<br>
/// Entradas: ID<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] i inteiro com o id
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetID(const int i){
	id = i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetTimeStamp.
/// Prop�sito: Gaurda string com data e hora de acesso armazenados neste objeto<br>
/// Entradas: Ponteiro para um vetor de char que cont�m o timestamp<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] *t Ponteiro para um vetor de char que cont�m o timestamp
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetTimeStamp(const char *t){
	strcpy(timestamp, t);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// VeirificaCartaoExiste.
/// Prop�sito: Verifica se dado n�mero serial j� existe na tabela do banco de dados<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se j� existe ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::VeirificaCartaoExiste(){	
	char	query[300]="";
	char	bfSerial[9];
	int		i;

	for(i=0;i<7;i++) bfSerial[i] = serial[i];							  // Obtem 8 primeiros chars
	bfSerial[8]='\0';													  // Descarta �ltimo char
	sprintf(query, "SELECT id FROM usuarios WHERE serial='%s'", bfSerial);// Prepara a query
	if(Cmysql->QueryNumRows(query)==0) return false;					  // Executa a query

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// VerificaIDExiste.
/// Prop�sito: Verifica se dado n�mero serial j� existe na tabela do banco de dados<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se j� existe ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::VeirificaIDExiste(){	
	char	query[300]="";
	
	sprintf(query, "SELECT id FROM usuarios WHERE id='%i'", id);		  // Prepara a query
	if(Cmysql->QueryNumRows(query)==0) return false;					  // Executa a query

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Cadastra.
/// Prop�sito: Verifica se o n�mero serial j� existe e ent�o cadastra-o<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se teve sucesso ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::Cadastra(){
	char nomeUsuario[100];
	int	 numCreditos;

	// 1. Verifica se j� existe este n�mero serial cadastrado
	if(!VeirificaCartaoExiste()){

		// 2. Se n�o existe, pede dados
		// Obt�m nome do usu�rio ------------------------------------------------------------------------------
		cout << endl << "Digite o nome do usu�rio: ";
		fflush(stdin);
		gets_s(nomeUsuario, 100);
		fflush(stdin);
		// ----------------------------------------------------------------------------------------------------

		// Obt�m n�mero de cr�ditos inicial --------------------------------------------------------------------
		cout << "Digite o n�mero de cr�ditos inicial: ";
		scanf("%i", &numCreditos);
		fflush(stdin);
		// ----------------------------------------------------------------------------------------------------

		// Salva no objeto cart�o e no banco de dados ---------------------------------------------------------
		SetUsuario(nomeUsuario);
		SetCreditos(numCreditos);

		if(InsereCartaoBanco()){
			system("cls");
			cout << "Cart�o cadastrado com sucesso!" << endl << endl;
			cout << "Usu�rio " << nomeUsuario << " possui " << numCreditos << " cr�dito(s)" << endl << endl;
			system("pause");
			return true;
		}
	}
	else{
		cout << endl << "Este n�mero de s�rie j� est� cadastrado" << endl << endl;
	}

	cout << "O cart�o n�o foi cadastrado" << endl << endl;
	system("pause");
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// InsereCartaoBanco.
/// Prop�sito: Prepara uma query mysql para inserir o cart�o no banco de dados e chama o m�todo da
/// classe CONEXAOMYSQL para inserir<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se teve sucesso ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::InsereCartaoBanco(){
	char bfQuery[100];
	sprintf(bfQuery, "('%s', '%s', %i)", usuario, serial, creditos);	// Prepara parte da query
	char query[500] = "INSERT INTO usuarios (nome, serial, creditos) VALUES ";

	strcat(query, bfQuery);												// Concatena query

	if(Cmysql->QueryInsert(query)) return true;							// Envia query ao banco

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ExibeDados.
/// Prop�sito: Exibe no console o ID, o nome do usu�rio e o n�mero de cr�ditos do cart�o<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::ExibeDados(){
	cout << "(" << id << ") Usu�rio: " << usuario << ". Cr�ditos: " << creditos << endl; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// DecrementaCredito.
/// Prop�sito: Prepara uma consulta mysql para decrementar o n�mero de cr�ditos do cart�o
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se decrementou ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::DecrementaCredito(){
	char bfQuery[100];

	if(id<=0) return false;
	
	sprintf(bfQuery, "UPDATE usuarios SET creditos = creditos - 1 WHERE id='%i'", id);// Prepara query

	if(Cmysql->QueryUpdate(bfQuery)){
		creditos = creditos - 1;
		return true;
	}
	else{
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// IncrementaCredito.
/// Prop�sito: Prepara uma consulta mysql para incrementar o n�mero de cr�ditos do cart�o.<br>
/// Semelhante a DecrementaCredito, por�m usada apenas em caso da haver erro no envio do c�digo
/// de confirma��o para o PC. Assim, retoma o valor anterior de cr�ditos do usu�rio.<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se decrementou ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::IncrementaCredito(){
	char bfQuery[100];

	if(id<=0) return false;
	
	sprintf(bfQuery, "UPDATE usuarios SET creditos = creditos + 1 WHERE id='%i'", id);// Prepara query

	if(Cmysql->QueryUpdate(bfQuery)){
		creditos = creditos + 1;
		return true;
	}
	else{
		return false;
	}
}