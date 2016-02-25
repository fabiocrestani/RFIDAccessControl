///////////////////////////////////////////////////////////////////////////////////////////////////
///	\file Menu.h
/// \class MEUMENU Menu.h "Menu.h" Classe para exibi��o de menus.
/// Autor: F�bio Crestani<br>
/// �ltima modifica��o em 14/01/2014<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>
#include "ConexaoMySQL.h"
#include "Cartao.h"

class MEUMENU{
private:

public:
	MEUMENU();
	~MEUMENU();

	void BoasVindas();
	int  Principal();	
	bool InserirCreditos(CONEXAOMYSQL *Cmysql);
};