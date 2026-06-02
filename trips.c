/* ============================================================
   Trips
   ============================================================ */

Trip *loadTrips(int *count){
    FILE *fp = fopen(TRIP_FILE, "rb");

    if(fp == NULL){
        *count = 0;
        return NULL;
    }

    Trip *trips = NULL;
    int quantity;
    long bytes;

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);
    quantity = bytes / sizeof(Trip);

    if(quantity > 0){
        trips = malloc(quantity * sizeof(Trip));
        if(trips == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(trips, sizeof(Trip), quantity, fp);
    }

    fclose(fp);
    *count = quantity;
    return trips;
}

void saveTrips(Trip *trips, int count){
    FILE *fp = fopen(TRIP_FILE, "wb");
    if(fp == NULL){
        printf("Erro ao abrir o arquivo de viagens.\n");
        return;
    }
    fwrite(trips, sizeof(Trip), count, fp);
    fclose(fp);
}

int findTrip(Trip *trips, int count, int code){
    int i;
    for(i = 0; i < count; i++){
        if(trips[i].code == code){
            return i;
        }
    }
    return -1;
}

void printTrip(Trip *t){
    printf("Código: %d\n", t->code);
    printf("Origem: %s\n", t->origin);
    printf("Destino: %s\n", t->destination);
    printf("Data: %s\n", t->date);
    printf("Horário: %s\n", t->time);
    printf("Empresa: %s\n", t->company);
    printf("Número de poltronas: %d\n", t->seatCount);
}

/* Reads the editable trip fields (everything except the code) into t. */
void readTripData(Trip *t){
    printf("Origem: ");
    readLine(t->origin, sizeof(t->origin));
    printf("Destino: ");
    readLine(t->destination, sizeof(t->destination));
    readDate("Data (DD/MM/AAAA): ", t->date);
    printf("Horário (HH:MM): ");
    readLine(t->time, sizeof(t->time));
    printf("Empresa: ");
    readLine(t->company, sizeof(t->company));
    t->seatCount = readIntRange("Número de poltronas: ", 1, 1000);
}

Trip *insertTrip(Trip *trips, int *count){
    Trip novo;
    Trip *tmp;
    int code;

    code = readInt("Código da viagem: ");
    if(findTrip(trips, *count, code) != -1){
        printf("Já existe uma viagem com esse código.\n");
        return trips;
    }
    novo.code = code;
    readTripData(&novo);

    tmp = realloc(trips, (*count + 1) * sizeof(Trip));
    if(tmp == NULL){
        printf("Erro de memória.\n");
        exit(1);
    }
    trips = tmp;
    trips[*count] = novo;
    (*count)++;

    saveTrips(trips, *count);
    printf("Viagem cadastrada com sucesso!\n");
    return trips;
}

void listTrips(Trip *trips, int count){
    int i;
    if(count == 0){
        printf("Nenhuma viagem cadastrada.\n");
        return;
    }
    for(i = 0; i < count; i++){
        printf("\n--- Viagem %d ---\n", i + 1);
        printTrip(&trips[i]);
    }
}

void searchTrip(Trip *trips, int count){
    int code, idx;
    code = readInt("Código da viagem: ");
    idx = findTrip(trips, count, code);
    if(idx == -1){
        printf("Viagem não encontrada.\n");
        return;
    }
    printf("\n");
    printTrip(&trips[idx]);
}

void updateTrip(Trip *trips, int count){
    int code, idx;
    code = readInt("Código da viagem a alterar: ");
    idx = findTrip(trips, count, code);
    if(idx == -1){
        printf("Viagem não encontrada.\n");
        return;
    }
    printf("\nDados atuais:\n");
    printTrip(&trips[idx]);
    printf("\nDigite os novos dados:\n");
    readTripData(&trips[idx]);

    saveTrips(trips, count);
    printf("Viagem alterada com sucesso!\n");
}

void deleteTrip(Trip *trips, int *count){
    int code, idx, i;
    code = readInt("Código da viagem a excluir: ");
    idx = findTrip(trips, *count, code);
    if(idx == -1){
        printf("Viagem não encontrada.\n");
        return;
    }
    printf("\nDados da viagem:\n");
    printTrip(&trips[idx]);
    if(!confirm("\nConfirma a exclusão? (S/N): ")){
        printf("Exclusão cancelada.\n");
        return;
    }
    for(i = idx; i < *count - 1; i++){
        trips[i] = trips[i + 1];
    }
    (*count)--;
    saveTrips(trips, *count);
    printf("Viagem excluída com sucesso!\n");
}

Trip *tripMenu(Trip *trips, int *count){
    int option;
    do {
        printf("\n--- Submenu de Viagens ---\n");
        printf("1- Listar todas\n");
        printf("2- Listar uma específica\n");
        printf("3- Incluir\n");
        printf("4- Alterar\n");
        printf("5- Excluir\n");
        printf("6- Voltar\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: listTrips(trips, *count); break;
            case 2: searchTrip(trips, *count); break;
            case 3: trips = insertTrip(trips, count); break;
            case 4: updateTrip(trips, *count); break;
            case 5: deleteTrip(trips, count); break;
            case 6: break;
            default: printf("Opção inválida\n");
        }
        if(option >= 1 && option <= 5){
            pauseScreen();
        }
    } while(option != 6);
    return trips;
}
