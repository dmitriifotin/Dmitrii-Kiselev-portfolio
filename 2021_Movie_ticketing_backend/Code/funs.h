#ifndef _FUNS_
#define _FUNS_
// function to abort the order without saving to file and exit the program
void endSession (int *adminAccess);
void uploadData(FILE* names, char movieNames[][500], int *movieCount, FILE* schedule, int importSchedule[][6],
                int *scheduleCount, FILE* foodList, char food[][2][50], int *foodItems, FILE* foodPriceList, int foodPrices[],
                FILE* reservationsList, int *reservationCount, int importReservations[][7], char userNames[][2][50],
                double totals[]);

void selectMovie(int movieCount, char movieNames[][500], int *userMovieChoice, int scheduleCount, int importSchedule[][6],
                 char movieMonth[][50], char movieTimes[][10], int counting[], int *count, int *userDateChoice,
                 int *userMovieToSave, int *userMonthToSave, int *userDayToSave, int *userTimeToSave, int *orderNumber,
                 int *rows, int *seats, int *reservationCount);

// function to prompt user to select food in UI
void selectFood(int itemsSelected[][3], int *itemCounter, int foodItems, char food[][2][50], int foodPrices[],
                double *total, int orderNumber);

// function to prompt user to complete order in UI
void pay(int importReservations[][7], int reservationCount, double total, int orderNumber, int itemsSelected[][3],
         int itemCounter, char userNames[][2][50], double totals[]);
void printTicket(int orderNumber, int importReservations[][7], int reservationCount, char movieNames[][500],
                 int movieCount, char movieMonth[12][50], char movieTimes[5][10], char rowLetters[26],
                 int itemsSelected[][3], int itemCounter, char food[][2][50]);

// function to prompt user to apply promo code in UI
void applyPromo(double *total, int *promoApplied);

// function to prompt user to change snack/drink quantity selections in UI
void changeOrder(int itemsSelected[][3], int itemCounter, char food[][2][50], int foodPrices[],
                 double *total, int orderNumber);

// function to cdiscard any changes and start the reservation over in UI
void startOver(int *orderReview, int *count, double *total, char *add, int *reservationCount, int importReservations[][7],
               int orderNumber);
// function to print out available seats in UI
void printSeats (int rows, int seats, int reservationCount, int importReservations[][7], int userMovieToSave,
                 int userMonthToSave, int userDayToSave, int userTimeToSave, char rowLetters[5]);

// function to confirm the selected seat is available in UI
int reservationConfirmation (int importReservations[][7], int reservationCount, int userMovieToSave, int userMonthToSave,
                             int userDayToSave, int userTimeToSave, int userRowChoice, int userSeatChoice, int scheduleCount,
                             char userRowChoiceLetter);

void adminLogin(int *adminAccess);

void viewBookings(int orderNumber, char userNames[][2][50], char movieNames[][500], char movieMonth[][50],
                  int importReservations[][7], double totals[], int reservationCount, char movieTimes[][10]);
void addEvent(int *movieCount, char movieNames[][500], char movieTimes[][10], int importSchedule[][6], int *scheduleCount,
              FILE* schedule, FILE* names);
void editEvent(int scheduleCount, char movieNames[][500], char movieMonth[][50], char movieTimes[][10], int movieCount,
               int importSchedule[][6], FILE* schedule);
void editBooking(int reservationCount, int importReservations[][7], char rowLetters[5], int rows, int seats, int scheduleCount,
                 FILE* reservationsList, char food[][2][50], int importSchedule[][6]);
void editBlacklist();

#endif
