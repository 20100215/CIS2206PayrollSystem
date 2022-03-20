#ifndef PAYROLL_MODULE_H
#define PAYROLL_MODULE_H

#include "UtilityFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum month {
    JAN, FEB, MAR, APR, MAY, JUNE, JUL, AUG, SEPT, OCT, NOV, DEC
} month;

typedef struct{
    float basicSalary;      // basic monthly salary, in PHP
    float dailyRate;
    float hourlyRate;
    float overtimeHourlyRate;      // overtime monthly pap, in PHP
    float contributions;    // total government contributions, in PHP
}paymentDetails;

typedef struct{
    char LName[16];         // last name
    char fName[24];         // first name
    char MI;                // middle initial
}nameDetails;

typedef struct{
    char empID[8];          // employee ID (CONSTRAINT: Must match an existing employee ID)
    char payrollID[7];      // payroll ID  (CONSTRAINT: See format below)
    int daysAbsent;         // number of days absent for the period
    int hoursOvertime;      // overtime duration for the period, in hours
    int hoursUndertime;      // undertime duration for the period, in minutes
}attendanceDetails;  

typedef struct cell{
    attendanceDetails attendance;
    struct cell* link;
}cellType, *attendanceHistory;

typedef struct {
    char payrollID[7];
    char empID[8];
    nameDetails name;
    float netSalary;
    float grossIncome;
    float taxableIncome;
    float contributions;
    float withTax; // withholding tax
} payrollDetail;

typedef struct {
    char payrollID[7];
    payrollDetail *payrollDetailArr; 
} payrollDetails;

/**
 * @brief Asking for the group and the dates for the payroll
 * 
 * @param group - update the group variable in the createPayroll function
 * @param payrollDate - 0 - year
 *                    - 1 - month
 *                    - 2 - start date
 *                    - 3 - end date
 *                    - 4 - period
 */
void askDetails(char group[], int payrollDate[]) {
    printf("Enter Group: ");
    scanf("%c", &group[0]);
    // For start date
    payrollDate[0] = getIntInput("Enter the year: ");
    payrollDate[1] = getIntInput("Enter the month: ");
    printf("Enter the start and end date\n");
    payrollDate[2] = getIntInput("Enter the start date: ");
    payrollDate[3] = getIntInput("Etner the end date: ");
    payrollDate[4] = payrollDate[2] == 1 ? 1 : 2;
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
char* generatePayrollID(char group, int payrollDate[]) {
    int yearLastTwoDigits, x;
    char *ID, year[3], month[3], period[2];

    yearLastTwoDigits = payrollDate[0] % 100;
    sprintf(year, "%d", yearLastTwoDigits);
    sprintf(month, "%d", payrollDate[1]);
    sprintf(period, "%d", payrollDate[4]);

    ID = (char*)malloc(sizeof(char)*7);
    strcat(ID, year);
    strcat(ID, month);
    strcat(ID, period);
    ID[5] = group;

    return ID;
}

/**
 * @brief Get the All Payrolls based on the given payroll ID
 * 
 * @param payrollId - what kind of employees we want to get
 * @param numOfEmployees - total num of employees within a group 
 * @return payrollDetail* - array of all the employees with the matching payrollId
 */
payrollDetail *getAllPayrolls(char payrollId[], int numOfEmployees) {
    payrollDetail *retpd, temp;
    FILE *fptr;
    int x;

    fptr = fopen(PAY_FILENAME, "rb");
    retpd = (payrollDetail*)malloc(sizeof(numOfEmployees));
    while(fread(&temp, sizeof(payrollDetail), 1, fptr) != 0) {
        if(strcmp(payrollId, temp.payrollID) == 0) {
            retpd[x] = temp;
            x++;
        }
    }

    return retpd;
}

/**
 * @brief computing total income of a person before deductions
 * 
 * @param payDetails - differing salary rates based on time of an employee
 * @param attendance - attendance of an employee
 * @return float - gross income
 */
float computeGrossIncome(paymentDetails payDetails, attendanceDetails attendance) {
    float absentDeduction, overtimePay, undertimeDeduction, bisalary;

    bisalary = payDetails.basicSalary / 2;
    absentDeduction = payDetails.dailyRate * attendance.daysAbsent * -1;
    overtimePay = payDetails.overtimeHourlyRate * attendance.hoursOvertime;
    undertimeDeduction = payDetails.hourlyRate * attendance.hoursUndertime * -1;

    return bisalary + absentDeduction + overtimePay + undertimeDeduction;
}

/**
 * @brief Get the gross income of the first period
 * 
 * @param pdptr - array of all the payroll details in the first period of the month
 * @param empId - employee ID
 * @param numOfEmployees 
 * @return float - first period gross income of a specific employee
 */
float getFirstGrossIncome(payrollDetail *pdptr, char empId[], int numOfEmployees) {
    float ret;
    int x;

    for(x = 0; x < numOfEmployees; x++) {
        if(strcmp(pdptr->empID, empId) == 0) {
            ret = pdptr->grossIncome;
        }
    }

    return ret;
}

/**
 * @brief computes the net salary or the final salary; contributions; taxableIncome; and withholding tax
 * 
 * @param grossIncome - total income before deductions
 * @param ptrPd  - points to the second period payroll detail in the createPayroll()
 * @return float - net salary
 */
float computeNetSalary(float grossIncome, payrollDetail *ptrPd) {
    ptrPd->contributions = computeContributions(grossIncome);
    ptrPd->taxableIncome = grossIncome - ptrPd->contributions;
    ptrPd->withTax = computeWithTax(ptrPd->taxableIncome);
    return ptrPd->taxableIncome - ptrPd->withTax;
}

float computeContributions(float income) {
    float sss, phealth, pagibig;

    pagibig = 100;
    sss = computeSSS(income);
    phealth = computePHealth(income);

    return pagibig + sss + phealth;
}

float computeSSS(float income) {
    float ret = 0;

    if(income >= 1000 && income < 1250) {
        ret = 36.30;
    } else if(income >= 1250 && income < 1750) {
        ret = 54.50;
    } else if(income >= 1750 && income < 2250) {
        ret = 72.70;
    } else if(income >= 2250 && income < 2750) {
        ret = 90.80;
    } else if(income >= 2750 && income < 3250) {
        ret = 109.00;
    } else if(income >= 3250 && income < 3750) {
        ret = 127.20;
    } else if(income >= 3750 && income < 4250) {
        ret = 145.30;
    } else if(income >= 4250 && income < 4750) {
        ret = 163.50;
    } else if(income >= 4750 && income < 5250) {
        ret = 181.70;
    } else if(income >= 5250 && income < 5750) {
        ret = 199.80;
    } else if(income >= 5750 && income < 6250) {
        ret = 218.00;
    } else if(income >= 6250 && income < 6750) {
        ret = 236.20;
    } else if(income >= 6750 && income < 7250) {
        ret = 254.30;
    } else if(income >= 7250 && income < 7750) {
        ret = 272.50;
    } else if(income >= 7750 && income < 8250) {
        ret = 290.70;
    } else if(income >= 8250 && income < 8750) {
        ret = 308.80;
    } else if(income >= 8750 && income < 9250) {
        ret = 327.00;
    } else if(income >= 9250 && income < 9750) {
        ret = 345.20;
    } else if(income >= 9750 && income < 10250) {
        ret = 363.30;
    } else if(income >= 10250 && income < 10750) {
        ret = 381.50;
    } else if(income >= 10750 && income < 11250) {
        ret = 399.70;
    } else if(income >= 11250 && income < 11750) {
        ret = 417.80;
    } else if(income >= 11750 && income < 12250) {
        ret = 436.00;
    } else if(income >= 12250 && income < 12750) {
        ret = 454.20;
    } else if(income >= 12750 && income < 13250) {
        ret = 472.30;
    } else if(income >= 13250 && income < 13750) {
        ret = 490.50;
    } else if(income >= 13750 && income < 14250) {
        ret = 508.70;
    } else if(income >= 14250 && income < 14750) {
        ret = 526.80;
    } else if(income >= 14750 && income < 15250) {
        ret = 545.00;
    } else if(income >= 15250 && income < 15750) {
        ret = 563.20;
    } else { // income >= 15750
        ret = 581.30;
    }

    return ret;
}

float computePHealth(float income) {
    float ret; 

    if(income <= 10000) {
        ret = 275;
    } else if(income > 10000 && income < 40000) {
        ret = income * 0.0275 / 2;
    } else { // 40000 and above
        ret = 1100/2;
    }

    return ret;
}

float computeWithTax(float salary) {
    float ret;
    salary *= 12;
    if(salary <= 250000) {
        ret = salary;
    } else if(salary > 250000 && salary <= 400000) {
        ret = (salary - 250000) * 0.2;
    } else if(salary > 400000 && salary <= 800000) {
        ret = (salary - 400000) * 0.25 + 30000;
    } else if(salary > 800000 && salary <= 2000000) {
        ret = (salary - 800000) * 0.3 + 130000;
    } else if(salary > 2000000 && salary <= 8000000) {
        ret = (salary - 2000000) * 0.32 + 490000;
    } else { // salary > 8000000
        ret = (salary - 8000000) * 0.35 + 2410000;
    }
    ret /= 12;
    return ret;
}

/**
 * @brief add payroll to the Payrolls.bin
 * 
 * @param pd payroll to be added
 */
void addPayroll(payrollDetail pd) {
    FILE *fptr;

    fptr = fopen(PAY_FILENAME, "ab");
    fwrite(&pd, sizeof(payrollDetail), 1, fptr);

    fclose(fptr);
}

#endif