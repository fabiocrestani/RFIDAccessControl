///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Interface8051.h
/// \class INTERFACE8051 Interface8051.h "Interface8051.h" Classe para comunicação e tratamento de 
/// mensagens com o programa do projeto de Controle de Acesso com RFID utilizando microcontrolador 
/// AT89C5131A.
///  Autor: Fábio Crestani<br>
///  Última modificação em 14/01/2015<br>
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Serial.h"
#include "ConexaoMySQL.h"
#include "Log.h"

#define	TIMEOUT_ACK 1
#define FLAG_INICIO_NUM_SERIE 58
#define FLAG_FIM_NUM_SERIE	  59

// Mensagens de retorno para o microcontrolador
#define ERRO_LEITURA_SERIAL       1 ///< Não foi possível ler o número de série do cartão
#define ERRO_PC                   2 ///< Erro genérico no programa do PC
#define ERRO_NAO_CADASTRADO       3 ///< Número de série não cadastrado
#define ERRO_CREDITO_INSUFICIENTE 4 ///< Número de créditos insuficiente (igual a 0)
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

	// Comunicação serial
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