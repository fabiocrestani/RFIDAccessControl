///////////////////////////////////////////////////////////////////////////////////////////////////
///	\file ConexaoMYSQL.h
/// \class CONEXAOMYSQL ConexaoMYSQL.h "ConexaoMYSQL.h "  Classe para conexão com banco de dados 
/// MySQL através de driver ODBC.
///  Autor:<br>
///  Adaptado por Fábio Crestani<br>
///  Última modificação em 14/01/2014<br>
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <tchar.h>

class CARTAO;								// Para evitar a inclusão recursiva

class CONEXAOMYSQL{

private:
	SQLHENV		henv;
    SQLHDBC		hdbc;
    SQLHSTMT	hstmt;
    SQLRETURN	retcode;

    SQLWCHAR	OutConnStr[255];
    SQLSMALLINT OutConnStrLen;

	TCHAR		DSN[50];
	TCHAR		SERVER[100];
	TCHAR		UID[20];
	TCHAR		PWD[50];
	TCHAR		DRIVER[30];
	TCHAR		stringConexaoMySQL[250];

	bool		conectado;

public:
	CONEXAOMYSQL(TCHAR d[50], 
				 TCHAR s[100], 
				 TCHAR u[20], 
				 TCHAR p[50], 
				 TCHAR dvr[30]);

	CONEXAOMYSQL(void);

	~CONEXAOMYSQL();

	// Conexão
	void SetaParametros(TCHAR d[50]=L"", 
						TCHAR s[100]=L"", 
						TCHAR u[20]=L"", 
						TCHAR p[50]=L"", 
						TCHAR dvr[30]=L"");

	bool GetConectado(void);
	bool Conecta();
	bool MostraConcetado(void);
	
	// Queries
	bool PegaTodos();
	int  QueryNumRows(char query[300]);
	bool QueryInsert(char query[300]);
	bool QueryDelete(char query[300]);
	bool QueryCartao(CARTAO *cartao);
	bool RemoveItem();
	bool QueryUpdate(char query[300]);
};