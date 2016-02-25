///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Interface8051.h
/// \class INTERFACE8051 Interface8051.h "Interface8051.h" Classe para comunica��o e tratamento de 
/// mensagens com o programa do projeto de Controle de Acesso com RFID utilizando microcontrolador 
/// AT89C5131A.
///  Autor: F�bio Crestani<br>
///  �ltima modifica��o em 14/01/2015<br>
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Serial.h"
#include "ConexaoMySQL.h"
#include "Log.h"

#define	TIMEOUT_ACK 1
#define FLAG_INICIO_NUM_SERIE 58
#define FLAG_FIM_NUM_SERIE	  59

// Mensagens de retorno para o microcontrolador
#define ERRO_LEITURA_SERIAL       1 ///< N�o foi poss�vel ler o n�mero de s�rie do cart�o
#define ERRO_PC                   2 ///< Erro gen�rico no programa do PC
#define ERRO_NAO_CADASTRADO       3 ///< N�mero de s�rie n�o cadastrado
#define ERRO_CREDITO_INSUFICIENTE 4 ///< N�mero de cr�ditos insuficiente (igual a 0)
#define OK_ACESSO                 7 ///< Acesso ok

enum { EOF_Char = 27 };

class INTERFACE8051{

	bool		 primeiraConexao;
	CSerial		 *serial;
	CONEXAOMYSQL *Cmysql;
	LOG			 *log;

public:
	INTERFACE8051(CSerial *s, CONEXAOMYSQL *mysql, LOG *l);
	INTERFACE8051();
	~INTERFACE8051();
	
	void SetaParametros(CSerial *s, CONEXAOMYSQL *mysql, LOG *l);

	// Utilidades
	void Delay();
	void Delay(int delay);

	// Comunica��o serial
	bool ConfLeCharSerial();
	bool LeStringSerial(char *saida);
	bool LeCharSerial(char *saida);
	bool SerialEnquiry();
	bool SerialAcknowledge();
	bool SerialTentativaAcesso();

	// Projeto
	void ControleDeAcesso();
	bool CadastroCartao();
	bool LeNumeroSerieCartao(char *numserie, int *numSerieMax, bool *cancelar);
	bool ExtraiNumSerialAcesso(char *saida, char *timestamp_buffer);

	bool EnviaCodigoResposta(int codigo_resposta);
	bool EnviaNumCreditos(int numCreditos);
};