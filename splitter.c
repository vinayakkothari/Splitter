#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 30
#define INFINITY 999999.0

typedef struct {
    char name[MAX_NAME_LENGTH];
    float balance;
} User;

typedef struct {
    int payer;
    int *receivers;
    int numReceivers;
    float amount;
} Payment;

User *users = NULL;
Payment *payments = NULL;
int numUsers = 0;
int numPayments = 0;

void addUser(const char *name) {
    users = realloc(users, (numUsers + 1) * sizeof(User));
    strcpy(users[numUsers].name, name);
    users[numUsers].balance = 0.0;
    numUsers++;
}

void addPayment(int payer, const int *receivers, int numReceivers, float amount) {
    payments = realloc(payments, (numPayments + 1) * sizeof(Payment));
    payments[numPayments].payer = payer;
    payments[numPayments].numReceivers = numReceivers;
    payments[numPayments].receivers = malloc(numReceivers * sizeof(int));
    memcpy(payments[numPayments].receivers, receivers, numReceivers * sizeof(int));
    payments[numPayments].amount = amount;
    numPayments++;
}

void settleBalances() {
    for (int i = 0; i < numPayments; i++) {
        int payerIndex = payments[i].payer;
        float perReceiverAmount = payments[i].amount / payments[i].numReceivers;

        // Update payer's balance
        users[payerIndex].balance -= payments[i].amount;

        // Update receivers' balances
        for (int j = 0; j < payments[i].numReceivers; j++) {
            int receiverIndex = payments[i].receivers[j];
            users[receiverIndex].balance += perReceiverAmount;
        }
    }

    // Print updated balances
    for (int i = 0; i < numUsers; i++) {
        printf("%s: %.2f\n", users[i].name, users[i].balance);
    }
}

void suggestPayments() {
    while (1) {
        int maxCreditor = -1;
        int maxDebtor = -1;
        float maxAmount = 0.0;

        // Find the user with the maximum positive balance (creditor)
        for (int i = 0; i < numUsers; i++) {
            if (users[i].balance > 0 && (maxCreditor == -1 || users[i].balance > users[maxCreditor].balance)) {
                maxCreditor = i;
            }
        }

        // Find the user with the maximum negative balance (debtor)
        for (int i = 0; i < numUsers; i++) {
            if (users[i].balance < 0 && (maxDebtor == -1 || users[i].balance < users[maxDebtor].balance)) {
                maxDebtor = i;
            }
        }

        // If no creditor or debtor is found, break the loop
        if (maxCreditor == -1 || maxDebtor == -1) {
            break;
        }

        // Calculate the suggested payment amount
        float suggestedAmount = (users[maxCreditor].balance < -users[maxDebtor].balance) ? users[maxCreditor].balance : -users[maxDebtor].balance;

        // Update balances and print the suggested payment
        users[maxCreditor].balance -= suggestedAmount;
        users[maxDebtor].balance += suggestedAmount;

        printf("%s pays %.2f to %s\n", users[maxCreditor].name, suggestedAmount, users[maxDebtor].name);
    }
}

void freeMemory() {
    free(users);
    for (int i = 0; i < numPayments; i++) {
        free(payments[i].receivers);
    }
    free(payments);
}

int main() {
    int choice;
    char name[MAX_NAME_LENGTH];
    char payer[MAX_NAME_LENGTH];
    char receiver[MAX_NAME_LENGTH];
    int numReceivers;
    float amount;

    do {
        printf("\nSplitter Program\n");
        printf("1. Add user\n");
        printf("2. Add payment\n");
        printf("3. Settle balances\n");
        printf("4. Suggest payments\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter user name: ");
                scanf("%s", name);
                addUser(name);
                printf("User added successfully.\n");
                break;
            case 2:
                if (numUsers < 1) {
                    printf("At least one user is required to add a payment.\n");
                    break;
                }

                printf("Enter payer name: ");
                scanf("%s", payer);

                int payerIndex = -1;
                for (int i = 0; i < numUsers; i++) {
                    if (strcmp(payer, users[i].name) == 0) {
                        payerIndex = i;
                        break;
                    }
                }

                if (payerIndex == -1) {
                    printf("Error: Payer not found.\n");
                    break;
                }

                printf("Enter the number of receivers: ");
                scanf("%d", &numReceivers);

                if (numReceivers < 1 || numReceivers > numUsers) {
                    printf("Invalid number of receivers.\n");
                    break;
                }

                int *receivers = malloc(numReceivers * sizeof(int));

                printf("Enter receiver names (separated by spaces): ");
                for (int i = 0; i < numReceivers; i++) {
                    scanf("%s", receiver);
                    for (int j = 0; j < numUsers; j++) {
                        if (strcmp(receiver, users[j].name) == 0) {
                            receivers[i] = j;
                            break;
                        }
                    }
                }

                printf("Enter amount: ");
                scanf("%f", &amount);

                addPayment(payerIndex, receivers, numReceivers, amount);
                free(receivers);
                printf("Payment added successfully.\n");
                break;
            case 3:
                settleBalances();
                break;
            case 4:
                suggestPayments();
                break;
            case 0:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);

    freeMemory();
    return 0;
}
