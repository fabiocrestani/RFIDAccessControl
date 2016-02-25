/////////////////////////////////////////////////////////////////////////////////////////////
/// \file Main.cpp Main.
/// N�o h� muita coisa por aqui. A classe PRINCIPAL � a classe mais importante.
/// \sa Principal.h
///
/////////////////////////////////////////////////////////////////////////////////////////////

/*! \mainpage Documenta��o do programa para PC.
 * 
 * <i>Por F�bio Crestani</i>
 *
 * \section proposta Proposta
 *
 * Este projeto se prop�e a implementar um sistema de controle de acesso utilizando um sistema
 * com um microcontrolador da fam�lia 8051 e um m�dulo de RFID.<br> O microcontrolador se comunica
 * com o computador via porta serial e valida ou n�o o acesso de um usu�rio.
 *
 * \image html diagrama_blocos.jpg
 *
 * \section fluxograma Fluxograma resumido
 * \image html fluxograma.jpg
 *
 * \section classes Diagrama de classes
 * \image html DiagramaDeClasses.jpg
 *
 * \section mais_info Mais informa��es
 *
 * Para mais informa��es acesse o relat�rio do projeto e/ou a documenta��o do programa do microcontrolador.
 *
 */

#include "Principal.h"

int main(){

	PRINCIPAL objetoPrincipal;

	return(objetoPrincipal.ProgramaPrincipal());
}