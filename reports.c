/* ============================================================
   Reports (text files, overwritten on each generation)
   ============================================================ */

void reportTicketsByPassenger(Passenger *passengers, int passengerCount, Trip *trips, int tripCount, Ticket *tickets, int ticketCount){
    char cpf[15];
    int pIdx, tripIdx, i, found = 0;
    FILE *fp;

    readCpf("CPF do passageiro: ", cpf);
    pIdx = findPassenger(passengers, passengerCount, cpf);
    if(pIdx == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }

    fp = fopen(REPORT1_FILE, "w");
    if(fp == NULL){
        printf("Erro ao gerar o relatório.\n");
        return;
    }

    fprintf(fp, "Relatório de passagens do passageiro %s\n", passengers[pIdx].name);
    fprintf(fp, "CPF: %s\n\n", cpf);

    for(i = 0; i < ticketCount; i++){
        if(strcmp(tickets[i].passengerCpf, cpf) == 0){
            found++;
            tripIdx = findTrip(trips, tripCount, tickets[i].tripCode);
            fprintf(fp, "Passageiro: %s\n", passengers[pIdx].name);
            if(tripIdx != -1){
                fprintf(fp, "Origem: %s\n", trips[tripIdx].origin);
                fprintf(fp, "Destino: %s\n", trips[tripIdx].destination);
                fprintf(fp, "Data: %s\n", trips[tripIdx].date);
            } else {
                fprintf(fp, "Origem: (viagem não encontrada)\n");
                fprintf(fp, "Destino: (viagem não encontrada)\n");
                fprintf(fp, "Data: (viagem não encontrada)\n");
            }
            fprintf(fp, "Poltrona: %d\n", tickets[i].seat);
            fprintf(fp, "Status: %s\n", tickets[i].status);
            fprintf(fp, "----------------------------------------\n");
        }
    }

    if(found == 0){
        fprintf(fp, "Nenhuma passagem encontrada para este passageiro.\n");
    }

    fclose(fp);
    printf("Relatório gerado em %s (%d passagem(ns)).\n", REPORT1_FILE, found);
}

void reportPassengersByTrip(Passenger *passengers, int passengerCount, Trip *trips, int tripCount, Ticket *tickets, int ticketCount){
    int code, tIdx, pIdx, i, found = 0;
    FILE *fp;

    code = readInt("Código da viagem: ");
    tIdx = findTrip(trips, tripCount, code);
    if(tIdx == -1){
        printf("Viagem não encontrada.\n");
        return;
    }

    fp = fopen(REPORT2_FILE, "w");
    if(fp == NULL){
        printf("Erro ao gerar o relatório.\n");
        return;
    }

    fprintf(fp, "Relatório de passageiros da viagem %d\n", code);
    fprintf(fp, "%s -> %s em %s\n\n", trips[tIdx].origin, trips[tIdx].destination, trips[tIdx].date);

    for(i = 0; i < ticketCount; i++){
        if(tickets[i].tripCode == code){
            found++;
            pIdx = findPassenger(passengers, passengerCount, tickets[i].passengerCpf);
            if(pIdx != -1){
                fprintf(fp, "Passageiro: %s\n", passengers[pIdx].name);
            } else {
                fprintf(fp, "Passageiro: (passageiro não encontrado)\n");
            }
            fprintf(fp, "Poltrona: %d\n", tickets[i].seat);
            fprintf(fp, "Tipo: %s\n", tickets[i].type);
            fprintf(fp, "----------------------------------------\n");
        }
    }

    if(found == 0){
        fprintf(fp, "Nenhum passageiro nesta viagem.\n");
    }

    fclose(fp);
    printf("Relatório gerado em %s (%d passageiro(s)).\n", REPORT2_FILE, found);
}

void reportTripsByPeriod(Trip *trips, int tripCount){
    char startDate[11], endDate[11];
    long keyStart, keyEnd, key;
    int i, found = 0;
    FILE *fp;

    readDate("Data inicial (DD/MM/AAAA): ", startDate);
    readDate("Data final (DD/MM/AAAA): ", endDate);
    keyStart = dateToKey(startDate);
    keyEnd = dateToKey(endDate);

    fp = fopen(REPORT3_FILE, "w");
    if(fp == NULL){
        printf("Erro ao gerar o relatório.\n");
        return;
    }

    fprintf(fp, "Relatório de viagens de %s a %s\n\n", startDate, endDate);

    for(i = 0; i < tripCount; i++){
        key = dateToKey(trips[i].date);
        if(key >= keyStart && key <= keyEnd){
            found++;
            fprintf(fp, "Código: %d\n", trips[i].code);
            fprintf(fp, "Origem: %s\n", trips[i].origin);
            fprintf(fp, "Destino: %s\n", trips[i].destination);
            fprintf(fp, "----------------------------------------\n");
        }
    }

    if(found == 0){
        fprintf(fp, "Nenhuma viagem encontrada no período.\n");
    }

    fclose(fp);
    printf("Relatório gerado em %s (%d viagem(ns)).\n", REPORT3_FILE, found);
}

void reportMenu(Passenger *passengers, int passengerCount, Trip *trips, int tripCount, Ticket *tickets, int ticketCount){
    int option;
    do {
        printf("\n--- Submenu de Relatórios ---\n");
        printf("1- Passagens de um passageiro\n");
        printf("2- Passageiros de uma viagem\n");
        printf("3- Viagens em um período\n");
        printf("4- Voltar\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: reportTicketsByPassenger(passengers, passengerCount, trips, tripCount, tickets, ticketCount); break;
            case 2: reportPassengersByTrip(passengers, passengerCount, trips, tripCount, tickets, ticketCount); break;
            case 3: reportTripsByPeriod(trips, tripCount); break;
            case 4: break;
            default: printf("Opção inválida\n");
        }
        if(option >= 1 && option <= 3){
            pauseScreen();
        }
    } while(option != 4);
}
