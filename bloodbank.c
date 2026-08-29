#include <stdio.h>
#include <string.h>
#include "bloodbank.h"
#include "common.h"
#include "utils.h"


static int loadDonations(Donation list[]){
    FILE *fp = fopen(FILE_DONATIONS, "rb");

    if(fp == NULL){
        return 0;
    }

    int count = fread(list, sizeof(Donation), MAX_DONATIONS, fp);

    fclose(fp);

    return count;
}


static void saveDonations(Donation list[], int count){
    FILE *fp = fopen(FILE_DONATIONS, "wb");

    if(fp == NULL){
        printf("Error: could not save donation data.\n");
        return;
    }

    fwrite(list, sizeof(Donation), count, fp);

    fclose(fp);
}


static int loadBloodRequests(BloodRequest list[]){
    FILE *fp = fopen(FILE_BLOODREQ, "rb");

    if(fp == NULL){
        return 0;
    }

    int count = fread(list, sizeof(BloodRequest), MAX_BLOODREQ, fp);

    fclose(fp);

    return count;
}


static void saveBloodRequests(BloodRequest list[], int count){
    FILE *fp = fopen(FILE_BLOODREQ, "wb");

    if(fp == NULL){
        printf("Error: could not save blood request data.\n");
        return;
    }

    fwrite(list, sizeof(BloodRequest), count, fp);

    fclose(fp);
}


static int loadBloodStock(BloodStock list[]){
    FILE *fp = fopen(FILE_BLOODSTOCK, "rb");

    if(fp == NULL){
        return 0;
    }

    int count = fread(list, sizeof(BloodStock), 8, fp);

    fclose(fp);

    return count;
}


static void saveBloodStock(BloodStock list[], int count){
    FILE *fp = fopen(FILE_BLOODSTOCK, "wb");

    if(fp == NULL){
        printf("Error: could not save blood stock data.\n");
        return;
    }

    fwrite(list, sizeof(BloodStock), count, fp);

    fclose(fp);
}


void initializeBloodStock(void){
    BloodStock stock[8];
    int count = loadBloodStock(stock);

    if(count > 0){
        return;
    }

    char groups[8][BLOODGRP_LEN] = {"A+", "A-", "B+", "B-","AB+", "AB-", "O+", "O-"};

    for(int i = 0; i < 8; i++){
        strcpy(stock[i].bloodGroup, groups[i]);
        stock[i].units = 0;
    }

    saveBloodStock(stock, 8);
}


void donateBlood(int donorId){
    Donation list[MAX_DONATIONS];
    int count = loadDonations(list);

    if(count >= MAX_DONATIONS){
        printf("Sorry, the donation list is full.\n");
        return;
    }

    Donation d;

    d.id = count + 1;
    d.donorId = donorId;

    while(1){
        readString("Your blood group (A+, A-, B+, B-, AB+, AB-, O+, O-): ", d.bloodGroup, BLOODGRP_LEN);

        if(isValidBloodGroup(d.bloodGroup)){
            break;
        }

        printf("Please enter a valid blood group.\n");
    }

    getToday(d.date);

    d.status = DONATE_PENDING;

    list[count] = d;

    count = count + 1;

    saveDonations(list, count);

    printf("\nThanks! Your donation request (ID %d) is recorded as PENDING.\n", d.id);
    printf("An admin will confirm it and add it to the blood bank stock.\n");
}


void requestBlood(int requesterId, int requesterRole, int forPatientId){
    BloodRequest list[MAX_BLOODREQ];
    int count = loadBloodRequests(list);

    if(count >= MAX_BLOODREQ){
        printf("Sorry, the blood request list is full.\n");
        return;
    }

    BloodRequest r;

    r.id = count + 1;
    r.requesterId = requesterId;
    r.requesterRole = requesterRole;
    r.forPatientId = forPatientId;

    while(1){
        readString("Blood group needed (A+, A-, B+, B-, AB+, AB-, O+, O-): ", r.bloodGroup, BLOODGRP_LEN);

        if(isValidBloodGroup(r.bloodGroup)){
            break;
        }

        printf("Please enter a valid blood group.\n");
    }

    r.units = readIntRange("Units needed: ", 1, 100);

    getToday(r.date);

    r.status = REQ_PENDING;

    list[count] = r;

    count = count + 1;

    saveBloodRequests(list, count);

    printf("\nBlood request (ID %d) submitted as PENDING. An admin will review it.\n", r.id);
}


void viewMyDonations(int patientId){
    Donation list[MAX_DONATIONS];
    int count = loadDonations(list);

    char statusText[3][12] = {
        "PENDING",
        "CONFIRMED",
        "REJECTED"
    };

    printf("\n--- My Blood Donations ---\n");

    int found = 0;
    int i;

    for(i = 0; i < count; i++){
        if(list[i].donorId == patientId){
            printf("ID %d | %s | %s | Status: %s\n",
                   list[i].id,
                   list[i].bloodGroup,
                   list[i].date,
                   statusText[list[i].status]);

            found = 1;
        }
    }

    if(found == 0){
        printf("(none yet)\n");
    }
}


void viewMyBloodRequestsPatient(int patientId){
    BloodRequest list[MAX_BLOODREQ];
    int count = loadBloodRequests(list);

    char statusText[3][12] = {
        "PENDING",
        "APPROVED",
        "REJECTED"
    };

    printf("\n--- My Blood Requests ---\n");

    int found = 0;

    for(int i = 0; i < count; i++){
        if(list[i].requesterRole == ROLE_PATIENT &&
           list[i].requesterId == patientId){

            printf("ID %d | %s | %d unit(s) | %s | Status: %s\n",
                   list[i].id,
                   list[i].bloodGroup,
                   list[i].units,
                   list[i].date,
                   statusText[list[i].status]);

            found = 1;
        }
    }

    if(found == 0){
        printf("(none yet)\n");
    }
}


void viewMyBloodRequestsDoctor(int doctorId){
    BloodRequest list[MAX_BLOODREQ];
    int count = loadBloodRequests(list);

    char statusText[3][12] = {
        "PENDING",
        "APPROVED",
        "REJECTED"
    };

    printf("\n--- Blood Requests I Filed For Patients ---\n");

    int found = 0;

    for(int i = 0; i < count; i++){
        if(list[i].requesterRole == ROLE_DOCTOR &&
           list[i].requesterId == doctorId){

            printf("ID %d | For Patient #%d | %s | %d unit(s) | %s | Status: %s\n",
                   list[i].id,
                   list[i].forPatientId,
                   list[i].bloodGroup,
                   list[i].units,
                   list[i].date,
                   statusText[list[i].status]);

            found = 1;
        }
    }

    if(found == 0){
        printf("(none yet)\n");
    }
}


void viewBloodInventory(void){
    BloodStock stock[8];
    int count = loadBloodStock(stock);

    printf("\n--- Blood Bank Inventory ---\n");

    for(int i = 0; i < count; i++){
        printf("%-4s : %d unit(s)\n",
               stock[i].bloodGroup,
               stock[i].units);
    }
}


void adminReviewBloodRequests(void){
    BloodRequest list[MAX_BLOODREQ];
    int count = loadBloodRequests(list);

    printf("\n--- Pending Blood Requests ---\n");

    int any = 0;

    for(int i = 0; i < count; i++){
        if(list[i].status == REQ_PENDING){

            char roleText[10];

            if(list[i].requesterRole == ROLE_DOCTOR){
                strcpy(roleText, "Doctor");
            }
            else{
                strcpy(roleText, "Patient");
            }

            printf("ID %d | Filed by %s #%d | For Patient #%d | %s | %d unit(s) | %s\n",
                   list[i].id,
                   roleText,
                   list[i].requesterId,
                   list[i].forPatientId,
                   list[i].bloodGroup,
                   list[i].units,
                   list[i].date);

            any = 1;
        }
    }

    if(any == 0){
        printf("(no pending requests)\n");
        return;
    }

    int id = readInt("\nEnter request ID to review (0 to cancel): ");

    if(id == 0){
        return;
    }

    int index = -1;

    for(int i = 0; i < count; i++){
        if(list[i].id == id){
            index = i;
            break;
        }
    }

    if(index == -1 || list[index].status != REQ_PENDING){
        printf("Invalid or already-resolved request ID.\n");
        return;
    }

    int decision = readIntRange("1. Approve  2. Reject : ", 1, 2);

    if(decision == 1){

        BloodStock stock[8];
        int scount = loadBloodStock(stock);

        int sindex = bloodGroupIndex(list[index].bloodGroup);

        if(sindex == -1){
            printf("Invalid blood group.\n");
            return;
        }

        if(sindex >= scount){
            printf("Blood group not found in stock.\n");
            return;
        }

        if(stock[sindex].units < list[index].units){
            printf("Not enough units in stock. %s has %d unit(s).\n",
                   list[index].bloodGroup,
                   stock[sindex].units);
            return;
        }

        stock[sindex].units = stock[sindex].units - list[index].units;

        saveBloodStock(stock, scount);

        list[index].status = REQ_APPROVED;

        saveBloodRequests(list, count);

        printf("Request approved. Stock updated.\n");
    }
    else{
        list[index].status = REQ_REJECTED;

        saveBloodRequests(list, count);

        printf("Request rejected.\n");
    }
}


void adminReviewDonations(void){
    Donation list[MAX_DONATIONS];
    int count = loadDonations(list);

    printf("\n--- Pending Donations ---\n");

    int any = 0;

    for(int i = 0; i < count; i++){
        if(list[i].status == DONATE_PENDING){

            printf("ID %d | Donor #%d | %s | %s\n",
                   list[i].id,
                   list[i].donorId,
                   list[i].bloodGroup,
                   list[i].date);

            any = 1;
        }
    }

    if(any == 0){
        printf("(no pending donations)\n");
        return;
    }

    int id = readInt("\nEnter donation ID to review (0 to cancel): ");

    if(id == 0){
        return;
    }

    int index = -1;

    for(int i = 0; i < count; i++){
        if(list[i].id == id){
            index = i;
            break;
        }
    }

    if(index == -1 || list[index].status != DONATE_PENDING){
        printf("Invalid or already-resolved donation ID.\n");
        return;
    }

    int decision = readIntRange(
        "1. Confirm (adds 1 unit to stock)  2. Reject : ", 1, 2);

    if(decision == 1){

        BloodStock stock[8];
        int scount = loadBloodStock(stock);

        int sindex = bloodGroupIndex(list[index].bloodGroup);

        if(sindex != -1 && sindex < scount){

            stock[sindex].units = stock[sindex].units + 1;

            saveBloodStock(stock, scount);
        }

        list[index].status = DONATE_CONFIRMED;

        saveDonations(list, count);

        printf("Donation confirmed. Stock updated.\n");
    }
    else{
        list[index].status = DONATE_REJECTED;

        saveDonations(list, count);

        printf("Donation marked rejected.\n");
    }
}