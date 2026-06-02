#include "utils.c"
#include "passenger.c"
#include "trips.c"
#include "ticket.c"
#include "reports.c"

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
