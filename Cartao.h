//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Cartao.h Arquivo de cabe�alho do arquivo Cartao.c.
/// \class CARTAO Cartao.h "Cartao.h" Rotinas de manipula��o dos dados do cart�o como n�mero de s�rie, nome, 
///  id e cr�ditos.
///  O atributo de n�mero de s�rie assume que esteja-se usando um cart�o do tipo MIFARE com n�mero serial de 
///  16 bytes.<br>
///  Um objeto de conex�o com o banco de dados � agregado para facilitar a comunica��o com o mesmo.<br>
///  Autor: F�bio Crestani<br>
///  �ltima modifica��o em 12/01/2014<br>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Windows.h>
#include "ConexaoMySQL.h"

class CARTAO{
private:
	char		 serial[16];						// Aassumindo cart�o MIFARE com 16 bytes para serial
	char		 usuario[100];						//  nome do usu�rio do cart�o
	int			 creditos;							//  n�mero de cr�ditos do usu�rio neste cart�o
	int			 id;								//  identificador	
	char		 timestamp[50];
	CONEXAOMYSQL *Cmysql;                           // Objeto da classe CONEXAOMYSQL

public:
	CARTAO(CONEXAOMYSQL *mysql);
	~CARTAO();

	// Gets
	void GetDados(char *s, char *u, int *c, int *i);
	void GetNumSerie(char *s);
	void GetUsuario(char *u);
	int  GetCreditos(); 
	int  GetID();
	void ExibeDados();
	void GetTimeStamp(char *t);

	// Sets
	void SetDados(const char *s, const char *u, const int c);
	void SetNumSerie(const char *s);
	void SetUsuario(const char *u);
	void SetCreditos(const int c);
	void SetID(const int i);
	void SetTimeStamp(const char *t);

	// Cadastro
	bool VeirificaCartaoExiste();
	bool VeirificaIDExiste();
	bool Cadastra();
	bool InsereCartaoBanco();

	// Controle de acesso
	bool DecrementaCredito();
	bool IncrementaCredito();
};