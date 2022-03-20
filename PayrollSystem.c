/**************************************************************
 *
 * CIS 2206 Practice Activity #1 - Payroll System using C
 * Created by: Team 1
 * Members: 
 * Date: March 14-21, 2022
 *  
 * ************************************************************/


/***** HEADER FILES TO INCLUDE *****/
#include "UtilityFunc.h"
#include "PayrollModule.h"
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>

/***** CONSTANTS *****/
#define SIZE 512                    // hash table size
#define EMP_FILENAME 'EmployeeFile.bin'
#define ATT_FILENAME 'AttendanceFile.bin'
#define PAY_FILENAME 'PayrollIDList.bin'
/* Note: Actual file names will be '<CompanyName>_EmployeeFile.bin'. 
 * Company name will be inputted at main menu upon program and data initialization.
 */

/***** DATA STRUCTURE DEFINITION *****/
typedef struct{
    char LName[16];         // last name
    char fName[24];         // first name
    char MI;                // middle initial
}nameDetails;

typedef enum{
    ACTIVE,                 // only active employees will be prompted attendance entry
    INACTIVE                // in the specified group for each new payroll entry
}empStatus;

typedef struct{
    unsigned int year:7;    // YY
    unsigned int month:4;   // MM
    unsigned int day:5;     // DD
}dateDetails;                      // CONSTRAINT: No negative values

typedef struct{
    char email[32];         // must have '@' and '.' for valid email
    char phone[11];         // 11-digit mobile number
}contactDetails;

// typedef struct{
//     float basicSalary;      // basic monthly salary, in PHP
//     float dailyRate;
//     float overtimeHourlyRate;      // overtime monthly pap, in PHP
//     float contributions;    // total government contributions, in PHP
// }paymentDetails;            // CONSTRAINT: No negative values

// typedef struct{
//     char empID[8];          // employee ID (CONSTRAINT: Must match an existing employee ID)
//     char payrollID[7];      // payroll ID  (CONSTRAINT: See format below)
//     int daysAbsent;         // number of days absent for the period
//     int hoursOvertime;      // overtime duration for the period, in hours
//     int minsUndertime;      // undertime duration for the period, in minutes
// }attendanceDetails;         // the structure to be written in the 'EmployeeFile.bin'

/* Payroll ID is YYMMPG - Year, Month, Period, Group #
 * Example:      22011A - Period 1, January 2022, Group A
 */

/* Linked list for attendance details */
// typedef struct cell{
//     attendanceDetails attendance;
//     struct cell* link;
// }cellType, *attendanceHistory;

/* Employee structure, with attendance details LL pointer */
typedef struct{
    char empID[8];          // employee ID (7 characters)
                            // for has table: Dummy values - "EMPTY" and "DELETED"
    nameDetails name; 
    char group;      
    contactDetails contact;
    dateDetails dateEmployed;
    empStatus status;
    paymentDetails details;
}employeeDetails;           // the structure to be written in the 'EmployeeFile.bin'

typedef struct{
    employeeDetails employee;   //employee details
    attendanceHistory history;  //head pointer of the attendance linked list
}employeeInfo;

/* Hash table of employees - CLOSED HASHING WITH MAX BUCKETS, WITH LINEAR PROBING
 *  I chose closed hashing ra para sayun ma clear ang memory by avoiding dynamic cells,
    since when we terminate a program we have to free all dynamic memory
 */
typedef employeeInfo employeeTable[SIZE];  // constant hash table

/***** FUNCTION PROTOTYPES *****/
void initialize();          // initialize by loading existing file. If none, new file will be created
void terminate();           // properly terminate the file by freeing all dynamic memory (attendance LL)
int saveData();             // returns 1 if successful save and 0 if not
int loadData();

// addEmployee();
// editEmployee(char empID[], int mode);
// createPayroll();
// 
//

int hash(char empID);
// date manipulation functions


/***** main() function - Handles the main menu and calls the subfunction *****/
int main(){
    
    /* Employee hash table declaration */
    employeeTable empTable;
    
    printf("\n==========================================");
    printf("\n        CIS 2206 - PAYROLL SYSTEM         ");
    printf("\n==========================================");
    printf("\n\n");
    // initialize(); // ask for input of company name



    /* MAIN MENU */
    
    /*

        Create employee
        View list of employees
            (Display ID and names)
        View specific employee info
            Enter employee ID
                1. (Display complete details
                2. (Display Periods)
                    Enter period to display
                        Retrieve desired attendance info -> Compute on the spot and display salary breakdown

        Edit employee info
            Enter employee ID
            Edit Personal Details
            Edit Attendance Info
                (Display Periods) Enter period to edit
                
        Create Payroll
            Enter Payroll Period and Group
                Confirm period entered or cancel
                For each employee -> Enter Attendance for Individual Employees -> insertFirst() to LL of the employee
        Reload 
        Exit
            Confirm exit
    */

    return 0;
}

/***** FUNCTION DEFINITIONS *****/

/**
 * @brief initializes the data and loads the file, sets up the internal memory
 * @param -
 * @return -
 */
void initialize(){
    /* Variable declarations */
    /* Variable initializations */
    /* Body */
    /* Exit/return statement, if applicable */
}

/**
 * @brief give a brief description of the function
 * @param describe the parameters
 * @return state what the function returns
 */
void func1(int args1,int args2){
    /* Variable declarations */
    /* Variable initializations */
    /* Body */
    /* Exit/return statement, if applicable */
}

/**
 * @brief 
 * 
 */
void createPayroll() {
    int payrollDate[5], numOfEmployees, x; // 0 - year, 1 - month, 2 - start day, 3 - end day, 4 - period
    float grossIncome, firstGrossIncome, totalGrossIncome;
    char group[1], *payrollId, period, payrollId1stPeriod[7];
    payrollDetails pds;
    employeeDetails *eds, employeeDetailTemp;
    employeeTable et;
    attendanceHistory ahptr;
    payrollDetail payrollDetailTemp = {};

    askDetails(group, payrollDate);
    payrollId = generatePayrollID(group[0], payrollDate);
    period = payrollId[4];
    numOfEmployees = getEmployeesInGroup(group[0]);
    eds = (employeeDetails*)calloc(numOfEmployees, sizeof(employeeDetails));
    // et = (get from file that the employee table that belongs to the desired group)
    pds.payrollDetailArr = (payrollDetail*)calloc(numOfEmployees, sizeof(payrollDetail));
    
    for(x = 0; x < numOfEmployees; x++) {
        // Assuming the table only has employees from the same group
        for(ahptr = et[x].history; ahptr != NULL; ahptr = ahptr->link) {
            if(strcmp(payrollId, ahptr->attendance.payrollID) == 0) {
                employeeDetailTemp = et[x].employee;
                payrollDetailTemp.grossIncome = computeGrossIncome(employeeDetailTemp.details, ahptr->attendance);
                strcpy(payrollDetailTemp.empID, employeeDetailTemp.empID);
                strcpy(payrollDetailTemp.payrollID, payrollId);
                memcpy(&payrollDetailTemp.name, &employeeDetailTemp.name, sizeof(nameDetails));
                // we alredy found the desired attendance given the period
                break;
            }
        }
        if(period == '2') {
            strcpy(payrollId1stPeriod, payrollId);
            payrollId1stPeriod[4] = '1';
            pds.payrollDetailArr = getAllPayrolls(payrollId1stPeriod, numOfEmployees);

            firstGrossIncome = getFirstGrossIncome(pds.payrollDetailArr, payrollDetailTemp.empID, numOfEmployees);
            totalGrossIncome = grossIncome + firstGrossIncome;
            payrollDetailTemp.netSalary = computeNetSalary(totalGrossIncome, &payrollDetailTemp);
        }
        addPayroll(payrollDetailTemp);
    }
}