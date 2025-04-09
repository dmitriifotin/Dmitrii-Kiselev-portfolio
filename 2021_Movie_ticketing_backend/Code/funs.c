#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funs.h"
// function to abort the order without saving to file and exit the program
void endSession (int *adminAccess) {
    printf("Exiting the program. Thank you!\n\n\n");
    adminAccess = 0;
    abort();
}
void uploadData(FILE* names, char movieNames[][500], int *movieCount, FILE* schedule, int importSchedule[][6],
                int *scheduleCount, FILE* foodList, char food[][2][50], int *foodItems, FILE* foodPriceList, int foodPrices[],
                FILE* reservationsList, int *reservationCount, int importReservations[][7], char userNames[][2][50],
                double totals[]) {
    names = fopen("movieNames.txt", "r");
    if (names == NULL) {
        printf("Could not open file movieNames.txt.\n");
    }
    while (!feof(names)) {
        fgets(movieNames[*movieCount], 500, names);
        if (movieNames[*movieCount][strlen(movieNames[*movieCount])-1] == '\n') {
            movieNames[*movieCount][strlen(movieNames[*movieCount])-1] = '\0';
        }
        ++*movieCount;
    }
    fclose(names);

    schedule = fopen("schedule.txt", "r");
    if (schedule == NULL) {
        printf("Could not open file schedule.txt.\n");
    }
    while (!feof(schedule)) {
        fscanf(schedule, "%d %d %d %d %d %d", &importSchedule[*scheduleCount][0], &importSchedule[*scheduleCount][1],
               &importSchedule[*scheduleCount][2], &importSchedule[*scheduleCount][3], &importSchedule[*scheduleCount][4],
               &importSchedule[*scheduleCount][5]);
        ++*scheduleCount;
    }
    fclose(schedule);

    foodList = fopen("food.txt", "r");
    if (foodList == NULL) {
        printf("Could not open file food.txt.\n");
    }
    int i = 0;
    while (!feof(foodList)) {
        char temp1[50];
        fgets(temp1, 50, foodList);
        if(temp1[1] == '\n') {
            break;
        }
        if (temp1[strlen(temp1)-1] == '\n') {
            temp1[strlen(temp1)-1] = '\0';
        }
        sprintf(food[i][1], "%c", temp1[strlen(temp1)-1]);
        temp1[strlen(temp1)-1] = '\0';
        strcpy(food[i][0], temp1);
        ++i;
        ++(*foodItems);
    }
    fclose(foodList);

    foodPriceList = fopen("foodPrices.txt", "r");
    if (foodPriceList == NULL) {
        printf("Could not open file foodPrices.txt.\n");
    }
    i = 0;
    while (!feof(foodPriceList)) {
        fscanf(foodPriceList, "%d", &foodPrices[i]);
        ++i;
    }
    fclose(foodPriceList);

    reservationsList = fopen("reservations.txt", "r");
    if (reservationsList == NULL) {
        printf("Could not open file reservations.txt.\n");
    }
    while (!feof(reservationsList)) {
        fscanf(reservationsList, "%d %d %d %d %d %d %d", &importReservations[*reservationCount][0],
               &importReservations[*reservationCount][1], &importReservations[*reservationCount][2],
               &importReservations[*reservationCount][3], &importReservations[*reservationCount][4],
               &importReservations[*reservationCount][5], &importReservations[*reservationCount][6]);
        ++(*reservationCount);
    }
    fclose(reservationsList);

    i = 0;
    int throwawayI;
    FILE* orderList = NULL;
    orderList = fopen("orders.txt", "r");
    if (orderList == NULL) {
        printf("Could not open file orders.txt.\n");
    }
    while (!feof(orderList)) {
        fscanf(orderList, "%d", &i);
        fscanf(orderList, "%s %s %d %d/%d %d $%lf", userNames[i][0], userNames[i][1], &throwawayI, &throwawayI,
               &throwawayI, &throwawayI, &totals[i]);
    }
    fclose(orderList);
}
void selectMovie(int movieCount, char movieNames[][500], int *userMovieChoice, int scheduleCount, int importSchedule[][6],
                 char movieMonth[][50], char movieTimes[][10], int counting[], int *count, int *userDateChoice,
                 int *userMovieToSave, int *userMonthToSave, int *userDayToSave, int *userTimeToSave, int *orderNumber,
                 int *rows, int *seats, int *reservationCount) {
    printf("\nSelect movie:\n\n");
    for (int i = 0; i < movieCount; ++i) {
        printf("%d: %s\n", i+1, movieNames[i]);
    }
    while(scanf("%d", &*userMovieChoice) != 1);
    while(*userMovieChoice < 1 || *userMovieChoice > movieCount) {
        printf("\nInvalid input. Select option below:\n");
        for (int i = 0; i < movieCount; ++i) {
            printf("%d: %s\n", i+1, movieNames[i]);
        }
        scanf("%d", &*userMovieChoice);
    }

    *userMovieChoice -= 1;
    printf("Select date and time:\n\n");
    for (int i = 0; i < scheduleCount; ++i) {
        if (*userMovieChoice == importSchedule[i][0]) {
            printf("%d: %s %d %s\n", (*count)+1 , movieMonth[(importSchedule[i][1] - 1)], importSchedule[i][2],
                   movieTimes[importSchedule[i][3]]);
            counting[*count] = i;
            ++*count;
        }
    }
    while(scanf("%d", &*userDateChoice) != 1);
    while(*userDateChoice < 1 || *userDateChoice > *count) {
        *count = 0;
        printf("\nInvalid input. Select option below:\n");
        for (int i = 0; i < scheduleCount; ++i) {
            if (*userMovieChoice == importSchedule[i][0]) {
                printf("%d: %s %d %s\n", (*count)+1 , movieMonth[(importSchedule[i][1] - 1)], importSchedule[i][2],
                       movieTimes[importSchedule[i][3]]);
                counting[*count] = i;
                ++*count;
            }
        }
        scanf("%d", &*userDateChoice);
    }

    *userDateChoice -= 1;
    *userDateChoice = counting[*userDateChoice];

    *userMovieToSave = importSchedule[*userDateChoice][0];
    *userMonthToSave = importSchedule[*userDateChoice][1];
    *userDayToSave = importSchedule[*userDateChoice][2];
    *userTimeToSave = importSchedule[*userDateChoice][3];
    *orderNumber = *reservationCount - 1;
    *rows = importSchedule[*userDateChoice][4];
    *seats = importSchedule[*userDateChoice][5];
}

void printSeats (int rows, int seats, int reservationCount, int importReservations[][7], int userMovieToSave,
                 int userMonthToSave, int userDayToSave, int userTimeToSave, char rowLetters[5]){
    int k;
    printf("Select seat (NA means not available)\n\n");
    for (int i = rows - 1; i >= 0; --i) {
        for (int j = 0; j < seats; ++j) {
            for (k = 0; k < reservationCount; ++k) {
                // print seats and "NA" if the seat is unavailable
                if (importReservations[k][0] == userMovieToSave &&
                    importReservations[k][1] == userMonthToSave &&
                    importReservations[k][2] == userDayToSave &&
                    importReservations[k][3] == userTimeToSave &&
                    importReservations[k][4] == i &&
                    importReservations[k][5] == j + 1)
                    {
                    printf("NA ");
                    k = -1;
                    break;
                }
            }
            if (k != -1) {
                printf("%d%c ", j+1, rowLetters[i]);
            }
        }
        printf("\n");
    }
    for (int i = 0; i < ((seats * 3) - 6) / 2; ++i) {
        printf("_");
    }
    printf("SCREEN");
    for (int i = 0; i < ((seats * 3) - 6) / 2; ++i) {
        printf("_");
    }
    printf("\n\n");
}
// function to prompt user to select food in UI
void selectFood(int itemsSelected[][3], int *itemCounter, int foodItems, char food[][2][50], int foodPrices[],
                double *total, int orderNumber) {
    int selectedItem;
    int itemQuantity;
    printf("Select item:\n\n");
    int foodCounter = 0;
    int counting[50];
    for (int i = 0; i <= foodItems; ++i) {
        char temp = food[i][1][0];
        if (temp == 'A') {
            printf("%d: %s - $%d\n", foodCounter + 1, food[i][0], foodPrices[i]);
            counting[foodCounter] = i;
            ++foodCounter;
        }
    }

    while(scanf("%d", &selectedItem) != 1);
    while(selectedItem < 1 || selectedItem > foodCounter - 1) {
        printf("\nInvalid input. Select option below:\n");
        foodCounter = 0;
        for (int i = 0; i <= foodItems; ++i) {
            char temp = food[i][1][0];
            if (temp == 'A') {
                printf("%d: %s - $%d\n", foodCounter + 1, food[i][0], foodPrices[i]);
                counting[foodCounter] = i;
                ++foodCounter;
            }
        }
        scanf("%d", &selectedItem);
    }
    selectedItem -= 1;
    selectedItem = counting[selectedItem];

    printf("Enter quantity:\n");
    scanf("%d", &itemQuantity);

    *total += (double) itemQuantity * foodPrices[selectedItem];
    itemsSelected[*itemCounter][0] = selectedItem;
    itemsSelected[*itemCounter][1] = itemQuantity;
    itemsSelected[*itemCounter][2] = orderNumber;
    *itemCounter += 1;

    printf("Would you like to add more drinks/snacks to the order?\n");
}
// function to prompt user to complete order in UI
void pay(int importReservations[][7], int reservationCount, double total, int orderNumber, int itemsSelected[][3],
         int itemCounter, char userNames[][2][50], double totals[]) {

    char firstName[100];
    char lastName[100];
    char fullName[200];
    char cardNumber[20];
    int expMonth;
    int expYear;
    int CVC;

    printf("\nEnter your first name:\n");
    fflush(stdin);
    fgets(firstName, 50, stdin);
    firstName[strlen(firstName) - 1] = '\0';
    fflush(stdin);

    printf("\nEnter your last name name:\n");
    fflush(stdin);
    fgets(lastName, 50, stdin);
    lastName[strlen(lastName) - 1] = '\0';
    fflush(stdin);

    printf("\nEnter your card number:\n");
    fflush(stdin);
    fgets(cardNumber, 17, stdin);
    fflush(stdin);
    while(strlen(cardNumber) < 16) {
        printf("\nInvalid input, enter a 16 digit card number without spaces.\n");
        fflush(stdin);
        fgets(cardNumber, 17, stdin);
        fflush(stdin);
    }

    printf("\nEnter your card expiry month:\n");
    while(scanf("%d", &expMonth) != 1);
    while(expMonth < 1 || expMonth > 12) {
        printf("\nInvalid input, enter a number from 1 to 12.\n");
        scanf("%d", &expMonth);
    }

    printf("\nEnter your card expiry year:\n");
    while(scanf("%d", &expYear) != 1);
    while(expYear <= 2021 && expMonth <= 8) {
        printf("\nInvalid input, enter a year in the future.\n");
        scanf("%d", &expYear);
    }

    printf("\nEnter your card CVC code:\n");
    while(scanf("%d", &CVC) != 1);
    while(CVC < 100 || CVC > 999) {
        printf("\nInvalid input, enter a 3 digit CVC code.\n");
        scanf("%d", &CVC);
    }

    //blacklist compare
    strcat(fullName, firstName);
    strcat(fullName, " ");
    strcat(fullName, lastName);

    char blacklistImport[500][200];
    FILE* blacklist = NULL;
    blacklist = fopen("blacklist.txt", "r");
    if (blacklist == NULL) {
        printf("Could not open file blacklist.txt.\n");
    }
    int i = 0;
    while (!feof(blacklist)) {
        fgets(blacklistImport[i], 500, blacklist);
        if (strcmp (blacklistImport[i], fullName) == 0) {
            printf("Unfortunately, it is not possible for us to complete this order\ndue to an incident recorded under your name.\n");
            abort();
        }
        ++i;
    }

    strcpy(userNames[reservationCount][0], firstName);
    strcpy(userNames[reservationCount][1], lastName);

    FILE* reservationsList = NULL;
    reservationsList = fopen("reservations.txt", "a");
    if (reservationsList == NULL) {
        printf("Could not open file reservations.txt.\n");
    }

    for (int i = 0; i <= reservationCount; ++i) {
        if (orderNumber == importReservations[i][6]) {
            fprintf(reservationsList, "\n%d %d %d %d %d %d %d", importReservations[i][0], importReservations[i][1],
                    importReservations[i][2], importReservations[i][3], importReservations[i][4], importReservations[i][5],
                    orderNumber);
        }
    }
    fclose(reservationsList);

    //update the reservations.txt
    FILE* orders = NULL;
    orders = fopen("orders.txt", "a");
    if (orders == NULL) {
        printf("Could not open file orders.txt.\n");
    }
    fprintf(orders, "\n%d %s %s %s %d/%d %d $%.2lf", orderNumber, firstName, lastName, cardNumber, expMonth, expYear, CVC, total);

    //update the foodOrders.txt
    FILE* foodOrders = NULL;
    foodOrders = fopen("foodOrders.txt", "a");
    if (foodOrders == NULL) {
        printf("Could not open file foodOrders.txt.\n");
    }
    for(int i = 0; i < itemCounter; ++i) {
        fprintf(foodOrders, "\n%d %d %d", orderNumber, itemsSelected[i][0], itemsSelected[i][1]);
    }
    totals[reservationCount] = total;
    printf("Your order has been complete. You order number is %04d.\n", orderNumber);
}
void printTicket(int orderNumber, int importReservations[][7], int reservationCount, char movieNames[][500],
                 int movieCount, char movieMonth[12][50], char movieTimes[5][10], char rowLetters[26],
                 int itemsSelected[][3], int itemCounter, char food[][2][50]) {
    char orderString[10];
    sprintf(orderString, "%04d", orderNumber);
    char ticketFileName[20] = "ticket_";
    strcat(ticketFileName, orderString);
    strcat(ticketFileName, ".txt");

    FILE* ticket = NULL;
    ticket = fopen(ticketFileName, "w+");
    if (ticket == NULL) {
        printf("Could not open file %s.\n", ticketFileName);
    }
    fprintf(ticket, "Order #%04d\n\n%s\n%s %d at %s\n\n", orderNumber, movieNames[importReservations[reservationCount][0]],
            movieMonth[importReservations[orderNumber][1]], importReservations[orderNumber][2],
            movieTimes[importReservations[orderNumber][3]]);

    for (int i = 0; i <= reservationCount; ++i) {
        if (importReservations[i][6] == orderNumber) {
            fprintf(ticket, "Seat %d%c\n", importReservations[i][5] + 1, rowLetters[importReservations[i][4]]);
        }
    }
    fprintf(ticket, "Food/drinks ordered:\n");
    for (int i = 0; i < itemCounter; ++i) {
        fprintf(ticket, "%s x%d\n", *food[itemsSelected[i][0]], itemsSelected[i][1]);
    }

    printf("Order details:\n%s\n%s %d at %s\n\n", movieNames[importReservations[reservationCount][0]],
            movieMonth[importReservations[orderNumber][1]], importReservations[orderNumber][2],
            movieTimes[importReservations[orderNumber][3]]);

    for (int i = 0; i <= reservationCount; ++i) {
        if (importReservations[i][6] == orderNumber) {
            printf("Seat %d%c\n", importReservations[i][5], rowLetters[importReservations[i][4]]);
        }
    }
    printf("Food/drinks ordered:\n");
    for (int i = 0; i < itemCounter; ++i) {
        printf("%s x%d\n", *food[itemsSelected[i][0]], itemsSelected[i][1]);
    }
}
// function to prompt user to apply promo code in UI
void applyPromo(double *total, int *promoApplied) {
    char userPromo[20];
    char promoList[10][10];
    char temp[5];
    double promoListVal[10];

    printf("Enter your promo code:\n");
    scanf("%s", userPromo);

    FILE* promos = NULL;
    promos = fopen("promoCodes.txt", "r");
    if (promos == NULL) {
        printf("Could not open file promoCodes.txt.\n");
    }
    int i = 0;
    while (!feof(promos)) {
        fscanf(promos, "%s", promoList[i]);
        if (promoList[i][strlen(promoList[i]) - 1] == '\n') {
                promoList[i][strlen(promoList[i])] = '\0';
        }
        strncpy(temp, promoList[i], 2);
        promoListVal[i] = atof(temp);
        if(strcmp(userPromo, promoList[i]) == 0) {
            *total -= (*total * promoListVal[i] / 100.00);
            *promoApplied = 1;
            printf("Promo applied!\n");
            break;
        }
        ++i;
    }
    if (*promoApplied != 1) {
        printf("Invalid code.\n\n\n");
    }
    fclose(promos);
}
// function to prompt user to change snack/drink quantity selections in UI
void changeOrder(int itemsSelected[][3], int itemCounter, char food[][2][50], int foodPrices[],
                 double *total, int orderNumber) {
    int userChoice;
    int newQuantity;

    printf("\nWhat item would you like to update?\n");
    for (int i = 0; i < itemCounter; ++i) {
        if (itemsSelected[i][1] != 0) {
            printf("%d: %s x%d - $%d\n", i + 1, *food[itemsSelected[i][0]], itemsSelected[i][1], foodPrices[itemsSelected[i][0]] * itemsSelected[i][1]);
        }
        else {
            printf("%d: %s has been removed\n", i + 1, *food[itemsSelected[i][0]]);
        }
    }
    while(scanf("%d", &userChoice) != 1);
    while(userChoice < 1 || userChoice > itemCounter) {
        printf("\nInvalid input. Select option below:\n");
        for (int i = 0; i < itemCounter; ++i) {
            if (itemsSelected[i][1] != 0) {
                printf("%d: %s x%d - $%d\n", i + 1, *food[itemsSelected[i][0]], itemsSelected[i][1], foodPrices[itemsSelected[i][0]] * itemsSelected[i][1]);
            }
            else {
                printf("%d: %s has been removed\n", i + 1, *food[itemsSelected[i][0]]);
            }
        }
        scanf("%d", &userChoice);
    }


    userChoice -= 1;
    printf("\nEnter the new quantity.\n");
    while(scanf("%d", &newQuantity) != 1);
    while(newQuantity < 0) {
        printf("Invalid quantity. Quantity must be positive or 0.\n\n");
        printf("\nEnter the new quantity.\n");
        scanf("%d", &newQuantity);
    }
    *total -= (itemsSelected[userChoice][1] - newQuantity) * foodPrices[itemsSelected[userChoice][0]];
    itemsSelected[userChoice][1] = newQuantity;
    printf("Order details updated.\n");
}
// function to print out available seats in UI
void startOver(int *orderReview, int *count, double *total, char *add, int *reservationCount, int importReservations[][7],
               int orderNumber) {
    *orderReview = 1;
    *count = 0;
    *total = 0;
    *add = 'Y';
    int linesToDelete = 0;
    for (int i = 0; i < *reservationCount; ++i) {
        if (importReservations[i][6] == orderNumber) {
            ++linesToDelete;
        }
    }
    for (int i = 0; i <= linesToDelete; ++i) {
        --*reservationCount;
    }
}

// function to confirm the selected seat is available in UI
int reservationConfirmation (int importReservations[][7], int reservationCount, int userMovieToSave, int userMonthToSave,
                             int userDayToSave, int userTimeToSave, int userRowChoice, int userSeatChoice, int scheduleCount,
                             char userRowChoiceLetter) {
    int k;

    for (k = 0; k < reservationCount; ++k) {
        if (importReservations[k][0] == userMovieToSave &&
            importReservations[k][1] == userMonthToSave &&
            importReservations[k][2] == userDayToSave &&
            importReservations[k][3] == userTimeToSave &&
            importReservations[k][4] == userRowChoice &&
            importReservations[k][5] == userSeatChoice)
            {
            printf("\nThis seat is already taken, select another seat.\n\n");
            fflush(stdin);
            return 0;
            break;
            }
        else if (userSeatChoice < 0 && userSeatChoice > 4 && userRowChoiceLetter < 'A' && userRowChoiceLetter > 'E') {
            printf("Invalid selection, select an available seat from the table below.\n");
            return 0;
            break;
        }
    }
    return 1;
}
void adminLogin(int *adminAccess) {
    char password[50] = "ece103";
    char userInput[50];
    printf("Enter password:\n");
    scanf("%s", userInput);
    fflush(stdin);
    if (userInput[0] == '0') {
        *adminAccess = -1;
    }
    else if (strcmp(userInput, password) != 0) {
        printf("Incorrect password, try again or enter 0 to quit.\n");
        adminLogin(adminAccess);
    }
    else {
        printf("Welcome to the admin panel.");
    }
}
void viewBookings(int orderNumber, char userNames[][2][50], char movieNames[][500], char movieMonth[][50],
                  int importReservations[][7], double totals[], int reservationCount, char movieTimes[][10]) {
    for (int i = 0; i < reservationCount; ++i) {
        if (importReservations[i][2] != 0) {
            printf("%04d\t%10s %10s\t%*.*s\t%8s %d %5s\t $%.2lf\n", importReservations[i][6], userNames[importReservations[i][6]][0],
                   userNames[importReservations[i][6]][1], 20, 20, movieNames[importReservations[i][0]],
                   movieMonth[importReservations[i][1] - 1], importReservations[i][2], movieTimes[importReservations[i][3]],
                   totals[importReservations[i][6]]);
        }
    }
}
void addEvent(int *movieCount, char movieNames[][500], char movieTimes[][10], int importSchedule[][6], int *scheduleCount,
              FILE* schedule, FILE* names) {
    printf("\nSelect existing movie or add a new movie\n\n");
    int optionSelect;
    int month;
    int day;
    int time;
    int rowNum;
    int seatNum;
    int i;
    for (i = 0; i < *movieCount; ++i) {
        printf("%d: %s\n", i + 1, movieNames[i]);
    }
    printf("%d: Add new movie\n\n", i + 1);
    while(scanf("%d", &optionSelect) != 1);
    while(optionSelect < 1 || optionSelect > i + 1) {
        printf("\nInvalid input. Select options below:\n");
        for (i = 0; i < *movieCount; ++i) {
            printf("%d: %s\n", i + 1, movieNames[i]);
        }
        printf("%d: Add new movie\n\n", i + 1);
        scanf("%d", &optionSelect);
    }
    optionSelect -= 1;
    int daysInMonth[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (optionSelect != i) {
        //add month
        printf("Enter month:\n");
        while(scanf("%d", &month) != 1);
        while(month < 1 || month > 12) {
            printf("\nInvalid input. Enter number from 1 to 12:\n");
            scanf("%d", &month);
        }

        //add day
        printf("Enter day:\n");
        while(scanf("%d", &day) != 1);
        while(day < 1 || day > daysInMonth[month]) {
            printf("\nInvalid input. Enter a number from 1 to %d:\n", daysInMonth[month]);
            scanf("%d", &day);
        }

        //add time
        printf("Select time:\n");
        int j;
        for (j = 0; j < 5; ++j) {
            printf("%d: %s\n", j + 1, movieTimes[j]);
        }
        while(scanf("%d", &time) != 1);
        while(time < 1 || time > j) {
            printf("\nInvalid input. Select option below:\n");
            for (j = 0; j < 5; ++j) {
                printf("%d: %s\n", j + 1, movieTimes[j]);
            }
            scanf("%d", &time);
        }
        time -= 1;

        //add rows
        printf("Enter number of rows:\n");
        while(scanf("%d", &rowNum) != 1);
        while(rowNum < 1 || rowNum > 20) {
            printf("\nInvalid input. Enter a positive number up to 20:\n");
            scanf("%d", &rowNum);
        }

        //add seats
        printf("Enter number of seats per row:\n");
        while(scanf("%d", &seatNum) != 1);
        while(seatNum < 1 || seatNum > 20) {
            printf("\nInvalid input. Enter a positive number up to 20:\n");
            scanf("%d", &seatNum);
        }

        //update schedule.txt
        schedule = fopen("schedule.txt", "a");
        if (schedule == NULL) {
            printf("Could not open file schedule.txt.\n");
        }
        fprintf(schedule, "\n%d %d %d %d %d %d", optionSelect, month, day, time, rowNum, seatNum);
        fclose(schedule);
        //update importSchedule array
        importSchedule[*scheduleCount][0] = optionSelect;
        importSchedule[*scheduleCount][1] = month;
        importSchedule[*scheduleCount][2] = day;
        importSchedule[*scheduleCount][3] = time;
        importSchedule[*scheduleCount][4] = rowNum;
        importSchedule[*scheduleCount][5] = seatNum;
        ++(*scheduleCount);
        printf("\nEvent added. Returning to main menu.\n\n");
    }
    else {
        //enter new name
        char movieName[50];
        printf("Enter the movie name:\n");
        fflush(stdin);
        fgets(movieName, 50, stdin);
        movieName[strlen(movieName) - 1] = '\0';
        fflush(stdin);
        //update movie names txt
        names = fopen("movieNames.txt", "a");
        if (names == NULL) {
            printf("Could not open file movieNames.txt.\n");
        }
        fprintf(names, "\n");
        fprintf(names, "%s", movieName);
        fclose(names);
        //update movie names array
        ++*movieCount;
        strcpy(movieNames[*movieCount], movieName);

        //add month
        printf("Enter month:\n");
        while(scanf("%d", &month) != 1);
        while(month < 1 || month > 12) {
            printf("\nInvalid input. Enter number from 1 to 12:\n");
            scanf("%d", &month);
        }

        //add day
        printf("Enter day:\n");
        while(scanf("%d", &day) != 1);
        while(day < 1 || day > daysInMonth[month]) {
            printf("\nInvalid input. Enter a number from 1 to %d:\n", daysInMonth[month]);
            scanf("%d", &day);
        }

        //add time
        printf("Select time:\n");
        int j;
        for (j = 0; j < 5; ++j) {
            printf("%d: %s\n", j + 1, movieTimes[j]);
        }
        while(scanf("%d", &time) != 1);
        while(time < 1 || time > j) {
            printf("\nInvalid input. Select option below:\n");
            for (j = 0; j < 5; ++j) {
                printf("%d: %s\n", j + 1, movieTimes[j]);
            }
            scanf("%d", &time);
        }
        time -= 1;
        //add rows
        printf("Enter number of rows:\n");
        while(scanf("%d", &rowNum) != 1);
        while(rowNum < 1 || rowNum > 20) {
            printf("\nInvalid input. Enter a positive number up to 20:\n");
            scanf("%d", &rowNum);
        }

        //add seats
        printf("Enter number of seats per row:\n");
        while(scanf("%d", &seatNum) != 1);
        while(seatNum < 1 || seatNum > 20) {
            printf("\nInvalid input. Enter a positive number up to 20:\n");
            scanf("%d", &seatNum);
        }

        //update schedule.txt
        schedule = fopen("schedule.txt", "a");
        if (schedule == NULL) {
            printf("Could not open file schedule.txt.\n");
        }
        fprintf(schedule, "\n%d %d %d %d %d %d", (*movieCount) - 1, month, day, time, rowNum, seatNum);
        fclose(schedule);

        //update importSchedule array
        importSchedule[*scheduleCount][0] = *movieCount;
        importSchedule[*scheduleCount][1] = month;
        importSchedule[*scheduleCount][2] = day;
        importSchedule[*scheduleCount][3] = time;
        importSchedule[*scheduleCount][4] = rowNum;
        importSchedule[*scheduleCount][5] = seatNum;
        ++(*scheduleCount);
        printf("\nEvent added. Returning to main menu.\n\n");
    }
}
void editEvent(int scheduleCount, char movieNames[][500], char movieMonth[][50], char movieTimes[][10], int movieCount,
               int importSchedule[][6], FILE* schedule) {
    //printf all scheduled events
    int daysInMonth[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int optionSelect;
    int temp;
    printf("Select event to update:\n\n");
    int i;
    for(i = 0; i < scheduleCount; ++i) {
        printf("%03d: %*.*s\t%*.*s %02d %s\n", i + 1, 20, 20, movieNames[importSchedule[i][0]], 10, 10, movieMonth[importSchedule[i][1] - 1],
               importSchedule[i][2], movieTimes[importSchedule[i][3]]);
    }
    //select event to edit
    while(scanf("%d", &optionSelect) != 1);
    while(optionSelect < 1 || optionSelect > i) {
        printf("\nInvalid input. Select event to update:\n\n");
        for(i = 0; i < scheduleCount; ++i) {
            printf("%03d: %*.*s\t%*.*s %02d %s\n", i + 1, 20, 20, movieNames[importSchedule[i][0]], 10, 10, movieMonth[importSchedule[i][1] - 1],
               importSchedule[i][2], movieTimes[importSchedule[i][3]]);
        }
        scanf("%d", &optionSelect);
    }

    //select new movie name
    printf("Select new movie name:\n\n");
    for(i = 0; i < movieCount; ++i) {
        printf("%d: %s\n", i + 1, movieNames[i]);
    }
    while(scanf("%d", &temp) != 1);
    while(temp < 1 || temp > i) {
        printf("\nInvalid input. Select new movie name:\n\n");
        for(i = 0; i < movieCount; ++i) {
            printf("%d: %s\n", i + 1, movieNames[i]);
        }
        scanf("%d", &temp);
    }
    importSchedule[optionSelect - 1][0] = temp - 1;

    //enter month
    printf("Enter month:\n\n");
    while(scanf("%d", &temp) != 1);
    while(temp < 1 || temp > 12) {
        printf("Invalid input. Enter number from 1 to 12:\n\n");
        scanf("%d", &temp);
    }
    importSchedule[optionSelect - 1][1] = temp;

    //enter day
    printf("Enter day:\n\n");
    while(scanf("%d", &importSchedule[optionSelect - 1][2]) != 1);
    while(importSchedule[optionSelect - 1][2] < 1 ||
          importSchedule[optionSelect - 1][2] > daysInMonth[importSchedule[optionSelect - 1][1]]) {
        printf("\nInvalid input. Enter a number from 1 to %d.\n\n", daysInMonth[importSchedule[optionSelect - 1][1]]);
        scanf("%d", &importSchedule[optionSelect - 1][2]);
    }

    //select time
    printf("Select viewing time:\n\n");
    for(int i = 0; i < 5; ++i) {
        printf("%d: %s\n", i + 1, movieTimes[i]);
    }
    while(scanf("%d", &temp) != 1);
    while(temp < 1 || temp > 5) {
        printf("\nInvalid input. Select option:\n\n");
        for(int i = 0; i < 5; ++i) {
            printf("%d: %s\n", i + 1, movieTimes[i]);
        }
        scanf("%d", &temp);
    }
    importSchedule[optionSelect - 1][3] = temp - 1;

    //enter rownum
    printf("Enter number of rows:\n\n");
    while(scanf("%d", &importSchedule[optionSelect - 1][4]) != 1);
    while(importSchedule[optionSelect - 1][4] < 1 ||
          importSchedule[optionSelect - 1][4] > 20) {
        printf("\nInvalid input. Enter number from 1 to 20.\n\n");
        scanf("%d", &importSchedule[optionSelect - 1][4]);
    }

    //enter seatnum
    printf("Enter number of seats:\n\n");
    while(scanf("%d", &importSchedule[optionSelect - 1][5]) != 1);
    while(importSchedule[optionSelect - 1][5] < 1||
          importSchedule[optionSelect - 1][5] > 20) {
        printf("\nInvalid input. Enter number from 1 to 20.\n\n");
        scanf("%d", &importSchedule[optionSelect - 1][5]);
    }

    //update schedule txt
    schedule = fopen("schedule.txt", "w+");
    if (schedule == NULL) {
        printf("Could not open file schedule.txt.\n");
    }
    for (int i = 0; i < scheduleCount; ++i) {
        fprintf(schedule, "%d %d %d %d %d %d", importSchedule[i][0], importSchedule[i][1], importSchedule[i][2],
                importSchedule[i][3], importSchedule[i][4], importSchedule[i][5]);
        if (i < scheduleCount - 1) {
            fprintf(schedule, "\n");
        }
    }
    fclose(schedule);
    printf("Event details have been updated. Returning to main menu.\n\n");
}
void editBooking(int reservationCount, int importReservations[][7], char rowLetters[5], int rows, int seats, int scheduleCount,
                 FILE* reservationsList, char food[][2][50], int importSchedule[][6]){
    int optionSelect;
    int orderNumber;
    //select order number
    printf("Enter the order number:\n");
    while(scanf("%d", &orderNumber) != 1);
    while(orderNumber < 1) {
        printf("\nInvalid input, enter order number:\n");
        scanf("%d", &orderNumber);
    }
    printf("Select option:\n1: Edit/remove seats\n2: Edit/remove snacks\n");
    while(scanf("%d", &optionSelect) != 1);
    while(optionSelect < 1 || optionSelect > 2) {
        printf("\nInvalid input. Select option:\n1: Edit/remove seats\n2: Edit/remove snacks\n");
        scanf("%d", &optionSelect);
    }
    //change seat info
    if (optionSelect == 1) {
        int seatSelect;
        int selections[30];
        int userSeatChoice;
        char userRowChoiceLetter;
        int userRowChoice;
        int resCount = 0;
        printf("\nSelect seat to update/remove\n");
        for (int i = 0; i <= reservationCount - 1; ++i) {
            if (importReservations[i][6] == orderNumber) {
                printf("%d: %d%c\n", resCount + 1, importReservations[i][5], rowLetters[importReservations[i][4]]);
                selections[resCount] = i;
                ++resCount;
            }
        }
        while(scanf("%d", &seatSelect) != 1);
        while(seatSelect < 1 || seatSelect > resCount) {
            printf("\nInvalid input. Select option from the list.\n");
            scanf("%d", &seatSelect);
        }
        seatSelect -= 1;


        printf("\nSelect action:\n1: Change seat\n2: Remove seat\n");
        while(scanf("%d", &optionSelect) != 1);
        while(optionSelect < 1 || optionSelect > 2) {
            printf("\nInvalid input. Select action:\n1: Change seat\n2: Remove seat\n");
            scanf("%d", &optionSelect);
        }

        int scheduleIndex = 0;
        if (optionSelect == 1) {
            for(int i  = 0; i < scheduleCount; ++i) {
                if (importSchedule[i][0] == importReservations[selections[seatSelect]][0] &&
                    importSchedule[i][1] == importReservations[selections[seatSelect]][1] &&
                    importSchedule[i][2] == importReservations[selections[seatSelect]][2] &&
                    importSchedule[i][3] == importReservations[selections[seatSelect]][3]) {
                    scheduleIndex = i;
                }
            }

            printSeats (importSchedule[scheduleIndex][4], importSchedule[scheduleIndex][5], reservationCount, importReservations, importReservations[selections[seatSelect]][0],
                 importReservations[selections[seatSelect]][1], importReservations[selections[seatSelect]][2],
                 importReservations[selections[seatSelect]][3], rowLetters);

            //check both seat and row entered
            while(scanf("%d%c", &userSeatChoice, &userRowChoiceLetter) != 2) {
                printf("Invalid input, select available seat from the table below.\n\n");
                printSeats(importSchedule[scheduleIndex][4], importSchedule[scheduleIndex][5], reservationCount, importReservations, importReservations[selections[seatSelect]][0],
                           importReservations[selections[seatSelect]][1], importReservations[selections[seatSelect]][2],
                           importReservations[selections[seatSelect]][3], rowLetters);
            }

            //check the seat is not out of range
            while(userSeatChoice < 0 || userSeatChoice > importSchedule[scheduleIndex][5] || userRowChoiceLetter < 'A' || userRowChoiceLetter > rowLetters[importSchedule[scheduleIndex][4]] - 1) {
                printf("Invalid input, select available seat from the table below.\n\n");
                printSeats(importSchedule[scheduleIndex][4], importSchedule[scheduleIndex][5], reservationCount, importReservations, importReservations[selections[seatSelect]][0],
                           importReservations[selections[seatSelect]][1], importReservations[selections[seatSelect]][2],
                           importReservations[selections[seatSelect]][3], rowLetters);
                scanf("%d%c", &userSeatChoice, &userRowChoiceLetter);
            }

            //encode the row with a number
            for (int i = 0; i < importSchedule[scheduleIndex][4]; ++i) {
                if (userRowChoiceLetter == rowLetters[i]) {
                    userRowChoice = i;
                }
            }
            while (reservationConfirmation(importReservations, reservationCount, importReservations[selections[seatSelect]][0],
                                           importReservations[selections[seatSelect]][1],
                                           importReservations[selections[seatSelect]][2],
                                           importReservations[selections[seatSelect]][3], userRowChoice, userSeatChoice,
                                           scheduleCount,userRowChoiceLetter) == 0) {
                printSeats(importSchedule[scheduleIndex][4], importSchedule[scheduleIndex][5], reservationCount,
                           importReservations, importReservations[selections[seatSelect]][0],
                           importReservations[selections[seatSelect]][1], importReservations[selections[seatSelect]][2],
                           importReservations[selections[seatSelect]][3], rowLetters);

                //check both seat and row entered
                while(scanf("%d%c", &userSeatChoice, &userRowChoiceLetter) != 2) {
                    printf("Invalid input, select available seat from the table below.\n\n");
                    printSeats(importSchedule[scheduleIndex][4], importSchedule[scheduleIndex][5], reservationCount, importReservations, importReservations[selections[seatSelect]][0],
                               importReservations[selections[seatSelect]][1], importReservations[selections[seatSelect]][2],
                               importReservations[selections[seatSelect]][3], rowLetters);
                }

                //check the seat is not out of range
                while(userSeatChoice < 0 || userSeatChoice > importSchedule[scheduleIndex][5] || userRowChoiceLetter < 'A' || userRowChoiceLetter > rowLetters[importSchedule[scheduleIndex][4]] - 1) {
                    printf("Invalid input, select available seat from the table below.\n\n");
                    printSeats(importSchedule[scheduleIndex][4], importSchedule[scheduleIndex][5], reservationCount, importReservations, importReservations[selections[seatSelect]][0],
                               importReservations[selections[seatSelect]][1], importReservations[selections[seatSelect]][2],
                               importReservations[selections[seatSelect]][3], rowLetters);
                    scanf("%d%c", &userSeatChoice, &userRowChoiceLetter);
                }

                //encode the row with a number
                for (int i = 0; i < importSchedule[scheduleIndex][4]; ++i) {
                    if (userRowChoiceLetter == rowLetters[i]) {
                        userRowChoice = i;
                    }
                }
            }
            importReservations[selections[seatSelect]][4] = userRowChoice;
            importReservations[selections[seatSelect]][5] = userSeatChoice;

            //update the file
            reservationsList = fopen("reservations.txt", "w+");
            if (reservationsList == NULL) {
                printf("Could not open file reservations.txt.\n");
            }
            for(int i = 0; i < reservationCount; ++i) {
                fprintf(reservationsList, "%d %d %d %d %d %d %d", importReservations[i][0], importReservations[i][1],
                        importReservations[i][2], importReservations[i][3], importReservations[i][4],
                        importReservations[i][5], importReservations[i][6]);
                if(i < reservationCount - 1) {
                    fprintf(reservationsList, "\n");
                }
            }
            fclose(reservationsList);
            printf("Your order has been updated. Returning to main menu.\n\n");

        }
        else if (optionSelect == 2) {
            importReservations[selections[seatSelect]][2] = 0;
            importReservations[selections[seatSelect]][4] = -1;
            importReservations[selections[seatSelect]][5] = -1;
            //update the file
            reservationsList = fopen("reservations.txt", "w+");
            if (reservationsList == NULL) {
                printf("Could not open file reservations.txt.\n");
            }
            for(int i = 0; i < reservationCount; ++i) {
                fprintf(reservationsList, "%d %d %d %d %d %d %d", importReservations[i][0], importReservations[i][1],
                        importReservations[i][2], importReservations[i][3], importReservations[i][4],
                        importReservations[i][5], importReservations[i][6]);
                if (i < reservationCount - 1) {
                    fprintf(reservationsList, "\n");
                }
            }
            optionSelect = 0;
            fclose(reservationsList);
            printf("Your order has been updated. Returning to main menu.\n\n");
        }
    }
    else if(optionSelect == 2) {
        int foodOrderDetails[5000][3];
        int i = 0;
        int userChoice;
        int newQuantity;
        int itemCounter = 0;
        int items[5000][3];
        FILE* foodOrders = NULL;
        foodOrders = fopen("foodOrders.txt", "r");
        if (foodOrders == NULL) {
                printf("Could not open file foodOrders.txt.\n");
            }
        while(!feof(foodOrders)) {
            fscanf(foodOrders, "%d %d %d", &foodOrderDetails[i][0], &foodOrderDetails[i][1], &foodOrderDetails[i][2]);
            ++i;
        }
        fclose(foodOrders);
        printf("\nSelect item to update:\n\n");
        for (int j = 0; j < i; ++j) {
            if (foodOrderDetails[j][0] == orderNumber) {
                items[itemCounter][0] = foodOrderDetails[j][1];
                items[itemCounter][1] = foodOrderDetails[j][2];
                items[itemCounter][2] = j;
                printf("%d: %s x%d\n", itemCounter + 1, food[items[itemCounter][0]][0], items[itemCounter][1]);
                ++itemCounter;
            }
        }
        while(scanf("%d", &userChoice) != 1);
        while(userChoice < 0 || userChoice > itemCounter) {
            printf("\nInvalid input. Selection option below.\n");
            for (int j = 0; j < i; ++j) {
                if (foodOrderDetails[j][0] == orderNumber) {
                    items[itemCounter][0] = foodOrderDetails[j][1];
                    items[itemCounter][1] = foodOrderDetails[j][2];
                    items[itemCounter][2] = j;
                    printf("%d: %s x%d\n", itemCounter + 1, food[items[itemCounter][0]][0], items[itemCounter][1]);
                    ++itemCounter;
                }
            }
            scanf("%d", &userChoice);
        }
        userChoice -= 1;

        printf("\nSelect new quantity:\n\n");
        while(scanf("%d", &newQuantity) != 1);
        while(newQuantity < 0) {
            printf("\nInvalid input. Enter positive number or 0.\n\n");
            scanf("%d", &newQuantity);
        }

        foodOrderDetails[items[userChoice][2]][2] = newQuantity;

        foodOrders = fopen("foodOrders.txt", "w+");
        if (foodOrders == NULL) {
            printf("Could not open file foodOrders.txt.\n");
        }
        for (int j = 0; j < i; ++j) {
            fprintf(foodOrders, "%d %d %d", foodOrderDetails[j][0], foodOrderDetails[j][1], foodOrderDetails[j][2]);
            if(j < i - 1) {
                fprintf(foodOrders, "\n");
            }
        }
        fclose(foodOrders);
        printf("Your order has been updated. Returning to main menu.\n\n");
    }
}
void editBlacklist(){
    int optionSelect;
    int selectEntry;
    char yes_no;
    FILE* blacklist = NULL;
    printf("\nWould you like to add or delete an entry?\n\n1: Add\n2: Delete\n");
    while(scanf("%d", &optionSelect) != 1);
    while(optionSelect < 1 || optionSelect > 2) {
        printf("\nInvalid input. Select option below:\n\n1: Add\n2: Delete\n");
        scanf("%d", &optionSelect);
    }

    if (optionSelect == 1) {
        char newEntryFirstName[50];
        char newEntryLastName[50];
        printf("Enter the first name:\n");
        fflush(stdin);
        fgets(newEntryFirstName, "%s", stdin);
        newEntryFirstName[strlen(newEntryFirstName) - 1] = '\0';
        fflush(stdin);
        printf("Enter the last name:\n");
        fflush(stdin);
        fgets(newEntryLastName, "%s", stdin);
        newEntryLastName[strlen(newEntryLastName) - 1] = '\0';
        fflush(stdin);
        strcat(newEntryFirstName, " ");
        strcat(newEntryFirstName, newEntryLastName);
        blacklist = fopen("blacklist.txt", "a");
        if (blacklist == NULL) {
            printf("Could not open file blacklist.txt.\n");
        }
        fprintf(blacklist, "\n");
        fprintf(blacklist, "%s", newEntryFirstName);
        fclose(blacklist);
    }
    else if (optionSelect == 2) {
        char blacklistImport[500][200];
        blacklist = fopen("blacklist.txt", "r");
        if (blacklist == NULL) {
            printf("Could not open file blacklist.txt.\n");
        }
        int i = 0;
        while (!feof(blacklist)) {
            fgets(blacklistImport[i], 500, blacklist);
            if (blacklistImport[i][strlen(blacklistImport[i]) - 1] == '\n') {
                blacklistImport[i][strlen(blacklistImport[i]) - 1] = '\0';
            }

            printf("%d - %s\n", i + 1, blacklistImport[i]);
            ++i;
        }
        fclose(blacklist);

        while(scanf("%d", &selectEntry) != 1);
        while(selectEntry < 0 || selectEntry > i - 1) {
            printf("\nInvalid input. Select option from the list.\n");
            scanf("%d", &selectEntry);
        }

        selectEntry -= 1;
        printf("Confirm you would like to remove %s from the blacklist? (Y/N)\n", blacklistImport[selectEntry]);
        while(scanf(" %c", &yes_no) != 1);
        while(!(yes_no == 'y' || yes_no == 'Y' || yes_no == 'n' || yes_no == 'N')) {
            printf("\nInvalid input. Enter Y or N.\n");
            scanf(" %c", &yes_no);
        }
        fflush(stdin);
        if (yes_no == 'y' || yes_no == 'Y') {
            --i;
            memset(blacklistImport[selectEntry],0,strlen(blacklistImport[selectEntry]));
            for(int k = selectEntry; k < i - 1; ++k) {
                memset(blacklistImport[k],0,strlen(blacklistImport[k]));
                strcpy(blacklistImport[k], blacklistImport[k + 1]);
            }
            //overwrite the file with the new array
            blacklist = fopen("blacklist.txt", "w+");
            if (blacklist == NULL) {
                printf("Could not open file blacklist.txt.\n");
            }
            for(int j = 0; j <= i; ++j) {
                fprintf(blacklist, "%s", blacklistImport[j]);
                if (j < i - 1) {
                    fprintf(blacklist, "\n");
                }
            }
            fclose(blacklist);
        }
        else {
            printf("Deletion not executed. Returning to main menu.\n");
        }
    }
    else {
        printf("Invalid input. Returning to main menu.\n");
    }
}
