#include "header.h"

#define MAX_RECORDS 100
#define MAX_USERNAME_SIZE 50
#define MAX_PASSWORD_SIZE 50
#define MAX_ID_SIZE 5
#define MAX_COUNTRY_SIZE 100
#define MAX_TRANSACTION_TYPE_SIZE 10

const char *RECORDS = "./data/records.txt";

int getAccountFromFile(FILE *ptr, char name[MAX_USERNAME_SIZE],
                       struct Record *r)
{
  return fscanf(ptr, "%d %d %s %d %d/%d/%d %s %d %lf %s", &r->id, &r->userId,
                name, &r->accountNbr, &r->deposit.month, &r->deposit.day,
                &r->deposit.year, r->country, &r->phone, &r->amount,
                r->accountType) != EOF;
}

void saveAccountToFile(FILE *ptr, struct User u, struct Record r)
{
  fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n", r.id, u.id, u.name,
          r.accountNbr, r.deposit.month, r.deposit.day, r.deposit.year,
          r.country, r.phone, r.amount, r.accountType);
}

void updateUserAccountInFile(FILE *ptr, struct Record r)
{
  fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n", r.id, r.userId,
          r.name, r.accountNbr, r.deposit.month, r.deposit.day, r.deposit.year,
          r.country, r.phone, r.amount, r.accountType);
}

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
  int option;
  do
  {
    if (notGood == 0)
    {
      printf("\n\t\tEnter 0 to try again, 1 to return to main menu and 2 to "
             "exit! \n\n");
    }
    else
    {
      printf("\n\t\tEnter 1 to go to the main menu and 0 to exit! \n\n");
    }

    scanf("%d", &option);

    if (option == 0)
    {
      f(u);
      break;
    }
    else if (option == 1)
    {
      mainMenu(u);
      break;
    }
    else if (option == 2)
    {
      exit(0);
    }
    else
    {
      printf("Insert a valid operation!\n");
    }
  } while (option < 0 || option > 2);
}

void success(struct User u)
{
  int option;

  while (1)
  {
    printf("\n\t\tEnter 1 to go to the main menu and 0 to exit! \n\n");
    scanf("%d", &option);
    system("clear");

    if (option == 1)
    {
      mainMenu(u);
      break;
    }
    else if (option == 0)
    {
      exit(1);
    }
    else
    {
      printf("\n\t\tInsert a valid operation!\n");
    }
  }
}

int getUserId(const char *username)
{
  FILE *fp = fopen("./data/users.txt", "r");
  if (!fp)
  {
    perror("\n\t\tError opening file");
    exit(1);
  }

  char id[MAX_ID_SIZE], name[MAX_USERNAME_SIZE], pass[MAX_PASSWORD_SIZE];
  while (fscanf(fp, "%s %s %s", id, name, pass) != EOF)
  {
    if (strcmp(name, username) == 0)
    {
      fclose(fp);
      return atoi(id);
    }
  }

  fclose(fp);
  return -1;
}

int doesUserHaveAccounts(struct User u)
{
  struct Record r;
  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return 0;
  }
  char userName[MAX_USERNAME_SIZE];
  while (getAccountFromFile(pf, userName, &r))
  {
    if (strcmp(userName, u.name) == 0)
    {
      fclose(pf);
      return 1;
    }
  }
  fclose(pf);
  return 0;
}

void createNewAccount(struct User u)
{
  struct Record r;
  struct Record cr;
  char userName[MAX_USERNAME_SIZE];
  FILE *pf = fopen(RECORDS, "a+");
  FILE *rf = fopen(RECORDS, "r");

  if (!rf)
  {
    perror("\n\t\tError opening file");
    exit(1);
  }

  int lastRecordId = 0;
  while (fscanf(rf, "%d %*d %*s %*d %*s %*s %*d %*lf %*s", &r.id) != EOF)
  {
    lastRecordId = r.id;
  }
  fclose(rf);
  do
  {
    system("clear");
    printf("\t\t\t===== New record =====\n");
    int validDate = 0;
    while (!validDate)
    {
    loop:
      printf("\n\t\tEnter today's date(mm/dd/yyyy): ");
      scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);

      if (isValidDate(r.deposit.month, r.deposit.day, r.deposit.year))
      {
        validDate = 1;
      }
      else
      {
        int option;
        int choice = 2;
        while (choice != 0)
        {
          printf("\n\t\tInvalid date format or values. Please try again.\n");
          printf("\n\t\t1. Try again\n");
          printf("\t\t2. Exit\n");
          if (scanf("%d", &option) != 1)
          {
            while (getchar() != '\n')
              ;
            continue;
          }
          if (option == 1)
          {
            goto loop;
          }
          else if (option == 2)
          {
            mainMenu(u);
            break;
          }
          choice--;
        }
        if (choice == 0)
        {
          printf("\n\t\tTry again later.\n");
          exit(1);
        }
      }
    }

    printf("\n\t\tEnter the account number: ");
    scanf("%d", &r.accountNbr);

    int accountExists = 0;

    rewind(pf);

    while (getAccountFromFile(pf, userName, &cr))
    {
      if (strcmp(userName, u.name) == 0 && cr.accountNbr == r.accountNbr)
      {
        printf("\n\t\t✖ This Account already exists for this user\n");
        accountExists = 1;
        break;
      }
    }

    if (accountExists)
    {
      fclose(pf);
      stayOrReturn(0, createNewAccount, u);
    }
    else
    {
      r.id = lastRecordId + 1;
      u.id = getUserId(u.name);
      printf("\n\t\tEnter the country: ");
      scanf("%s", r.country);

      printf("\n\t\tEnter the phone number: ");
      scanf("%d", &r.phone);

      printf("\n\t\tEnter amount to deposit: $");
      scanf("%lf", &r.amount);

      printf("\n\t\tChoose the type of account:\n\n\t\t -> savings\n\t\t -> "
             "current\n\t\t -> fixed01(for 1 year)\n\t\t -> fixed02(for 2 "
             "years)\n\t\t -> "
             "fixed03(for 3 "
             "years)\n\n\t\tEnter your choice: ");
      scanf("%s", r.accountType);

      saveAccountToFile(pf, u, r);

      fclose(pf);
      printf("\n\t\t✔ Success!\n");
      success(u);
      break;
    }
  } while (1);
}

void checkAllAccounts(struct User u)
{
  if (!doesUserHaveAccounts(u))
  {
    system("clear");
    printf("\n\t\tNo accounts found for %s. Returning to main menu.\n", u.name);
    stayOrReturn(1, checkAllAccounts, u);
    return;
  }

  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  int accountsFound = 0;
  struct Record r;
  char userName[MAX_USERNAME_SIZE];

  system("clear");
  printf("\t\t====== All accounts for %s =====\n\n", u.name);

  while (getAccountFromFile(pf, userName, &r))
  {
    if (strcmp(userName, u.name) == 0)
    {
      accountsFound = 1;
      printf("\t\t_____________________\n");
      printf(
          "\n\t\tAccount number: %d\n\t\tDeposit date: %d/%d/%d\n\t\tCountry: "
          "%s \n\t\tPhone number: %d \n\t\tAmount deposited: $%.2f \n\t\tType "
          "of account: %s\n",
          r.accountNbr, r.deposit.day, r.deposit.month, r.deposit.year,
          r.country, r.phone, r.amount, r.accountType);
    }
  }

  fclose(pf);

  if (accountsFound == 0)
  {
    printf("\t\tNo accounts found for %s.\n", u.name);
    stayOrReturn(1, checkAllAccounts, u);
    return;
  }

  success(u);
}

void updateAccountInformation(struct User u)
{
  if (!doesUserHaveAccounts(u))
  {
    system("clear");
    printf("\n\t\tNo accounts found for %s. Returning to main menu.\n", u.name);
    stayOrReturn(1, updateAccountInformation, u);
    return;
  }

  int accountNbr, choice;
  char newCountry[MAX_COUNTRY_SIZE];
  int newPhoneNumber;
  int found = 0;

  system("clear");

  printf("\t\t====== Update accounts for %s =====\n\n", u.name);
  printf("\n\t\tEnter the account number you wish to update: ");
  scanf("%d", &accountNbr);

  struct Record records[MAX_RECORDS];
  int recordCount = 0;

  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  while (getAccountFromFile(pf, records[recordCount].name,
                            &records[recordCount]))
  {
    if (records[recordCount].accountNbr == accountNbr &&
        strcmp(records[recordCount].name, u.name) == 0)
    {
      found = 1;
    }
    recordCount++;
  }
  fclose(pf);

  if (!found)
  {
    printf("\n\t\tNo account found with account number %d.\n", accountNbr);
    stayOrReturn(0, updateAccountInformation, u);
    return;
  }

  printf("\n\t\tWhich field do you want to update?\n\n\t\t 1. Phone "
         "number\n\n\t\t 2. Country\n\n\t\tEnter choice (1/2): ");
  scanf("%d", &choice);

  switch (choice)
  {
  case 1:
    printf("\n\t\tEnter new phone number: ");
    scanf("%d", &newPhoneNumber);
    for (int i = 0; i < recordCount; i++)
    {
      if (records[i].accountNbr == accountNbr &&
          strcmp(records[i].name, u.name) == 0)
      {
        records[i].phone = newPhoneNumber;
      }
    }
    break;
  case 2:
    printf("\n\t\tEnter new country: ");
    scanf("%s", newCountry);
    for (int i = 0; i < recordCount; i++)
    {
      if (records[i].accountNbr == accountNbr &&
          strcmp(records[i].name, u.name) == 0)
      {
        strcpy(records[i].country, newCountry);
      }
    }
    break;
  default:
    printf("\n\t\tInvalid choice. Returning to main menu.\n");
    mainMenu(u);
    return;
  }

  pf = fopen(RECORDS, "w");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  for (int i = 0; i < recordCount; i++)
  {
    updateUserAccountInFile(pf, records[i]);
  }
  fclose(pf);

  printf("\n\t\t✔ Account information updated successfully.\n");
  success(u);
}

void checkAccountDetails(struct User u)
{
  if (!doesUserHaveAccounts(u))
  {
    system("clear");
    printf("\n\t\tNo accounts found for %s. Returning to main menu.\n", u.name);
    stayOrReturn(1, checkAccountDetails, u);
    return;
  }

  int accountNbr;
  struct Record r;
  char userName[MAX_USERNAME_SIZE];
  int found = 0;

  system("clear");
  printf("\t\t====== Check Account Details for %s =====\n\n", u.name);
  printf("\n\t\tEnter the account number you wish to check: ");
  scanf("%d", &accountNbr);

  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  while (getAccountFromFile(pf, userName, &r))
  {
    if (r.accountNbr == accountNbr && strcmp(userName, u.name) == 0)
    {
      found = 1;
      break;
    }
  }
  fclose(pf);

  if (!found)
  {
    printf("\n\t\tNo account found with account number %d.\n", accountNbr);
    stayOrReturn(0, checkAccountDetails, u);
    return;
  }

  printf("\n\t\tAccount number: %d\n\t\tDeposit date: %d/%d/%d\n\t\tCountry: "
         "%s \n\t\tPhone number: %d \n\t\tAmount deposited: $%.2f \n\t\tType "
         "of account: %s\n",
         r.accountNbr, r.deposit.day, r.deposit.month, r.deposit.year,
         r.country, r.phone, r.amount, r.accountType);

  double rate;
  if (strcmp(r.accountType, "savings") == 0)
  {
    rate = 0.07;
    double interest = r.amount * (1 + rate / 12) - r.amount;
    printf("\n\t\tYou will get $%.2f as interest on day %d of every month.\n",
           interest, r.deposit.day);
  }
  else if (strcmp(r.accountType, "fixed01") == 0)
  {
    rate = 0.04;
    double interest = r.amount * (1 + rate / 12) - r.amount;
    interest *= 12;
    printf("\n\t\tYou will get $%.2f as interest on %d/%d/%d.\n", interest,
           r.deposit.month, r.deposit.day, r.deposit.year + 1);
  }
  else if (strcmp(r.accountType, "fixed02") == 0)
  {
    rate = 0.05;
    double interest = r.amount * (1 + rate / 12) - r.amount;
    interest *= 24;
    printf("\n\t\tYou will get $%.2f as interest on %d/%d/%d.\n", interest,
           r.deposit.month, r.deposit.day, r.deposit.year + 2);
  }
  else if (strcmp(r.accountType, "fixed03") == 0)
  {
    rate = 0.08;
    double interest = r.amount * (1 + rate / 12) - r.amount;
    interest *= 36;
    printf("\n\t\tYou will get $%.2f as interest on %d/%d/%d.\n", interest,
           r.deposit.month, r.deposit.day, r.deposit.year + 3);
  }
  else if (strcmp(r.accountType, "current") == 0)
  {
    printf("\n\t\tYou will not get interests because the account is of type "
           "current\n");
  }
  else
  {
    printf("\n\t\tInvalid account type.\n");
    stayOrReturn(0, checkAccountDetails, u);
    return;
  }
  success(u);
}

void makeTransaction(struct User u)
{
  if (!doesUserHaveAccounts(u))
  {
    system("clear");
    printf("\n\t\tNo accounts found for %s. Returning to main menu.\n", u.name);
    stayOrReturn(1, makeTransaction, u);
    return;
  }

  int accountNbr;
  double amount;
  char transactionType[MAX_TRANSACTION_TYPE_SIZE];

  int found = 0;

  system("clear");
  printf("\t\t====== Make Transaction for %s =====\n\n", u.name);

  printf("\n\t\tEnter the account number for the transaction: ");
  scanf("%d", &accountNbr);

  struct Record records[MAX_RECORDS];
  int recordCount = 0;

  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  while (getAccountFromFile(pf, records[recordCount].name,
                            &records[recordCount]))
  {
    if (records[recordCount].accountNbr == accountNbr &&
        strcmp(records[recordCount].name, u.name) == 0)
    {
      found = 1;
    }
    recordCount++;
  }
  fclose(pf);

  if (!found)
  {
    printf("\n\t\t✖ No account found with account number %d.\n", accountNbr);
    stayOrReturn(0, makeTransaction, u);
    return;
  }

  char *restrictedAccountTypes[] = {"fixed01", "fixed02", "fixed03"};
  for (int i = 0; i < recordCount; i++)
  {
    if (records[i].accountNbr == accountNbr &&
        strcmp(records[i].name, u.name) == 0)
    {

      for (int j = 0; j < 3; ++j)
      {
        if (strcmp(records[i].accountType, restrictedAccountTypes[j]) == 0)
        {
          printf("\n\t\t✖ Accounts of type %s are not allowed to make "
                 "transactions.\n",
                 records[i].accountType);
          stayOrReturn(0, makeTransaction, u);
          return;
        }
      }
      break;
    }
  }

  printf("\n\t\tEnter the transaction type (deposit/withdraw): ");
  scanf("%s", transactionType);

  printf("\n\t\tEnter the amount: ");
  scanf("%lf", &amount);

  for (int i = 0; i < recordCount; i++)
  {
    if (records[i].accountNbr == accountNbr &&
        strcmp(records[i].name, u.name) == 0)
    {
      if (strcmp(transactionType, "withdraw") == 0)
      {
        if (records[i].amount < amount)
        {
          printf("\n\t\t✖ Not enough balance for withdrawal.\n");
          stayOrReturn(0, makeTransaction, u);
          return;
        }
        records[i].amount -= amount;
      }
      else if (strcmp(transactionType, "deposit") == 0)
      {
        records[i].amount += amount;
      }
      else
      {
        printf("\n\t\t✖ Invalid transaction type.\n");
        stayOrReturn(0, makeTransaction, u);
        return;
      }
      break;
    }
  }

  pf = fopen(RECORDS, "w");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  for (int i = 0; i < recordCount; i++)
  {
    updateUserAccountInFile(pf, records[i]);
  }
  fclose(pf);

  printf("\n\t\t✔ Transaction successful.\n");
  success(u);
}

void removeAccount(struct User u)
{
  if (!doesUserHaveAccounts(u))
  {
    system("clear");
    printf("\n\t\tNo accounts found for %s. Returning to main menu.\n", u.name);
    stayOrReturn(1, removeAccount, u);
    return;
  }

  int accountNbr;
  struct Record records[MAX_RECORDS];
  int recordCount = 0;
  int found = 0;

  system("clear");
  printf("\t\t====== Remove Account for %s =====\n\n", u.name);
  printf("\n\t\tEnter the account number you wish to remove: ");
  scanf("%d", &accountNbr);

  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    exit(1);
  }

  while (getAccountFromFile(pf, records[recordCount].name,
                            &records[recordCount]))
  {
    if (records[recordCount].accountNbr == accountNbr &&
        strcmp(records[recordCount].name, u.name) == 0)
    {
      found = 1;
    }
    recordCount++;
  }
  fclose(pf);

  if (!found)
  {
    printf("\n\t\tNo account found with account number %d.\n", accountNbr);
    stayOrReturn(0, removeAccount, u);
    return;
  }

  for (int i = 0; i < recordCount; i++)
  {
    if (records[i].accountNbr == accountNbr &&
        strcmp(records[i].name, u.name) == 0)
    {
      for (int j = i; j < recordCount - 1; j++)
      {
        records[j] = records[j + 1];
      }
      recordCount--;
      break;
    }
  }

  pf = fopen(RECORDS, "w");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    exit(1);
  }

  int newId = 0;
  for (int i = 0; i < recordCount; i++)
  {
    records[i].id = newId++;
    updateUserAccountInFile(pf, records[i]);
  }
  fclose(pf);

  printf("\n\t\t✔ Account removed successfully.\n");
  success(u);
}

void transferOwnership(struct User u)
{
  if (!doesUserHaveAccounts(u))
  {
    system("clear");
    printf("\n\t\tNo accounts found for %s. Returning to main menu.\n", u.name);
    stayOrReturn(1, transferOwnership, u);
    return;
  }

  int accountNbr, newUserId;
  char newUserName[MAX_USERNAME_SIZE];
  struct Record records[MAX_RECORDS];
  int recordCount = 0;
  int found = 0;

  system("clear");
  printf("\t\t====== Transfer Ownership for %s =====\n\n", u.name);

  printf("\n\t\tEnter the account number you want to transfer: ");
  scanf("%d", &accountNbr);

  FILE *pf = fopen(RECORDS, "r");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  while (getAccountFromFile(pf, records[recordCount].name, &records[recordCount]))
  {
    if (records[recordCount].accountNbr == accountNbr &&
        strcmp(records[recordCount].name, u.name) == 0)
    {
      found = 1;
    }
    recordCount++;
  }
  fclose(pf);

  if (!found)
  {
    printf("\n\t\tNo account found with account number %d.\n", accountNbr);
    stayOrReturn(0, transferOwnership, u);
    return;
  }

  printf("\n\t\tEnter the username of the new owner: ");
  scanf("%s", newUserName);
  newUserId = getUserId(newUserName);

  if (newUserId == -1)
  {
    printf("\n\t\tNew owner not found.\n");
    stayOrReturn(0, transferOwnership, u);
    return;
  }

  for (int i = 0; i < recordCount; i++)
  {
    if (records[i].accountNbr == accountNbr &&
        strcmp(records[i].name, u.name) == 0)
    {
      records[i].userId = newUserId;
      strcpy(records[i].name, newUserName);
      strcpy(records[i].country, "N/A");
      records[i].phone = 0;
      break;
    }
  }

  pf = fopen(RECORDS, "w");
  if (pf == NULL)
  {
    perror("\n\t\tFailed to open file");
    return;
  }

  for (int i = 0; i < recordCount; i++)
  {
    updateUserAccountInFile(pf, records[i]);
  }
  fclose(pf);

  printf("\n\t\t✔ Ownership successfully transferred.\n");
  success(u);
}

bool checkValid(char str[])
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || str[i] == ' '))
    {
      return false;
    }
  }
  return true;
}

bool isNumber(int num)
{
  if (num >= 0 && num <= 9)
  {
    return true;
  }
  return false;
}

int isValidDate(int month, int day, int year)
{
  if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31)
    return 0;
  if (month == 2)
    return (day <= 29);
  if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
    return 0;
  return 1;
}