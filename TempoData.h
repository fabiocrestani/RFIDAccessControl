///////////////////////////////////////////////////////////////////////////////////////////////////
///	\file TempoData.h
/// \class TEMPODATA TempoData.h "TempoData.h" Classe para tratamento de estruturas de tempo e data.
///  Autor: Fábio Crestani<br>
///  Última modificação em 29/12/2014<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "windows.h"
#include <fstream>
using namespace std;


class TEMPODATA{
private:
	SYSTEMTIME lpLocalTime;

public:
	TEMPODATA();
	~TEMPODATA();
	
	void PegaLocalTime();
	void ImprimeHora();
	void ImprimeHoraAtualizada();

	void RetornaDataEHora(char saida[100]);
	void RetornaDataEHoraAtualizada(char saida[100]);
};