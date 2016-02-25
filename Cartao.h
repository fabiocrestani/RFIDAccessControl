//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Cartao.h Arquivo de cabeçalho do arquivo Cartao.c.
/// \class CARTAO Cartao.h "Cartao.h" Rotinas de manipulação dos dados do cartão como número de série, nome, 
///  id e créditos.
///  O atributo de número de série assume que esteja-se usando um cartão do tipo MIFARE com número serial de 
///  16 bytes.<br>
///  Um objeto de conexão com o banco de dados é agregado para facilitar a comunicação com o mesmo.<br>
///  Autor: Fábio Crestani<br>
///  Última modificação em 12/01/2014<br>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Windows.h>
#include "ConexaoMySQL.h"

class CARTAO{
private:
	char		 serial[16];						// Aassumindo cartão MIFARE com 16 bytes para serial
	char		 usuario[100];						//  nome do usuário do cartão
	int			 creditos;							//  número de créditos do usuário neste cartão
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