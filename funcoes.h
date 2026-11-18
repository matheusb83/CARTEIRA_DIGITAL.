typedef struct {             // strutct pra gurdar os dados
    char nome[35];          
    int numero;
    float corrente;
    float salario;
    float poupanca;
    char historico[50];
    int fez_emprestimo;
} Cliente;

void cadastrarCliente(Cliente banco[], int *t) {           // cadast. clientes 
    int i = *t;
    printf("nome do cliente: ");
    scanf(" %[^\n]", banco[i].nome);

    printf("numero da conta: ");
    scanf("%d", &banco[i].numero);

    printf("saldo conta corrente: ");
    scanf("%f", &banco[i].corrente);

    printf("saldo conta salario: ");
    scanf("%f", &banco[i].salario);

    printf("saldo conta poupanca: ");
    scanf("%f", &banco[i].poupanca);
  
    strcpy(banco[i].historico, "Conta criada");     //historico incial + cliente não fez emprestimo
    banco[i].fez_emprestimo = 0;
  
    (*t)++;     //Aumenta a quantidade de clientes cadastrados
    printf("Cliente cadastrado com sucesso!\n");
}

void realizarEmprestimo(Cliente banco[], int t) {     // função pra verificar se cliente fez emprestimo
    int num;
    printf("Digite o numero da conta para o emprestimo: ");
    scanf("%d", &num);

    for (int i = 0; i < t; i++) {                      // procurar a conta pelo numero
        if (banco[i].numero == num) {
            if (banco[i].fez_emprestimo == 1) {             // Verifica se o cliente ja fez um emprestimo
                printf("Erro: Cliente ja possui 1 emprestimo.\n");          
            }  
            else if (banco[i].poupanca > 0) {                // Verifica se o cliente tem dinheiro na poupanca
                float lim = 2.5 * (banco[i].poupanca + banco[i].corrente);
                float v;
                printf("Limite liberado: R$ %.2f\n", lim);
                printf("Valor do emprestimo: R$ ");
                scanf("%f", &v);
              
                if (v <= lim) {                              // Verifica se o valor pedido esta dentro do limite
                    banco[i].corrente += v;
                    banco[i].fez_emprestimo = 1;
                    strcpy(banco[i].historico, "Emprestimo realizado");
                    printf("Sucesso! Valor creditado na conta corrente.\n");
                }   else {
                    printf("Erro: Valor acima do limite!\n");
                }
            } else {
                printf("Erro: Sem saldo na poupanca.\n");
            }

            return;
        }
    }
    printf("Conta nao encontrada.\n");                 // Mostra uma mensagem caso a conta nao seja encontrada
