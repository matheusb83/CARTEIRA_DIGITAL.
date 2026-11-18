#include <stdio.h>
#include "meu_banco.h"

int main(void) {
    Cliente banco[MAX_CLIENTES];

    int t = 0;
    int aberto = 0;
    int op;

    do {
        limparTela();

        if (aberto) {
            alertaNegativo(banco, t);  /* Alerta de saldo negativo */
        }

        printf("===== MENU PRINCIPAL =====\n");
        printf("1 - Abrir o banco\n");
        printf("2 - Consultar historico\n");
        printf("3 - Efetuar sorteio\n");
        printf("4 - Depositar / Sacar\n");
        printf("5 - Pedir emprestimo\n");
        printf("6 - Comprar titulo de capitalizacao\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        scanf("%d", &op);
        limparTela();

        if (op == 1) {
            if (aberto) {
                printf("O banco ja esta aberto.\n");
            } else {
                abrirBanco(banco, &t);
                aberto = 1;
            }

        } else if (op >= 2 && op <= 6) {

            if (!aberto) {
                printf("Abra o banco primeiro (opcao 1).\n");

            } else if (op == 2) {
                consultarHistorico(banco, t);

            } else if (op == 3) {
                efetuarSorteio(banco, t);

            } else if (op == 4) {
                movimentar(banco, t);

            } else if (op == 5) {
                realizarEmprestimo(banco, t);

            } else if (op == 6) {
                comprarTitulo(banco, t);
            }

        } else if (op != 0) {
            printf("Opcao invalida.\n");
        }

        if (op != 0) {
            pausar();
        }

    } while (op != 0);

    printf("Encerrando...\n");

    pausar();  /* Mantem a janela aberta no Dev-C++ */

    return 0;
}
