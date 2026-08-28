#include <stdio.h>
#include <string.h>
#include "patient.h"
#include "common.h"
#include "utils.h"
#include "doctor.h"
#include "appointment.h"
#include "medrecord.h"
#include "rating.h"
#include "ambulance.h"
#include "bloodbank.h"

int loadPatients(Patient list[]) 
{
    FILE *fp = fopen(FILE_PATIENTS, "rb");
    if (fp == NULL) {
        return 0; /* file does not exist yet, so there are no patients */
    }
    int count = fread(list, sizeof(Patient), MAX_PATIENTS, fp);
    fclose(fp);
    return count;
}

void savePatients(Patient list[], int count) 
{
    FILE *fp = fopen(FILE_PATIENTS, "wb");
    if (fp == NULL) 
    {
        printf("Error: could not save patient data.\n");
        return;
    }
    fwrite(list, sizeof(Patient), count, fp);
    fclose(fp);
}

int findPatientById(Patient list[], int count, int id) 
{
    int i;
    for (i = 0; i < count; i++) 
    {
        if (list[i].id == id) 
        {
            return i;
        }
    }
    return -1;
}

static int usernameTakenPatient(Patient list[], int count, const char *username) 
{
    int i;
    for (i = 0; i < count; i++) 
    {
        if (strcmp(list[i].username, username) == 0) 
        {
            return 1;
        }
    }
    return 0;
}

void registerPatient(void) 
{
    Patient list[MAX_PATIENTS];
    int count = loadPatients(list);

    if (count >= MAX_PATIENTS)
    {
        printf("Sorry, the patient list is full.\n");
        return;
    }

    Patient p;
    p.id = count + 1;

    while (1) 
    {
        readString("Choose a username: ", p.username, USERNAME_LEN);
        if (usernameTakenPatient(list, count, p.username)) 
        {
            printf("That username is already taken, please choose another.\n");
        } 
        else 
        {
            break;
        }
    }

    readString("Choose a password: ", p.password, PASSWORD_LEN);
    readString("Full name: ", p.name, NAME_LEN);
    p.age = readInt("Age: ");
    readString("Gender: ", p.gender, GENDER_LEN);
    readString("Contact number: ", p.contact, CONTACT_LEN);
    readString("Address: ", p.address, ADDRESS_LEN);

    while (1) 
    {
        readString("Blood group (A+, A-, B+, B-, AB+, AB-, O+, O-): ", p.bloodGroup, BLOODGRP_LEN);
        if (isValidBloodGroup(p.bloodGroup)) 
        {
            break;
        }
        printf("Please enter a valid blood group.\n");
    }

    p.active = 1;

    list[count] = p;
    count = count + 1;
    savePatients(list, count);

    printf("\nRegistration successful! You can now log in with your username and password.\n");
}

int loginPatient(int *outPatientId) 
{
    Patient list[MAX_PATIENTS];
    int count = loadPatients(list);

    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    readString("Username: ", username, USERNAME_LEN);
    readString("Password: ", password, PASSWORD_LEN);

    int i;
    for (i = 0; i < count; i++) 
    {
        if (strcmp(list[i].username, username) == 0) 
        {
            if (list[i].active == 0) 
            {
                printf("This account has been deactivated.\n");
                return 0;
            }
            if (strcmp(list[i].password, password) == 0) 
            {
                *outPatientId = list[i].id;
                return 1;
            }
            printf("Incorrect password.\n");
            return 0;
        }
    }

    printf("No patient found with that username.\n");
    return 0;
}

void patientEditProfile(int patientId) 
{
    Patient list[MAX_PATIENTS];
    int count = loadPatients(list);
    int index = findPatientById(list, count, patientId);
    if (index == -1) 
    {
        printf("Could not find your profile.\n");
        return;
    }

    printf("\n--- Edit Profile (leave blank to keep the current value) ---\n");
    char temp[ADDRESS_LEN];

    printf("Contact [%s]: ", list[index].contact);
    readLine(temp, sizeof(temp));
    if (strlen(temp) > 0) 
    {
        strcpy(list[index].contact, temp);
    }

    printf("Address [%s]: ", list[index].address);
    readLine(temp, sizeof(temp));
    if (strlen(temp) > 0) 
    {
        strcpy(list[index].address, temp);
    }

    printf("New password (leave blank to keep current): ");
    readLine(temp, sizeof(temp));
    if (strlen(temp) > 0) 
    {
        strcpy(list[index].password, temp);
    }

    savePatients(list, count);
    printf("Profile updated.\n");
}

static void patientBookAppointmentFlow(int patientId) 
{
    int doctorId = pickDoctorForBooking();
    if (doctorId == -1) 
    {
        return;
    }
    bookAppointment(patientId, doctorId);
}

static void patientViewDoctorRatingsFlow(void) 
{
    Doctor doctors[MAX_DOCTORS];
    int count = loadDoctors(doctors);
    int doctorId = readInt("Enter Doctor ID to view ratings for: ");
    int index = findDoctorById(doctors, count, doctorId);
    if (index == -1) 
    {
        printf("No doctor found with that ID.\n");
        return;
    }
    viewDoctorRatings(doctorId);
}

static void patientBloodBankMenu(int patientId) 
{
    printf("\n--- Blood Bank ---\n");
    printf("1. Donate blood\n");
    printf("2. Request blood\n");
    printf("3. View my donations\n");
    printf("4. View my blood requests\n");
    printf("5. View current inventory\n");
    printf("6. Back\n");

    int choice = readIntRange("Choice: ", 1, 6);
    if (choice == 1) 
    {
        donateBlood(patientId);
    } 
    else if (choice == 2) 
    {
        requestBlood(patientId, ROLE_PATIENT, patientId);
    } 
    else if (choice == 3) 
    {
        viewMyDonations(patientId);
    } 
    else if (choice == 4) 
    {
        viewMyBloodRequestsPatient(patientId);
    } 
    else if (choice == 5) 
    {
        viewBloodInventory();
    }
}

void runPatientMenu(int patientId) 
{
    Patient list[MAX_PATIENTS];
    int count = loadPatients(list);
    int index = findPatientById(list, count, patientId);

    if (index == -1) 
    {
    printf("Patient profile not found.\n");
    return;
    }

    char name[NAME_LEN];
    strcpy(name, list[index].name);

    int running = 1;
    while (running) 
    {
        printf("\n===== Patient Menu (%s) =====\n", name);
        printf("1. Search doctors\n");
        printf("2. Book an appointment\n");
        printf("3. Book emergency ambulance\n");
        printf("4. View my appointments (upcoming and previous)\n");
        printf("5. View my prescriptions and test info\n");
        printf("6. View my medical history\n");
        printf("7. Rate and comment on a doctor\n");
        printf("8. View ratings and comments for a doctor\n");
        printf("9. Edit my profile\n");
        printf("10. Blood bank\n");
        printf("11. View my ambulance requests\n");
        printf("12. Logout\n");

        int choice = readIntRange("Choice: ", 1, 12);

        if (choice == 1) 
        {
            searchDoctors();
        } 
        else if (choice == 2) 
        {
            patientBookAppointmentFlow(patientId);
        } 
        else if (choice == 3) 
        {
            bookAmbulance(patientId);
        } 
        else if (choice == 4) 
        {
            viewPatientAppointments(patientId);
        } 
        else if (choice == 5) 
        {
            viewPatientPrescriptions(patientId);
        } 
        else if (choice == 6) 
        {
            viewPatientMedicalHistory(patientId);
        } 
        else if (choice == 7) 
        {
            rateDoctor(patientId);
        } 
        else if (choice == 8) 
        {
            patientViewDoctorRatingsFlow();
        } 
        else if (choice == 9) 
        {
            patientEditProfile(patientId);
        } 
        else if (choice == 10) 
        {
            patientBloodBankMenu(patientId);
        } 
        else if (choice == 11) 
        {
            viewMyAmbulanceRequests(patientId);
        } 
        else if (choice == 12) 
        {
            printf("Logging out...\n");
            running = 0;
        }
    }
}