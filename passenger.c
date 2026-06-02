Passenger *loadPassengers(int *count){
    FILE *fp = fopen(PASSENGER_FILE, "rb");

    if(fp == NULL){
        *count = 0;
        return NULL;
    }
    
    Passenger *passengers = NULL;
    int quantity;
    long bytes;

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);
    quantity = bytes / sizeof(Passenger);

    if(quantity > 0){
        passengers = malloc(quantity * sizeof(Passenger));
        if(passengers == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(passengers, sizeof(Passenger), quantity, fp);
    }

    fclose(fp);
    *count = quantity;
    return passengers;
}

void savePassengers(Passenger *passengers, int count){
    FILE *fp = fopen(PASSENGER_FILE, "wb");
    if(fp == NULL){
        printf("Erro ao abrir o arquivo de passageiros.\n");
        return;
    }
    fwrite(passengers, sizeof(Passenger), count, fp);
    fclose(fp);
}

int findPassenger(Passenger *passengers, int count, char *cpf){
    int i;
    for(i = 0; i < count; i++){
        if(strcmp(passengers[i].cpf, cpf) == 0){
            return i;
        }
    }
    return -1;
}

void printPassenger(Passenger *p){
    int j;
    printf("CPF: %s\n", p->cpf);
    printf("Nome: %s\n", p->name);
    printf("Data de nascimento: %s\n", p->birthDate);
    printf("Email: %s\n", p->email);
    if(p->phoneCount == 0){
        printf("Telefones: (nenhum)\n");
    } else {
        printf("Telefones: ");
        for(j = 0; j < p->phoneCount; j++){
            printf("%s", p->phones[j]);
            if(j < p->phoneCount - 1){
                printf(", ");
            }
        }
        printf("\n");
    }
}

/* Reads the editable passenger fields (everything except the CPF) into p. */
void readPassengerData(Passenger *p){
    int i;
    printf("Nome: ");
    readLine(p->name, sizeof(p->name));
    readDate("Data de nascimento (DD/MM/AAAA): ", p->birthDate);
    p->phoneCount = readIntRange("Quantos telefones? (0 a 5): ", 0, MAX_PHONES);
    for(i = 0; i < p->phoneCount; i++){
        printf("Telefone %d: ", i + 1);
        readLine(p->phones[i], sizeof(p->phones[i]));
    }
    printf("Email: ");
    readLine(p->email, sizeof(p->email));
}

Passenger *insertPassenger(Passenger *passengers, int *count){
    Passenger novo;
    Passenger *tmp;
    char cpf[15];

    readCpf("CPF: ", cpf);
    if(findPassenger(passengers, *count, cpf) != -1){
        printf("Já existe um passageiro com esse CPF.\n");
        return passengers;
    }
    strcpy(novo.cpf, cpf);
    readPassengerData(&novo);

    tmp = realloc(passengers, (*count + 1) * sizeof(Passenger));
    if(tmp == NULL){
        printf("Erro de memória.\n");
        exit(1);
    }
    passengers = tmp;
    passengers[*count] = novo;
    (*count)++;

    savePassengers(passengers, *count);
    printf("Passageiro cadastrado com sucesso!\n");
    return passengers;
}

void listPassengers(Passenger *passengers, int count){
    int i;
    if(count == 0){
        printf("Nenhum passageiro cadastrado.\n");
        return;
    }
    for(i = 0; i < count; i++){
        printf("\n--- Passageiro %d ---\n", i + 1);
        printPassenger(&passengers[i]);
    }
}

void searchPassenger(Passenger *passengers, int count){
    char cpf[15];
    int idx;
    readCpf("CPF do passageiro: ", cpf);
    idx = findPassenger(passengers, count, cpf);
    if(idx == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }
    printf("\n");
    printPassenger(&passengers[idx]);
}

void updatePassenger(Passenger *passengers, int count){
    char cpf[15];
    int idx;
    readCpf("CPF do passageiro a alterar: ", cpf);
    idx = findPassenger(passengers, count, cpf);
    if(idx == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }
    printf("\nDados atuais:\n");
    printPassenger(&passengers[idx]);
    printf("\nDigite os novos dados:\n");
    readPassengerData(&passengers[idx]);

    savePassengers(passengers, count);
    printf("Passageiro alterado com sucesso!\n");
}

void deletePassenger(Passenger *passengers, int *count){
    char cpf[15];
    int idx, i;
    readCpf("CPF do passageiro a excluir: ", cpf);
    idx = findPassenger(passengers, *count, cpf);
    if(idx == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }
    printf("\nDados do passageiro:\n");
    printPassenger(&passengers[idx]);
    if(!confirm("\nConfirma a exclusão? (S/N): ")){
        printf("Exclusão cancelada.\n");
        return;
    }
    for(i = idx; i < *count - 1; i++){
        passengers[i] = passengers[i + 1];
    }
    (*count)--;
    savePassengers(passengers, *count);
    printf("Passageiro excluído com sucesso!\n");
}

Passenger *passengerMenu(Passenger *passengers, int *count){
    int option;
    do {
        printf("\n--- Submenu de Passageiros ---\n");
        printf("1- Listar todos\n");
        printf("2- Listar um específico\n");
        printf("3- Incluir\n");
        printf("4- Alterar\n");
        printf("5- Excluir\n");
        printf("6- Voltar\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: listPassengers(passengers, *count); break;
            case 2: searchPassenger(passengers, *count); break;
            case 3: passengers = insertPassenger(passengers, count); break;
            case 4: updatePassenger(passengers, *count); break;
            case 5: deletePassenger(passengers, count); break;
            case 6: break;
            default: printf("Opção inválida\n");
        }
        if(option >= 1 && option <= 5){
            pauseScreen();
        }
    } while(option != 6);
    return passengers;
}
