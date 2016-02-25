#include <iostream>
using namespace std;
#include <windows.h>
#include <tchar.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include "ConexaoMySQL.h"
#include "InterfaceGrafica.h"
#include "Cartao.h"
#include <string>
#include <Strsafe.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CONEXAOMYSQL (construtor).
/// Propósito: Seta os parâmetros da conexão SQL<br>
/// Entradas: Parâmetros da conexão. Veja mais detalhes abaixo<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] d alias de conexão com driver ODBC
/// \param [in] s nome do servidor para conectar
/// \param [in] u nome do usuário para acessar o banco de dados
/// \param [in] p senha do usuário para acessar o banco de dados
/// \param [in] dvr versão do driver ODBC. (Ex.: MySQL Server;)
///////////////////////////////////////////////////////////////////////////////////////////////////
CONEXAOMYSQL::CONEXAOMYSQL(TCHAR d[50], TCHAR s[100], TCHAR u[20], TCHAR p[50], TCHAR dvr[30]){
	conectado = false;

	wcscpy_s(DSN, d);
	wcscpy_s(SERVER, s);
	wcscpy_s(UID, u);
	wcscpy_s(PWD, p);
	wcscpy_s(DRIVER, dvr);

	StringCchPrintf(stringConexaoMySQL,sizeof(stringConexaoMySQL)/sizeof(TCHAR),
		_T("DSN=%s;SERVER=%s;UID=%s;PWD=%s;DRIVER=%s;"), DSN, SERVER, UID, PWD, DRIVER);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CONEXAOMYSQL (construtor).
/// Propósito: Sobrecarga do método construtor sem parâmetros.<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CONEXAOMYSQL::CONEXAOMYSQL(void){
	conectado = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: destrutor da classe CONEXAOMYSQL
/// ~CONEXAOMYSQL (destrutor).
/// Propósito: Encerra a conexão liberando os handles<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CONEXAOMYSQL::~CONEXAOMYSQL(){
	if(conectado==true){
		//SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		//if(hdbc!=NULL) SQLDisconnect(hdbc);
	    //if(SQL_HANDLE_DBC!=NULL) SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		//if(SQL_HANDLE_ENV!=NULL) SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetaParâmetros.
/// Propósito: Seta os parâmetros da conexão SQL<br>
/// Entradas: Parâmetros da conexão. Veja mais detalhes abaixo<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] d alias de conexão com driver ODBC
/// \param [in] s nome do servidor para conectar
/// \param [in] u nome do usuário para acessar o banco de dados
/// \param [in] p senha do usuário para acessar o banco de dados
/// \param [in] dvr versão do driver ODBC. (Ex.: MySQL Server;)
///////////////////////////////////////////////////////////////////////////////////////////////////
void CONEXAOMYSQL::SetaParametros(TCHAR d[50], TCHAR s[100], TCHAR u[20], TCHAR p[50], TCHAR dvr[30]){
	wcscpy_s(DSN, d);
	wcscpy_s(SERVER, s);
	wcscpy_s(UID, u);
	wcscpy_s(PWD, p);
	wcscpy_s(DRIVER, dvr);

	StringCchPrintf(stringConexaoMySQL,sizeof(stringConexaoMySQL)/sizeof(TCHAR),
		_T("DSN=%s;SERVER=%s;UID=%s;PWD=%s;DRIVER=%s;"), DSN, SERVER, UID, PWD, DRIVER);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// GetConectado.
/// Propósito: Obtém o estado da conexão<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se está conectado ou false caso contrário
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::GetConectado(void){
	return conectado;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Conecta.
/// Propósito: Faz uma conexão com o banco de dados e mantém conectado para uso futuro<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se teve sucesso ou false caso contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::Conecta(){
    retcode = SQLAllocHandle(SQL_HANDLE_ENV,						  // Allocate environment handle
							 SQL_NULL_HANDLE, 
							 &henv);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { // Set the ODBC version environment attribute
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 

        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { // Allocate connection handle
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc); 

            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { // Set login timeout to 5 seconds
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);

                // Connect to data source
                retcode = SQLDriverConnect(
                    hdbc, 
                    0,
					(SQLWCHAR*)((stringConexaoMySQL)),
					_tcslen(stringConexaoMySQL),
                    OutConnStr,
                    255, 
                    &OutConnStrLen,
					SQL_DRIVER_COMPLETE );

				if(!(retcode==SQL_ERROR || retcode==SQL_INVALID_HANDLE || retcode==SQL_STILL_EXECUTING || retcode==SQL_NO_DATA)){
					conectado = true;
					return true;
				}
			}
		}
	}
	conectado = false;
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// PegaTodos.
/// Propósito: Imprime no console todos os usuários e seus respectivos dados<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se teve sucesso ou false caso contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::PegaTodos(){
	SQLINTEGER sId, sCreditos, cbCreditos, cbId, cbTestStr, cbTestStr2, iCount = 1;
    SQLCHAR szTestStr[200];
	SQLCHAR szTestStr2[200];

	system("cls");
	cout << " ---------------------------------------------------"  << endl;
	cout << " Listando todos os usuários cadastrados" << endl;
	cout << " ---------------------------------------------------"  << endl << endl;

	cout << "# " << "id\tCréditos    Usuário " << endl << endl ;

	if(conectado==false){
		cout << "Não é possível fazer uma consulta pois não está conectado ao banco" << endl;
		return false;
	}

	// Allocate statement handle
	retcode = SQL_SUCCESS;
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 

        // Process data
		retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT id, nome, serial, creditos FROM usuarios", SQL_NTS);

        if(retcode == SQL_SUCCESS){
            
            while(TRUE) {
                retcode = SQLFetch(hstmt);
                if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
                    cout<<"An error occurred";
					return false;
                }
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO){
                    SQLGetData(hstmt, 1, SQL_C_ULONG, &sId, 0, &cbId);
					SQLGetData(hstmt, 2, SQL_C_CHAR, szTestStr, 200, &cbTestStr);
                    SQLGetData(hstmt, 3, SQL_C_CHAR, szTestStr2, 200, &cbTestStr2);
					SQLGetData(hstmt, 4, SQL_C_ULONG, &sCreditos, 0, &cbCreditos);

                    cout << iCount << " " << sId << "\t" << sCreditos << "\t    " << szTestStr << endl;

                    iCount++;
                }else{
					cout << endl << endl;
                    break;
					return true;
                }
            }
		}
	}else{
		cout<<"Query execution error."<<endl;
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryNumRows.
/// Propósito: Retorna o número de linhas de uma determinada query<br>
/// Entradas: char query com a consulta<br>
/// Saídas: Retorna -1 se houve erro. Ou o número de linhas da query<br>
///
/// \param [in] query com a consulta a se verificar o número de linhas
///////////////////////////////////////////////////////////////////////////////////////////////////
int CONEXAOMYSQL::QueryNumRows(char query[300]){
	SQLINTEGER sTestInt, cbTestInt, iCount = 0;
	wchar_t w_query[500];

	if(conectado==false){
		cout << "Não é possível fazer uma consulta pois não está conectado ao banco" << endl;
		return -1;
	}

	// Allocate statement handle
	retcode = SQL_SUCCESS;
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 

        // Process data
		mbstowcs(w_query, query, 500);									// Converte char para wide char
		retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(w_query), SQL_NTS);

        if(retcode == SQL_SUCCESS){
            
            while(TRUE) {
                retcode = SQLFetch(hstmt);
                if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
                    cout<<"An error occurred";
					return -1;
                }
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO){
                    SQLGetData(hstmt, 1, SQL_C_ULONG, &sTestInt, 0, &cbTestInt);
                    iCount++;
                }else{
                    break;
					return iCount;
                }
            }
		}
	}else{
		cout<<"Query execution error."<<endl;
		return -1;
	}
	return iCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryInsert.
/// Propósito: Faz uma query de inserção de dados no banco de dados<br>
/// Entradas: char query com a consulta<br>
/// Saídas: Retorna true se teve sucesso ou false caso contrário<br>
///
/// \param [in] query com a consulta de inserção
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryInsert(char query[500]){
	wchar_t w_query[500];

	if(conectado==false){
		cout << "Não é possível fazer uma consulta pois não está conectado ao banco" << endl;
		return false;
	}
	
	retcode = SQL_SUCCESS;
	// Allocate statement handle
    if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO){
        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 

		mbstowcs(w_query, query, 500);									// Converte char para wide char
		retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(w_query), SQL_NTS);	// Process data

        if(retcode == SQL_SUCCESS) return true;
	}
	else{
		cout << "Query execution error." << endl;
		return false;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryDelete.
/// Propósito: Remove um item ou mais do banco de dados<br>
/// Entradas: char Query[300] com a consulta de remoção<br>
/// Saídas: Retorna true se removeu e false caso contrário<br>
///
/// \param [in] query tem a consulta de remoção
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryDelete(char query[300]){
	wchar_t w_query[500];

	if(conectado==false){
		cout << "Não é possível fazer uma consulta pois não está conectado ao banco" << endl;
		return false;
	}

	// Allocate statement handle
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 

    // Process data
	mbstowcs(w_query, query, 500);									// Converte char para wide char
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(w_query), SQL_NTS);

    if(retcode == SQL_SUCCESS) return true; else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveItem.
/// Propósito: Prepara uma query para remoção de um item do banco de dados<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se removeu e false caso contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::RemoveItem(){
	int id_remover;
	char bfQuery[100]="";

	system("cls");
	PegaTodos();														// mostra todos os cadastrados
	cout << "Digite o ID do cartão que deseja remover ou 0 para cancelar: ";
	cin >> id_remover;
	if(id_remover==0){
		system("cls");
		cout << "Cancelado" << endl;
		return false;
	}
	cout << endl << endl << "Removendo";
	sprintf(bfQuery, "DELETE FROM usuarios WHERE id='%i'", id_remover);	// prepara query
	
	system("cls");
	if(QueryDelete(bfQuery)){											// executa query
		cout << "Item removido com sucesso" << endl << endl;
		return true;
	}
	else{
		cout << endl << "Não foi possível remover. Verifique o item com este ID existe" << endl << endl;
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MostraConectado.
/// Propósito: Exibe na tela o status da conexão (conectado ou não)
/// Entradas: Nenhuma
/// Saídas: Retorna true se está conectado ou false caso contrário
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::MostraConcetado(void){
	if(GetConectado()){
		cout << " MYSQL:  Conectado!" << endl << endl;
		return true;
	}
	else{
		cout << " MYSQL:  Não Conectado!" << endl << endl;
		return false;
	}
}
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryCartao.
/// Propósito: Consulta o banco de dados com o número de série fornecido no objeto cartão e retorna
/// no mesmo objeto o id, o nome do usuário e número de créditos do cartão<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se consulta ok ou false caso contrário
///
/// \param [in,out] cartao ponteiro da classe CARTAO para transferência das informações
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryCartao(CARTAO *cartao){
	char	bfNumSerie[1024];
	char	numSerie[9];
	char	bfQuery[500];
	wchar_t w_query[500];
	int		i;

	cartao->GetNumSerie(bfNumSerie);										// Buffer para núm. de série
	for(i=0;i<7;i++) numSerie[i] = bfNumSerie[i];						// Obtem 8 primeiros chars
	numSerie[8]='\0';													// Descarta último char

	SQLINTEGER sId, sCreditos, cbCreditos, cbId, cbNome, iCount = 1;
    SQLCHAR szNome[200];

																// pega num serie
	numSerie[8]='\0';	
	sprintf(bfQuery, "SELECT id, nome, creditos FROM usuarios WHERE serial='%s'", numSerie);// prepara query

	if(conectado==false){
		cout << "Não é possível fazer uma consulta pois não está conectado ao banco" << endl;
		return false;
	}

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);			// Allocate statement handle

	mbstowcs(w_query, bfQuery, 500);									// Converte char para wide char
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)w_query, SQL_NTS);		// Envia a query

        if(retcode == SQL_SUCCESS){
            
            while(TRUE) {
                retcode = SQLFetch(hstmt);
                if(retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO){
                    cout<<"An error occurred";
					return false;
                }

                if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO){
                    SQLGetData(hstmt, 1, SQL_C_ULONG, &sId,       0,   &cbId);
					SQLGetData(hstmt, 2, SQL_C_CHAR,  szNome,     200, &cbNome);
                    SQLGetData(hstmt, 3, SQL_C_ULONG, &sCreditos, 0,   &cbCreditos);

					cartao->SetUsuario((char *)szNome);
					cartao->SetCreditos(sCreditos);
					cartao->SetID(sId);

                    iCount++;
                }else{
					cout << endl << endl;
					return true;
                }
            }
	}else{
		cout<<"Query execution error."<<endl;
		return false;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryUpdate.
/// Propósito: Envia query de UPDATE ao banco de dados.<br>
/// Entradas: Query de update<br>
/// Saídas: Retorna true se consulta ok ou false caso contrário
///
/// \param [in] query com uma query de update
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryUpdate(char query[300]){
	/// Notou-se que este método seria idêntico ao método QueryDelete.<br>
	/// Assim, o método QueryUpdate simplesmente chama o método QueryDelete passando a query:<br>
	/// return QueryDelete(query);<br>
	return QueryDelete(query);
}