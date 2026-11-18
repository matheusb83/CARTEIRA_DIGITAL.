#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "meu_banco.h"

/* ---------- utilitarios ---------- */

void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);   /* limpa o buffer */
    printf("\nPressione ENTER para continuar...");
    getchar();
}

void addHistorico(Cliente *c, const char *msg) {
    if (c->qtd_hist < MAX_HIST) {
        strncpy(c->historico[c->qtd_hist], msg, TAM_HIST - 1);
        c->historico[c->qtd_hist][TAM_HIST - 1] = '\0';
        c->qtd_hist++;
    }
}

static float saldoTotalCliente(const Cliente *c) {
    float s = 0;
    for (int k = 1; k <= 3; k++)
        if (c->contas[k].existe) s += c->contas[k].saldo;
    return s;
}

static void criarConta(Cliente *c, int tipo) {
    c->contas[tipo].existe = 1;
    c->contas[tipo].saldo = 0;
    sprintf(c->contas[tipo].numero, "%d-%02d", c->base, tipo);
}

static int buscarCliente(Cliente banco[], int t) {
    int num;
    printf("Numero do cliente (ex: 1000): ");
    scanf("%d", &num);
    for (int i = 0; i < t; i++)
        if (banco[i].base == num) return i;
    printf("Cliente nao encontrado.\n");
    return -1;
}

/* cliente apto = tem corrente + poupanca com saldo > 0 e ainda nao fez emprestimo */
static int aptoEmprestimo(const Cliente *c) {
    return c->contas[CORRENTE].existe &&
           c->contas[POUPANCA].existe &&
           c->contas[POUPANCA].saldo > 0 &&
           !c->fez_emprestimo;
}

/* limite = 25% de 10x o saldo total */
static float limiteEmprestimo(const Cliente *c) {
    return 0.25f * 10 * saldoTotalCliente(c);
}

static int aptoTitulo(const Cliente *c) {
    return c->contas[SALARIO].existe &&
           c->contas[SALARIO].saldo >= 1000 &&
           c->contas[SALARIO].saldo <= 1500 &&
           !c->titulo;
}

/* ---------- opcoes do menu ---------- */

void abrirBanco(Cliente banco[], int *t) {
    int n, princ;
    char p;

    printf("Quantos clientes deseja cadastrar (max %d)? ", MAX_CLIENTES);
    scanf("%d", &n);
    if (n < 1 || n > MAX_CLIENTES) {
        printf("Quantidade invalida.\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        Cliente *c = &banco[i];
        memset(c, 0, sizeof(Cliente));
        c->base = 1000 + i;

        printf("\n--- Cliente %d ---\n", i + 1);
        printf("Nome: ");
        scanf(" %34[^\n]", c->nome);

        do {
            printf("Conta principal (1-corrente | 2-salario | 0-nenhuma): ");
            scanf("%d", &princ);
        } while (princ < 0 || princ > 2);

        if (princ != 0) criarConta(c, princ);   /* corrente e salario nunca juntas */

        printf("Abrir conta poupanca? (s/n): ");
        scanf(" %c", &p);
        if (p == 's' || p == 'S') criarConta(c, POUPANCA);

        if (!c->contas[CORRENTE].existe && !c->contas[SALARIO].existe && !c->contas[POUPANCA].existe) {
            printf("O cliente precisa de pelo menos uma conta. Abrindo poupanca.\n");
            criarConta(c, POUPANCA);
        }

        addHistorico(c, "Conta criada com saldo zerado");
        printf("Cliente cadastrado!\n");
    }
    *t = n;
}

void alertaNegativo(Cliente banco[], int t) {
    for (int i = 0; i < t; i++) {
        if (banco[i].contas[CORRENTE].existe && banco[i].contas[CORRENTE].saldo < 0)
            printf("!! ALERTA: %s (%s) esta com saldo negativo: R$ %.2f\n",
                   banco[i].nome, banco[i].contas[CORRENTE].numero,
                   banco[i].contas[CORRENTE].saldo);
    }
}

void movimentar(Cliente banco[], int t) {
    int i = buscarCliente(banco, t);
    if (i < 0) return;
    Cliente *c = &banco[i];
    int tipo, op;
    float v;
    char msg[TAM_HIST];

    printf("Conta (1-corrente | 2-salario | 3-poupanca): ");
    scanf("%d", &tipo);
    if (tipo < 1 || tipo > 3 || !c->contas[tipo].existe) {
        printf("Cliente nao possui essa conta.\n");
        return;
    }

    printf("1-Depositar | 2-Sacar: ");
    scanf("%d", &op);
    printf("Valor: R$ ");
    scanf("%f", &v);
    if (v <= 0) {
        printf("Valor invalido.\n");
        return;
    }

    if (op == 1) {
        c->contas[tipo].saldo += v;
        snprintf(msg, TAM_HIST, "Deposito de R$ %.2f na conta %s", v, c->contas[tipo].numero);
        addHistorico(c, msg);
        printf("Deposito realizado.\n");
    } else if (op == 2) {
        /* so a conta corrente pode ficar negativa */
        if (tipo != CORRENTE && v > c->contas[tipo].saldo) {
            printf("Saldo insuficiente.\n");
            return;
        }
        c->contas[tipo].saldo -= v;
        snprintf(msg, TAM_HIST, "Saque de R$ %.2f na conta %s", v, c->contas[tipo].numero);
        addHistorico(c, msg);
        printf("Saque realizado.\n");
    } else {
        printf("Opcao invalida.\n");
    }
}

void realizarEmprestimo(Cliente banco[], int t) {
    int i = buscarCliente(banco, t);
    if (i < 0) return;
    Cliente *c = &banco[i];

    if (c->fez_emprestimo) {
        printf("Erro: cliente ja possui 1 emprestimo.\n");
        return;
    }
    if (!aptoEmprestimo(c)) {
        printf("Erro: o cliente precisa ter conta corrente e saldo na poupanca.\n");
        return;
    }

    float lim = limiteEmprestimo(c), v;
    char msg[TAM_HIST];
    printf("Limite liberado: R$ %.2f\n", lim);
    printf("Valor do emprestimo: R$ ");
    scanf("%f", &v);

    if (v <= 0 || v > lim) {
        printf("Erro: valor invalido ou acima do limite.\n");
        return;
    }
    c->contas[CORRENTE].saldo += v;
    c->fez_emprestimo = 1;
    snprintf(msg, TAM_HIST, "Emprestimo de R$ %.2f creditado na conta %s", v, c->contas[CORRENTE].numero);
    addHistorico(c, msg);
    printf("Sucesso! Valor creditado na conta corrente.\n");
}

void comprarTitulo(Cliente banco[], int t) {
    int i = buscarCliente(banco, t);
    if (i < 0) return;
    Cliente *c = &banco[i];

    if (c->titulo) {
        printf("Cliente ja possui titulo de capitalizacao.\n");
        return;
    }
    if (!aptoTitulo(c)) {
        printf("Cliente nao apto: precisa de conta-salario com saldo entre R$ 1000 e R$ 1500.\n");
        return;
    }
    c->titulo = 1;
    addHistorico(c, "Titulo de capitalizacao adquirido");
    printf("Titulo adquirido com sucesso!\n");
}

void consultarHistorico(Cliente banco[], int t) {
    float total = 0;
    for (int i = 0; i < t; i++) total += saldoTotalCliente(&banco[i]);

    printf("== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", t, total);

    printf("\n== Clientes Aptos a Emprestimos ==\n");
    for (int i = 0; i < t; i++)
        if (aptoEmprestimo(&banco[i]))
            printf("%s | %s | R$ %.2f\n", banco[i].nome,
                   banco[i].contas[CORRENTE].numero, limiteEmprestimo(&banco[i]));

    printf("\n== Clientes com Saldo Negativo na Conta Corrente ==\n");
    for (int i = 0; i < t; i++)
        if (banco[i].contas[CORRENTE].existe && banco[i].contas[CORRENTE].saldo < 0)
            printf("%s | %s | R$ %.2f\n", banco[i].nome,
                   banco[i].contas[CORRENTE].numero, banco[i].contas[CORRENTE].saldo);

    printf("\n== Clientes Aptos a Compra de Titulos ==\n");
    for (int i = 0; i < t; i++)
        if (aptoTitulo(&banco[i]))
            printf("%s | %s | R$ %.2f\n", banco[i].nome,
                   banco[i].contas[SALARIO].numero, banco[i].contas[SALARIO].saldo);

    printf("\n== Clientes Contemplado no Sorteio ==\n");
    for (int i = 0; i < t; i++)
        if (banco[i].premiado)
            printf("%s | %s | R$ %.2f\n", banco[i].nome,
                   banco[i].contas[SALARIO].numero, banco[i].premio);

    printf("\n== Historico de movimentacoes ==\n");
    for (int i = 0; i < t; i++) {
        printf("%s (%d):\n", banco[i].nome, banco[i].base);
        for (int k = 0; k < banco[i].qtd_hist; k++)
            printf("   - %s\n", banco[i].historico[k]);
    }
}

void efetuarSorteio(Cliente banco[], int t) {
    if (t < 5) {
        printf("Sao necessarios pelo menos 5 clientes para o sorteio.\n");
        return;
    }

    /* ordena os indices pelo saldo total (maior primeiro) */
    int idx[MAX_CLIENTES];
    for (int i = 0; i < t; i++) idx[i] = i;
    for (int i = 0; i < t - 1; i++)
        for (int j = 0; j < t - 1 - i; j++)
            if (saldoTotalCliente(&banco[idx[j]]) < saldoTotalCliente(&banco[idx[j + 1]])) {
                int aux = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = aux;
            }

    /* todos os 5 maiores saldos precisam ter titulo */
    for (int k = 0; k < 5; k++)
        if (!banco[idx[k]].titulo) {
            printf("Sorteio cancelado: %s (top %d em saldo) nao possui titulo de capitalizacao.\n",
                   banco[idx[k]].nome, k + 1);
            return;
        }

    /* sorteio com pesos de Fibonacci: 1o lugar = 5, 2o = 3, 3o = 2, 4o = 1, 5o = 1 */
    int peso[5] = {5, 3, 2, 1, 1};
    int soma = 12;
    srand((unsigned) time(NULL));
    int r = rand() % soma, acum = 0, venc = 0;
    for (int k = 0; k < 5; k++) {
        acum += peso[k];
        if (r < acum) { venc = k; break; }
    }

    for (int i = 0; i < t; i++) { banco[i].premiado = 0; banco[i].premio = 0; }

    Cliente *c = &banco[idx[venc]];
    float premio = 10 * c->contas[SALARIO].saldo;
    c->contas[SALARIO].saldo += premio;
    c->premiado = 1;
    c->premio = premio;

    char msg[TAM_HIST];
    snprintf(msg, TAM_HIST, "Vencedor do sorteio! Premio de R$ %.2f", premio);
    addHistorico(c, msg);

    printf("Participantes:\n");
    for (int k = 0; k < 5; k++)
        printf("  %d. %s (saldo total R$ %.2f)\n", k + 1, banco[idx[k]].nome,
               saldoTotalCliente(&banco[idx[k]]));
    printf("\nVencedor: %s | Premio: R$ %.2f\n", c->nome, premio);
}
