/**************************************************************
 *
 * CIS 2206 Practice Activity #1 - Payroll System using C
 * Created by: Team 1
 * Members: C. Stewart, J. Leano, J. Samson,  J. Rosales, J. Tejada, I. Monzales,
 *           J. Guzman, E. Celdran, I. Woogue, M. Valeros, W. Dayata 
 * Date: March 14-29, 2022
 *
 * ************************************************************/

/***** HEADER FILES TO INCLUDE *****/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>

/***** CONSTANTS *****/
#define SIZE 512 // hash table size
#define EMP_FILENAME "_EmployeeFile.txt"
#define PAY_FILENAME "_PayrollFile.bin"

/* Note: Actual file names will be '<CompanyName>_EmployeeFile.bin'.
 * Company name will be inputted at main menu upon program and data initialization.
 */

/***** DATA STRUCTURE DEFINITION *****/
typedef struct
{
    char LName[16]; // last name
    char fName[24]; // first name
    char MI;        // middle initial
} nameDetails;

typedef enum
{
    ACTIVE,  // only active employees will be prompted attendance entry
    INACTIVE // in the specified group for each new payroll entry
} empStatus;

typedef struct
{
    unsigned int year : 7;  // YY
    unsigned int month : 4; // MM
    unsigned int day : 5;   // DD
} dateDetails;              // CONSTRAINT: No negative values

typedef struct
{
    char email[32]; // must have '@' and '.' for valid email
    char phone[12]; // 11-digit mobile number
} contactDetails;

typedef struct
{
    float basicSalary;        // basic monthly salary, in PHP
    float overtimeHourlyRate; // overtime monthly pap, in PHP
} paymentDetails;             // CONSTRAINT: No negative values

/* Payroll ID is YYMMPG - Year, Month, Period, Group #
 * Example:      22011A - Period 1, January 2022, Group A
 * Note: A period of 3 indicates 13th month pay. A dedicated function has been made for its computation
 */

typedef struct
{
    char payrollID[7];   // payroll ID  (CONSTRAINT: See format above)
    char empID[8];       // employee ID (CONSTRAINT: Must match an existing employee ID)
    int daysAbsent;      // number of days absent for the period
    int hoursOvertime;   // overtime duration for the period, in hours
    int hoursUndertime;  // undertime duration for the period, in hours
    float basicSalary;   // basic salary used for the computations
    float grossIncome;   // for the period
    float contributions; // applicable only ro period 2
    float taxableIncome; // applicable only to period 2
    float withTax;       // applicable only to period 2, withholding tax
    float netSalary;     // the net salary -- period 1 -> same as gross income
                         //                   period 2 -> gross income minus monthly deductions
    /*In view specific employee's priliminary displays are payroll ID and Net Salary only*/
} payrollDetail, attendanceDetails; // the structure to be written in the 'PayrollFile.bin'

/* Linked list for attendance details */
typedef struct cell
{
    payrollDetail payrollInfo;
    struct cell *link;
} cellType, *attendanceHistory, *List;

typedef struct
{
    char group;
    int numOfEmployees;
} employeesInGroup;

/* Employee structure, with attendance details LL pointer */
typedef struct
{
    char empID[8]; // employee ID (7 characters)
                   // for hash table: Dummy values - "EMPTY" and "DELETED"
    nameDetails name;
    contactDetails contact;
    dateDetails dateEmployed;
    empStatus status;
    char department;
    paymentDetails details;
} employeeDetails; // the structure to be written in the 'EmployeeFile.bin'

typedef struct
{
    employeeDetails employee;  // employee details
    attendanceHistory history; // head pointer of the attendance linked list
} employeeInfo;

/* Hash table of employees - CLOSED HASHING WITH MAX BUCKETS, WITH LINEAR PROBING
 *  I chose closed hashing ra para sayun ma clear ang memory by avoiding dynamic cells,
    since when we terminate a program we have to free all dynamic memory
 */
typedef employeeInfo employeeTable[SIZE]; // constant hash table

typedef struct node
{
    int index;
    struct node *link;
} nodeType, *employeeList[26], *List2; // the structure for sorting the employees by department

/***** FUNCTION PROTOTYPES *****/
void initialize(employeeTable empTable, char companyName[]);
int initEmpList(employeeTable empTable, char companyName[]);
int initAttendanceList(employeeTable empTable, char companyName[]);

int hash(char empID[]);
int insertEmployee(employeeTable empTable, employeeInfo emp);
int searchEmployee(employeeTable empTable, char empID[]);
int insertAttendance(employeeTable empTable, attendanceDetails att);
int insertEmployeeToFile(char companyName[], employeeInfo emp);
int insertAttendanceToFile(char companyName[], attendanceDetails att);

int assignEmployeeID(employeeTable empTable, employeeInfo *emp);
int nameValidation(char name[]);
int dateValidation(int month, int day, int year);
int emailValidation(char email[]);
int phoneValidation(char phone[]);
int payValidation(float amount);
int addEmployee(employeeTable empTable, char companyName[]);

void viewEmployeeList(employeeTable empTable);                                      // view entire list of employees
void sortEmployees(employeeTable empTable, employeeList listEmployee);              // sort employees by department
int displayEmployees(employeeTable empTable, employeeList listEmployee, char dept); // display employee given by a department
void clearEmployeeLL(employeeList listEmployee);

void viewEmployee(employeeTable empTable);
int displayAdvancedSalaryInfo(attendanceHistory head, int numEntries)

void createPayroll(employeeTable empTable, char companyName[]);
int getIntInput(char question[]);
int askPayrollPeriodDetails(char *group, int payrollDate[]);
char* generatePayrollID(char group, int payrollDate[])
void inputAttendanceInfo(employeeDetails empTemp, payrollDetail *payTemp);
float computeGrossIncome(paymentDetails payDetails, payrollDetail *attendance, int payrollDate[])
float getFirstGrossIncome(employeeTable empTable, char empID[], char payrollIDFirstPeriod[]);
float computeSSS(float income);
float computePHealth(float income);
float computeContributions(float income);
float computeWithTax(float salary);
float computeNetSalary(float grossIncome, payrollDetail *ptrPd);
float compute13thMonthPay(attendanceHistory head, int year);
int addPayrollToFile(char companyName[], payrollDetail pd)

int editEmployee(employeeTable empTable, char companyName[]);
int updateEmployee(employeeTable empTable, employeeDetails newEmployee, int index);
int updateEmployeeToFile(char companyName[], employeeDetails newEmployee);
int updateAttendanceRecord(attendanceHistory history, attendanceDetails newRecord);
int updateAttendanceToFile(char companyName[], attendanceDetails newRecord);

/*End of initialization function Protypes */

/***** main() function - Handles the main menu and calls the subfunction *****/
int main()
{

    /* Employee hash table declaration */
    employeeTable empTable;
    char companyName[32];

    /* Variable declarations */
    char choice; // for switch statement main menu
    int status;  // to check if operation was successful

    printf("\n===================================================");
    printf("\n|                     Welcome!                    |");
    printf("\n|            CIS 2206 - PAYROLL SYSTEM            |");
    printf("\n|             Version 1.0 - 3/21/2022             |");
    printf("\n|                                                 |");
    printf("\n| Developers: C. Stewart, J. Leano, J. Samson,    |");
    printf("\n|             J. Rosales, J. Tejada, I. Monzales, |");
    printf("\n|             J. Guzman, E. Celdran, I. Woogue,   |");
    printf("\n|             M. Valeros, W. Dayata               |");
    printf("\n===================================================");

    /*Start of the Program */
    printf("\n\nEnter Company Name: ");
    gets(companyName);
    initialize(empTable, companyName); // ask for input of company name
    /* Body */
    do
    {
        system("cls");
        printf("\n==========================================");
        printf("\n                MAIN MENU                 ");
        printf("\n==========================================");
        printf("\n| [ 1 ]    Add New Employee              |");
        printf("\n| [ 2 ]    View Employee List            |");
        printf("\n| [ 3 ]    View Specific Employee Info   |");
        printf("\n| [ 4 ]    Edit Employee Detais          |");
        printf("\n| [ 5 ]    Create Payroll                |");
        printf("\n| [ 0 ]    Exit                          |");
        printf("\n==========================================");
        printf("\n\nYour Choice: ");

        fflush(stdin);
        scanf("%c", &choice);

        switch (choice)
        {
        case '1':
            addEmployee(empTable, companyName);
            break;
        case '2':
            viewEmployeeList(empTable);
            break;
        case '3':
            viewEmployee(empTable);
            break;
        case '4':
            editEmployee(empTable, companyName);
            break;
        case '5':
            createPayroll(empTable, companyName);
            break;
        case '0':
            printf("\nPress any key to exit program...");
            break;
        default:
            printf("\nInvalid Choice - Please use digits only");
            break;
        }
    } while (choice != '0');

    /* Exit/return statement, if applicable */

    getch();
    return 0;
}

/***** FUNCTION DEFINITIONS *****/

/**
 * @brief initializes the data and loads the file, sets up the internal memory
 * @param - gets pointer of employeeTable to initialize with the company name
 * @return - implicit return
 */
void initialize(employeeTable empTable, char companyName[])
{
    int i; // counter
    int empCount, attCount;
    int exit = 0;
    char newChoice;
    char fileName[40];
    FILE *fp;

    for (i = 0; i < SIZE; i++) // loop for hash table initialization
    {
        strcpy(empTable[i].employee.empID, "EMPTY"); // sets each employee ID to EMPTY
        empTable[i].history = NULL;                  // sets each head pointer to NULL
    }

    while (exit == 0)
    {
        empCount = initEmpList(empTable, companyName);
        attCount = initAttendanceList(empTable, companyName);

        if (empCount == -1)
        {
            printf("\nNo files found. Proceed to create new files?");
            printf("\n[ 1 ] Yes");
            printf("\n[ 2 ] No");
            printf("\n\nChoice: ");
            do
            {
                fflush(stdin);
                scanf("%c", &newChoice);
                switch (newChoice)
                {
                case '1':
                    strcpy(fileName, companyName);
                    strcat(fileName, EMP_FILENAME);
                    fp = fopen(fileName, "wb");
                    if (fp != NULL)
                    {
                        fclose(fp);
                        strcpy(fileName, "");
                        strcpy(fileName, companyName);
                        strcat(fileName, PAY_FILENAME);
                        fp = fopen(fileName, "wb");
                        if (fp != NULL)
                        {
                            fclose(fp);
                            printf("\nFiles successfully created.");
                            exit = 1;
                        }
                    }
                    break;
                case '2':
                    printf("\nEnter company name: ");
                    fflush(stdin);
                    gets(companyName);
                    break;
                default:
                    printf("\nInvalid choice. Please try again.");
                }
            } while (newChoice != '1' && newChoice != '2');
        }
        else
        {
            printf("\nFiles loaded. %d employee and %d attendance entries found.", empCount, attCount);
            exit = 1;
        }
    }

    printf("\nPress any key to proceed to main menu.");
    getch();
}

/**
 * @brief fetches data from the employee file and loads to the hash table
 * @param - gets pointer of employeeTable and name of company
 * @return - returns the number of records read and -1 if no file found
 */
int initEmpList(employeeTable empTable, char companyName[])
{
    FILE *fp;
    employeeInfo catcher;
    catcher.history = NULL;
    int count = 0;
    char fileName[40];

    strcpy(fileName, companyName);
    strcat(fileName, EMP_FILENAME);
    fp = fopen(fileName, "rb");

    if (fp != NULL)
    { // Read each record and insert to hash table
        while (fread(&catcher.employee, sizeof(employeeDetails), 1, fp) != 0)
        {
            count++;
            insertEmployee(empTable, catcher);
        }
        fclose(fp);
    }
    else
    { // Means that company is new and not yet created.
        count = -1;
    }
    return count;
}

/**
 * @brief fetches data from the attendance file and loads to the LL portion of the hash table
 * @param - gets pointer of employeeTable and name of company
 * @return - returns the number of records read and -1 if no file found
 */
int initAttendanceList(employeeTable empTable, char companyName[])
{
    FILE *fp;
    attendanceDetails catcher;
    int count = 0;
    char fileName[40];

    strcpy(fileName, companyName);
    strcat(fileName, PAY_FILENAME);

    fp = fopen(fileName, "rb");
    if (fp != NULL)
    { // Means that company is new and not yet created.
        while (fread(&catcher, sizeof(attendanceDetails), 1, fp) != 0)
        {
            count++;
            insertAttendance(empTable, catcher);
        }
        fclose(fp);
    }
    else
    {
        // Means that company is new and not yet created.
        count = -1;
        // fp = fopen(fileName, "wb");
        // fclose(fp);
    }
    return count;
}

/**
 * @brief multiplication hashing method
 * @param employee id
 * @return returns the hash value
 */
int hash(char empID[])
{
    int i;
    unsigned long sum = 1;
    for (i = 0; i < 7; i++)
    {
        sum += sum * empID[i];
    }
    return sum % SIZE;
}

/**
 * @brief inserts employee to the hash table
 * @param hash table and employee structure
 * @return returns 1 if successful and 0 if unsuccessful
 */
int insertEmployee(employeeTable empTable, employeeInfo emp)
{
    int ctr, index, retVal = 0;
    for (ctr = 0, index = hash(emp.employee.empID); ctr < SIZE; ctr++)
    {
        if (strcmp(empTable[index].employee.empID, "EMPTY") == 0 || strcmp(empTable[index].employee.empID, "DELETED") == 0)
        {
            empTable[index] = emp;
            break;
        }
        index = (index + 1) % SIZE;
    }
    return (ctr < SIZE) ? 1 : 0;
}

/**
 * @brief checks the position of a given employee in the hash table
 * @param hash table and the employee id
 * @return returns the index of the employee table where it exists and -1 if does not exist
 */
int searchEmployee(employeeTable empTable, char empID[])
{
    int ctr, index;
    for (ctr = 0, index = hash(empID); ctr < SIZE && strcmp(empTable[index].employee.empID, empID) != 0; ctr++)
    {
        if (strcmp(empTable[index].employee.empID, "EMPTY") == 0)
        {
            ctr = SIZE;
        }
        index = (index + 1) % SIZE;
    }
    return ctr < SIZE ? index : -1;
}

/**
 * @brief inserts attendance record to the hash table
 * @param hash table and attendance details structure
 * @return returns 1 if successful and 0 if unsuccessful
 */
int insertAttendance(employeeTable empTable, attendanceDetails att)
{
    int retVal = 0;
    List *ptr, temp;
    int index = searchEmployee(empTable, att.empID);
    if (index != -1)
    {
        // traverse linked list (sorted in increasing order)
        for (ptr = &empTable[index].history; *ptr != NULL && strcmp((*ptr)->payrollInfo.payrollID, att.payrollID) < 0; ptr = &(*ptr)->link)
        {
        }
        temp = (List)malloc(sizeof(struct cell));
        if (temp != NULL)
        {
            temp->payrollInfo = att;
            temp->link = *ptr;
            *ptr = temp;
            retVal = 1;
        }
    }
    return retVal;
}

/**
 * @brief appends new employee record to the file
 * @param company name and employee structure
 * @return returns 1 if successful and 0 if unsuccessful
 */
int insertEmployeeToFile(char companyName[], employeeInfo emp)
{
    int retVal = 0;
    FILE *fp;
    employeeDetails newEmployee;
    char fileName[40];

    strcpy(fileName, companyName);
    strcat(fileName, EMP_FILENAME);
    newEmployee = emp.employee; // datatype conversion, removing LL pointer

    fp = fopen(fileName, "ab");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        fwrite(&newEmployee, sizeof(employeeDetails), 1, fp);
        fclose(fp);
        retVal = 1;
    }
    return retVal;
}

/**
 * @brief appends new attendance record to the file
 * @param company name and employee structure
 * @return returns 1 if successful and 0 if unsuccessful
 */
int insertAttendanceToFile(char companyName[], attendanceDetails att)
{
    int retVal = 0;
    FILE *fp;
    char fileName[40];

    strcpy(fileName, companyName);
    strcat(fileName, PAY_FILENAME);

    fp = fopen(fileName, "ab");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        fwrite(&att, sizeof(attendanceDetails), 1, fp);
        fclose(fp);
        retVal = 1;
    }
    return retVal;
}

/**
 * @brief assigns a newly created employee an id
 * @param hash table and the newly created employee
 * @return returns 1 if successful and 0 if unsuccessful
 */
int assignEmployeeID(employeeTable empTable, employeeInfo *emp)
{
    int i;
    char newID[8] = {0}, year[3], month[3], empNum[4];      /* Employee ID format = year(2) month(2) increment(3) ex. 2203001 */
    sprintf(year, "%02d", emp->employee.dateEmployed.year); /* converts int to string with leading zeroes */
    sprintf(month, "%02d", emp->employee.dateEmployed.month);
    for (i = 1; i < 1000; i++)
    {
        memset(newID, 0, 8); /* sets string to empty */
        strcat(newID, year); /* catenate strings in order for id format */
        strcat(newID, month);
        sprintf(empNum, "%03d", i);
        strcat(newID, empNum);
        if (searchEmployee(empTable, newID) == -1) /* checks if id is unique */
        {
            strcpy(emp->employee.empID, newID);
            break;
        }
    }
    return (i < 1000) ? 1 : 0;
}

/**
 * @brief checks if name is valid (only alphabet characters and spaces)
 * @param name
 * @return returns 1 if valid and 0 if invalid
 */
int nameValidation(char name[])
{
    int x;
    for (x = 0; name[x] != '\0' && (isalpha(name[x]) != 0 || isspace(name[x]) != 0); x++)
    {
    }
    return (x != 0 && name[x] == '\0') ? 1 : 0;
}

/**
 * @brief checks if date is valid
 * @param month, day, and year
 * @return returns 1 if valid and 0 if invalid
 */
int dateValidation(int month, int day, int year)
{
    int retval = 0;
    int daysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year > 0 && year < 100 && day > 0)
    {
        if (month > 0 && month < 13)
        {
            if (month == 2 && year % 4 == 0)
            { /*check if leap year*/
                if (daysOfMonth[month - 1] + 1 >= day)
                {
                    retval = 1;
                }
            }
            else if (daysOfMonth[month - 1] >= day)
            {
                retval = 1;
            }
        }
    }
    return retval;
}

/**
 * @brief checks if email is valid
 * @param email string
 * @return returns 1 if valid and 0 if invalid
 */
int emailValidation(char email[]) /* magic code, sorry daan wayne hahaha */
{
    int emailCheck = 0;
    int i;

    for (i = 0; email[i] != '\0'; i++)
    {
        switch (email[i])
        {
        case '@':
            if (!emailCheck && email[i] == '@' && i != 0)
            { /*check if '@' exists and is preceeded by characters*/
                emailCheck++;
            }
            else if (email[i] == '@')
            { /*check multiple @s*/
                emailCheck = 0;
            }
            break;
        case '.':
            if (email[i] == '.' && email[i + 1] == '\0')
            { /*check if '.' not at end*/
                emailCheck = 0;
            }
            if (emailCheck && email[i] == '.')
            { /*check if '.' exists*/
                emailCheck = (email[i - 1] != '@') ? emailCheck + 1 : 0;
            }
            break;
        }
    }

    return (emailCheck >= 2) ? 1 : 0;
}

/**
 * @brief checks if phone number is valid
 * @param phone string
 * @return returns 1 if valid and 0 if invalid
 */
int phoneValidation(char phone[])
{
    /*Philippine mobile number system - 09xxxxxxxxx */
    int i;
    if (phone[0] == '0' && phone[1] == '9')
    {
        for (i = 0; phone[i] != '\0' && isdigit(phone[i]); i++)
        {
        } /*check if correct length and is num*/
    }
    return (i == 11) ? 1 : 0;
}

/**
 * @brief checks if salary or payment is valid
 * @param amount
 * @return returns 1 if valid and 0 if invalid
 */
int payValidation(float amount)
{
    return (amount > 0) ? 1 : 0; /*check if non-negative*/
}

/**
 * @brief adds employee to the hash table based on user input
 * @param hash table
 * @return returns 1 if successful, 0 if unsuccessful, and -1 if cancelled
 */
int addEmployee(employeeTable empTable, char companyName[])
{
    /* buffer for input validation */
    char dateString[11], emailString[32], contactString[12];
    int month, day, year;
    float basicSalary = 0, overtimePay = 0;

    int validFlag[9] = {0}; /* check if all fields are correctly inputted */
    int retValue = 0;       /* return value */
    int exitFlag = 0;       /* condition if the loop will continue */
    int i;                  /* counter for loops */
    int ch;

    char choice; /* 0-9 = inputs, e = exit, c = create */

    /* initialize empty employee */
    employeeInfo newEmployee =
        {
            .employee =
                {
                    .empID = 0,
                    .name =
                        {
                            .fName = "",
                            .LName = "",
                            .MI = ' '},
                    .contact =
                        {
                            .email = "",
                            .phone = ""},
                    .dateEmployed =
                        {
                            .month = 0,
                            .day = 0,
                            .year = 0},
                    .status = ACTIVE,
                    .details =
                        {
                            .basicSalary = 0,
                            .overtimeHourlyRate = 0}},
            .history = NULL};

    while (!exitFlag) /* Loop until the exit flag is changed */
    {
        system("cls");
        printf("\n==========================================");
        printf("\n            CREATE NEW EMPLOYEE	        ");
        printf("\n==========================================");
        printf("\n[ 1 ] Last Name:              \t%s", newEmployee.employee.name.LName);
        printf("\n[ 2 ] First Name:             \t%s", newEmployee.employee.name.fName);
        printf("\n[ 3 ] MI:                     \t%c.", newEmployee.employee.name.MI);
        printf("\n[ 4 ] Date employed(MM/DD/YY):\t%02d/%02d/%02d", newEmployee.employee.dateEmployed.month, newEmployee.employee.dateEmployed.day, newEmployee.employee.dateEmployed.year);
        printf("\n[ 5 ] Email:                  \t%s", newEmployee.employee.contact.email);
        printf("\n[ 6 ] Contact No.:            \t%s", newEmployee.employee.contact.phone);
        printf("\n[ 7 ] Basic Salary:           \t%.02f", newEmployee.employee.details.basicSalary);
        printf("\n[ 8 ] Overtime Pay:           \t%.02f", newEmployee.employee.details.overtimeHourlyRate);
        printf("\n[ 9 ] Department:             \t%c", newEmployee.employee.department);
        printf("\n[ 0 ] Employee Status:        \t%s", (newEmployee.employee.status) ? "Inactive" : "Active");
        printf("\n==========================================");
        printf("\n[c] Confirm and create | [e] Exit ");
        printf("\n\nChoice: ");
        fflush(stdin);
        scanf("%c", &choice);
        while ((ch = getchar()) != '\n' && ch != EOF)
            ; // To eat up all the characters left behind by scanf call including '\n'.

        switch (choice)
        {
        case '1':
            printf("\nLast Name: ");
            fflush(stdin);
            gets(newEmployee.employee.name.LName);
            if (nameValidation(newEmployee.employee.name.LName))
            {
                validFlag[0] = 1;
            }
            else
            {
                strcpy(newEmployee.employee.name.LName, "");
                printf("Invalid input\n");
                printf("\nPress any key to continue");
                getch();
            }
            break;

        case '2':
            printf("\nFirst Name: ");
            fflush(stdin);
            gets(newEmployee.employee.name.fName);
            if (nameValidation(newEmployee.employee.name.fName))
            {
                validFlag[1] = 1;
            }
            else
            {
                strcpy(newEmployee.employee.name.fName, "");
                printf("Invalid input\n");
                printf("\nPress any key to continue");
                getch();
            }
            break;

        case '3':
            printf("\nMiddle Initial (enter a space if N/A): ");
            fflush(stdin);
            scanf("%c", &newEmployee.employee.name.MI);
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            if (isalpha(newEmployee.employee.name.MI) || isspace(newEmployee.employee.name.MI))
            {
                newEmployee.employee.name.MI = toupper(newEmployee.employee.name.MI);
                validFlag[2] = 1;
            }
            else
            {
                newEmployee.employee.name.MI = ' ';
                printf("Invalid input\n");
                printf("\nPress any key to continue");
                getch();
            }

            break;

        case '4':
            printf("\nDate employed(MM/DD/YY): ");
            fflush(stdin);
            gets(dateString);

            char *token = strtok(dateString, "/"); /* separates date by '/' */
            month = atoi(token);                   /* converts string to int */
            token = strtok(NULL, "/");
            day = atoi(token);
            token = strtok(NULL, "/");
            year = atoi(token);

            if (dateValidation(month, day, year))
            {
                newEmployee.employee.dateEmployed.month = month;
                token = strtok(NULL, "/");
                newEmployee.employee.dateEmployed.day = day;
                token = strtok(NULL, "/");
                newEmployee.employee.dateEmployed.year = year;
                validFlag[3] = 1;
            }
            else
            {
                printf("Invalid input\n");
                printf("\nPress any key to continue");
                getch();
            }

            break;

        case '5':
            printf("\nEmail: ");
            fflush(stdin);
            gets(emailString);
            if (emailValidation(emailString))
            {
                strcpy(newEmployee.employee.contact.email, emailString);
                validFlag[4] = 1;
            }
            else
            {
                printf("Invalid Input");
                printf("\nPress any key to continue");
                getch();
            }

            break;

        case '6':
            printf("\nContact No.: ");
            fflush(stdin);
            gets(contactString);
            if (phoneValidation(contactString))
            {
                strcpy(newEmployee.employee.contact.phone, contactString);
                validFlag[5] = 1;
            }
            else
            {
                printf("Input invalid");
                printf("\nPress any key to continue");
                getch();
            }

            break;

        case '7':
            printf("\nBasic Monthly Salary: ");
            fflush(stdin);
            scanf("%f", &basicSalary);
            if (payValidation(basicSalary))
            {
                newEmployee.employee.details.basicSalary = basicSalary;
                validFlag[6] = 1;
            }
            else
            {
                printf("Input invalid");
                printf("\nPress any key to continue");
                getch();
            }
            break;

        case '8':
            printf("\nOvertime Hourly Pay: ");
            fflush(stdin);
            scanf("%f", &overtimePay);
            if (payValidation(overtimePay))
            {
                newEmployee.employee.details.overtimeHourlyRate = overtimePay;
                validFlag[7] = 1;
            }
            else
            {
                printf("Input invalid - please enter a positive amount");
                printf("\nPress any key to continue");
                getch();
            }
            break;

        case '9':
            printf("\nDepartment [A-Z]: ");
            fflush(stdin);
            scanf("%c", &newEmployee.employee.department);
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            if (isalpha(newEmployee.employee.department))
            {
                newEmployee.employee.department = toupper(newEmployee.employee.department);
                validFlag[8] = 1;
            }
            else
            {
                newEmployee.employee.name.MI = ' ';
                printf("Invalid input. Please enter a letter\n");
                printf("\nPress any key to continue");
                getch();
            }

            break;

        case '0':
            printf("\n[ 0 ] Active");
            printf("\n[ 1 ] Inactive");
            printf("\nStatus: ");
            fflush(stdin);
            scanf("%c", &choice);
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            if (choice == '0' || choice == '1')
            { /*check if input is valid*/
                newEmployee.employee.status = choice - '0';
            }
            else
            {
                printf("Invalid input");
                printf("\nPress any key to continue");
                getch();
            }
            break;

        case 'c':
            for (i = 0; i < 9 && validFlag[i] != 0; i++)
            {
            }
            if (i == 9)
            {
                if (assignEmployeeID(empTable, &newEmployee))
                {
                    if (insertEmployee(empTable, newEmployee) && insertEmployeeToFile(companyName, newEmployee))
                    {
                        printf("\nSuccessfully created employee!");
                        printf("\nYour employee ID is %s", newEmployee.employee.empID);
                        printf("\n\nPress any key to continue");
                        getch();
                        retValue = 1;
                    }
                    else
                    {
                        printf("Employee Table is full\n");
                        printf("\nPress any key to continue");
                        getch();
                        retValue = 0;
                    }
                }
                else
                {
                    printf("Error: Failed to assign Employee ID\n");
                    printf("\nPress any key to continue");
                    getch();
                    retValue = 0;
                }
                exitFlag = 1; /* prompts to exit the loop */
            }
            else
            {
                printf("Missing input at [%d]\n", i + 1);
                printf("\nPress any key to continue");
                getch();
            }
            break;

        case 'e':
            retValue = -1;
            exitFlag = 1; /* prompts to exit the loop with the default return value of 0*/
            break;

        default:
            printf("Not a valid choice!\n");
            printf("\nPress any key to continue");
            getch();
        }
    }

    return retValue;
}

/**
 * @brief - view entire list of employees or by department
 * @param describe the parameters
 * @return state what the function returns
 */
void viewEmployeeList(employeeTable empTable)
{
    system("cls");
    printf("\n==========================================");
    printf("\n            VIEW EMPLOYEE LIST	        ");
    printf("\n==========================================");

    /* Employee linked list declaration and initialization */
    employeeList listEmployee;
    int x;
    char choice, ch, flag = 1;
    int count;

    for (x = 0; x < 26; x++)
    {
        listEmployee[x] = NULL;
    }

    sortEmployees(empTable, listEmployee);

    while (flag)
    {
        printf("\n[ 1 ] Display all employees");
        printf("\n[ 2 ] Display employees of a department");
        printf("\n[ 3 ] Main menu");
        printf("\n\nChoice: ");
        fflush(stdin);
        scanf("%c", &choice);
        switch (choice)
        {
        case '1':
            // display all employees
            flag = 0;
            system("cls");
            printf("\n==========================================");
            printf("\n               EMPLOYEE LIST   	        ");
            printf("\n==========================================");
            count = 0;
            for (ch = 'A'; ch <= 'Z'; ch++)
            {
                count += displayEmployees(empTable, listEmployee, ch);
            }
            printf("\n\nTotal: %d employees", count);
            break;
        case '2':
            // display all employees of a department
            do
            {
                printf("\nPlease enter a department[A-Z] (0 to exit): ");
                fflush(stdin);
                scanf("%c", &ch);
            } while (!(isalpha(ch) != 0 || ch == '0'));
            if (ch != '0' && (count = displayEmployees(empTable, listEmployee, ch)) == 0)
            {
                printf("\nError: No employees found in the selected department [%c]. ", toupper(ch));
            }
            else
            {
                flag = 0;
                printf("\n\nTotal: %d employees", count);
            }
            break;
        case '3':
            clearEmployeeLL(listEmployee);
            return;
            break;
        default:
            printf("Invalid input. Please enter 1 or 2");
            break;
        }
    }
    printf("\n\nPress any key to exit...");
    getch();
    clearEmployeeLL(listEmployee);
}

/**
 * @brief - display all employees in a given department
 * @param - list of employees, department name
 * @return number of entries displayed, 0 if none displayed
 */
int displayEmployees(employeeTable empTable, employeeList listEmployee, char dept)
{
    dept = toupper(dept);
    int index = dept - 'A';
    int count = 0;
    List2 trav, ptr;

    if (listEmployee[index] != NULL)
    {
        printf("\n\nDEPARTMENT %c", dept);
        printf("\n----------------------------------------------------------------------");

        for (trav = listEmployee[index]; trav != NULL; trav = trav->link)
        {
            employeeDetails temp = empTable[trav->index].employee;
            printf("\n  %-8s %-15s, %-26s %c.   %s", temp.empID, temp.name.LName, temp.name.fName, temp.name.MI, (temp.status) ? "Inactive" : "Active");
            count++;
        }
    }
    return count;
}

/**
 * @brief - sort employees by department
 * @param - list of employees
 * @return state what the function returns
 */
void sortEmployees(employeeTable empTable, employeeList listEmployee)
{
    int x;
    List2 ptr, *trav, new;
    employeeDetails temp;
    for (x = 0; x < SIZE; x++)
    {
        temp = empTable[x].employee;
        if (strcmp(temp.empID, "EMPTY") != 0 && strcmp(temp.empID, "DELETED") != 0)
        {
            int index = temp.department - 'A';

            /*insert the index of the employee to their department - sorted by empID*/
            for (trav = &listEmployee[index]; *trav != NULL && (strcmp(empTable[(*trav)->index].employee.name.LName, temp.name.LName) <= 0); trav = &(*trav)->link)
            {
            }
            new = (List2)calloc(1, sizeof(struct node));
            new->index = x;
            new->link = *trav;
            *trav = new;
        }
    }
}

void clearEmployeeLL(employeeList listEmployee)
{
    int index;
    List2 ptr, *trav;
    for (index = 0; index < 26; index++)
    {
        for (trav = &listEmployee[index]; *trav != NULL;)
        {
            ptr = *trav;
            *trav = ptr->link;
            free(ptr);
        }
    }
}

int displayAdvancedSalaryInfo(attendanceHistory head, int numEntries)
{
    int flag = 0, ctr = 1;
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int choice;
    List trav;
    printf("\nEnter payroll number to display its details, or enter 0 to exit.");
    printf("\n\nChoice: ");
    fflush(stdin);
    scanf("%d", &choice);
    if (choice > 0 && choice <= numEntries)
    {
        // traverse to the given period
        for (trav = head; trav != NULL && ctr < choice; trav = trav->link, ctr++)
        {
        }
        // get period number:
        int month = (trav->payrollInfo.payrollID[2] - '0') * 10 + trav->payrollInfo.payrollID[3] - '0';
        int year = 2000 + (trav->payrollInfo.payrollID[0] - '0') * 10 + trav->payrollInfo.payrollID[1] - '0';
        int period = trav->payrollInfo.payrollID[4] - '0';
        int day1 = (period == 1) ? 1 : monthDays[month - 1] / 2 + 1;
        int day2 = (period == 1) ? monthDays[month - 1] / 2 : monthDays[month - 1];
        switch (period)
        {
        case 1:
            printf("\nAttendance details for the period %02d/%02d/%04d - %02d/%02d/%04d\n", month, day1, year, month, day2, year);
            printf("\nDays absent:           %d", trav->payrollInfo.daysAbsent);
            printf("\nHours overtime:        %d", trav->payrollInfo.hoursOvertime);
            printf("\nHours late/undertime:  %d\n", trav->payrollInfo.hoursUndertime);
            printf("\nSalary for the period: P %9.2f\n", trav->payrollInfo.netSalary);
            printf("\nNote: Deductions such as contributions and withholding tax will be computed");
            printf("\nand reflected every second payment period of the month.");
            printf("\n---------------------------------------------------------------");
            printf("\nPress any key to exit...");
            getch();
            break;
        case 2:
            printf("\nAttendance details for the period %02d/%02d/%04d - %02d/%02d/%04d\n", month, day1, year, month, day2, year);
            printf("\nDays absent:           %d", trav->payrollInfo.daysAbsent);
            printf("\nHours overtime:        %d", trav->payrollInfo.hoursOvertime);
            printf("\nHours late/undertime:  %d\n", trav->payrollInfo.hoursUndertime);
            printf("\nGross Salary for the period:     P %9.2f", trav->payrollInfo.grossIncome);
            printf("\nLess: Government contributions: (P %9.2f)", trav->payrollInfo.contributions);
            printf("\nTaxable income:                  P %9.2f", trav->payrollInfo.taxableIncome);
            printf("\nLess: Witholding taxes:         (P %9.2f)\n", trav->payrollInfo.withTax);
            printf("\nNet salary for the period:       P %9.2f\n", trav->payrollInfo.netSalary);
            printf("\nNote: Deductions such as contributions and withholding tax are computed");
            printf("\nbased on the combined gross income of the first and second periods of the month.");
            printf("\n---------------------------------------------------------------");
            printf("\nPress any key to exit...");
            getch();
            break;
        case 3:
            printf("\n13th month bonus earned for the year %d:      P %9.2f\n", year, trav->payrollInfo.netSalary);
            printf("\nNote: 13th month pay is computed by getting 1/12 of the employee's total gross");
            printf("\nincome in the year only if he or she works from more than a month within the year. ");
            printf("\nThis amount is not included in any tax deductions nor contributions.");
            printf("\n---------------------------------------------------------------");
            printf("\nPress any key to exit...");
            getch();
            break;
        }
    }
    else if (choice == 0)
    {
        return 0;
    }

    return 1;
}

void viewEmployee(employeeTable empTable)
{
    int index, count;
    float sum = 0;
    char givenID[8], flag = 1, choice;
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    List trav;
    attendanceDetails temp;

    system("cls");
    printf("\n==========================================");
    printf("\n       VIEW SPECIFIC EMPLOYEE	INFO        ");
    printf("\n==========================================");
    printf("\n\nEnter Employee ID: ");
    fflush(stdin);
    scanf("%s", givenID);

    index = searchEmployee(empTable, givenID);

    if (index != -1)
    {
        while (flag)
        {
            system("cls");
            printf("\n==========================================");
            printf("\n       VIEW SPECIFIC EMPLOYEE DETAILS     ");
            printf("\n==========================================");
            printf("\n\nID: %s - %s, %s %c.", empTable[index].employee.empID, empTable[index].employee.name.LName, empTable[index].employee.name.fName, empTable[index].employee.name.MI);
            printf("\n\n[ 1 ] Display complete details");
            printf("\n[ 2 ] Display periods");
            printf("\n[ 3 ] Main menu");
            printf("\n\nChoice: ");
            fflush(stdin);
            scanf("%c", &choice);
            switch (choice)
            {
            case '1':
                flag = 0;
                system("cls");
                printf("\n===================================================");
                printf("\n             COMPLETE EMPLOYEE DETAILS     	     ");
                printf("\n===================================================\n");
                printf("\nEmployee ID:            \t%s", empTable[index].employee.empID);
                printf("\nName:                   \t%s, %s %c.", empTable[index].employee.name.LName, empTable[index].employee.name.fName, empTable[index].employee.name.MI);
                printf("\nDate employed(MM/DD/YY):\t%02d/%02d/%02d", empTable[index].employee.dateEmployed.month, empTable[index].employee.dateEmployed.day, empTable[index].employee.dateEmployed.year);
                printf("\nEmail address:          \t%s", empTable[index].employee.contact.email);
                printf("\nMobile number:          \t%s", empTable[index].employee.contact.phone);
                printf("\nBasic Salary:           \t%.02f", empTable[index].employee.details.basicSalary);
                printf("\nOvertime Pay:           \t%.02f", empTable[index].employee.details.overtimeHourlyRate);
                printf("\nDepartment:             \t%c", empTable[index].employee.department);
                printf("\nEmployee Status:        \t%s\n", (empTable[index].employee.status) ? "Inactive" : "Active");
                printf("\n===================================================");
                break;
            case '2':
                flag = 0;
                do
                {
                    count = 0;
                    sum = 0;
                    system("cls");
                    printf("\n==========================================");
                    printf("\n          EMPLOYEE PAYROLL SUMMARY        ");
                    printf("\n==========================================");

                    system("cls");
                    printf("\nPAYROLL DETAILS OF %s - %s, %s %c.", empTable[index].employee.empID, empTable[index].employee.name.LName, empTable[index].employee.name.fName, empTable[index].employee.name.MI);
                    printf("\n----------------------------------------------------------------------");
                    printf("\n  Period #    From   To             Amount (PHP)");
                    for (trav = empTable[index].history; trav != NULL; trav = trav->link)
                    {
                        count++;
                        temp = trav->payrollInfo;
                        int month = (temp.payrollID[2] - '0') * 10 + temp.payrollID[3] - '0';
                        int year = 2000 + (temp.payrollID[0] - '0') * 10 + temp.payrollID[1] - '0';
                        int period = temp.payrollID[4] - '0';
                        int day1 = (period == 1) ? 1 : monthDays[month - 1] / 2;
                        int day2 = (period == 1) ? monthDays[month - 1] / 2 + 1 : monthDays[month - 1];
                        sum += temp.netSalary;
                        if (period != 3)
                            printf("\n  [%2d]  %02d/%02d/%04d - %02d/%02d/%04d      P %9.2f", count, month, day1, year, month, day2, year, temp.netSalary);
                        else
                            printf("\n  [%2d]  Dec %04d - 13th Month Pay    P %9.2f", count, year, temp.netSalary);
                    }
                    printf("\n\nTotal payouts: P %9.2f", sum);
                    printf("\n------------------------------------------------------------------------");
                } while (count != 0 && displayAdvancedSalaryInfo(empTable[index].history, count) != 0);
                if (count == 0)
                {
                    printf("\nNo payroll record has been generated for the employee yet.");
                }
                break;
            case '3':
                return;
                break;
            default:
                printf("Invalid input. Please enter 1 or 2");
                break;
            }
        }
    }
    else
    {
        printf("\nError: Emplyoee does not exist. ");
    }
    printf("\n\nPress any key to exit...");
    getch();
}

/**
 * @brief Get user int input
 * @param question - prompt question
 * @return int - user int input
 */
int getIntInput(char question[])
{
    int ret;
    printf("%s", question);
    fflush(stdin);
    scanf("%d", &ret);
    return ret;
}

/**
 * @brief Asking for the group and the dates for the payroll
 *
 * @param group - update the group variable in the createPayroll function
 * @param payrollDate - 0 - year
 *                    - 1 - month
 *                    - 2 - start date
 *                    - 3 - end date
 *                    - 4 - period
 * @return 1 if proceed to create payroll or 0 if abort
 */
int askPayrollPeriodDetails(char *group, int payrollDate[])
{
    int flag = 0;
    char choice = 0;
    while (flag == 0)
    {
        system("cls");
        printf("\n==========================================");
        printf("\n            CREATE NEW PAYROLL	        ");
        printf("\n==========================================");
        printf("\n\n");
        payrollDate[0] = getIntInput("Enter the year: ");
        payrollDate[1] = getIntInput("Enter the month: ");
        payrollDate[2] = getIntInput("Enter the start date: ");
        payrollDate[3] = getIntInput("Enter the end date: ");
        payrollDate[4] = payrollDate[2] == 1 ? 1 : 2;
        if (dateValidation(payrollDate[1], payrollDate[2], payrollDate[0] % 100) && dateValidation(payrollDate[1], payrollDate[2], payrollDate[4] % 100) && payrollDate[2] < payrollDate[3])
        {
            do
            {
                printf("Enter Department/Group [A-Z]: ");
                fflush(stdin);
                scanf("%c", group);
            } while (!isalpha(*group));
            *group = toupper(*group);

            printf("\nYou are creating a payroll with the following details:");
            printf("\n    - Payroll Period Dates:      %02d-%02d/%02d/%04d", payrollDate[2], payrollDate[3], payrollDate[1], payrollDate[0]);
            printf("\n    - Employee Group/Department: %c\n", *group);
            do
            {
                printf("\nProceed? [1] - Yes, [2] - Input again, [0] - Exit to main menu: ");
                printf("\nChoice: ");
                fflush(stdin);
                scanf("%c", &choice);
            } while (!(choice == '1' || choice == '2' || choice == '0'));
            if (choice == '1' || choice == '0')
            {
                flag = 1;
            }
        }
        else
        {
            printf("\nInvalid input/s received. Please try again.");
            printf("\nPress any key to continue.");
            getch();
        }
    }
    return choice - '0';
}

/**
 * @brief Generates the payroll ID based on group and payrollDate[]
 *
 * @param group - A, B, C, etc.
 * @param payrollDate - 0 - year
 *                    - 1 - month
 *                    - 2 - start date
 *                    - 3 - end date
 *                    - 4 - period
 * @return char* - generated Payroll ID
 */
char *generatePayrollID(char group, int payrollDate[])
{
    int yearLastTwoDigits, x;
    char *ID, year[3], month[3], period[2];

    yearLastTwoDigits = payrollDate[0] % 100;
    sprintf(year, "%02d", yearLastTwoDigits);
    sprintf(month, "%02d", payrollDate[1]);
    sprintf(period, "%1d", payrollDate[4]);

    ID = (char *)calloc(7, sizeof(char));
    strcat(ID, year);
    strcat(ID, month);
    strcat(ID, period);
    ID[5] = group;

    return ID;
}

void inputAttendanceInfo(employeeDetails empTemp, payrollDetail *payTemp)
{
    if (isspace(empTemp.name.MI))
        printf("\nFor employee %s- %s, %s:\n", empTemp.empID, empTemp.name.LName, empTemp.name.fName);
    else
        printf("\nFor employee %s- %s, %s %c.:\n", empTemp.empID, empTemp.name.LName, empTemp.name.fName, empTemp.name.MI);
    printf("  - Number of days absent: ");
    fflush(stdin);
    scanf("%d", &payTemp->daysAbsent);
    if (payTemp->daysAbsent == '-')
        payTemp->daysAbsent = 0;
    printf("  - Number of hours overtime: ");
    fflush(stdin);
    scanf("%d", &payTemp->hoursOvertime);
    if (payTemp->hoursOvertime == '-')
        payTemp->hoursOvertime = 0;
    printf("  - Number of hours late/undertime: ");
    fflush(stdin);
    scanf("%d", &payTemp->hoursUndertime);
    if (payTemp->hoursUndertime == '-')
        payTemp->hoursUndertime = 0;
}

float compute13thMonthPay(attendanceHistory head, int year)
{
    /* 13th month pay is 1/12 of the annual gross income if person worked for
       more than a month within the given year. Not included in taxations nor deductions. */
    float amount = 0;
    int count = 0;
    char yearFormatted[3] = {year % 100 / 10 + '0', year % 10 + '0'};
    List trav;
    for (trav = head; trav != NULL; trav = trav->link)
    {
        // compare the year part of the payroll ID
        if (strncmp(trav->payrollInfo.payrollID,yearFormatted,2) == 0)
        {
            amount += trav->payrollInfo.grossIncome;
            count++;
        }
    }
    return (count > 2) ? amount / 12.0 : 0;
}

/**
 * @brief computing total income of a person before deductions
 *
 * @param payDetails  differing salary rates based on time of an employee
 * @param attendance  attendance of an employee
 * @return float - gross income
 */
float computeGrossIncome(paymentDetails payDetails, payrollDetail *attendance, int payrollDate[])
{
    float absentDeduction, overtimePay, undertimeDeduction, bisalary;
    int numWorkdays = payrollDate[3] - payrollDate[2];
    bisalary = payDetails.basicSalary / 2;
    absentDeduction = attendance->daysAbsent / numWorkdays * payDetails.basicSalary;
    overtimePay = payDetails.overtimeHourlyRate * attendance->hoursOvertime;
    /* DOLE's standard number of work hours a day = 8 */
    undertimeDeduction = payDetails.basicSalary / 30 / 8 * attendance->hoursUndertime;

    return bisalary - absentDeduction + overtimePay - undertimeDeduction;
}

/**
 * @brief Get the gross income of the first period
 *
 * @param pdptr - array of all the payroll details in the first period of the month
 * @param empId - employee ID
 * @param numOfEmployees
 * @return float - first period gross income of a specific employee
 */
float getFirstGrossIncome(employeeTable empTable, char empID[], char payrollIDFirstPeriod[])
{
    float ret = 0;
    int index = searchEmployee(empTable, empID);
    List trav;

    for (trav = empTable[index].history; trav != NULL && strcmp(trav->payrollInfo.payrollID, payrollIDFirstPeriod) != 0; trav = trav->link)
    {
    }
    if (trav != NULL)
        ret = trav->payrollInfo.grossIncome;
    else
        return ret;
}

float computeSSS(float income)
{
    float ret = 0;

    if (income >= 1000 && income < 1250)
        ret = 36.30;
    else if (income >= 1250 && income < 1750)
        ret = 54.50;
    else if (income >= 1750 && income < 2250)
        ret = 72.70;
    else if (income >= 2250 && income < 2750)
        ret = 90.80;
    else if (income >= 2750 && income < 3250)
        ret = 109.00;
    else if (income >= 3250 && income < 3750)
        ret = 127.20;
    else if (income >= 3750 && income < 4250)
        ret = 145.30;
    else if (income >= 4250 && income < 4750)
        ret = 163.50;
    else if (income >= 4750 && income < 5250)
        ret = 181.70;
    else if (income >= 5250 && income < 5750)
        ret = 199.80;
    else if (income >= 5750 && income < 6250)
        ret = 218.00;
    else if (income >= 6250 && income < 6750)
        ret = 236.20;
    else if (income >= 6750 && income < 7250)
        ret = 254.30;
    else if (income >= 7250 && income < 7750)
        ret = 272.50;
    else if (income >= 7750 && income < 8250)
        ret = 290.70;
    else if (income >= 8250 && income < 8750)
        ret = 308.80;
    else if (income >= 8750 && income < 9250)
        ret = 327.00;
    else if (income >= 9250 && income < 9750)
        ret = 345.20;
    else if (income >= 9750 && income < 10250)
        ret = 363.30;
    else if (income >= 10250 && income < 10750)
        ret = 381.50;
    else if (income >= 10750 && income < 11250)
        ret = 399.70;
    else if (income >= 11250 && income < 11750)
        ret = 417.80;
    else if (income >= 11750 && income < 12250)
        ret = 436.00;
    else if (income >= 12250 && income < 12750)
        ret = 454.20;
    else if (income >= 12750 && income < 13250)
        ret = 472.30;
    else if (income >= 13250 && income < 13750)
        ret = 490.50;
    else if (income >= 13750 && income < 14250)
        ret = 508.70;
    else if (income >= 14250 && income < 14750)
        ret = 526.80;
    else if (income >= 14750 && income < 15250)
        ret = 545.00;
    else if (income >= 15250 && income < 15750)
        ret = 563.20;
    else
        // income >= 15750
        ret = 581.30;

    return ret;
}

float computePHealth(float income)
{
    float ret;

    if (income <= 10000)
        ret = 275;
    else if (income > 10000 && income < 40000)
        ret = income * 0.0275 / 2;
    else
        // 40000 and above
        ret = 1100 / 2;

    return ret;
}

float computeWithTax(float salary)
{
    float ret;
    salary *= 12;
    if (salary <= 250000)
        ret = 0;
    else if (salary > 250000 && salary <= 400000)
        ret = (salary - 250000) * 0.2;
    else if (salary > 400000 && salary <= 800000)
        ret = (salary - 400000) * 0.25 + 30000;
    else if (salary > 800000 && salary <= 2000000)
        ret = (salary - 800000) * 0.3 + 130000;
    else if (salary > 2000000 && salary <= 8000000)
        ret = (salary - 2000000) * 0.32 + 490000;
    else
        // salary > 8000000
        ret = (salary - 8000000) * 0.35 + 2410000;

    ret /= 12;
    return ret;
}

float computeContributions(float income)
{
    /* Taken from https://cloudcfo.ph/blog/payroll/government-mandatory-deductions-philippines/ */
    float sss, phealth, pagibig;

    pagibig = 100;
    sss = computeSSS(income);
    phealth = computePHealth(income);

    return (float)(pagibig + sss + phealth);
}

/**
 * @brief computes the net salary or the final salary; contributions; taxableIncome; and withholding tax
 *
 * @param grossIncome - total income before deductions
 * @param ptrPd  - points to the second period payroll detail in the createPayroll()
 * @return float - net salary
 */
float computeNetSalary(float grossIncome, payrollDetail *ptrPd)
{
    ptrPd->contributions = (float)(computeContributions(grossIncome));
    ptrPd->taxableIncome = ptrPd->grossIncome - ptrPd->contributions;
    ptrPd->withTax = (float)(computeWithTax(ptrPd->taxableIncome));
    return ptrPd->taxableIncome - (float)(ptrPd->withTax);
}

/**
 * @brief add payroll to the PayrollFile.bin
 * @param pd payroll to be added
 * @return 1 if successful and 0 if failed
 */
int addPayrollToFile(char companyName[], payrollDetail pd)
{
    int retVal = 0;
    FILE *fp;
    char fileName[40];

    strcpy(fileName, companyName);
    strcat(fileName, PAY_FILENAME);

    fp = fopen(fileName, "ab");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        fwrite(&pd, sizeof(attendanceDetails), 1, fp);
        fclose(fp);
        retVal = 1;
    }
    return retVal;
}

/**
 * @brief handles the creation of a payroll from obtaining period from user to saving details to file
 *
 */
void createPayroll(employeeTable empTable, char companyName[])
{
    int payrollDate[5] = {0}, x, index; // 0 - year, 1 - month, 2 - start day, 3 - end day, 4 - period
    float firstGrossIncome, totalGrossIncome;
    char group = 0, *payrollId, period, payrollId1stPeriod[7] = {0}, payrollId3rdPeriod[7] = {0};
    employeeDetails employeeDetailTemp = {0};
    payrollDetail payrollDetailTemp = {};
    List *ptr, temp;
    List2 trav;
    employeeList listEmployee;
    for (x = 0; x < 26; x++)
    {
        listEmployee[x] = NULL;
    }

    if (askPayrollPeriodDetails(&group, payrollDate) != 0)
    {
        sortEmployees(empTable, listEmployee);
        payrollId = generatePayrollID(group, payrollDate);
        period = payrollId[4] - '0';
        index = group - 'A';

        system("cls");
        printf("\n==========================================");
        printf("\n            CREATE NEW PAYROLL	        ");
        printf("\n==========================================");
        printf("\n\nCreating payroll for Department %c for the period %02d-%02d/%02d/%04d\n", group, payrollDate[2], payrollDate[3], payrollDate[1], payrollDate[0]);

        for (trav = listEmployee[index]; trav != NULL; trav = trav->link)
        {
            // Search for existing payroll info for the given payroll ID for the employee
            if (empTable[trav->index].employee.status == ACTIVE)
            {
                for (ptr = &empTable[trav->index].history; *ptr != NULL && strcmp((*ptr)->payrollInfo.payrollID, payrollId) != 0; ptr = &(*ptr)->link)
                {
                }
                if (*ptr == NULL) // no existing payroll info for the given payroll ID for the employee yet
                {
                    temp = (List)malloc(sizeof(struct cell));
                    if (temp != NULL)
                    {
                        employeeDetailTemp = empTable[trav->index].employee;
                        strcpy(payrollDetailTemp.empID, employeeDetailTemp.empID);
                        strcpy(payrollDetailTemp.payrollID, payrollId);
                        inputAttendanceInfo(employeeDetailTemp, &payrollDetailTemp);
                        payrollDetailTemp.grossIncome = computeGrossIncome(employeeDetailTemp.details, &payrollDetailTemp, payrollDate);
                        if (period == 2)
                        {
                            strcpy(payrollId1stPeriod, payrollId);
                            payrollId1stPeriod[4] = '1';

                            firstGrossIncome = getFirstGrossIncome(empTable, payrollDetailTemp.empID, payrollId1stPeriod);
                            totalGrossIncome = payrollDetailTemp.grossIncome + firstGrossIncome;
                            payrollDetailTemp.netSalary = computeNetSalary(totalGrossIncome, &payrollDetailTemp);
                        }
                        else
                        {
                            payrollDetailTemp.netSalary = payrollDetailTemp.grossIncome;
                        }
                        // insert payroll details to end of list
                        temp->payrollInfo = payrollDetailTemp;
                        temp->link = NULL;
                        addPayrollToFile(companyName, payrollDetailTemp);

                        // 13th month pay consideration
                        if (payrollDate[1] == 12 && payrollDate[4] == 1)
                        {
                            payrollDetail payrollDetail13thMonth = {};
                            strcpy(payrollId3rdPeriod, payrollId);
                            payrollId3rdPeriod[4] = '3';
                            strcpy(payrollDetail13thMonth.empID, employeeDetailTemp.empID);
                            strcpy(payrollDetail13thMonth.payrollID, payrollId3rdPeriod);
                            payrollDetail13thMonth.netSalary = compute13thMonthPay(empTable[trav->index].history, payrollDate[0]);
                            temp->link = (List)malloc(sizeof(struct cell));
                            temp->link->payrollInfo = payrollDetail13thMonth;
                            temp->link->link = NULL;
                            addPayrollToFile(companyName, payrollDetail13thMonth);
                        }
                        *ptr = temp;
                    }
                }
            }
        }
        clearEmployeeLL(listEmployee);
        printf("\nAll attendance details for the department has been inputted.");
        printf("\nPayroll details have been successfully generated for the above employees.");
        printf("\nPayroll file successfully updated.");
        printf("\n\nPress any key to go back to the main menu.");
        getch();
    }
}

int updateEmployee(employeeTable empTable, employeeDetails newEmployee, int index)
{
    int ret = 0;
    if (strcmp(empTable[index].employee.empID, newEmployee.empID) == 0)
    {
        empTable[index].employee = newEmployee;
        ret = 1;
    }
    return ret;
}

int updateEmployeeToFile(char companyName[], employeeDetails newEmployee)
{
    int ret = 0, ndx = 0;
    FILE *fp;
    char fileName[40] = "";
    employeeDetails catcher;
    strcpy(fileName, companyName);
    strcat(fileName, EMP_FILENAME);
    fp = fopen(fileName, "r+");
    if (fp != NULL)
    {
        while (fread(&catcher, sizeof(employeeDetails), 1, fp) != 0 && ret == 0)
        {

            if (strcmp(catcher.empID, newEmployee.empID) == 0)
            {
                fseek(fp, ndx * sizeof(employeeDetails), SEEK_SET);
                fwrite(&newEmployee, sizeof(employeeDetails), 1, fp);
                fseek(fp, (ndx + 1) * sizeof(employeeDetails), SEEK_SET);
                ret = 1;
            }
            else
                ndx++;
        }
    }
    return ret;
}

int updateAttendanceRecord(attendanceHistory history, attendanceDetails newRecord)
{
    int ret = 0;
    List trav;
    for (trav = history; trav != NULL && strcmp(trav->payrollInfo.payrollID, newRecord.payrollID) != 0; trav = trav->link)
    {
    }
    if (trav != NULL)
    {
        trav->payrollInfo = newRecord;
        ret = 1;
    }
    return ret;
}

int updateAttendanceToFile(char companyName[], attendanceDetails newRecord)
{
    int ret = 0, ndx = 0;
    FILE *fp;
    char fileName[40] = "";
    attendanceDetails catcher;
    strcpy(fileName, companyName);
    strcat(fileName, PAY_FILENAME);
    fp = fopen(fileName, "r+");
    if (fp != NULL)
    {
        while (fread(&catcher, sizeof(attendanceDetails), 1, fp) != 0 && ret == 0)
        {
            if (strcmp(catcher.payrollID, newRecord.empID) == 0 && strcmp(catcher.empID, newRecord.empID) == 0)
            {
                fseek(fp, ndx * sizeof(attendanceDetails), SEEK_SET);
                fwrite(&newRecord, sizeof(attendanceDetails), 1, fp);
                fseek(fp, (ndx + 1) * sizeof(attendanceDetails), SEEK_SET);
                ret = 1;
            }
            else
                ndx++;
        }
    }
    return ret;
}

int editEmployee(employeeTable empTable, char companyName[])
{
    /* buffer for input validation */
    char givenID[10];
    char dateString[11], emailString[32], contactString[12];
    int month, day, year, index;
    float basicSalary = 0, overtimePay = 0;

    int validFlag[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1}; /* check if all fields are correctly inputted */
    int retValue = 0;                               /* return value */
    int exitFlag = 0;                               /* condition if the loop will continue */
    int i;                                          /* counter for loops */
    int ch;

    char choice; /* 0-9 = inputs, e = exit, c = create */
    employeeInfo newEmployee;

    system("cls");
    printf("\n==========================================");
    printf("\n            EDIT EMPLOYEE INFO            ");
    printf("\n==========================================");
    printf("\n\nEnter Employee ID: ");
    fflush(stdin);
    scanf("%s", givenID);

    index = searchEmployee(empTable, givenID);
    if (index != -1)
    {
        newEmployee = empTable[index];

        while (!exitFlag) /* Loop until the exit flag is changed */
        {
            system("cls");
            printf("\n==========================================");
            printf("\n          EDIT EMPLOYEE (%s)              ", givenID);
            printf("\n==========================================");
            printf("\n[ 1 ] Last Name:              \t%s", newEmployee.employee.name.LName);
            printf("\n[ 2 ] First Name:             \t%s", newEmployee.employee.name.fName);
            printf("\n[ 3 ] MI:                     \t%c.", newEmployee.employee.name.MI);
            printf("\n[ 4 ] Date employed(MM/DD/YY):\t%02d/%02d/%02d", newEmployee.employee.dateEmployed.month, newEmployee.employee.dateEmployed.day, newEmployee.employee.dateEmployed.year);
            printf("\n[ 5 ] Email:                  \t%s", newEmployee.employee.contact.email);
            printf("\n[ 6 ] Contact No.:            \t%s", newEmployee.employee.contact.phone);
            printf("\n[ 7 ] Basic Salary:           \t%.02f", newEmployee.employee.details.basicSalary);
            printf("\n[ 8 ] Overtime Pay:           \t%.02f", newEmployee.employee.details.overtimeHourlyRate);
            printf("\n[ 9 ] Department:             \t%c", newEmployee.employee.department);
            printf("\n[ 0 ] Employee Status:        \t%s", (newEmployee.employee.status) ? "Inactive" : "Active");
            printf("\n==========================================");
            printf("\n[c] Confirm changes | [e] Exit ");
            printf("\n\nChoice: ");
            fflush(stdin);
            scanf("%c", &choice);
            while ((ch = getchar()) != '\n' && ch != EOF)
                ; // To eat up all the characters left behind by scanf call including '\n'.

            switch (choice)
            {
            case '1':
                printf("\nLast Name: ");
                fflush(stdin);
                gets(newEmployee.employee.name.LName);
                if (nameValidation(newEmployee.employee.name.LName))
                {
                    validFlag[0] = 1;
                }
                else
                {
                    validFlag[0] = 0;
                    strcpy(newEmployee.employee.name.LName, "");
                    printf("Invalid input\n");
                    printf("\nPress any key to continue");
                    getch();
                }
                break;

            case '2':
                printf("\nFirst Name: ");
                fflush(stdin);
                gets(newEmployee.employee.name.fName);
                if (nameValidation(newEmployee.employee.name.fName))
                {
                    validFlag[1] = 1;
                }
                else
                {
                    validFlag[1] = 0;
                    strcpy(newEmployee.employee.name.fName, "");
                    printf("Invalid input\n");
                    printf("\nPress any key to continue");
                    getch();
                }
                break;

            case '3':
                printf("\nMiddle Initial (enter a space if N/A): ");
                fflush(stdin);
                scanf("%c", &newEmployee.employee.name.MI);
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
                if (isalpha(newEmployee.employee.name.MI) || isspace(newEmployee.employee.name.MI))
                {
                    newEmployee.employee.name.MI = toupper(newEmployee.employee.name.MI);
                    validFlag[2] = 1;
                }
                else
                {
                    validFlag[2] = 0;
                    newEmployee.employee.name.MI = '\0';
                    printf("Invalid input\n");
                    printf("\nPress any key to continue");
                    getch();
                }

                break;

            case '4':
                printf("\nDate employed(MM/DD/YY): ");
                fflush(stdin);
                gets(dateString);

                char *token = strtok(dateString, "/"); /* separates date by '/' */
                month = atoi(token);                   /* converts string to int */
                token = strtok(NULL, "/");
                day = atoi(token);
                token = strtok(NULL, "/");
                year = atoi(token);

                if (dateValidation(month, day, year))
                {
                    newEmployee.employee.dateEmployed.month = month;
                    token = strtok(NULL, "/");
                    newEmployee.employee.dateEmployed.day = day;
                    token = strtok(NULL, "/");
                    newEmployee.employee.dateEmployed.year = year;
                    validFlag[3] = 1;
                }
                else
                {
                    printf("Invalid input\n");
                    printf("\nPress any key to continue");
                    getch();
                }

                break;

            case '5':
                printf("\nEmail: ");
                fflush(stdin);
                gets(emailString);
                if (emailValidation(emailString))
                {
                    strcpy(newEmployee.employee.contact.email, emailString);
                    validFlag[4] = 1;
                }
                else
                {
                    printf("Invalid Input");
                    printf("\nPress any key to continue");
                    getch();
                }

                break;

            case '6':
                printf("\nContact No.: ");
                fflush(stdin);
                gets(contactString);
                if (phoneValidation(contactString))
                {
                    strcpy(newEmployee.employee.contact.phone, contactString);
                    validFlag[5] = 1;
                }
                else
                {
                    printf("Input invalid");
                    printf("\nPress any key to continue");
                    getch();
                }

                break;

            case '7':
                printf("\nBasic Monthly Salary: ");
                fflush(stdin);
                scanf("%f", &basicSalary);
                if (payValidation(basicSalary))
                {
                    newEmployee.employee.details.basicSalary = basicSalary;
                    validFlag[6] = 1;
                }
                else
                {
                    printf("Input invalid");
                    printf("\nPress any key to continue");
                    getch();
                }
                break;

            case '8':
                printf("\nOvertime Hourly Pay: ");
                fflush(stdin);
                scanf("%f", &overtimePay);
                if (payValidation(overtimePay))
                {
                    newEmployee.employee.details.overtimeHourlyRate = overtimePay;
                    validFlag[7] = 1;
                }
                else
                {
                    printf("Input invalid - please enter a positive amount");
                    printf("\nPress any key to continue");
                    getch();
                }
                break;

            case '9':
                printf("\nDepartment [A-Z]: ");
                fflush(stdin);
                scanf("%c", &newEmployee.employee.department);
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
                if (isalpha(newEmployee.employee.department))
                {
                    newEmployee.employee.department = toupper(newEmployee.employee.department);
                    validFlag[8] = 1;
                }
                else
                {
                    validFlag[8] = 0;
                    newEmployee.employee.name.MI = '\0';
                    printf("Invalid input. Please enter a letter\n");
                    printf("\nPress any key to continue");
                    getch();
                }

                break;

            case '0':
                printf("\n[ 0 ] Active");
                printf("\n[ 1 ] Inactive");
                printf("\nStatus: ");
                fflush(stdin);
                scanf("%c", &choice);
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
                if (choice == '0' || choice == '1')
                { /*check if input is valid*/
                    newEmployee.employee.status = choice - '0';
                }
                else
                {
                    printf("Invalid input");
                    printf("\nPress any key to continue");
                    getch();
                }
                break;

            case 'c':
                for (i = 0; i < 9 && validFlag[i] != 0; i++)
                {
                }
                if (i == 9)
                {
                    if (updateEmployee(empTable, newEmployee.employee, index) && updateEmployeeToFile(companyName, newEmployee.employee))
                    {
                        printf("\nSuccessfully updated employee info!");
                        printf("\n\nPress any key to continue");
                        getch();
                        retValue = 1;
                    }
                    else
                    {
                        printf("Error in updating employee info. Please try again.\n");
                        printf("\nPress any key to continue");
                        getch();
                        retValue = 0;
                    }

                    exitFlag = 1; /* prompts to exit the loop */
                }
                else
                {
                    printf("Missing input at [%d]\n", i + 1);
                    printf("\nPress any key to continue");
                    getch();
                }
                break;

            case 'e':
                retValue = -1;
                exitFlag = 1; /* prompts to exit the loop with the default return value of 0*/
                break;

            default:
                printf("Not a valid choice!\n");
                printf("\nPress any key to continue");
                getch();
            }
        }
    }
    else
    {
        printf("\nError: Emplyoee does not exist. ");
        printf("\n\nPress any key to exit...");
        getch();
    }

    return retValue;
}
