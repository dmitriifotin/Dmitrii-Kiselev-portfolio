#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vars.h"
#include "funs.h"

int main()
{
/******
UPLOADS
******/
    FILE* schedule = NULL;
    FILE* names = NULL;
    FILE* foodList = NULL;
    FILE* foodPriceList = NULL;
    FILE* reservationsList = NULL;

    uploadData(names, movieNames, &movieCount, schedule, importSchedule, &scheduleCount, foodList, food,
               &foodItems, foodPriceList, foodPrices, reservationsList, &reservationCount, importReservations,
               userNames, totals);

/*************
USER INTERFACE
*************/

    printf("Welcome to ECE Theaters!\nTo exit, press 0 at any time.\nSelect option:\n1: User\n2: Admin\n\n");
    while(scanf("%d", &adminAccess) != 1) {
        printf("Invalid input.\nSelect option:\n1: User\n2: Admin\n\n");
    }
    while(adminAccess > 2 || adminAccess < 1) {
        printf("Invalid input.\nSelect option:\n1: User\n2: Admin\n\n");
        scanf("%d", &adminAccess);
    }

    while(adminAccess == 1){

        selectMovie(movieCount, movieNames, &userMovieChoice, scheduleCount, importSchedule,
                 movieMonth, movieTimes, counting, &count, &userDateChoice,
                 &userMovieToSave, &userMonthToSave, &userDayToSave, &userTimeToSave, &orderNumber,
                 &rows, &seats, &reservationCount);

        // print out available seats and ask for user selection
        while (add == 'Y' || add == 'y') {
            printSeats(rows, seats, reservationCount, importReservations, userMovieToSave, userMonthToSave,
                       userDayToSave, userTimeToSave, rowLetters);

            //check both seat and row entered
            while(scanf("%d%c", &userSeatChoice, &userRowChoiceLetter) != 2) {
                printf("Invalid input, select available seat from the table below.\n\n");
                printSeats(rows, seats, scheduleCount, importReservations, userMovieToSave, userMonthToSave,
                           userDayToSave, userTimeToSave, rowLetters);
            }
            //check the seat is not out of range
            while(userSeatChoice < 0 || userSeatChoice > seats || userRowChoiceLetter < 'A' || userRowChoiceLetter > rowLetters[rows] - 1) {
                printf("Invalid input, select available seat from the table below.\n\n");
                printSeats(rows, seats, scheduleCount, importReservations, userMovieToSave, userMonthToSave,
                           userDayToSave, userTimeToSave, rowLetters);
                scanf("%d%c", &userSeatChoice, &userRowChoiceLetter);
            }

            //encode the row with a number
            for (int i = 0; i < rows; ++i) {
                if (userRowChoiceLetter == rowLetters[i]) {
                    userRowChoice = i;
                }
            }

            // loop to keep asking for a seat
            while (1) {
                if (reservationConfirmation(importReservations, reservationCount, userMovieToSave, userMonthToSave,
                                            userDayToSave, userTimeToSave, userRowChoice, userSeatChoice, scheduleCount,
                                            userRowChoiceLetter) != 0) {
                    break;
                    printf("\n\n");
                }

                printSeats(rows, seats, scheduleCount, importReservations, userMovieToSave, userMonthToSave,
                           userDayToSave, userTimeToSave, rowLetters);

                //check both seat and row entered
                while(scanf("%d%c", &userSeatChoice, &userRowChoiceLetter) != 2) {
                    printf("Invalid input, select available seat from the table below.\n\n");
                    printSeats(rows, seats, scheduleCount, importReservations, userMovieToSave, userMonthToSave,
                               userDayToSave, userTimeToSave, rowLetters);
                }

                //check the seat is not out of range
                while(userSeatChoice < 0 || userSeatChoice > seats || userRowChoiceLetter < 'A' || userRowChoiceLetter > rowLetters[rows] - 1) {
                    printf("Invalid input, select available seat from the table below.\n\n");
                    printSeats(rows, seats, scheduleCount, importReservations, userMovieToSave, userMonthToSave,
                               userDayToSave, userTimeToSave, rowLetters);
                    scanf("%d%c", &userSeatChoice, &userRowChoiceLetter);
                }

                fflush(stdin);

                //encode the row with a number
                for (int i = 0; i < rows; ++i) {
                    if (userRowChoiceLetter == rowLetters[i]) {
                        userRowChoice = i;
                    }
                }
            }

            total += 20.0; //$20 per ticket


            //update array with new reservation
            importReservations[reservationCount][0] = userMovieChoice;
            importReservations[reservationCount][1] = userMonthToSave;
            importReservations[reservationCount][2] = userDayToSave;
            importReservations[reservationCount][3] = userTimeToSave;
            importReservations[reservationCount][4] = userRowChoice;
            importReservations[reservationCount][5] = userSeatChoice;
            importReservations[reservationCount][6] = orderNumber;

            reservationCount += 1; //increment reservation count (for imports and exports)

            printf("Would you like to add another seat?\n");
            while(scanf(" %c", &add) != 1) {
                printf("Invalid input, enter Y or N.\n\n");
            }

            //input check
            while(!(add == 'Y' || add == 'y' || add == 'n' || add == 'N' || add == '0')) {
                printf("Invalid input, enter Y or N.\n\n");
                scanf(" %c", &add);
            }

            fflush(stdin);

            //increment reservation array if yes
            if (add == 'y' || add == 'Y') {
                ++reservationCount;
            }

            //quit if 0
            if (add == '0') {
                endSession(&adminAccess);
            }
        }

        //ask user if they want to add food to order
        printf("Would you like to add drinks/snacks to your order?\n");
        while(scanf(" %c", &add) != 1) {
            printf("Invalid input. Enter Y or N.\n\n");
        }

        //input check
        while(!(add == 'Y' || add == 'y' || add == 'n' || add == 'N' || add == '0')) {
            printf("Invalid input, enter Y or N.\n\n");
            scanf(" %c", &add);
        }
        fflush(stdin);

        //quit if 0
        if (add == '0') {
            endSession(&adminAccess);
        }

        //keep calling food selection function as long as user responds yes to adding more items
        while (add == 'Y' || add == 'y') {
                selectFood(itemsSelected, &itemCounter, foodItems, food, foodPrices, &total, orderNumber);

                //check char is received
                while(scanf(" %c", &add) != 1) {
                    printf("Invalid input, enter Y or N.\n\n");
                }
                //input check
                while(!(add == 'Y' || add == 'y' || add == 'n' || add == 'N' || add == '0')) {
                    printf("Invalid input, enter Y or N.\n\n");
                    scanf(" %c", &add);
                }
                fflush(stdin);
                if (add == '0') {
                    endSession(&adminAccess);
                }
        }

        //finalize order
        orderReview = 0;
        int promoApplied = 0;
        while(orderReview != 1) {
            printf("Order details:\n%s\n%s %d at %s\n\n", movieNames[importReservations[reservationCount - 1][0]],
                    movieMonth[importReservations[reservationCount - 1][1] - 1], importReservations[reservationCount - 1][2],
                    movieTimes[importReservations[reservationCount - 1][3]]);

            for (int i = 0; i <= reservationCount; ++i) {
                if (importReservations[i][6] == orderNumber) {
                    printf("Seat %d%c\n", importReservations[i][5], rowLetters[importReservations[i][4]]);
                }
            }
            printf("Food/drinks ordered:\n");
            for (int i = 0; i < itemCounter; ++i) {
                printf("%s x%d\n", food[itemsSelected[i][0]][0], itemsSelected[i][1]);
            }

            printf("\n\nYour total is $%.2lf.\n", total);
            printf("How would you like to proceed?\n\n");
            printf("1: Confirm and pay\n2: Apply promo code\n3: Add snack/drink selections\n4: Change snack/drink selections\n5: Start over\n6: Cancel order\n\n");
            //input check
            while(scanf("%d", &orderReview) != 1) {
                printf("Invalid input, try again.\n");
                printf("\n\nYour total is $%.2lf.\n", total);
                printf("How would you like to proceed?\n\n");
                printf("1: Confirm and pay\n2: Apply promo code\n3: Add snack/drink selections\n4: Change snack/drink selections\n5: Start over\n6: Cancel order\n\n");
            }
            // exit if input is 0
            if (orderReview == 0) {
                endSession(&adminAccess);
            }

            switch (orderReview) {
            case 1 :
                pay(importReservations, reservationCount, total, orderNumber, itemsSelected, itemCounter, userNames, totals);
                printTicket(orderNumber, importReservations, reservationCount, movieNames,
                            movieCount, movieMonth, movieTimes, rowLetters,
                            itemsSelected, itemCounter, food);
                endSession(&adminAccess);
                break;
            case 2 :
                if (promoApplied == 0) {
                        applyPromo(&total, &promoApplied);
                }
                else {
                    printf("\nNot possible to apply multiple promo codes.\n");
                }
                break;
            case 3 :
                selectFood(itemsSelected, &itemCounter, foodItems, food, foodPrices, &total, orderNumber);
                break;
            case 4 :
                changeOrder(itemsSelected, itemCounter, food, foodPrices, &total, orderNumber);
                break;
            case 5 :
                startOver(&orderReview, &count, &total, &add, &reservationCount, importReservations, orderNumber);
                break;
            case 6 :
                printf("Your order has been canceled.\n");
                endSession(&adminAccess);
                break;
            default :
                printf("Invalid input, try again.\n\n");
                break;
            }
        }
    }
/**************
ADMIN INTERFACE
**************/
    // if user selects to log in as admin, prompt them to enter password
    if(adminAccess == 2) {
        adminLogin(&adminAccess);
    }

    // if user selected to quit instead of entering the password, abort program
    if (adminAccess == -1) {
        endSession(&adminAccess);
    }

    while(adminAccess == 2){
        printf("\nWhat would you like to do?\n\n1: View bookings\n2: Add a new event\n3: Edit existing event\n4: Edit booking\n5: Edit blacklist\n6: Quit\n");
        scanf("%d", &adminOption);

        switch(adminOption) {
        case 1:
            viewBookings(orderNumber, userNames, movieNames, movieMonth, importReservations, totals,
                         reservationCount, movieTimes);
            break;
        case 2:
            addEvent(&movieCount, movieNames, movieTimes, importSchedule, &scheduleCount, schedule, names);
            break;
        case 3:
            editEvent(scheduleCount, movieNames, movieMonth, movieTimes, movieCount, importSchedule, schedule);
            break;
        case 4:
            editBooking(reservationCount, importReservations, rowLetters, rows, seats, scheduleCount,
                        reservationsList, food, importSchedule);
            break;
        case 5:
            editBlacklist();
            break;
        case 6:
            endSession(&adminAccess);
            break;
        default :
            printf("Invalid input, try again.\n\n");
            break;
        }

    }
}
