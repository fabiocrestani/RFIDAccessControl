#include <iostream>
using namespace std;
#include <conio.h>
#include "Menu.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MENU (construtor).
/// Prop�sito: <br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MEUMENU::MEUMENU(){
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ~MENU (destrutor).
/// Prop�sito: <br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MEUMENU::~MEUMENU(){
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// BoasVindas.
/// Prop�sito: Exibe mensagem de boas vindas<br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MEUMENU::BoasVindas(){
	cout << " ---------------------------------------------------"  << endl;
	cout << " Inicializando o programa.. " << endl;
	cout << " ---------------------------------------------------"  << endl << endl;
	cout << " Status: " << endl << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Principal.
/// Prop�sito: Exibe o menu principal deste projeto<br>
/// Entradas: Nenhuma<br>
/// Sa�das: inteiro correspondente � op��o selecionada no menu
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int MEUMENU::Principal(){
	int opcao_digitada = -1;

	system("cls");
	cout << " ---------------------------------------------------"  << endl;
	cout << " Menu Principal " << endl;
	cout << " ---------------------------------------------------"  << endl << endl;

	cout << "  Selecione uma op��o: -----------------------------" << endl <<
			"|                                                   |" << endl <<
		    "| 1 para iniciar modo normal de controle de acesso  |" << endl <<
			"| 2 para cadastrar novo cart�o                      |" << endl <<
			"| 3 para inserir cr�ditos                           |" << endl <<
			"| 4 para listar todos os cart�es                    |" << endl <<
			"| 5 para remover um cart�o do banco de dados        |" << endl <<
			"| 0 para sair do programa                           |" << endl <<
			"|                                                   |" << endl <<
			" ---------------------------------------------------"  << endl;

	opcao_digitada = getch();
	switch(opcao_digitada){
		case '1': opcao_digitada = 1; break;
		case '2': opcao_digitada = 2; break;
		case '3': opcao_digitada = 3; break;
		case '4': opcao_digitada = 4; break;
		case '5': opcao_digitada = 5; break;
		case '0': opcao_digitada = 0; break;
		default:  opcao_digitada = -1; break;
	}

	return opcao_digitada;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// InserirCreditos.
/// Prop�sito: Exibe o menu para inser��o de cr�ditos<br>
/// Entradas: Ponteiro para uma conex�o mysql
/// Sa�das: Nenhuma<br>
///
/// \param [in] *Cmysql ponteiro para CONEXAOMYSQL
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MEUMENU::InserirCreditos(CONEXAOMYSQL *Cmysql){
	char	bfQueryUpdate[500];
	int		opcao_digitada=-1;
	int		id_alterar, qnt_creditos;
	CARTAO	cartao_temp(Cmysql);

	while(opcao_digitada!=0){
		system("cls");
		cout << " ---------------------------------------------------"  << endl;
		cout << " Inser��o de cr�ditos " << endl;
		cout << " ---------------------------------------------------"  << endl << endl;

		cout << "Digite 1 para exibir todos os usu�rios cadastrados" << endl;
		cout << "Digite 2 para continuar sem exibir os usu�rios cadastrados" << endl;
		cout << "Digite 0 para cancelar e retornar ao menu principal" << endl;

		opcao_digitada = getch();
		switch(opcao_digitada){
			case '1': Cmysql->PegaTodos(); opcao_digitada=1; break;
			case '2': opcao_digitada=2; break;
			case '0': opcao_digitada=0; return false; break;
		} // fim do switch

		if(opcao_digitada == 1 || opcao_digitada == 2){
			cout << endl << "Digite o ID que deseja alterar os cr�ditos: ";
			cin >> id_alterar;
			cout << endl << "Digite a quantidade de cr�ditos que deseja inserir : ";
			cin >> qnt_creditos;
			system("cls");
			cout << "Inserindo cr�ditos..." << endl;

			cartao_temp.SetID(id_alterar);

			if(cartao_temp.VeirificaIDExiste()){									// 1. Verifica se existe
																					
				sprintf(bfQueryUpdate,											    // 2. prepara query de UPDATE
						"UPDATE usuarios SET creditos= creditos + %i WHERE id='%i'",//
						qnt_creditos, id_alterar);									//

				if(Cmysql->QueryUpdate(bfQueryUpdate)){								// 4. Envia essa query
					cout << "Cr�ditos inseridos. " << endl;							//
				}
				else cout << "Erro. Cr�ditos n�o inseridos" << endl << endl;		// Se houve erro no update
			}
			else																	// Se n�o encontrou o cart�o
				cout << "Erro. Cart�o com ID " << id_alterar << " n�o existe" << endl << endl;

			system("pause");
			cout << endl << endl << "Continuar no modo de inser��o?" << endl << 
				"Digite 1 para continuar ou " << endl <<
				"Digite 0 para retornar ao menu principal " << endl;
			opcao_digitada = getch();
			if(opcao_digitada == '1') opcao_digitada = -1;
			if(opcao_digitada == '0') return true;
		}

	} // fim do while principal
	return false;
}