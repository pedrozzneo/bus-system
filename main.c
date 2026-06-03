/* ============================================================
   Includes and configuration
   ============================================================ */

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
    int phoneQuantity;
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
    int seatQuantity;
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

/* Reads a line from stdin and replaces the trailing '\n' with '\0'. */
void readLine(char *buffer, int size){
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

/* Prints a prompt and reads an integer typed by the user. */
int readInt(char *prompt){
    char input[64];
    printf("%s", prompt);
    readLine(input, sizeof(input));
    return atoi(input);
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

/* Turns a DD/MM/AAAA date into a single number AAAAMMDD.
   A later date always gives a bigger number, so two dates can be
   compared with a plain >, < or ==. */
int dateToKey(char *date){
    int day   = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year  = (date[6] - '0') * 1000 + (date[7] - '0') * 100
              + (date[8] - '0') * 10 + (date[9] - '0');
    return year * 10000 + month * 100 + day;
}

/* Writes today's date (DD/MM/AAAA) into today. */
void getToday(char *today){
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    sprintf(today, "%02d/%02d/%04d", localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);
}

/* ============================================================
   Passengers
   ============================================================ */

Passenger *loadPassengers(int *quantity){
    FILE *file = fopen(PASSENGER_FILE, "rb");

    if(file == NULL){
        *quantity = 0;
        return NULL;
    }

    Passenger *passengers = NULL;
    int bytes;

    fseek(file, 0, SEEK_END);
    bytes = ftell(file);
    rewind(file);
    *quantity = bytes / sizeof(Passenger);

    if(*quantity > 0){
        passengers = malloc(*quantity * sizeof(Passenger));
        if(passengers == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(passengers, sizeof(Passenger), *quantity, file);
    }

    fclose(file);
    return passengers;
}

void savePassengers(Passenger *passengers, int quantity){
    FILE *file = fopen(PASSENGER_FILE, "wb");
    if(file == NULL){
        printf("Erro ao abrir o arquivo de passageiros.\n");
        return;
    }
    fwrite(passengers, sizeof(Passenger), quantity, file);
    fclose(file);
}

int findPassenger(Passenger *passengers, int quantity, char *cpf){
    int passengerIndex;
    for(passengerIndex = 0; passengerIndex < quantity; passengerIndex++){
        if(strcmp(passengers[passengerIndex].cpf, cpf) == 0){
            return passengerIndex;
        }
    }
    return -1;
}

void printPassenger(Passenger *passenger){
    int phoneIndex;
    printf("CPF: %s\n", passenger->cpf);
    printf("Nome: %s\n", passenger->name);
    printf("Data de nascimento: %s\n", passenger->birthDate);
    printf("Email: %s\n", passenger->email);
    if(passenger->phoneQuantity == 0){
        printf("Telefones: (nenhum)\n");
    } else {
        printf("Telefones: ");
        for(phoneIndex = 0; phoneIndex < passenger->phoneQuantity; phoneIndex++){
            printf("%s", passenger->phones[phoneIndex]);
            if(phoneIndex < passenger->phoneQuantity - 1){
                printf(", ");
            }
        }
        printf("\n");
    }
}

/* Reads the editable passenger fields (everything except the CPF) into passenger. */
void readPassengerData(Passenger *passenger){
    int phoneIndex;
    printf("Nome: ");
    readLine(passenger->name, sizeof(passenger->name));
    printf("Data de nascimento (DD/MM/AAAA): ");
    readLine(passenger->birthDate, sizeof(passenger->birthDate));
    passenger->phoneQuantity = readIntRange("Quantos telefones? (0 a 5): ", 0, MAX_PHONES);
    for(phoneIndex = 0; phoneIndex < passenger->phoneQuantity; phoneIndex++){
        printf("Telefone %d: ", phoneIndex + 1);
        readLine(passenger->phones[phoneIndex], sizeof(passenger->phones[phoneIndex]));
    }
    printf("Email: ");
    readLine(passenger->email, sizeof(passenger->email));
}

Passenger *insertPassenger(Passenger *passengers, int *quantity){
    char cpf[15];

    printf("CPF: ");
    readLine(cpf, sizeof(cpf));
    if(findPassenger(passengers, *quantity, cpf) != -1){
        printf("Já existe um passageiro com esse CPF.\n");
        return passengers;
    }

    passengers = realloc(passengers, (*quantity + 1) * sizeof(Passenger));
    if(passengers == NULL){
        printf("Erro de memória.\n");
        exit(1);
    }
    strcpy(passengers[*quantity].cpf, cpf);
    readPassengerData(&passengers[*quantity]);
    (*quantity)++;

    savePassengers(passengers, *quantity);
    printf("Passageiro cadastrado com sucesso!\n");
    return passengers;
}

void listPassengers(Passenger *passengers, int quantity){
    int passengerIndex;
    if(quantity == 0){
        printf("Nenhum passageiro cadastrado.\n");
        return;
    }
    for(passengerIndex = 0; passengerIndex < quantity; passengerIndex++){
        printf("\n--- Passageiro %d ---\n", passengerIndex + 1);
        printPassenger(&passengers[passengerIndex]);
    }
}

void searchPassenger(Passenger *passengers, int quantity){
    char cpf[15];
    int passengerIndex;
    printf("CPF do passageiro: ");
    readLine(cpf, sizeof(cpf));
    passengerIndex = findPassenger(passengers, quantity, cpf);
    if(passengerIndex == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }
    printf("\n");
    printPassenger(&passengers[passengerIndex]);
}

void updatePassenger(Passenger *passengers, int quantity){
    char cpf[15];
    int passengerIndex;
    printf("CPF do passageiro a alterar: ");
    readLine(cpf, sizeof(cpf));
    passengerIndex = findPassenger(passengers, quantity, cpf);
    if(passengerIndex == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }
    printf("\nDados atuais:\n");
    printPassenger(&passengers[passengerIndex]);
    printf("\nDigite os novos dados:\n");
    readPassengerData(&passengers[passengerIndex]);

    savePassengers(passengers, quantity);
    printf("Passageiro alterado com sucesso!\n");
}

void deletePassenger(Passenger *passengers, int *quantity){
    char cpf[15], answer[16];
    int passengerIndex, position, confirmed = 0;
    printf("CPF do passageiro a excluir: ");
    readLine(cpf, sizeof(cpf));
    passengerIndex = findPassenger(passengers, *quantity, cpf);
    if(passengerIndex == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }
    printf("\nDados do passageiro:\n");
    printPassenger(&passengers[passengerIndex]);
    do {
        printf("\nConfirma a exclusão? (sim/nao): ");
        readLine(answer, sizeof(answer));
        if(strcmp(answer, "sim") == 0){
            confirmed = 1;
        } else if(strcmp(answer, "nao") == 0){
            printf("Exclusão cancelada.\n");
            return;
        } else {
            printf("Resposta inválida. Digite sim ou nao.\n");
        }
    } while(!confirmed);
    for(position = passengerIndex; position < *quantity - 1; position++){
        passengers[position] = passengers[position + 1];
    }
    (*quantity)--;
    savePassengers(passengers, *quantity);
    printf("Passageiro excluído com sucesso!\n");
}

Passenger *passengerMenu(Passenger *passengers, int *quantity){
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
            case 1: listPassengers(passengers, *quantity); break;
            case 2: searchPassenger(passengers, *quantity); break;
            case 3: passengers = insertPassenger(passengers, quantity); break;
            case 4: updatePassenger(passengers, *quantity); break;
            case 5: deletePassenger(passengers, quantity); break;
            case 6: break;
            default: printf("Opção inválida\n");
        }
    } while(option != 6);
    return passengers;
}

/* ============================================================
   Trips
   ============================================================ */

Trip *loadTrips(int *quantity){
    FILE *file = fopen(TRIP_FILE, "rb");

    if(file == NULL){
        *quantity = 0;
        return NULL;
    }

    Trip *trips = NULL;
    int bytes;

    fseek(file, 0, SEEK_END);
    bytes = ftell(file);
    rewind(file);
    *quantity = bytes / sizeof(Trip);

    if(*quantity > 0){
        trips = malloc(*quantity * sizeof(Trip));
        if(trips == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(trips, sizeof(Trip), *quantity, file);
    }

    fclose(file);
    return trips;
}

void saveTrips(Trip *trips, int quantity){
    FILE *file = fopen(TRIP_FILE, "wb");
    if(file == NULL){
        printf("Erro ao abrir o arquivo de viagens.\n");
        return;
    }
    fwrite(trips, sizeof(Trip), quantity, file);
    fclose(file);
}

int findTrip(Trip *trips, int quantity, int code){
    int tripIndex;
    for(tripIndex = 0; tripIndex < quantity; tripIndex++){
        if(trips[tripIndex].code == code){
            return tripIndex;
        }
    }
    return -1;
}

void printTrip(Trip *trip){
    printf("Código: %d\n", trip->code);
    printf("Origem: %s\n", trip->origin);
    printf("Destino: %s\n", trip->destination);
    printf("Data: %s\n", trip->date);
    printf("Horário: %s\n", trip->time);
    printf("Empresa: %s\n", trip->company);
    printf("Número de poltronas: %d\n", trip->seatQuantity);
}

/* Reads the editable trip fields (everything except the code) into trip. */
void readTripData(Trip *trip){
    printf("Origem: ");
    readLine(trip->origin, sizeof(trip->origin));
    printf("Destino: ");
    readLine(trip->destination, sizeof(trip->destination));
    printf("Data (DD/MM/AAAA): ");
    readLine(trip->date, sizeof(trip->date));
    printf("Horário (HH:MM): ");
    readLine(trip->time, sizeof(trip->time));
    printf("Empresa: ");
    readLine(trip->company, sizeof(trip->company));
    trip->seatQuantity = readIntRange("Número de poltronas: ", 1, 1000);
}

Trip *insertTrip(Trip *trips, int *quantity){
    int code;

    code = readInt("Código da viagem: ");
    if(findTrip(trips, *quantity, code) != -1){
        printf("Já existe uma viagem com esse código.\n");
        return trips;
    }

    trips = realloc(trips, (*quantity + 1) * sizeof(Trip));
    if(trips == NULL){
        printf("Erro de memória.\n");
        exit(1);
    }
    trips[*quantity].code = code;
    readTripData(&trips[*quantity]);
    (*quantity)++;

    saveTrips(trips, *quantity);
    printf("Viagem cadastrada com sucesso!\n");
    return trips;
}

void listTrips(Trip *trips, int quantity){
    int tripIndex;
    if(quantity == 0){
        printf("Nenhuma viagem cadastrada.\n");
        return;
    }
    for(tripIndex = 0; tripIndex < quantity; tripIndex++){
        printf("\n--- Viagem %d ---\n", tripIndex + 1);
        printTrip(&trips[tripIndex]);
    }
}

void searchTrip(Trip *trips, int quantity){
    int code, tripIndex;
    code = readInt("Código da viagem: ");
    tripIndex = findTrip(trips, quantity, code);
    if(tripIndex == -1){
        printf("Viagem não encontrada.\n");
        return;
    }
    printf("\n");
    printTrip(&trips[tripIndex]);
}

void updateTrip(Trip *trips, int quantity){
    int code, tripIndex;
    code = readInt("Código da viagem a alterar: ");
    tripIndex = findTrip(trips, quantity, code);
    if(tripIndex == -1){
        printf("Viagem não encontrada.\n");
        return;
    }
    printf("\nDados atuais:\n");
    printTrip(&trips[tripIndex]);
    printf("\nDigite os novos dados:\n");
    readTripData(&trips[tripIndex]);

    saveTrips(trips, quantity);
    printf("Viagem alterada com sucesso!\n");
}

void deleteTrip(Trip *trips, int *quantity){
    char answer[16];
    int code, tripIndex, position, confirmed = 0;
    code = readInt("Código da viagem a excluir: ");
    tripIndex = findTrip(trips, *quantity, code);
    if(tripIndex == -1){
        printf("Viagem não encontrada.\n");
        return;
    }
    printf("\nDados da viagem:\n");
    printTrip(&trips[tripIndex]);
    do {
        printf("\nConfirma a exclusão? (sim/nao): ");
        readLine(answer, sizeof(answer));
        if(strcmp(answer, "sim") == 0){
            confirmed = 1;
        } else if(strcmp(answer, "nao") == 0){
            printf("Exclusão cancelada.\n");
            return;
        } else {
            printf("Resposta inválida. Digite sim ou nao.\n");
        }
    } while(!confirmed);
    for(position = tripIndex; position < *quantity - 1; position++){
        trips[position] = trips[position + 1];
    }
    (*quantity)--;
    saveTrips(trips, *quantity);
    printf("Viagem excluída com sucesso!\n");
}

Trip *tripMenu(Trip *trips, int *quantity){
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
            case 1: listTrips(trips, *quantity); break;
            case 2: searchTrip(trips, *quantity); break;
            case 3: trips = insertTrip(trips, quantity); break;
            case 4: updateTrip(trips, *quantity); break;
            case 5: deleteTrip(trips, quantity); break;
            case 6: break;
            default: printf("Opção inválida\n");
        }
    } while(option != 6);
    return trips;
}

/* ============================================================
   Tickets
   ============================================================ */

Ticket *loadTickets(int *quantity){
    FILE *file = fopen(TICKET_FILE, "rb");

    if(file == NULL){
        *quantity = 0;
        return NULL;
    }

    Ticket *tickets = NULL;
    int bytes;

    fseek(file, 0, SEEK_END);
    bytes = ftell(file);
    rewind(file);
    *quantity = bytes / sizeof(Ticket);

    if(*quantity > 0){
        tickets = malloc(*quantity * sizeof(Ticket));
        if(tickets == NULL){
            printf("Erro de memória.\n");
            exit(1);
        }
        fread(tickets, sizeof(Ticket), *quantity, file);
    }

    fclose(file);
    return tickets;
}

void saveTickets(Ticket *tickets, int quantity){
    FILE *file = fopen(TICKET_FILE, "wb");
    if(file == NULL){
        printf("Erro ao abrir o arquivo de passagens.\n");
        return;
    }
    fwrite(tickets, sizeof(Ticket), quantity, file);
    fclose(file);
}

int findTicket(Ticket *tickets, int quantity, char *cpf, int tripCode){
    int ticketIndex;
    for(ticketIndex = 0; ticketIndex < quantity; ticketIndex++){
        if(tickets[ticketIndex].tripCode == tripCode && strcmp(tickets[ticketIndex].passengerCpf, cpf) == 0){
            return ticketIndex;
        }
    }
    return -1;
}

int seatIsTaken(Ticket *tickets, int quantity, int tripCode, int seat){
    int ticketIndex;
    for(ticketIndex = 0; ticketIndex < quantity; ticketIndex++){
        if(tickets[ticketIndex].tripCode == tripCode && tickets[ticketIndex].seat == seat){
            return 1;
        }
    }
    return 0;
}

void showAvailableSeats(Ticket *tickets, int quantity, int tripCode, int seatQuantity){
    int seat;
    printf("\nPoltronas (X = ocupada):\n");
    for(seat = 1; seat <= seatQuantity; seat++){
        if(seatIsTaken(tickets, quantity, tripCode, seat)){
            printf("[%d-X] ", seat);
        } else {
            printf("[%d] ", seat);
        }
        if(seat % 10 == 0){
            printf("\n");
        }
    }
    printf("\n");
}

int nextTicketNumber(Ticket *tickets, int quantity){
    int ticketIndex, highestNumber = 0;
    for(ticketIndex = 0; ticketIndex < quantity; ticketIndex++){
        if(tickets[ticketIndex].ticketNumber > highestNumber){
            highestNumber = tickets[ticketIndex].ticketNumber;
        }
    }
    return highestNumber + 1;
}

void printTicket(Ticket *ticket){
    printf("Passagem nº: %d\n", ticket->ticketNumber);
    printf("CPF do passageiro: %s\n", ticket->passengerCpf);
    printf("Código da viagem: %d\n", ticket->tripCode);
    printf("Poltrona: %d\n", ticket->seat);
    printf("Tipo: %s\n", ticket->type);
    printf("Status: %s\n", ticket->status);
    printf("Data da compra: %s\n", ticket->purchaseDate);
}

/* Asks the user for a free seat on the trip, re-prompting until valid.
   currentSeat is the seat the ticket already holds (-1 when registering a new one),
   so the user is allowed to keep it. */
int chooseSeat(Ticket *tickets, int quantity, int tripCode, int seatQuantity, int currentSeat){
    int seat, valid;
    showAvailableSeats(tickets, quantity, tripCode, seatQuantity);
    do {
        valid = 1;
        seat = readInt("Escolha a poltrona: ");
        if(seat < 1 || seat > seatQuantity){
            printf("Poltrona fora do intervalo (1 a %d).\n", seatQuantity);
            valid = 0;
        } else if(seat != currentSeat && seatIsTaken(tickets, quantity, tripCode, seat)){
            printf("Poltrona já ocupada. Escolha outra.\n");
            valid = 0;
        }
    } while(!valid);
    return seat;
}

/* Asks the user to choose the ticket type and writes it into type. */
void chooseType(char *type){
    int option;
    printf("Tipo: 1- Convencional  2- Leito\n");
    option = readIntRange("Escolha o tipo: ", 1, 2);
    if(option == 1){
        strcpy(type, "Convencional");
    } else {
        strcpy(type, "Leito");
    }
}

/* Asks the user to choose the ticket status and writes it into status. */
void chooseStatus(char *status){
    int option;
    printf("Status: 1- confirmada  2- cancelada  3- em espera\n");
    option = readIntRange("Escolha o status: ", 1, 3);
    if(option == 1){
        strcpy(status, "confirmada");
    } else if(option == 2){
        strcpy(status, "cancelada");
    } else {
        strcpy(status, "em espera");
    }
}

Ticket *insertTicket(Ticket *tickets, int *quantity, Passenger *passengers, int passengerQuantity, Trip *trips, int tripQuantity){
    char cpf[15];
    int code, tripIndex;

    printf("CPF do passageiro: ");
    readLine(cpf, sizeof(cpf));
    if(findPassenger(passengers, passengerQuantity, cpf) == -1){
        printf("Passageiro não encontrado. Cadastre o passageiro primeiro.\n");
        return tickets;
    }

    code = readInt("Código da viagem: ");
    tripIndex = findTrip(trips, tripQuantity, code);
    if(tripIndex == -1){
        printf("Viagem não encontrada. Cadastre a viagem primeiro.\n");
        return tickets;
    }

    if(findTicket(tickets, *quantity, cpf, code) != -1){
        printf("Esse passageiro já possui passagem para essa viagem.\n");
        return tickets;
    }

    tickets = realloc(tickets, (*quantity + 1) * sizeof(Ticket));
    if(tickets == NULL){
        printf("Erro de memória.\n");
        exit(1);
    }
    strcpy(tickets[*quantity].passengerCpf, cpf);
    tickets[*quantity].tripCode = code;
    tickets[*quantity].seat = chooseSeat(tickets, *quantity, code, trips[tripIndex].seatQuantity, -1);
    chooseType(tickets[*quantity].type);
    tickets[*quantity].ticketNumber = nextTicketNumber(tickets, *quantity);
    strcpy(tickets[*quantity].status, "confirmada");
    getToday(tickets[*quantity].purchaseDate);
    (*quantity)++;

    saveTickets(tickets, *quantity);
    printf("Passagem cadastrada com sucesso! (nº %d)\n", tickets[*quantity - 1].ticketNumber);
    return tickets;
}

void listTickets(Ticket *tickets, int quantity){
    int ticketIndex;
    if(quantity == 0){
        printf("Nenhuma passagem cadastrada.\n");
        return;
    }
    for(ticketIndex = 0; ticketIndex < quantity; ticketIndex++){
        printf("\n--- Passagem %d ---\n", ticketIndex + 1);
        printTicket(&tickets[ticketIndex]);
    }
}

void searchTicket(Ticket *tickets, int quantity){
    char cpf[15];
    int code, ticketIndex;
    printf("CPF do passageiro: ");
    readLine(cpf, sizeof(cpf));
    code = readInt("Código da viagem: ");
    ticketIndex = findTicket(tickets, quantity, cpf, code);
    if(ticketIndex == -1){
        printf("Passagem não encontrada.\n");
        return;
    }
    printf("\n");
    printTicket(&tickets[ticketIndex]);
}

void updateTicket(Ticket *tickets, int quantity, Trip *trips, int tripQuantity){
    char cpf[15];
    int code, ticketIndex, tripIndex;
    printf("CPF do passageiro: ");
    readLine(cpf, sizeof(cpf));
    code = readInt("Código da viagem: ");
    ticketIndex = findTicket(tickets, quantity, cpf, code);
    if(ticketIndex == -1){
        printf("Passagem não encontrada.\n");
        return;
    }
    printf("\nDados atuais:\n");
    printTicket(&tickets[ticketIndex]);
    printf("\nDigite os novos dados:\n");

    tripIndex = findTrip(trips, tripQuantity, code);
    if(tripIndex != -1){
        tickets[ticketIndex].seat = chooseSeat(tickets, quantity, code, trips[tripIndex].seatQuantity, tickets[ticketIndex].seat);
    } else {
        printf("Viagem não encontrada; mantendo a poltrona atual.\n");
    }
    chooseType(tickets[ticketIndex].type);
    chooseStatus(tickets[ticketIndex].status);

    saveTickets(tickets, quantity);
    printf("Passagem alterada com sucesso!\n");
}

void deleteTicket(Ticket *tickets, int *quantity){
    char cpf[15], answer[16];
    int code, ticketIndex, position, confirmed = 0;
    printf("CPF do passageiro: ");
    readLine(cpf, sizeof(cpf));
    code = readInt("Código da viagem: ");
    ticketIndex = findTicket(tickets, *quantity, cpf, code);
    if(ticketIndex == -1){
        printf("Passagem não encontrada.\n");
        return;
    }
    printf("\nDados da passagem:\n");
    printTicket(&tickets[ticketIndex]);
    do {
        printf("\nConfirma a exclusão? (sim/nao): ");
        readLine(answer, sizeof(answer));
        if(strcmp(answer, "sim") == 0){
            confirmed = 1;
        } else if(strcmp(answer, "nao") == 0){
            printf("Exclusão cancelada.\n");
            return;
        } else {
            printf("Resposta inválida. Digite sim ou nao.\n");
        }
    } while(!confirmed);
    for(position = ticketIndex; position < *quantity - 1; position++){
        tickets[position] = tickets[position + 1];
    }
    (*quantity)--;
    saveTickets(tickets, *quantity);
    printf("Passagem excluída com sucesso!\n");
}

Ticket *ticketMenu(Ticket *tickets, int *quantity, Passenger *passengers, int passengerQuantity, Trip *trips, int tripQuantity){
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
            case 1: listTickets(tickets, *quantity); break;
            case 2: searchTicket(tickets, *quantity); break;
            case 3: tickets = insertTicket(tickets, quantity, passengers, passengerQuantity, trips, tripQuantity); break;
            case 4: updateTicket(tickets, *quantity, trips, tripQuantity); break;
            case 5: deleteTicket(tickets, quantity); break;
            case 6: break;
            default: printf("Opção inválida\n");
        }
    } while(option != 6);
    return tickets;
}

/* ============================================================
   Reports (text files, overwritten on each generation)
   ============================================================ */

void reportTicketsByPassenger(Passenger *passengers, int passengerQuantity, Trip *trips, int tripQuantity, Ticket *tickets, int ticketQuantity){
    char cpf[15];
    int passengerIndex, tripIndex, ticketIndex, found = 0;
    FILE *file;

    printf("CPF do passageiro: ");
    readLine(cpf, sizeof(cpf));
    passengerIndex = findPassenger(passengers, passengerQuantity, cpf);
    if(passengerIndex == -1){
        printf("Passageiro não encontrado.\n");
        return;
    }

    file = fopen(REPORT1_FILE, "w");
    if(file == NULL){
        printf("Erro ao gerar o relatório.\n");
        return;
    }

    fprintf(file, "Relatório de passagens do passageiro %s\n", passengers[passengerIndex].name);
    fprintf(file, "CPF: %s\n\n", cpf);

    for(ticketIndex = 0; ticketIndex < ticketQuantity; ticketIndex++){
        if(strcmp(tickets[ticketIndex].passengerCpf, cpf) == 0){
            found = 1;
            tripIndex = findTrip(trips, tripQuantity, tickets[ticketIndex].tripCode);
            fprintf(file, "Passageiro: %s\n", passengers[passengerIndex].name);
            if(tripIndex != -1){
                fprintf(file, "Origem: %s\n", trips[tripIndex].origin);
                fprintf(file, "Destino: %s\n", trips[tripIndex].destination);
                fprintf(file, "Data: %s\n", trips[tripIndex].date);
                fprintf(file, "Poltrona: %d\n", tickets[ticketIndex].seat);
                fprintf(file, "Status: %s\n", tickets[ticketIndex].status);
                fprintf(file, "----------------------------------------\n");
            } else {
                fprintf(file, "Viagem não encontrada.\n");
            }
        }
    }

    if(!found){
        fprintf(file, "Nenhuma passagem encontrada para este passageiro.\n");
    }

    fclose(file);
    printf("Relatório gerado em %s.\n", REPORT1_FILE);
}

void reportPassengersByTrip(Passenger *passengers, int passengerQuantity, Trip *trips, int tripQuantity, Ticket *tickets, int ticketQuantity){
    int code, tripIndex, passengerIndex, ticketIndex, found = 0;
    FILE *file;

    code = readInt("Código da viagem: ");
    tripIndex = findTrip(trips, tripQuantity, code);
    if(tripIndex == -1){
        printf("Viagem não encontrada.\n");
        return;
    }

    file = fopen(REPORT2_FILE, "w");
    if(file == NULL){
        printf("Erro ao gerar o relatório.\n");
        return;
    }

    fprintf(file, "Relatório de passageiros da viagem %d\n", code);
    fprintf(file, "%s -> %s em %s\n\n", trips[tripIndex].origin, trips[tripIndex].destination, trips[tripIndex].date);

    for(ticketIndex = 0; ticketIndex < ticketQuantity; ticketIndex++){
        if(tickets[ticketIndex].tripCode == code){
            found++;
            passengerIndex = findPassenger(passengers, passengerQuantity, tickets[ticketIndex].passengerCpf);
            if(passengerIndex != -1){
                fprintf(file, "Passageiro: %s\n", passengers[passengerIndex].name);
            } else {
                fprintf(file, "Passageiro: (passageiro não encontrado)\n");
            }
            fprintf(file, "Poltrona: %d\n", tickets[ticketIndex].seat);
            fprintf(file, "Tipo: %s\n", tickets[ticketIndex].type);
            fprintf(file, "----------------------------------------\n");
        }
    }

    if(found == 0){
        fprintf(file, "Nenhum passageiro nesta viagem.\n");
    }

    fclose(file);
    printf("Relatório gerado em %s (%d passageiro(s)).\n", REPORT2_FILE, found);
}

void reportTripsByPeriod(Trip *trips, int tripQuantity){
    char startDate[11], endDate[11];
    int startKey, endKey, tripKey;
    int tripIndex, found = 0;
    FILE *file;

    printf("Data inicial (DD/MM/AAAA): ");
    readLine(startDate, sizeof(startDate));
    printf("Data final (DD/MM/AAAA): ");
    readLine(endDate, sizeof(endDate));
    startKey = dateToKey(startDate);
    endKey = dateToKey(endDate);

    file = fopen(REPORT3_FILE, "w");
    if(file == NULL){
        printf("Erro ao gerar o relatório.\n");
        return;
    }

    fprintf(file, "Relatório de viagens de %s a %s\n\n", startDate, endDate);

    for(tripIndex = 0; tripIndex < tripQuantity; tripIndex++){
        tripKey = dateToKey(trips[tripIndex].date);
        if(tripKey >= startKey && tripKey <= endKey){
            found++;
            fprintf(file, "Código: %d\n", trips[tripIndex].code);
            fprintf(file, "Origem: %s\n", trips[tripIndex].origin);
            fprintf(file, "Destino: %s\n", trips[tripIndex].destination);
            fprintf(file, "----------------------------------------\n");
        }
    }

    if(found == 0){
        fprintf(file, "Nenhuma viagem encontrada no período.\n");
    }

    fclose(file);
    printf("Relatório gerado em %s (%d viagem(ns)).\n", REPORT3_FILE, found);
}

void reportMenu(Passenger *passengers, int passengerQuantity, Trip *trips, int tripQuantity, Ticket *tickets, int ticketQuantity){
    int option;
    do {
        printf("\n--- Submenu de Relatórios ---\n");
        printf("1- Passagens de um passageiro\n");
        printf("2- Passageiros de uma viagem\n");
        printf("3- Viagens em um período\n");
        printf("4- Voltar\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: reportTicketsByPassenger(passengers, passengerQuantity, trips, tripQuantity, tickets, ticketQuantity); break;
            case 2: reportPassengersByTrip(passengers, passengerQuantity, trips, tripQuantity, tickets, ticketQuantity); break;
            case 3: reportTripsByPeriod(trips, tripQuantity); break;
            case 4: break;
            default: printf("Opção inválida\n");
        }
    } while(option != 4);
}

/* ============================================================
   Main
   ============================================================ */

int main(){
    int passengerQuantity, tripQuantity, ticketQuantity;
    int option;

    Passenger *passengers = loadPassengers(&passengerQuantity);
    Trip *trips = loadTrips(&tripQuantity);
    Ticket *tickets = loadTickets(&ticketQuantity);

    do {
        printf("\n===== Sistema de Passagens de Ônibus =====\n");
        printf("1- Submenu de Passageiros\n");
        printf("2- Submenu de Viagens\n");
        printf("3- Submenu de Passagens\n");
        printf("4- Submenu de Relatórios\n");
        printf("5- Sair\n");
        option = readInt("Escolha uma opção: ");
        switch(option){
            case 1: passengers = passengerMenu(passengers, &passengerQuantity); break;
            case 2: trips = tripMenu(trips, &tripQuantity); break;
            case 3: tickets = ticketMenu(tickets, &ticketQuantity, passengers, passengerQuantity, trips, tripQuantity); break;
            case 4: reportMenu(passengers, passengerQuantity, trips, tripQuantity, tickets, ticketQuantity); break;
            case 5: printf("Saindo...\n"); break;
            default: printf("Opção inválida\n");
        }
    } while(option != 5);

    free(passengers);
    free(trips);
    free(tickets);
    return 0;
}
