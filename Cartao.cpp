#include "Cartao.h"
#include <Windows.h>
#include "ConexaoMySQL.h"
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CARTAO (construtor).
/// Propósito: <br>
/// Entradas: Ponteiro para objeto da classe CONEXAOMYSQL<br>
/// Saídas: Nenhuma
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
/// Propósito: Retorna os dados do cartão<br>
/// Entradas: Nenhuma<br>
/// Saídas: núm. serial, nome do usuário e número de créditos<br>
///
/// \param [out] *s ponteiro para vetor com número serial
/// \param [out] *u ponteiro para vetor com nome de usuário
/// \param [out] *c ponteiro para inteiro com número de créditos
/// \param [out]  i ponteiro para inteiro com o ID do cartão
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetDados(char *s, char *u, int *c, int *i){
	strcpy(s, serial);
	strcpy(u, usuario);
	*c = creditos;
	*i = id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetNumSerie.
/// Propósito: Retorna número de série<br>
/// Entradas: Ponteiro para receber o número de série<br>
/// Saídas: Nenhuma<br>
///
/// \param [out] *s ponteiro para receber o número de série
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetNumSerie(char *s){
	strcpy(s, serial);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetUsuario.
/// Propósito: Retorna nome de usuário<br>
/// Entradas: Ponteiro para receber o nome de usuário<br>
/// Saídas: Nenhuma<br>
///
/// \param [out] *u ponteiro para receber o nome de usuário
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetUsuario(char *u){
	strcpy(u, usuario);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetCreditos.
/// Propósito: Retorna número de créditos<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna valor inteiro com o número de créditos<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int CARTAO::GetCreditos(){
	return creditos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetID.
/// Propósito: Retorna o id<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna valor inteiro com o id<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int CARTAO::GetID(){
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetTimeStamp.
/// Propósito: Retorna string com data e hora de acesso armazenados neste objeto<br>
/// Entradas: Ponteiro para um vetor de char que conterá o timestamp<br>
/// Saídas: Nenhuma<br>
///
/// \param [out] *t Ponteiro para um vetor de char que conterá o timestamp
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::GetTimeStamp(char *t){
	if(timestamp=="") strcpy(timestamp, "00/00/0000 00:00:00");
	strcpy(t, timestamp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetDados.
/// Propósito: Guarda os dados do cartão neste objeto<br>
/// Entradas: núm. serial, nome do usuário e número de créditos<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] *s vetor de char com o número de série
/// \param [in] *u vetor de char com o nome do usuario
/// \param [in] c inteiro com a quantidade de créditos
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetDados(const char *s, const char *u, const int c){
	strcpy(serial, s);
	strcpy(usuario, u);
	creditos = c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetNumSerie.
/// Propósito: Guarda o número de série no objeto<br>
/// Entradas: Número de série<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] *s vetor de char com o número de série
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetNumSerie(const char *s){
	strcpy(serial, s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetUsuario.
/// Propósito: Guarda o nome do usuário no objeto<br>
/// Entradas: Nome do usuário<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] *u vetor de char com o nome do usuário
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetUsuario(const char *u){
	strcpy(usuario, u);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetCreditos.
/// Propósito: Guarda o número de créditos no objeto<br>
/// Entradas: Número de créditos<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] c inteiro com a quantidade de créditos
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetCreditos(const int c){
	creditos = c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetID.
/// Propósito: Guarda o ID do cartão no objeto<br>
/// Entradas: ID<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] i inteiro com o id
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetID(const int i){
	id = i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetTimeStamp.
/// Propósito: Gaurda string com data e hora de acesso armazenados neste objeto<br>
/// Entradas: Ponteiro para um vetor de char que contém o timestamp<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] *t Ponteiro para um vetor de char que contém o timestamp
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::SetTimeStamp(const char *t){
	strcpy(timestamp, t);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// VeirificaCartaoExiste.
/// Propósito: Verifica se dado número serial já existe na tabela do banco de dados<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se já existe ou false caso contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::VeirificaCartaoExiste(){	
	char	query[300]="";
	char	bfSerial[9];
	int		i;

	for(i=0;i<7;i++) bfSerial[i] = serial[i];							  // Obtem 8 primeiros chars
	bfSerial[8]='\0';													  // Descarta último char
	sprintf(query, "SELECT id FROM usuarios WHERE serial='%s'", bfSerial);// Prepara a query
	if(Cmysql->QueryNumRows(query)==0) return false;					  // Executa a query

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// VerificaIDExiste.
/// Propósito: Verifica se dado número serial já existe na tabela do banco de dados<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se já existe ou false caso contrário<br>
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
/// Propósito: Verifica se o número serial já existe e então cadastra-o<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se teve sucesso ou false caso contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CARTAO::Cadastra(){
	char nomeUsuario[100];
	int	 numCreditos;

	// 1. Verifica se já existe este número serial cadastrado
	if(!VeirificaCartaoExiste()){

		// 2. Se não existe, pede dados
		// Obtém nome do usuário ------------------------------------------------------------------------------
		cout << endl << "Digite o nome do usuário: ";
		fflush(stdin);
		gets_s(nomeUsuario, 100);
		fflush(stdin);
		// ----------------------------------------------------------------------------------------------------

		// Obtém número de créditos inicial --------------------------------------------------------------------
		cout << "Digite o número de créditos inicial: ";
		scanf("%i", &numCreditos);
		fflush(stdin);
		// ----------------------------------------------------------------------------------------------------

		// Salva no objeto cartão e no banco de dados ---------------------------------------------------------
		SetUsuario(nomeUsuario);
		SetCreditos(numCreditos);

		if(InsereCartaoBanco()){
			system("cls");
			cout << "Cartão cadastrado com sucesso!" << endl << endl;
			cout << "Usuário " << nomeUsuario << " possui " << numCreditos << " crédito(s)" << endl << endl;
			system("pause");
			return true;
		}
	}
	else{
		cout << endl << "Este número de série já está cadastrado" << endl << endl;
	}

	cout << "O cartão não foi cadastrado" << endl << endl;
	system("pause");
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// InsereCartaoBanco.
/// Propósito: Prepara uma query mysql para inserir o cartão no banco de dados e chama o método da
/// classe CONEXAOMYSQL para inserir<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se teve sucesso ou false caso contrário<br>
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
/// Propósito: Exibe no console o ID, o nome do usuário e o número de créditos do cartão<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CARTAO::ExibeDados(){
	cout << "(" << id << ") Usuário: " << usuario << ". Créditos: " << creditos << endl; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// DecrementaCredito.
/// Propósito: Prepara uma consulta mysql para decrementar o número de créditos do cartão
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se decrementou ou false caso contrário<br>
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
/// Propósito: Prepara uma consulta mysql para incrementar o número de créditos do cartão.<br>
/// Semelhante a DecrementaCredito, porém usada apenas em caso da haver erro no envio do código
/// de confirmação para o PC. Assim, retoma o valor anterior de créditos do usuário.<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se decrementou ou false caso contrário<br>
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