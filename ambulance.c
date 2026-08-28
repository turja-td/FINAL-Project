#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ambulance.h"
#include "common.h"
#include "utils.h"

static int loadAmbulance(Ambulance list[]) 
{
    FILE *fp = fopen(FILE_AMBULANCE, "rb");
    if (fp == NULL) 
    {
        return 0;
    }
    int count = fread(list, sizeof(Ambulance), MAX_AMBULANCE, fp);
    fclose(fp);
    return count;
}

static void saveAmbulance(Ambulance list[], int count) 
{
    FILE *fp = fopen(FILE_AMBULANCE, "wb");
    if (fp == NULL) 
    {
        printf("Error: could not save ambulance data.\n");
        return;
    }
    fwrite(list, sizeof(Ambulance), count, fp);
    fclose(fp);
}

void bookAmbulance(int patientId) 
{
    Ambulance list[MAX_AMBULANCE];
    int count = loadAmbulance(list);

    if (count >= MAX_AMBULANCE) 
    {
        printf("Sorry, the ambulance request list is full.\n");
        return;
    }

    Ambulance r;
    r.id = count + 1;
    r.patientId = patientId;
    readString("Pickup location / address: ", r.location, LOCATION_LEN);
    readString("Contact number: ", r.contact, CONTACT_LEN);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(r.requestTime, sizeof(r.requestTime), "%Y-%m-%d %H:%M", t);

    r.status = AMB_DISPATCHED;

    list[count] = r;
    count = count + 1;
    saveAmbulance(list, count);

    printf("\nEmergency ambulance requested (ID %d). Help is on the way!\n", r.id);
}

void viewMyAmbulanceRequests(int patientId) 
{
    Ambulance list[MAX_AMBULANCE];
    int count = loadAmbulance(list);

    printf("\n--- My Ambulance Requests ---\n");
    int found = 0;
    int i;
    for (i = 0; i < count; i++) 
    {
        if (list[i].patientId == patientId) 
        {
            const char *statusText = (list[i].status == AMB_PENDING) ? "PENDING" : "DISPATCHED";
            printf("ID %d | %s | %s | Status: %s\n", list[i].id, list[i].location, list[i].requestTime, statusText);
            found = 1;
        }
    }
    if (found == 0) 
        printf("(none yet)\n");
}
