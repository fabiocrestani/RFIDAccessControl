///////////////////////////////////////////////////////////////////////////////////////////////////
///	\file Log.h
/// \class LOG Log.h "Log.h" Classe para grava��o de registros (logs) em arquivo de texto.
/// Autor: F�bio Crestani<br>
/// �ltima modifica��o em 13/01/2014<br>
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

	// M�todos que escrevem em arquivo de log
	bool Inicio();
	bool AcessoOK(CARTAO cartao);
	bool AcessoNaoOK(CARTAO cartao, char *mensagem);
	bool AcessoNaoOK2(char *mensagem);
};