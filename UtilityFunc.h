#ifndef UTILITY_FUNC_H
#define UTILITY_FUNC_H

#include <stdio.h>

#define PAY_FILENAME "Payrolls.bin"
#define EMPLOYEES_IN_GROUP "EmployeesInGroup.bin"

typedef struct {
    char group;
    int numOfEmployees;
} employeesInGroup;

/**
 * @brief Get user int input
 * @param question - prompt question
 * @return int - user int input
 */
int getIntInput(char question[]) {
    int ret;

    printf("%s", question);
    scanf(" %d", &ret);
    return ret;
}

/**
 * @brief adding employees within a group
 * 
 * @param eig - group and no. of employees to be added to a file
 */
void addEmployeesInGroup(employeesInGroup eig) {
    FILE *fptr;

    fptr = fopen(EMPLOYEES_IN_GROUP, "a");
    fwrite(&eig, sizeof(employeesInGroup), 1, fptr);
    
    fclose(fptr);
}

/**
 * @brief set the number of employees
 * 
 * @param dept - department
 * @param num - new total of employees
 */
void setNumOfEmployees(char dept, int num) {
    FILE *fptr;
    employeesInGroup temp;
    int eigSize;

    fptr = fopen(EMPLOYEES_IN_GROUP, "rb+");
    eigSize = sizeof(employeesInGroup);
    // Loop until the desired dept is found
    while(fread(&temp, eigSize, 1, fptr) != 0) {
        if(temp.group == dept) {
            temp.numOfEmployees = num;
            printf("\n%d", ftell(fptr));
            fseek(fptr, eigSize*-1, SEEK_CUR);
            printf("\n%d", ftell(fptr));
            fwrite(&temp, eigSize, 1, fptr);
            printf("\n%d", ftell(fptr));
            break;
        }
    }
    fclose(fptr);
}

/**
 * @brief Get the num of employees within a group
 * 
 * @param dept - chosen department
 * @return int - num of employees
 */
int getEmployeesInGroup(char dept) {
    FILE *fptr;
    employeesInGroup temp;
    int eigSize;

    fptr = fopen(EMPLOYEES_IN_GROUP, "r");
    eigSize = sizeof(employeesInGroup);
    // Loop until the desired dept is found
    while(fread(&temp, eigSize, 1, fptr) != 0) {
        if(temp.group == dept) {
            break;
        }
    }
    fclose(fptr);

    return temp.numOfEmployees;
}

/**
 * @brief just for renewing file for testing purposes
 * 
 * @param file - file name
 */
void renewFile(char file[]) {
    FILE *fptr;

    fptr = fopen(file, "w");
    fclose(fptr);
}

#endif