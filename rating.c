#include <stdio.h>
#include <string.h>
#include "rating.h"
#include "common.h"
#include "utils.h"
#include "appointment.h"
#include "doctor.h"

static int loadRatings(Rating list[]) 
{
    FILE *fp = fopen(FILE_RATINGS, "rb");
    if(fp == NULL) 
    {
        return 0;
    }
    int count = fread(list, sizeof(Rating), MAX_RATINGS, fp);
    fclose(fp);
    return count;
}

static void saveRatings(Rating list[], int count) 
{
    FILE *fp = fopen(FILE_RATINGS, "wb");
    if(fp == NULL) 
    {
        printf("Error: could not save rating data.\n");
        return;
    }
    fwrite(list, sizeof(Rating), count, fp);
    fclose(fp);
}

void rateDoctor(int patientId) 
{
    Appointment appts[MAX_APPOINTMENTS];
    int acount = loadAppointments(appts);
    Rating ratings[MAX_RATINGS];
    int rcount = loadRatings(ratings);
    int i, j;

    printf("\n--- Completed Appointments You Can Rate ---\n");
    int found = 0;
    for(i = 0; i < acount; i++) 
    {
        if(appts[i].patientId != patientId || appts[i].status != APPT_COMPLETED) continue;

        int alreadyRated = 0;
        for(j = 0; j < rcount; j++) 
        {
            if (ratings[j].appointmentId == appts[i].id) 
            {
                alreadyRated = 1;
                break;
            }
        }
        if(alreadyRated)
        {
            continue;
        }

        printf("Appointment ID %d | Doctor #%d | %s\n", appts[i].id, appts[i].doctorId, appts[i].date);
        found = 1;
    }
    if(found == 0) 
    {
        printf("(nothing left to rate)\n");
        return;
    }

    int apptId = readInt("\nEnter appointment ID to rate (0 to cancel): ");
    if(apptId == 0)
    {
        return;
    }

    int aindex = findAppointmentById(appts, acount, apptId);
    if(aindex == -1 || appts[aindex].patientId != patientId || appts[aindex].status != APPT_COMPLETED) 
    {
        printf("Invalid appointment.\n");
        return;
    }
    for(j = 0; j < rcount; j++) 
    {
        if(ratings[j].appointmentId == apptId) 
        {
            printf("You have already rated this appointment.\n");
            return;
        }
    }

    Rating r;
    r.id = rcount + 1;
    r.doctorId = appts[aindex].doctorId;
    r.patientId = patientId;
    r.appointmentId = apptId;
    r.stars = readIntRange("Stars (1-5): ", 1, 5);
    readString("Comment: ", r.comment, TEXT_LEN);
    getToday(r.date);

    ratings[rcount] = r;
    rcount = rcount + 1;
    saveRatings(ratings, rcount);

    Doctor doctors[MAX_DOCTORS];
    int dcount = loadDoctors(doctors);
    int dindex = findDoctorById(doctors, dcount, r.doctorId);
    if (dindex != -1) 
    {
        float total = doctors[dindex].avgRating * doctors[dindex].ratingCount + r.stars; 
        doctors[dindex].ratingCount = doctors[dindex].ratingCount + 1;
        doctors[dindex].avgRating = total / doctors[dindex].ratingCount;
        saveDoctors(doctors, dcount);
    }

    printf("Thanks for your feedback!\n");
}

void viewDoctorRatings(int doctorId) 
{
    Rating ratings[MAX_RATINGS];
    int count = loadRatings(ratings);

    Doctor doctors[MAX_DOCTORS];
    int dcount = loadDoctors(doctors);
    int dindex = findDoctorById(doctors, dcount, doctorId);

    if(dindex != -1) 
    {
        printf("\n--- Ratings for Dr. %s (average %.1f from %d review(s)) ---\n", doctors[dindex].name, doctors[dindex].avgRating, doctors[dindex].ratingCount);
    } 
    else 
    {
        printf("\n--- Ratings for Doctor #%d ---\n", doctorId);
    }

    int found = 0;
    int i;
    for(i = 0; i < count; i++) 
    {
        if(ratings[i].doctorId == doctorId) 
        {
            printf("[%s] %d/5 stars -- \"%s\"\n", ratings[i].date, ratings[i].stars, ratings[i].comment);
            found = 1;
        }
    }
    if(found == 0)
    {
        printf("(no ratings yet)\n");
    }
}
