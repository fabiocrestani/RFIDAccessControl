#include "Principal.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// PRINCIPAL (construtor).
/// Propósito: Inicializa variáveis e chama o método ProgramaPrincipal<br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
PRINCIPAL::PRINCIPAL(){
	setlocale(LC_ALL, "Portuguese");

	// Inicializa objetos
	Cmysql.SetaParametros(_T("Controle de Acesso"),_T("localhost"),_T("tester"),_T(""),_T("MySQL Server;"));
	I8051.SetaParametros(&serial, &Cmysql, &logs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// PRINCIPAL (destrutor).
/// Propósito: <br>
/// Entradas: Nenhuma<br>
/// Saídas: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
PRINCIPAL::~PRINCIPAL(){
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ProgramaPrincipal.
/// Propósito: Método principal. Na Orientação a Objetos substitui a função main()<br>
/// Entradas: Nenhuma<br>
/// Saídas: Inteiro com o código de retorno do programa<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int PRINCIPAL::ProgramaPrincipal(){
	bool			 em_loop = true;
	int				 opcao_menu = -1;
	LONG			 lLastError = ERROR_SUCCESS;
	TCHAR			 nomePortaSerial[5]=_T("COM5");

	menu.BoasVindas();
	if(!logs.Inicio()) return 1;								// Se houve falha nos logs, fecha programa

	// Tenta abrir a porta serial ----------------------------------------------------------------------------
	#if defined DefPortaPersonalizada							// Porta padrão COM5 ou porta personalizada
		cout << "Digite o número da porta: ";				    // Pede pro usuário digitar o nome da porta
		wscanf_s(_T("COM%s"), &nomePortaSerial);				// Recebe e formata porta digitada COMx
	#endif														
																//
	lLastError = serial.Open(nomePortaSerial, 0, 0, false);		// Abre a porta serial
	if(!serial.VerificaSeAbriu(lLastError, nomePortaSerial))	// Verifica se abriu a porta serial
		return ERROR_SERIAL_NO_DEVICE;							// Se não abriu, termina o programa
	// -------------------------------------------------------------------------------------------------------

	// Configura a porta serial para 9600bps, 8 bits, sem paridade, 1 stop bit -------------------------------
    lLastError = serial.Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if(!serial.VerificaSeConfigurou(lLastError))				// Verifica se configurou corretamente
		return lLastError;										// Se não configurou, termina o programa
																//
	serial.MostraConectado();									// Se aqui chegou, está conectado à serial
	// -------------------------------------------------------------------------------------------------------
	
	// Conecta com o banco de dados --------------------------------------------------------------------------
	Cmysql.Conecta();											// Tenta conectar ao banco de dados
	if(!Cmysql.MostraConcetado()){								// Mostra status (conectado ou não)
		system("pause");											//
		return 1;
	}else system("pause");
	// -------------------------------------------------------------------------------------------------------

	// Menu principal ----------------------------------------------------------------------------------------
	while(true){
		system("cls");
		while(opcao_menu==-1) opcao_menu = menu.Principal();	// Chama o menu principal
		switch(opcao_menu){										// Trata os casos
			case 0: return 0; break;							// 0: termina o programa
			case 1: I8051.ControleDeAcesso(); break;			// 1: controle de acesso
			case 2: I8051.CadastroCartao(); break;				// 2: cadastro de cartão
			case 3: menu.InserirCreditos(&Cmysql); break;		// 3: inserir créditos
			case 4: Cmysql.PegaTodos(); system("PAUSE"); break;	// 4: lista todos os cartões
			case 5: Cmysql.RemoveItem(); system("PAUSE"); break;// 5: remove cartão
			default: break;										// default: nada
		}
		opcao_menu = -1;
	}
	// -------------------------------------------------------------------------------------------------------

    serial.Close();
    return 0;
}