/////////////////////////////////////////////////////////////////////////////////////////////
/// \file Main.cpp Main.
/// Não há muita coisa por aqui. A classe PRINCIPAL é a classe mais importante.
/// \sa Principal.h
///
/////////////////////////////////////////////////////////////////////////////////////////////

/*! \mainpage Documentação do programa para PC.
 * 
 * <i>Por Fábio Crestani</i>
 *
 * \section proposta Proposta
 *
 * Este projeto se propõe a implementar um sistema de controle de acesso utilizando um sistema
 * com um microcontrolador da família 8051 e um módulo de RFID.<br> O microcontrolador se comunica
 * com o computador via porta serial e valida ou não o acesso de um usuário.
 *
 * \image html diagrama_blocos.jpg
 *
 * \section fluxograma Fluxograma resumido
 * \image html fluxograma.jpg
 *
 * \section classes Diagrama de classes
 * \image html DiagramaDeClasses.jpg
 *
 * \section mais_info Mais informações
 *
 * Para mais informações acesse o relatório do projeto e/ou a documentação do programa do microcontrolador.
 *
 */

#include "Principal.h"

int main(){

	PRINCIPAL objetoPrincipal;

	return(objetoPrincipal.ProgramaPrincipal());
}