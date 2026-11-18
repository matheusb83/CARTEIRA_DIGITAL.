#include <stdio.h>
#include <string.h>
#include <funções.h>
int main() {
    Cliente banco[100];
    int t = 0;
    char op;
    
    do {
        printf("Menu: {c} cadastrar um cliente | {e} emprestimo | {s} sair");
        printf("opcao: ");
        scanf("%c", &op);

        // funcao cadastro
        if (op == 'c') {
            cadastrarCliente(banco, &t);
        }
        // funcao emprestimo
        else if (op == 'e') {
            realizarEmprestimo(banco, t);
        }

    } while (op != 's');
    float saldo_banco = 0;

    for (int i = 0; i < t; i++) {                                                // somar saldo do cliente
        saldo_banco += banco[i].corrente + banco[i].salario + banco[i].poupanca;
    }
    printf("1. Saldo total do banco: R$ %.2f\n", saldo_banco);
    printf("2. Clientes que podem fazer emprestimo:\n");             //clientes que podem fazer emprestimo

    for (int i = 0; i < t; i++) {
        if (banco[i].poupanca > 0 && banco[i].fez_emprestimo == 0) {
            printf("   - %s\n", banco[i].nome);
        }
    }
    printf("\n3. Clientes com saldo negativo na Conta Corrente:\n");          // clientes com saldo negativo

    for (int i = 0; i < t; i++) {
        if (banco[i].corrente < 0) {
            printf("   - %s\n", banco[i].nome);
        }
    }

    printf("\n4. Clientes com R$1000 a R$1500 no total:\n");       //clientes que tem entre 1000/1500

    for (int i = 0; i < t; i++) {
        float soma = banco[i].corrente + banco[i].salario + banco[i].poupanca;

        if (soma >= 1000 && soma <= 1500) {
            printf("   - %s (R$ %.2f)\n", banco[i].nome, soma);
        }
    }

    return 0;
}
