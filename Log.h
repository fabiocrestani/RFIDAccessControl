///////////////////////////////////////////////////////////////////////////////////////////////////
///	\file Log.h
/// \class LOG Log.h "Log.h" Classe para gravação de registros (logs) em arquivo de texto.
/// Autor: Fábio Crestani<br>
/// Última modificação em 13/01/2014<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <fstream>
using namespace std;
#include "Cartao.h"


class LOG{
private:
	ofstream	arquivo;
	bool		arquivo_ok;

public:
	LOG();
	~LOG();

	// Métodos que escrevem em arquivo de log
	bool Inicio();
	bool AcessoOK(CARTAO cartao);
	bool AcessoNaoOK(CARTAO cartao, char *mensagem);
	bool AcessoNaoOK2(char *mensagem);
};