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
/// Prop�sito: Seta os par�metros da conex�o SQL<br>
/// Entradas: Par�metros da conex�o. Veja mais detalhes abaixo<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] d alias de conex�o com driver ODBC
/// \param [in] s nome do servidor para conectar
/// \param [in] u nome do usu�rio para acessar o banco de dados
/// \param [in] p senha do usu�rio para acessar o banco de dados
/// \param [in] dvr vers�o do driver ODBC. (Ex.: MySQL Server;)
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
/// Prop�sito: Sobrecarga do m�todo construtor sem par�metros.<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CONEXAOMYSQL::CONEXAOMYSQL(void){
	conectado = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: destrutor da classe CONEXAOMYSQL
/// ~CONEXAOMYSQL (destrutor).
/// Prop�sito: Encerra a conex�o liberando os handles<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
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
/// SetaPar�metros.
/// Prop�sito: Seta os par�metros da conex�o SQL<br>
/// Entradas: Par�metros da conex�o. Veja mais detalhes abaixo<br>
/// Sa�das: Nenhuma<br>
///
/// \param [in] d alias de conex�o com driver ODBC
/// \param [in] s nome do servidor para conectar
/// \param [in] u nome do usu�rio para acessar o banco de dados
/// \param [in] p senha do usu�rio para acessar o banco de dados
/// \param [in] dvr vers�o do driver ODBC. (Ex.: MySQL Server;)
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
/// Prop�sito: Obt�m o estado da conex�o<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se est� conectado ou false caso contr�rio
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::GetConectado(void){
	return conectado;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Conecta.
/// Prop�sito: Faz uma conex�o com o banco de dados e mant�m conectado para uso futuro<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se teve sucesso ou false caso contr�rio<br>
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
/// Prop�sito: Imprime no console todos os usu�rios e seus respectivos dados<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se teve sucesso ou false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::PegaTodos(){
	SQLINTEGER sId, sCreditos, cbCreditos, cbId, cbTestStr, cbTestStr2, iCount = 1;
    SQLCHAR szTestStr[200];
	SQLCHAR szTestStr2[200];

	system("cls");
	cout << " ---------------------------------------------------"  << endl;
	cout << " Listando todos os usu�rios cadastrados" << endl;
	cout << " ---------------------------------------------------"  << endl << endl;

	cout << "# " << "id\tCr�ditos    Usu�rio " << endl << endl ;

	if(conectado==false){
		cout << "N�o � poss�vel fazer uma consulta pois n�o est� conectado ao banco" << endl;
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
/// Prop�sito: Retorna o n�mero de linhas de uma determinada query<br>
/// Entradas: char query com a consulta<br>
/// Sa�das: Retorna -1 se houve erro. Ou o n�mero de linhas da query<br>
///
/// \param [in] query com a consulta a se verificar o n�mero de linhas
///////////////////////////////////////////////////////////////////////////////////////////////////
int CONEXAOMYSQL::QueryNumRows(char query[300]){
	SQLINTEGER sTestInt, cbTestInt, iCount = 0;
	wchar_t w_query[500];

	if(conectado==false){
		cout << "N�o � poss�vel fazer uma consulta pois n�o est� conectado ao banco" << endl;
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
/// Prop�sito: Faz uma query de inser��o de dados no banco de dados<br>
/// Entradas: char query com a consulta<br>
/// Sa�das: Retorna true se teve sucesso ou false caso contr�rio<br>
///
/// \param [in] query com a consulta de inser��o
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryInsert(char query[500]){
	wchar_t w_query[500];

	if(conectado==false){
		cout << "N�o � poss�vel fazer uma consulta pois n�o est� conectado ao banco" << endl;
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
/// Prop�sito: Remove um item ou mais do banco de dados<br>
/// Entradas: char Query[300] com a consulta de remo��o<br>
/// Sa�das: Retorna true se removeu e false caso contr�rio<br>
///
/// \param [in] query tem a consulta de remo��o
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryDelete(char query[300]){
	wchar_t w_query[500];

	if(conectado==false){
		cout << "N�o � poss�vel fazer uma consulta pois n�o est� conectado ao banco" << endl;
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
/// Prop�sito: Prepara uma query para remo��o de um item do banco de dados<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se removeu e false caso contr�rio<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::RemoveItem(){
	int id_remover;
	char bfQuery[100]="";

	system("cls");
	PegaTodos();														// mostra todos os cadastrados
	cout << "Digite o ID do cart�o que deseja remover ou 0 para cancelar: ";
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
		cout << endl << "N�o foi poss�vel remover. Verifique o item com este ID existe" << endl << endl;
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MostraConectado.
/// Prop�sito: Exibe na tela o status da conex�o (conectado ou n�o)
/// Entradas: Nenhuma
/// Sa�das: Retorna true se est� conectado ou false caso contr�rio
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::MostraConcetado(void){
	if(GetConectado()){
		cout << " MYSQL:  Conectado!" << endl << endl;
		return true;
	}
	else{
		cout << " MYSQL:  N�o Conectado!" << endl << endl;
		return false;
	}
}
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// QueryCartao.
/// Prop�sito: Consulta o banco de dados com o n�mero de s�rie fornecido no objeto cart�o e retorna
/// no mesmo objeto o id, o nome do usu�rio e n�mero de cr�ditos do cart�o<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Retorna true se consulta ok ou false caso contr�rio
///
/// \param [in,out] cartao ponteiro da classe CARTAO para transfer�ncia das informa��es
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryCartao(CARTAO *cartao){
	char	bfNumSerie[1024];
	char	numSerie[9];
	char	bfQuery[500];
	wchar_t w_query[500];
	int		i;

	cartao->GetNumSerie(bfNumSerie);										// Buffer para n�m. de s�rie
	for(i=0;i<7;i++) numSerie[i] = bfNumSerie[i];						// Obtem 8 primeiros chars
	numSerie[8]='\0';													// Descarta �ltimo char

	SQLINTEGER sId, sCreditos, cbCreditos, cbId, cbNome, iCount = 1;
    SQLCHAR szNome[200];

																// pega num serie
	numSerie[8]='\0';	
	sprintf(bfQuery, "SELECT id, nome, creditos FROM usuarios WHERE serial='%s'", numSerie);// prepara query

	if(conectado==false){
		cout << "N�o � poss�vel fazer uma consulta pois n�o est� conectado ao banco" << endl;
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
/// Prop�sito: Envia query de UPDATE ao banco de dados.<br>
/// Entradas: Query de update<br>
/// Sa�das: Retorna true se consulta ok ou false caso contr�rio
///
/// \param [in] query com uma query de update
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CONEXAOMYSQL::QueryUpdate(char query[300]){
	/// Notou-se que este m�todo seria id�ntico ao m�todo QueryDelete.<br>
	/// Assim, o m�todo QueryUpdate simplesmente chama o m�todo QueryDelete passando a query:<br>
	/// return QueryDelete(query);<br>
	return QueryDelete(query);
}