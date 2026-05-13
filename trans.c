#include <stdio.h>
#include <stdlib.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void initializeFile(void);

int main(int argc, char *argv[])
{
    FILE *cfPtr;
    unsigned int choice;

    // create file if it does not exist
    initializeFile();

    // open file for reading and writing
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr);

    return 0;
}

// initialize file with 100 blank records
void initializeFile(void)
{
    FILE *cfPtr;

    struct clientData blankClient = {0, "", "", 0.0};

    // try opening existing file
    if ((cfPtr = fopen("credit.dat", "rb")) != NULL)
    {
        fclose(cfPtr);
        return;
    }

    // create new file
    if ((cfPtr = fopen("credit.dat", "wb")) == NULL)
    {
        puts("File could not be created.");
        exit(EXIT_FAILURE);
    }

    // write 100 blank records
    for (int i = 0; i < 100; i++)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
    }

    fclose(cfPtr);
}

// create formatted text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s%-16s%-11s%10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance");

    // corrected fread loop
    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    puts("accounts.txt created successfully.");
}

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");

    if (scanf("%u", &account) != 1)
    {
        puts("Invalid input.");
        return;
    }

    if (account < 1 || account > 100)
    {
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
    }
    else
    {
        printf("%-6u%-16s%-11s%10.2f\n\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        printf("Enter charge (+) or payment (-): ");

        if (scanf("%lf", &transaction) != 1)
        {
            puts("Invalid transaction.");
            return;
        }

        client.balance += transaction;

        printf("%-6u%-16s%-11s%10.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        // move back one record
        fseek(fPtr,
              -((long)sizeof(struct clientData)),
              SEEK_CUR);

        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);
    }
}

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");

    if (scanf("%u", &accountNum) != 1)
    {
        puts("Invalid input.");
        return;
    }

    if (accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
    }
    else
    {
        fseek(fPtr,
              (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&blankClient,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Account %u deleted.\n", accountNum);
    }
}

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");

    if (scanf("%u", &accountNum) != 1)
    {
        puts("Invalid input.");
        return;
    }

    if (accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n",
               client.acctNum);
    }
    else
    {
        printf("Enter lastname firstname balance\n? ");

        scanf("%14s %9s %lf",
              client.lastName,
              client.firstName,
              &client.balance);

        client.acctNum = accountNum;

        fseek(fPtr,
              (client.acctNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Account created successfully.\n");
    }
}

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n? ");

    scanf("%u", &menuChoice);

    return menuChoice;
}
