#ifndef MEU_BANCO_H
#define MEU_BANCO_H

#define MAX_CLIENTES 100
#define MAX_HIST 5
#define TAM_HIST 80

/* Códigos dos tipos de conta */
#define CORRENTE 1
#define SALARIO  2
#define POUPANCA 3

typedef struct {
    int   existe;       /* 1 se o cliente possui esse tipo de conta */
    float saldo;
    char  numero[12];   /* Exemplo: 1000-01 */
} Conta;

typedef struct {
    char  nome[35];
    int   numero;                   /* Parte numérica: 1000, 1001... */
    Conta contas[4];                /* Utiliza os índices 1, 2 e 3 */

    char  historico[MAX_HIST][TAM_HIST];
    int   qtd_hist;

    int   fez_emprestimo;
    int   titulo;                   /* 1 se comprou título de capitalização */
    int   premiado;
    float premio;
} Cliente;

/* Funções utilitárias */
void limparTela(void);
void pausar(void);
void addHistorico(Cliente *c, const char *msg);

/* Regras do banco */
void abrirBanco(Cliente banco[], int *t);
void alertaNegativo(Cliente banco[], int t);
void movimentar(Cliente banco[], int t);
void realizarEmprestimo(Cliente banco[], int t);
void comprarTitulo(Cliente banco[], int t);
void consultarHistorico(Cliente banco[], int t);
void efetuarSorteio(Cliente banco[], int t);

#endif
