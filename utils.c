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
