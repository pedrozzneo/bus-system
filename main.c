#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX_PHONES 5

#define PASSENGER_FILE "passengers.dat"
#define TRIP_FILE "trips.dat"
#define TICKET_FILE "tickets.dat"
#define REPORT1_FILE "report_passenger_tickets.txt"
#define REPORT2_FILE "report_trip_passengers.txt"
#define REPORT3_FILE "report_trips_period.txt"

/* ============================================================
   Structs
   ============================================================ */

typedef struct {
    char cpf[15];
    char name[100];
    char birthDate[11];
    int phoneCount;
    char phones[MAX_PHONES][16];
    char email[100];
} Passenger;

typedef struct {
    int code;
    char origin[50];
    char destination[50];
    char date[11];
    char time[6];
    char company[50];
    int seatCount;
} Trip;

typedef struct {
    char passengerCpf[15];
    int tripCode;
    int ticketNumber;
    int seat;
    char type[15];
    char status[15];
    char purchaseDate[11];
} Ticket;

/* ============================================================
   Utility functions (input and dates)
   ============================================================ */

/* Reads a full line from stdin into buffer (without the '\n'). */
void readLine(char *buffer, int size){
    if(fgets(buffer, size, stdin) != NULL){
        int len = strlen(buffer);
        if(len > 0 && buffer[len - 1] == '\n'){
            buffer[len - 1] = '\0';
        } else {
            /* The line was longer than the buffer: discard the rest. */
            int c;
            while((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        buffer[0] = '\0';
    }
}

/* Reads an integer, re-prompting until the user types a valid number. */
int readInt(char *prompt){
    char buffer[64];
    int value;
    int ok;
    do {
        printf("%s", prompt);
        readLine(buffer, sizeof(buffer));
        ok = sscanf(buffer, "%d", &value);
        if(ok != 1){
            printf("Valor inválido. Digite um número inteiro.\n");
        }
    } while(ok != 1);
    return value;
}

/* Reads an integer between min and max (inclusive), re-prompting until valid. */
int readIntRange(char *prompt, int min, int max){
    int value;
    do {
        value = readInt(prompt);
        if(value < min || value > max){
            printf("Valor fora do intervalo (%d a %d).\n", min, max);
        }
    } while(value < min || value > max);
    return value;
}

/* Reads a CPF (11 numeric digits), re-prompting until valid. */
void readCpf(char *prompt, char *out){
    int valid;
    do {
        printf("%s", prompt);
        readLine(out, 15);
        valid = 1;
        if(strlen(out) != 11){
            valid = 0;
        } else {
            int i;
            for(i = 0; i < 11; i++){
                if(out[i] < '0' || out[i] > '9'){
                    valid = 0;
                    break;
                }
            }
        }
        if(!valid){
            printf("CPF inválido. Digite 11 dígitos numéricos.\n");
        }
    } while(!valid);
}

/* Checks if a string is in the format DD/MM/AAAA (syntax only). */
int isValidDateFormat(char *s){
    int i;
    if(strlen(s) != 10){
        return 0;
    }
    if(s[2] != '/' || s[5] != '/'){
        return 0;
    }
    for(i = 0; i < 10; i++){
        if(i == 2 || i == 5){
            continue;
        }
        if(s[i] < '0' || s[i] > '9'){
            return 0;
        }
    }
    return 1;
}

/* Reads a date in the format DD/MM/AAAA, re-prompting until the format is valid. */
void readDate(char *prompt, char *out){
    int valid;
    do {
        printf("%s", prompt);
        readLine(out, 11);
        valid = isValidDateFormat(out);
        if(!valid){
            printf("Data inválida. Use o formato DD/MM/AAAA.\n");
        }
    } while(!valid);
}

/* Converts a DD/MM/AAAA date into a comparable number (yyyymmdd). Returns -1 if malformed. */
long dateToKey(char *date){
    int d, m, y;
    if(sscanf(date, "%d/%d/%d", &d, &m, &y) != 3){
        return -1;
    }
    return (long)y * 10000 + m * 100 + d;
}

/* Writes today's date (DD/MM/AAAA) into out. */
void getToday(char *out){
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    sprintf(out, "%02d/%02d/%04d", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
}

/* Asks a yes/no question. Returns 1 if the user answers S/s. */
int confirm(char *prompt){
    char buffer[16];
    printf("%s", prompt);
    readLine(buffer, sizeof(buffer));
    if(buffer[0] == 'S' || buffer[0] == 's'){
        return 1;
    }
    return 0;
}

/* Waits for the user to press Enter. */
void pauseScreen(void){
    char buffer[16];
    printf("\nPressione Enter para continuar...");
    readLine(buffer, sizeof(buffer));
}

/* ============================================================
   Passengers
   ============================================================ */

Passenger *loadPassengers(int *count){
    FILE *fp = fopen(PASSENGER_FILE, "rb");
    Passenger *passengers = NULL;
    int n;
    long bytes;

    if(fp == NULL){
        *count = 0;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);
    n = bytes / sizeof(Passenger);

    if(n > 0){
        passengers = malloc(n * sizeof(Passenger));
        if(passengers == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(passengers, sizeof(Passenger), n, fp);
    }

    fclose(fp);
    *count = n;
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

/* ============================================================
   Trips
   ============================================================ */

Trip *loadTrips(int *count){
    FILE *fp = fopen(TRIP_FILE, "rb");
    Trip *trips = NULL;
    int n;
    long bytes;

    if(fp == NULL){
        *count = 0;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);
    n = bytes / sizeof(Trip);

    if(n > 0){
        trips = malloc(n * sizeof(Trip));
        if(trips == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(trips, sizeof(Trip), n, fp);
    }

    fclose(fp);
    *count = n;
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

/* ============================================================
   Tickets
   ============================================================ */

Ticket *loadTickets(int *count){
    FILE *fp = fopen(TICKET_FILE, "rb");
    Ticket *tickets = NULL;
    int n;
    long bytes;

    if(fp == NULL){
        *count = 0;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);
    n = bytes / sizeof(Ticket);

    if(n > 0){
        tickets = malloc(n * sizeof(Ticket));
        if(tickets == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(tickets, sizeof(Ticket), n, fp);
    }

    fclose(fp);
    *count = n;
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

/* ============================================================
   Main
   ============================================================ */

int main(){
    int passengerCount, tripCount, ticketCount;
    int option;

    Passenger *passengers = loadPassengers(&passengerCount);
    Trip *trips = loadTrips(&tripCount);
    Ticket *tickets = loadTickets(&ticketCount);

    do {
        printf("\n===== Sistema de Passagens de Ônibus =====\n");
        printf("1- Submenu de Passageiros\n");
        printf("2- Submenu de Viagens\n");
        printf("3- Submenu de Passagens\n");
        printf("4- Submenu de Relatórios\n");
        printf("5- Sair\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: passengers = passengerMenu(passengers, &passengerCount); break;
            case 2: trips = tripMenu(trips, &tripCount); break;
            case 3: tickets = ticketMenu(tickets, &ticketCount, passengers, passengerCount, trips, tripCount); break;
            case 4: reportMenu(passengers, passengerCount, trips, tripCount, tickets, ticketCount); break;
            case 5: printf("Saindo...\n"); break;
            default: printf("Opção inválida\n");
        }
    } while(option != 5);

    free(passengers);
    free(trips);
    free(tickets);
    return 0;
}
