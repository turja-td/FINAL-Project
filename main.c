#include <stdio.h>
#include "common.h"
#include "utils.h"
#include "patient.h"
#include "doctor.h"
#include "admin.h"
#include "bloodbank.h"

static void patientEntryMenu(void) {
    printf("\n--- Patient ---\n");
    printf("1. Register\n2. Login\n3. Back\n");
    int choice = readIntRange("Choice: ", 1, 3);

    if (choice == 1) {
        registerPatient();
        pressEnterToContinue();
    } else if (choice == 2) {
        int patientId;
        if (loginPatient(&patientId)) {
            runPatientMenu(patientId);
        } else {
            pressEnterToContinue();
        }
    }
}

static void doctorEntryMenu(void) {
    printf("\n--- Doctor Login ---\n");
    printf("(Doctor accounts are created by an administrator.)\n");
    int doctorId;
    if (loginDoctor(&doctorId)) {
        runDoctorMenu(doctorId);
    } else {
        pressEnterToContinue();
    }
}

static void adminEntryMenu(void) {
    printf("\n--- Admin Login ---\n");
    int adminId;
    if (loginAdmin(&adminId)) {
        runAdminMenu(adminId);
    } else {
        pressEnterToContinue();
    }
}

int main(void) {
    setDefaultAdmin();
    initializeBloodStock();

    printf("======================================\n");
    printf("   HOSPITAL MANAGEMENT SYSTEM\n");
    printf("======================================\n");

    int running = 1;
    while (running) {
        printf("\n===== Main Menu =====\n");
        printf("1. Patient\n");
        printf("2. Doctor\n");
        printf("3. Admin\n");
        printf("4. Exit\n");

        int choice = readIntRange("Choice: ", 1, 4);

        if (choice == 1) {
            patientEntryMenu();
        } else if (choice == 2) {
            doctorEntryMenu();
        } else if (choice == 3) {
            adminEntryMenu();
        } else if (choice == 4) {
            printf("Goodbye!\n");
            running = 0;
        }
    }

    return 0;
}
