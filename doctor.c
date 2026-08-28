#include <stdio.h>
#include <string.h>
#include "doctor.h"
#include "common.h"
#include "utils.h"
#include "appointment.h"
#include "medrecord.h"
#include "rating.h"
#include "bloodbank.h"
#include "patient.h"

int loadDoctors(Doctor list[])
{
    FILE *fp = fopen(FILE_DOCTORS, "rb");

    if (fp == NULL) {
        return 0;
    }

    int count = fread(list, sizeof(Doctor), MAX_DOCTORS, fp);

    fclose(fp);

    return count;
}

void saveDoctors(Doctor list[], int count)
{
    FILE *fp = fopen(FILE_DOCTORS, "wb");

    if (fp == NULL) {
        printf("Error: could not save doctor data.\n");
        return;
    }

    fwrite(list, sizeof(Doctor), count, fp);

    fclose(fp);
}

int findDoctorById(Doctor list[], int count, int id)
{
    int i;

    for (i = 0; i < count; i++) {
        if (list[i].id == id) {
            return i;
        }
    }

    return -1;
}

int loginDoctor(int *outDoctorId)
{
    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];

    readString("Username: ", username, USERNAME_LEN);
    readString("Password: ", password, PASSWORD_LEN);

    int i;

    for (i = 0; i < count; i++) {

        if (strcmp(list[i].username, username) == 0) {

            if (list[i].active == 0) {
                printf("This doctor account has been removed by an administrator.\n");
                return 0;
            }

            if (strcmp(list[i].password, password) == 0) {
                *outDoctorId = list[i].id;
                return 1;
            }

            printf("Incorrect password.\n");
            return 0;
        }
    }

    printf("No doctor found with that username.\n");
    return 0;
}

static void printDoctorLine(Doctor d)
{
    printf("ID %d | Dr. %-20s | %-15s | %s | Rating: %.1f (%d review(s))\n",
           d.id, d.name, d.specialization, d.contact,
           d.avgRating, d.ratingCount);
}

void searchDoctors(void)
{
    char term[NAME_LEN];

    readString("Search by name or specialization: ", term, NAME_LEN);

    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    printf("\n--- Matching Doctors ---\n");

    int found = 0;
    int i;

    for (i = 0; i < count; i++) {

        if (list[i].active == 0) {
            continue;
        }

        if (containsIgnoreCase(list[i].name, term) ||
            containsIgnoreCase(list[i].specialization, term)) {

            printDoctorLine(list[i]);
            found = 1;
        }
    }

    if (found == 0) {
        printf("No matching doctors found.\n");
    }
}

int pickDoctorForBooking(void)
{
    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    printf("\n--- Available Doctors ---\n");

    int found = 0;
    int i;

    for (i = 0; i < count; i++) {

        if (list[i].active == 1) {
            printDoctorLine(list[i]);
            found = 1;
        }
    }

    if (found == 0) {
        printf("No doctors are currently available.\n");
        return -1;
    }

    int id = readInt("\nEnter Doctor ID to book (0 to cancel): ");

    if (id == 0) {
        return -1;
    }

    int index = findDoctorById(list, count, id);

    if (index == -1 || list[index].active == 0) {
        printf("Invalid doctor ID.\n");
        return -1;
    }

    return id;
}

void doctorEditProfile(int doctorId)
{
    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    int index = findDoctorById(list, count, doctorId);

    if (index == -1) {
        printf("Could not find your profile.\n");
        return;
    }

    printf("\n--- Edit Profile (leave blank to keep the current value) ---\n");

    char specialization[SPEC_LEN];
    char contact[CONTACT_LEN];
    char password[PASSWORD_LEN];

    printf("Specialization [%s]: ", list[index].specialization);
    readLine(specialization, sizeof(specialization));

    if (strlen(specialization) > 0) {
        strcpy(list[index].specialization, specialization);
    }

    printf("Contact [%s]: ", list[index].contact);
    readLine(contact, sizeof(contact));

    if (strlen(contact) > 0) {
        strcpy(list[index].contact, contact);
    }

    printf("New password (leave blank to keep current): ");
    readLine(password, sizeof(password));

    if (strlen(password) > 0) {
        strcpy(list[index].password, password);
    }

    saveDoctors(list, count);

    printf("Profile updated.\n");
}

static void doctorScheduleFollowupStandalone(int doctorId)
{
    Appointment list[MAX_APPOINTMENTS];
    int count = loadAppointments(list);

    printf("\n--- My Completed Appointments ---\n");

    int found = 0;
    int i;

    for (i = 0; i < count; i++) {

        if (list[i].doctorId == doctorId &&
            list[i].status == APPT_COMPLETED) {

            printf("ID %d | Patient #%d | %s %s\n",
                   list[i].id,
                   list[i].patientId,
                   list[i].date,
                   list[i].time);

            found = 1;
        }
    }

    if (found == 0) {
        printf("(no completed appointments yet)\n");
        return;
    }

    int apptId = readInt(
        "\nEnter appointment ID to schedule a follow-up for (0 to cancel): "
    );

    if (apptId == 0) {
        return;
    }

    int index = findAppointmentById(list, count, apptId);

    if (index == -1 ||
        list[index].doctorId != doctorId ||
        list[index].status != APPT_COMPLETED) {

        printf("Invalid appointment.\n");
        return;
    }

    scheduleFollowup(doctorId, list[index].patientId, apptId);
}

void runDoctorMenu(int doctorId)
{
    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    int index = findDoctorById(list, count, doctorId);

    if (index == -1) {
        printf("Doctor profile not found.\n");
        return;
    }

    char name[NAME_LEN];
    strcpy(name, list[index].name);

    int running = 1;

    while (running) {

        printf("\n===== Doctor Menu (Dr. %s) =====\n", name);
        printf("1. View and manage my appointments\n");
        printf("2. View a patient's medical history\n");
        printf("3. Prescribe medicines/tests and complete an appointment\n");
        printf("4. Schedule a follow-up for a patient\n");
        printf("5. View my ratings and comments\n");
        printf("6. Edit my profile\n");
        printf("7. Request blood for a patient\n");
        printf("8. View blood bank inventory\n");
        printf("9. View blood requests I have filed\n");
        printf("10. Logout\n");

        int choice = readIntRange("Choice: ", 1, 10);

        if (choice == 1) {
            doctorManageAppointments(doctorId);
        }
        else if (choice == 2) {
            doctorViewPatientHistory();
        }
        else if (choice == 3) {
            doctorPrescribeAndComplete(doctorId);
        }
        else if (choice == 4) {
            doctorScheduleFollowupStandalone(doctorId);
        }
        else if (choice == 5) {
            viewDoctorRatings(doctorId);
        }
        else if (choice == 6) {
            doctorEditProfile(doctorId);
        }
        else if (choice == 7) {

            Patient patients[MAX_PATIENTS];
            int pcount = loadPatients(patients);

            int patientId =
                readInt("Which Patient ID is this blood request for? ");

            if (findPatientById(patients, pcount, patientId) == -1) {
                printf("No patient found with that ID.\n");
            }
            else {
                requestBlood(doctorId, ROLE_DOCTOR, patientId);
            }
        }
        else if (choice == 8) {
            viewBloodInventory();
        }
        else if (choice == 9) {
            viewMyBloodRequestsDoctor(doctorId);
        }
        else if (choice == 10) {
            printf("Logging out...\n");
            running = 0;
        }
    }
}