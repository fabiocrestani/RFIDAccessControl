#include <iostream>
using namespace std;
#include <windows.h>
#include "Interface8051.h"
#include "Cartao.h"
#include "TempoData.h"
#include <tchar.h>
#include <string>
#include <conio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// INTERFACE8051 (construtor).
/// Propósito: Inicializa o ponteiro para a classe serial. Parâmetro obrigatório!<br>
/// Entradas: s é um ponteiro da classe CSerial que contém uma conexão serial aberta<br>
///			 mysql é um ponteiro da classe CONEXAOMYSQL para uma conexão mysql aberta<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] *s ponteiro da classe CSerial
/// \param [in] *mysql ponteiro da classe CONEXAOMYSQL
/// \param [in] *l ponteiro da classe LOG
///////////////////////////////////////////////////////////////////////////////////////////////////
INTERFACE8051::INTERFACE8051(CSerial *s, CONEXAOMYSQL *mysql, LOG *l){
	primeiraConexao = true;
	serial = s;											// Objeto da classe CSerial agregado
	Cmysql = mysql;										// Objeto da classe CONEXAOMYSQL agregado
	log    = l;											// Objeto da classe LOG (fracamente) agregado
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// INTERFACE8051 (construtor).
/// Propósito: Sobrecarga do método construtor sem parâmetros.<br>
/// <b>Importante:</b> Chamar o método SetaParametros é obrigatório para continuar o programa.<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
INTERFACE8051::INTERFACE8051(){
	primeiraConexao = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ~INTERFACE8051 (destrutor).
/// Propósito: <br>
/// Entradas: <br>
/// Saídas: 
///
///////////////////////////////////////////////////////////////////////////////////////////////////
INTERFACE8051::~INTERFACE8051(){}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetaParametros.
/// Propósito: Seta os parâmetros desta classe. Útil quando se utiliza o construtor sem parâmetros<br>
/// Entradas: s é um ponteiro da classe CSerial que contém uma conexão serial aberta<br>
///			 mysql é um ponteiro da classe CONEXAOMYSQL para uma conexão mysql aberta<br>
/// Saídas: Nenhuma<br>
///
/// \param [in] *s ponteiro da classe CSerial
/// \param [in] *mysql ponteiro da classe CONEXAOMYSQL
/// \param [in] *l ponteiro da classe LOG
///////////////////////////////////////////////////////////////////////////////////////////////////
void INTERFACE8051::SetaParametros(CSerial *s, CONEXAOMYSQL *mysql, LOG *l){
	serial = s;											// Objeto da classe CSerial (fracamente) agregado
	Cmysql = mysql;										// Objeto da classe CONEXAOMYSQL (fracamente) agregado
	log    = l;											// Objeto da classe LOG (fracamente) agregado
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Delay.
/// Propósito: Delay padrão (sem parâmetros) de 100 milisegundos<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void INTERFACE8051::Delay(){
	Sleep(100);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Delay (método sobrecarregado).
/// Propósito: Delay com parâmetro de tempo em milisegundos<br>
/// Entradas: int delay é o tempo para o delay<br>
/// Saídas: Nenhuma
///
/// \param [in] delay tempo de delay em milissegundos
///////////////////////////////////////////////////////////////////////////////////////////////////
void INTERFACE8051::Delay(int delay){
	Sleep(delay);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ConfLeCharSerial.
/// Propósito: Configura a classe serial para receber dados<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se a comunicação com o microcontrolador foi bem sucessida, ou false caso 
///         contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::ConfLeCharSerial(){
	LONG		lLastError = ERROR_SUCCESS;

	// Configura máscas que podem ativar recepção da porta serial----------------------------------
    lLastError = serial->SetMask(CSerial::EEventBreak |	CSerial::EEventCTS  | CSerial::EEventDSR  | 
							     CSerial::EEventError | CSerial::EEventRing | CSerial::EEventRLSD | 
								 CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS){
		cout << "Unable to set COM-port event mask";
		system("pause");
		return false;
	} 
	// --------------------------------------------------------------------------------------------

	// Configura classe serial para modo NonBlocking ----------------------------------------------
    lLastError = serial->SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS){									// Don't know how many 
		cout << "Unable to set COM-port read timeout.";					// bytes will be received
		system("pause");												
		return false;
	} 
	// --------------------------------------------------------------------------------------------

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// LeStringSerial.
/// Propósito: Lê uma string da porta serial aberta<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se a comunicação com o microcontrolador foi bem sucessida, ou false caso 
///         contrário
///
/// \param [out] saida recebe uma string lida da serial
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::LeStringSerial(char *saida){
	DWORD		dwBytesRead = 0;
	char		abBuffer[1024];
	TEMPODATA	data_temp;
	//int			i=0;

	// Lê os dados na porta serial ----------------------------------------------------------------
	dwBytesRead = 0;												//
	do{
		serial->Read(abBuffer, sizeof(abBuffer), &dwBytesRead);		// Read data from the COM-port
		//if (dwBytesRead > 0){
			//for(i=0;i<dwBytesRead;i++){
				//printf("%i ", abBuffer[i]);
			//}
			//printf("\n");
			//i++;
		//}
	}while(dwBytesRead == sizeof(abBuffer));						// Enquanto não leu tudo
	// --------------------------------------------------------------------------------------------

	//saida = abBuffer;
	// TODO: strcpy é unsafe
	strcpy(saida, abBuffer);										// unsafe

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// LeCharSerial.
/// Propósito: Lê apenas um único char da porta serial aberta<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se a comunicação com o microcontrolador foi bem sucessida, ou false caso 
///         contrário
///
/// \param [out] saida recebe um caractere lido da serial
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::LeCharSerial(char *saida){
	char abBuffer[1024];
	char *pabBuffer = abBuffer;

	if(LeStringSerial(pabBuffer)){										// Lê uma string mas
		*saida = abBuffer[0];											// pega só a primeira letra
		return true;
	}
	else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SerialEnquiry.
/// Propósito: É chamado ao receber o caracter de Enquiry. Então responde com um ACK para o uC.<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se a comunicação com o microcontrolador foi bem sucessida, ou false caso 
///         contrário
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::SerialEnquiry(){					
	TEMPODATA		 data_temp;
	
	if(SerialAcknowledge()){						     // Responde o enquiry com um acknowledge
		cout << "Conectado ao microcontrolador em ";	 // Mostra informação no console
		data_temp.ImprimeHoraAtualizada();				 // Informação de hora
		cout << " (Pressione ESC para encerrar)" << endl;//
	}
									
	return true;													
}																	

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SerialAcknowledge.
/// Propósito: Método para responder o enquiry com um char de ackowledge<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se foi bem sucessido, ou false caso contrário
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::SerialAcknowledge(){
	char b[1] = "";									// Estas três linhas convertem o valor
	b[0] = ASCII_ACK;								// de ASCII_ACK (06) para um valor apontado
	LPCSTR buffer = b;								// por um LPCSTR para ser usado na função Write

	if((serial->Write(buffer))>=0)					// Envia o ASCII_ACK através da porta serial
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SerialTentativaAcesso.
/// Propósito: Método chamado quando é recebido um caractere '?' indicando tentativa de acesso<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se foi bem sucessido, ou false caso contrário
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::SerialTentativaAcesso(){
	char	num_serial_extraido[7];
	char    timestamp_extraido[50];
	int		i, status;
	CARTAO  cartao(Cmysql);

	cartao.SetDados("", "", 0);

	// 1. Extrai o número serial ------------------------------------------------------------------
	if(ExtraiNumSerialAcesso(num_serial_extraido, timestamp_extraido)){
		cartao.SetNumSerie(num_serial_extraido);				// Set numSerial no cartão
		cartao.SetTimeStamp(timestamp_extraido);				// Set timestamp no cartão
		cout << "Número serial extraído: ";						// Mostra numSerial extraído
		for(i=0; i<7;i++) printf("%x ", num_serial_extraido[i]);//
	}
	else{
		cartao.SetTimeStamp("00/00/0000 00:00:00");
		EnviaCodigoResposta(ERRO_LEITURA_SERIAL);
		log->AcessoNaoOK2("Erro na extração do número serial");
		return false;
	}
	// --------------------------------------------------------------------------------------------

	// 2. Verifica se o cartão existe -------------------------------------------------------------
	if(!cartao.VeirificaCartaoExiste()){						// Verifica se este serial existe
		cout << "Cartão não cadastrado" << endl;
		EnviaCodigoResposta(ERRO_NAO_CADASTRADO);				// Caso não exista, informa
		log->AcessoNaoOK2("Cartão não cadastrado");
		return false;
	}
	// --------------------------------------------------------------------------------------------

	// 3. Consulta o banco de dados com este valor ------------------------------------------------
	cartao.SetNumSerie(num_serial_extraido);					// Seta núm. serie
	if(!Cmysql->QueryCartao(&cartao))							// Esta consulta retorna nome e núm.
	{															// de créditos no objeto cartao
		EnviaCodigoResposta(ERRO_PC);
		log->AcessoNaoOK(cartao, "Erro com o banco de dados");
		cout << "Erro na comunicação com o banco de dados" << endl;
		return false;
	}
	cout << "Cartão encontrado: " << endl;
	// --------------------------------------------------------------------------------------------

	// 4. Verifica se tem créditos ----------------------------------------------------------------
	if(cartao.GetCreditos() < 1){								// 
		EnviaCodigoResposta(ERRO_CREDITO_INSUFICIENTE);
		log->AcessoNaoOK(cartao, "Crédito insuficiente");

		cout << "Créditos insuficientes para o acesso!" << endl;
		return false;
	}
	// --------------------------------------------------------------------------------------------

	// TODO: 5. Decrementa crédito ----------------------------------------------------------------------
	if(!cartao.DecrementaCredito()){
		EnviaCodigoResposta(ERRO_PC);
		log->AcessoNaoOK(cartao, "Erro com o banco de dados");
		cout << "Erro na comunicação com o banco de dados" << endl;
		return false;
	}

	cartao.ExibeDados();										// Dados após o acesso
	cout << endl;
	// --------------------------------------------------------------------------------------------

	// 7. Envia mensagem de confirmação concedendo acesso -----------------------------------------
	if(!EnviaCodigoResposta(OK_ACESSO)){						// Envia confirmação de acesso		
		log->AcessoNaoOK(cartao, "Erro ao enviar código de reposta");
		cout << "Erro na comunicação com o microcontrolador" << endl;
		cartao.IncrementaCredito();
		cartao.ExibeDados();									// Para conferir que crédito não mudou
		cout << endl;
		return false;
	}

	Delay(500);												    // Aguarda microcontrolador processar
	EnviaNumCreditos(cartao.GetCreditos());					    // Envia número de créditos
	log->AcessoOK(cartao);										// Escreve no arquivo de log

	return true;												// Fim deste metódo
	// --------------------------------------------------------------------------------------------
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ExtraiNumSerialAcesso.
/// Propósito: Método que recebe da porta serial o número de série do cartão e o retorna já 
/// formatado para ser utilizado no banco de dados.<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se foi bem sucessido, ou false caso contrário
///
/// \param [out] *saida ponteiro para o vetor com o número de série.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::ExtraiNumSerialAcesso(char *saida, char *timestamp_buffer){
	TEMPODATA data_temp;
	int		 i;
	char	 char_recebido[1024];
	char	 timestamp[1024];
	bool	 erro_num_serial = true, num_serial_vazio = true;
	char	 STRING_ERRO_NUM_SERIAL[40]={ 69, 114, 114, 111,  58,  32, 110, -29, 111,  32, 102, 
										 111, 105,  32, 112, 111, 115, 115, -19, 118, 101, 108,  
										  32, 108, 101, 114,  32, 110, 117, 109,  32, 100, 101,  
										  32, 115, -23, 114, 105, 101,  13};

	cout << "Recebeu tentativa de acesso em ";		
	data_temp.ImprimeHoraAtualizada();							// Informação de hora
	cout << endl;

	Delay(1000);												// Aguarda o uC ler o número serial
	ConfLeCharSerial();											// Configura para receber da serial
	LeStringSerial(char_recebido);								// Lê da serial

	for(i=0;i<40;i++){											// Percorre a string recebida
		if(STRING_ERRO_NUM_SERIAL[i] != char_recebido[i])       // Verificando se é a mensagem de erro
			erro_num_serial = false;

		if(char_recebido[i] != -36)								// Verifica também se está vazio
			num_serial_vazio = false;
	}

	if(erro_num_serial){
		cout << "Erro ao ler o número serial. Aproxime o cartão e aguarde a leitura" << endl;
		return false;
	}

	if(num_serial_vazio){
		cout << "Número serial vazio. Aproxime o cartão e aguarde a leitura" << endl;
		return false;
	}
	
	cout << "Número serial lido com sucesso" << endl;

	// desloca string para a esquerda, removendo o primeiro caractere:
	for(i=0;i<7;i++) saida[i] = char_recebido[i+1];

	// extrai timestamp
	for(i=0;i<19;i++) timestamp[i] = char_recebido[i+10];
	for(i=0;i<19;i++) timestamp_buffer[i] = timestamp[i];
	timestamp_buffer[i]='\0';
	cout << endl << "timestamp extraído: " << timestamp_buffer << endl << endl;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ControleDeAcesso.
/// Propósito: Este método fica "ouvindo" a porta serial pegando um caractere de cada vez, e então,
///				dependendo do caractere recebido decide o que fazer. Opções possíveis:<br>
///				1. ENQUIRY (5)  microcontrolador enviou um pedido de reconhecimento<br>
///             2. '?'     (63) microcontrolador enviou tentativa de acesso, isto coloca o pc em
///								estado aguardando o envio do número serial do cartão lido<br>
///             3. ':'     (58) microcontrolador avisa que vai enviar o número serial<br>
///				4. ';'     (59) microcontrolador avisa que terminou de enviar o número serial<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void INTERFACE8051::ControleDeAcesso(){
	TEMPODATA		 data_temp;
	bool			 em_loop = true;
	int				 i;
	char			 char_recebido = 0;
		
	ConfLeCharSerial();
	cout << "Aguardando mensagem da serial (Pressione ESC para encerrar)" << endl;

	while(em_loop){	

		LeCharSerial(&char_recebido);							// Recebe da porta serial e decide o que fazer

		switch(char_recebido){									// Trata opções possíveis

			case ASCII_ENQ:										// Se recebeu ENQ
				SerialEnquiry(); break;				 

			case ASCII_INTERROGACAO:							// Se recebeu tentativa de acesso
				SerialTentativaAcesso(); 
				Delay(1000);
				break;
				
		} // fim do switch
		
		// Tratamento de pressionar ESC ----------------------------------------------------------------------
		for(i=0;i<10;i++){										
			if(GetAsyncKeyState(VK_ESCAPE)){					
				em_loop = false;
				break;
			}
			Delay(1);
		}
		// ---------------------------------------------------------------------------------------------------
		
	} // fim do while principal
} // fim do método ControleDeAcesso

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CadastroCartao.
/// Propósito: Chama os método de cadastro de um novo cartão<br>
/// Entradas: Nenhuma<br>
/// Saídas: Retorna true se cadastrou ou false caso contrário<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::CadastroCartao(){
	char	numSerie[1024];
	int		numSerieMax;
	CARTAO  cartao(Cmysql);
	bool	cadastrado = false;
	bool	em_loop  = true;
	bool	cancelar = false;
	int		opcao;

	while(em_loop){

		cadastrado = false;

		if(LeNumeroSerieCartao(numSerie, &numSerieMax, &cancelar)){
			if(cancelar){ 
				em_loop=false;
				break;
			}
			system("cls");
			cout << endl << "Cadastrando cartão com o número de série: ";
			for(int k=0; k<numSerieMax-1;k++) printf("%x ", numSerie[k]);	// Mostra num. série no console

			cartao.SetNumSerie(numSerie);									// Guarda o num. série lido
			cadastrado = cartao.Cadastra();									// Cadastra
		}
		else{
			printf("\a");													// Alerta sonoro
			if(cancelar){ 
				em_loop=false;
				break;
			}
			cout << endl << endl << "Não foi possível cadastrar o cartão." << endl << 
									"Aproxime o cartão e digite 1 para tentar novamente ou 0 para cancelar" << endl;
			opcao = getch();
			if(opcao == '1') em_loop = true; else em_loop = false;
		}
	}

	return cadastrado;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// LeNumeroSerieCartao.
/// Propósito: Espera o microcontrolador enviar um ENQ, então responde com um ACK. Após isso, o
/// microcontrolador começa a enviar o número de série. Este método recebe este número de série e
/// o extrai corretamente dos dados recebidos.<br>
/// Entradas: char *numSerieSaida, int *numSerieMax<br>
/// Saídas: Retorna true se extraiu ou false caso contrário<br>
///
/// \param [out] *numSerieMax Indica a posição do último caractere do número de série
/// \param [out] *numSerieSaida Tem o número de série extraído
/// \param [out] *cancelar Variável auxiliar para cancelar o loop que chama este método
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::LeNumeroSerieCartao(char *numSerieSaida, int *numSerieMax, bool *cancelar){
	char numSerie[16];															// Assumindo no máximo 16 bytes
	char char_recebido = 0;
	bool em_loop = true;
	bool loop_interno = true;
	bool sucesso = false;
	int  i = 0, fim_serial = 0, j = 0;
	char opcao;
		
	ConfLeCharSerial();

	// Faz a leitura da porta serial  ----------------------------------------------------------------------------
		char bfSerial[1024];
		char *pbfSerial = bfSerial;
		sucesso = false;

		system("cls");
		cout << " ---------------------------------------------------"  << endl;
		cout << " Modo de cadastro de cartão" << endl;
		cout << " ---------------------------------------------------"  << endl << endl;
		cout << " Pressione a tecla Reset da placa do microntrolador." << endl <<
			    " Mantenha pressionada a tecla SW1 da placa até entrar no modo de cadastro. " << endl <<
				" Aproxime o cartão a ser cadastrado e aguarde a leitura. " << endl <<
				" Ou pressione ESC para cancelar " << endl << endl;

		// Agurda ENQ e envia ACK --------------------------------------------------------------------------------
		while(loop_interno){	
			LeCharSerial(&char_recebido);								// Recebe da porta serial e decide o que fazer
			cout << "Aguardando o microcontrolador" << endl;
			
			if(char_recebido == ASCII_ENQ){								// Se recebeu ENQ
				SerialEnquiry();										// envia ACK
				break;	
			}

			for(i=0;i<1000;i++){											// Tratamento de pressionar ESC
				if(GetAsyncKeyState(VK_ESCAPE)){					
					*cancelar = true;
					loop_interno = false;
					return false;
				}
			Delay(1);
			}
		}
		loop_interno = false;
		Delay(1000);
		// -------------------------------------------------------------------------------------------------------

		LeStringSerial(pbfSerial);

		if(bfSerial[0]==FLAG_INICIO_NUM_SERIE){							// caractere para iniciar num. série
			i = fim_serial = 0;
			while(i<16){												// percorre a string recebida
				if(bfSerial[i+1]==FLAG_FIM_NUM_SERIE){					// caractere para terminar num. série
					fim_serial = i+1;									// determina o final do num. série
					i = 16;
					em_loop = false;
					sucesso = true;
				}
				i++;

			} // fim do while interno
		} // fim do if :

		for(int k=0; k<fim_serial-1;k++) numSerie[k] = bfSerial[k+1];	// Descarta-se o primeiro byte
		cout << endl << "Número de serie extraído: ";
		for(int k=0; k<fim_serial-1;k++) printf("%x ", numSerie[k]);	// mostra num. série no console
			
		if(!sucesso){
			return false;
		}
		else{
			cout << endl << endl << "Número de série lido com sucesso." << endl <<
								    "Digite 1 para ler o número serial novamente ou " << endl << 
									"Digite 2 para aceitar o número de série lido" << endl;
			opcao = getch();
			if(opcao == '2'){											// Se é válido, prepara
																		// as variavéis de saída:
				strcpy(numSerieSaida, numSerie);						// Número de série e
				numSerieSaida[fim_serial] = '\0';
				*numSerieMax = fim_serial;								// Tamanho do número de série
				return true;
			}
			else return false;
		}

	return sucesso;
} // fim do método de leitura de número de série

///////////////////////////////////////////////////////////////////////////////////////////////////
/// EnviaCodigoResposta.
/// Propósito: Método que envia via porta serial um código de resposta que será interpretado pelo
/// micrcontrolador. Opções possíveis:<br>
/// <ul>
/// <li>ERRO_LEITURA_SERIAL       (1): Não foi possível ler o número de série do cartão</li>
/// <li>ERRO_PC                   (2): Erro genérico no programa do PC</li>
/// <li>ERRO_NAO_CADASTRADO       (3): Número de série não cadastrado</li>
/// <li>ERRO_CREDITO_INSUFICIENTE (4): Número de créditos insuficiente (igual a 0)</li>
/// <li>OK_ACESSO                 (5): Acesso ok</li>
/// </ul>
/// <br>
/// Entradas: Código de resposta a ser enviado<br>
/// Saídas: Retorna true se enviou ou false caso contrário<br>
///
/// \param [in] codigo_resposta inteiro com o código de resposta a ser enviado.
/// \sa Interface8051.h
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::EnviaCodigoResposta(int codigo_resposta){
	int _timeout = 0;
	char char_recebido = 0;
	char b[1] = "";												// Estas três linhas convertem o valor
	b[0] = codigo_resposta;										// de codigo_reposta para um valor apontado
	LPCSTR buffer = b;											// por um LPCSTR para ser usado na função Write

	SerialAcknowledge();

	ConfLeCharSerial();
	do{															// Espera o microcontrolador enviar código
		serial->Write(buffer);									// idêntico ao código a ele enviado
		LeCharSerial(&char_recebido);							// Recebe char do microcontrolador
		if(_timeout>70000) break;
		_timeout++;
		if(char_recebido==5) break;
	}while(char_recebido != codigo_resposta);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// EnviaNumCreditos.
/// Propósito: Envia via porta serial o número de créditos atual do usuário que teve seu acesso
/// confirmado, para que o microcontrolador possa exibi-lo no display.<br>
/// <br>
/// Este método envia o número de créditos em formato BCD e codificado em ASCII serializado do byte
/// BCD mais significativo para o menos siginificativo. Nesta versão, o valor máximo é 999. Se o
/// valor for maior ou igual a 999, o PC envia 999 e o microcontrolador exibe "999+" no display.<br>
/// Entradas: Número de créditos a ser enviado<br>
/// Saídas: Retorna true se enviou ou false caso contrário<br>
///
/// \param [in] numCreditos inteiro com o número de créditos a ser enviado.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool INTERFACE8051::EnviaNumCreditos(int numCreditos){
	char	b[3] = "";												
	LPCSTR  buffer = b;
	int		dez, cen, uni;

	// Conversão para BCD codificado em ASCII ------------------------------------------------------
	if(numCreditos>999) numCreditos = 999;						// Limita o núm. créditos para 999
	cen = numCreditos/100;										// Extrai centenas
	dez = (numCreditos - (cen*100))/10;							// Extrai dezenas
	uni = numCreditos - (cen*100) - (dez*10);					// Extrai unidades
	b[0] = cen + 48;											// Copia para o vetor
	b[1] = dez + 48;											// 48 = 0x30 (codifica em ASCII)
	b[2] = uni + 48;

	// Envia ---------------------------------------------------------------------------------------
	if(serial->Write(buffer))
		return true;
	else
		return false;
}
