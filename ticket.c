/* ============================================================
   Tickets
   ============================================================ */

Ticket *loadTickets(int *count){
    FILE *fp = fopen(TICKET_FILE, "rb");

    if(fp == NULL){
        *count = 0;
        return NULL;
    }

    Ticket *tickets = NULL;
    int quantity;
    long bytes;

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);
    quantity = bytes / sizeof(Ticket);

    if(quantity > 0){
        tickets = malloc(quantity * sizeof(Ticket));
        if(tickets == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(tickets, sizeof(Ticket), quantity, fp);
    }

    fclose(fp);
    *count = quantity;
    return tickets;
}

void saveTickets(Ticket *tickets, int count){
    FILE *fp = fopen(TICKET_FILE, "wb");
    if(fp == NULL){
        printf("Erro ao abrir o arquivo de passagens.\n");
        return;
    }
    fwrite(tickets, sizeof(Ticket), count, fp);
    fclose(fp);
}

int findTicket(Ticket *tickets, int count, char *cpf, int tripCode){
    int i;
    for(i = 0; i < count; i++){
        if(tickets[i].tripCode == tripCode && strcmp(tickets[i].passengerCpf, cpf) == 0){
            return i;
        }
    }
    return -1;
}

int seatIsTaken(Ticket *tickets, int count, int tripCode, int seat){
    int i;
    for(i = 0; i < count; i++){
        if(tickets[i].tripCode == tripCode && tickets[i].seat == seat){
            return 1;
        }
    }
    return 0;
}

void showAvailableSeats(Ticket *tickets, int count, int tripCode, int seatCount){
    int s;
    printf("\nPoltronas (X = ocupada):\n");
    for(s = 1; s <= seatCount; s++){
        if(seatIsTaken(tickets, count, tripCode, s)){
            printf("[%d-X] ", s);
        } else {
            printf("[%d] ", s);
        }
        if(s % 10 == 0){
            printf("\n");
        }
    }
    printf("\n");
}

int nextTicketNumber(Ticket *tickets, int count){
    int i, max = 0;
    for(i = 0; i < count; i++){
        if(tickets[i].ticketNumber > max){
            max = tickets[i].ticketNumber;
        }
    }
    return max + 1;
}

void printTicket(Ticket *t){
    printf("Passagem nº: %d\n", t->ticketNumber);
    printf("CPF do passageiro: %s\n", t->passengerCpf);
    printf("Código da viagem: %d\n", t->tripCode);
    printf("Poltrona: %d\n", t->seat);
    printf("Tipo: %s\n", t->type);
    printf("Status: %s\n", t->status);
    printf("Data da compra: %s\n", t->purchaseDate);
}

/* Asks the user for a free seat on the trip, re-prompting until valid.
   currentSeat is the seat the ticket already holds (-1 when registering a new one),
   so the user is allowed to keep it. */
int chooseSeat(Ticket *tickets, int count, int tripCode, int seatCount, int currentSeat){
    int seat, valid;
    showAvailableSeats(tickets, count, tripCode, seatCount);
    do {
        valid = 1;
        seat = readInt("Escolha a poltrona: ");
        if(seat < 1 || seat > seatCount){
            printf("Poltrona fora do intervalo (1 a %d).\n", seatCount);
            valid = 0;
        } else if(seat != currentSeat && seatIsTaken(tickets, count, tripCode, seat)){
            printf("Poltrona já ocupada. Escolha outra.\n");
            valid = 0;
        }
    } while(!valid);
    return seat;
}

/* Asks the user to choose the ticket type and writes it into out. */
void chooseType(char *out){
    int option;
    printf("Tipo: 1- Convencional  2- Leito\n");
    option = readIntRange("Escolha o tipo: ", 1, 2);
    if(option == 1){
        strcpy(out, "Convencional");
    } else {
        strcpy(out, "Leito");
    }
}

/* Asks the user to choose the ticket status and writes it into out. */
void chooseStatus(char *out){
    int option;
    printf("Status: 1- confirmada  2- cancelada  3- em espera\n");
    option = readIntRange("Escolha o status: ", 1, 3);
    if(option == 1){
        strcpy(out, "confirmada");
    } else if(option == 2){
        strcpy(out, "cancelada");
    } else {
        strcpy(out, "em espera");
    }
}

Ticket *insertTicket(Ticket *tickets, int *count, Passenger *passengers, int passengerCount, Trip *trips, int tripCount){
    Ticket novo;
    Ticket *tmp;
    char cpf[15];
    int code, tripIdx;

    readCpf("CPF do passageiro: ", cpf);
    if(findPassenger(passengers, passengerCount, cpf) == -1){
        printf("Passageiro não encontrado. Cadastre o passageiro primeiro.\n");
        return tickets;
    }

    code = readInt("Código da viagem: ");
    tripIdx = findTrip(trips, tripCount, code);
    if(tripIdx == -1){
        printf("Viagem não encontrada. Cadastre a viagem primeiro.\n");
        return tickets;
    }

    if(findTicket(tickets, *count, cpf, code) != -1){
        printf("Esse passageiro já possui passagem para essa viagem.\n");
        return tickets;
    }

    strcpy(novo.passengerCpf, cpf);
    novo.tripCode = code;
    novo.seat = chooseSeat(tickets, *count, code, trips[tripIdx].seatCount, -1);
    chooseType(novo.type);
    novo.ticketNumber = nextTicketNumber(tickets, *count);
    strcpy(novo.status, "confirmada");
    getToday(novo.purchaseDate);

    tmp = realloc(tickets, (*count + 1) * sizeof(Ticket));
    if(tmp == NULL){
        printf("Erro de memória.\n");
        exit(1);
    }
    tickets = tmp;
    tickets[*count] = novo;
    (*count)++;

    saveTickets(tickets, *count);
    printf("Passagem cadastrada com sucesso! (nº %d)\n", novo.ticketNumber);
    return tickets;
}

void listTickets(Ticket *tickets, int count){
    int i;
    if(count == 0){
        printf("Nenhuma passagem cadastrada.\n");
        return;
    }
    for(i = 0; i < count; i++){
        printf("\n--- Passagem %d ---\n", i + 1);
        printTicket(&tickets[i]);
    }
}

void searchTicket(Ticket *tickets, int count){
    char cpf[15];
    int code, idx;
    readCpf("CPF do passageiro: ", cpf);
    code = readInt("Código da viagem: ");
    idx = findTicket(tickets, count, cpf, code);
    if(idx == -1){
        printf("Passagem não encontrada.\n");
        return;
    }
    printf("\n");
    printTicket(&tickets[idx]);
}

void updateTicket(Ticket *tickets, int count, Trip *trips, int tripCount){
    char cpf[15];
    int code, idx, tripIdx;
    readCpf("CPF do passageiro: ", cpf);
    code = readInt("Código da viagem: ");
    idx = findTicket(tickets, count, cpf, code);
    if(idx == -1){
        printf("Passagem não encontrada.\n");
        return;
    }
    printf("\nDados atuais:\n");
    printTicket(&tickets[idx]);
    printf("\nDigite os novos dados:\n");

    tripIdx = findTrip(trips, tripCount, code);
    if(tripIdx != -1){
        tickets[idx].seat = chooseSeat(tickets, count, code, trips[tripIdx].seatCount, tickets[idx].seat);
    } else {
        printf("Viagem não encontrada; mantendo a poltrona atual.\n");
    }
    chooseType(tickets[idx].type);
    chooseStatus(tickets[idx].status);

    saveTickets(tickets, count);
    printf("Passagem alterada com sucesso!\n");
}

void deleteTicket(Ticket *tickets, int *count){
    char cpf[15];
    int code, idx, i;
    readCpf("CPF do passageiro: ", cpf);
    code = readInt("Código da viagem: ");
    idx = findTicket(tickets, *count, cpf, code);
    if(idx == -1){
        printf("Passagem não encontrada.\n");
        return;
    }
    printf("\nDados da passagem:\n");
    printTicket(&tickets[idx]);
    if(!confirm("\nConfirma a exclusão? (S/N): ")){
        printf("Exclusão cancelada.\n");
        return;
    }
    for(i = idx; i < *count - 1; i++){
        tickets[i] = tickets[i + 1];
    }
    (*count)--;
    saveTickets(tickets, *count);
    printf("Passagem excluída com sucesso!\n");
}

Ticket *ticketMenu(Ticket *tickets, int *count, Passenger *passengers, int passengerCount, Trip *trips, int tripCount){
    int option;
    do {
        printf("\n--- Submenu de Passagens ---\n");
        printf("1- Listar todas\n");
        printf("2- Listar uma específica\n");
        printf("3- Incluir\n");
        printf("4- Alterar\n");
        printf("5- Excluir\n");
        printf("6- Voltar\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: listTickets(tickets, *count); break;
            case 2: searchTicket(tickets, *count); break;
            case 3: tickets = insertTicket(tickets, count, passengers, passengerCount, trips, tripCount); break;
            case 4: updateTicket(tickets, *count, trips, tripCount); break;
            case 5: deleteTicket(tickets, count); break;
            case 6: break;
            default: printf("Opção inválida\n");
        }
        if(option >= 1 && option <= 5){
            pauseScreen();
        }
    } while(option != 6);
    return tickets;
}
