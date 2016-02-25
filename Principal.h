///////////////////////////////////////////////////////////////////////////////////////////////////
///	\file Principal.h
/// \class PRINCIPAL Principal.h "Principal.h" Classe principal para organizar todas as demais.
/// Autor: F�bio Crestani<br>
/// �ltima modifica��o em 21/01/2014<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include "ConexaoMySQL.h"
#include "InterfaceGrafica.h"
#include "Serial.h"
#include "Interface8051.h"
#include "TempoData.h"
#include "Menu.h"
#include "Log.h"
#include <string>
using namespace std;

//#define DefPortaPersonalizada 1		// Se n�o for definido, usar� a porta COM3.
										// Se for definido com qualquer valor, pedir� ao usu�rio para digitar a porta.

class PRINCIPAL{
private:
	CONEXAOMYSQL	 Cmysql;
	CSerial			 serial;
	LOG				 logs;
	INTERFACE8051    I8051;
	TEMPODATA		 data_temp;
	MEUMENU			 menu;

public:
	PRINCIPAL();
	~PRINCIPAL();

	int ProgramaPrincipal();
};