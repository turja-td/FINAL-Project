#include <stdio.h>
#include <string.h>
#include "appointment.h"
#include "common.h"
#include "utils.h"

int loadAppointments(Appointment list[])
{
    FILE *file=fopen(FILE_APPOINTMENTS,"rb")
    if(fp==NULL)
    return 0;
    int count=fread(list,sizeof(Appointment),MAX_APPOINTMENTS,fp);
    fclose(fp);
    return count;
}

void saveAppointments(Appointment list[], int count)
 {
    FILE *fp = fopen(FILE_APPOINTMENTS, "wb");
    if (fp == NULL) 
    {
        printf("Error: could not save appointment data.\n");
        return;
    }
    fwrite(list, sizeof(Appointment), count, fp);
    fclose(fp);
}

int findAppointmentById(Appointment list[], int count, int id)
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

const char *appointmentStatusText(int status) 
{
    if (status == APPT_PENDING) 
    return "PENDING";
    if (status == APPT_CONFIRMED) 
    return "CONFIRMED";
    if (status == APPT_COMPLETED) 
    return "COMPLETED";
    if (status == APPT_CANCELLED) 
    return "CANCELLED";
    return "UNKNOWN";
}

int bookAppointment(int patientId, int doctorId)
 {
    Appointment list[MAX_APPOINTMENTS];
    int count = loadAppointments(list);

    if (count >= MAX_APPOINTMENTS)
     {
        printf("Sorry, the appointment list is full.\n");
        return -1;
    }

    Appointment a;
    a.id = count + 1;
    a.patientId = patientId;
    a.doctorId = doctorId;

    while (1)
     {
        readString("Appointment date (YYYY-MM-DD): ", a.date, DATE_LEN);
        if (isValidDateFormat(a.date)) break;
        printf("Please use the format YYYY-MM-DD.\n");
    }
    while (1) 
    {
        readString("Appointment time (HH:MM): ", a.time, TIME_LEN);
        if (isValidTimeFormat(a.time)) break;
        printf("Please use the format HH:MM.\n");
    }

    a.status = APPT_PENDING;
    a.isFollowup = 0;
    a.parentId = -1;

    list[count] = a;
    count = count + 1;
    saveAppointments(list, count);

    printf("\nAppointment booked (ID %d). Status is PENDING until the doctor confirms it.\n", a.id);
    return a.id;
}

void viewPatientAppointments(int patientId)
 {
    Appointment list[MAX_APPOINTMENTS];
    int count = loadAppointments(list);
    int i;

    printf("\n--- Upcoming / Pending Appointments ---\n");
    int found = 0;
    for (i = 0; i < count; i++)
     {
        if (list[i].patientId != patientId) continue;
        if (list[i].status == APPT_PENDING || list[i].status == APPT_CONFIRMED)
         {
            printf("ID %d | Doctor #%d | %s %s | %s%s\n",
                   list[i].id, list[i].doctorId, list[i].date, list[i].time,
                   appointmentStatusText(list[i].status),
                   list[i].isFollowup ? " (follow-up)" : "");
            found = 1;
        }
    }
    if (found == 0) printf("(none)\n");

    printf("\n--- Previous Appointments ---\n");
    found = 0;
    for (i = 0; i < count; i++) 
    {
        if (list[i].patientId != patientId) continue;
        if (list[i].status == APPT_COMPLETED || list[i].status == APPT_CANCELLED)
         {
            printf("ID %d | Doctor #%d | %s %s | %s%s\n",
                   list[i].id, list[i].doctorId, list[i].date, list[i].time,
                   appointmentStatusText(list[i].status),
                   list[i].isFollowup ? " (follow-up)" : "");
            found = 1;
        }
    }
    if (found == 0) printf("(none)\n");
}

void doctorManageAppointments(int doctorId) 
{
    Appointment list[MAX_APPOINTMENTS];
    int count = loadAppointments(list);
    int i;

    printf("\n--- My Appointments ---\n");
    int found = 0;
    for (i = 0; i < count; i++) 
    {
        if (list[i].doctorId != doctorId) continue;
        printf("ID %d | Patient #%d | %s %s | %s%s\n",
               list[i].id, list[i].patientId, list[i].date, list[i].time,
               appointmentStatusText(list[i].status),
               list[i].isFollowup ? " (follow-up)" : "");
        found = 1;
    }
    if (found == 0) 
    {
        printf("(none)\n");
        return;
    }

    int id = readInt("\nEnter appointment ID to update (0 to go back): ");
    if (id == 0)
     return;

    int index = findAppointmentById(list, count, id);
    if (index == -1 || list[index].doctorId != doctorId)
    {
        printf("Invalid appointment ID.\n");
        return;
    }
    if (list[index].status == APPT_COMPLETED || list[index].status == APPT_CANCELLED) 
    {
        printf("This appointment is already %s.\n", appointmentStatusText(list[index].status));
        return;
    }

    printf("1. Confirm\n2. Cancel\n3. Back\n");
    printf("(To mark it COMPLETED, use option 3 'Prescribe and complete' in the doctor menu instead.)\n");
    int choice = readIntRange("Choice: ", 1, 3);
    if (choice == 1) 
    {
        list[index].status = APPT_CONFIRMED;
        saveAppointments(list, count);
        printf("Appointment confirmed.\n");
    } else if (choice == 2) 
    {
        list[index].status = APPT_CANCELLED;
        saveAppointments(list, count);
        printf("Appointment cancelled.\n");
    }
}

void scheduleFollowup(int doctorId, int patientId, int parentId) {
    Appointment list[MAX_APPOINTMENTS];
    int count = loadAppointments(list);

    if (count >= MAX_APPOINTMENTS) {
        printf("Sorry, the appointment list is full.\n");
        return;
    }

    Appointment a;
    a.id = count + 1;
    a.patientId = patientId;
    a.doctorId = doctorId;

    while (1) {
        readString("Follow-up date (YYYY-MM-DD): ", a.date, DATE_LEN);
        if (isValidDateFormat(a.date)) break;
        printf("Please use the format YYYY-MM-DD.\n");
    }
    while (1) {
        readString("Follow-up time (HH:MM): ", a.time, TIME_LEN);
        if (isValidTimeFormat(a.time)) break;
        printf("Please use the format HH:MM.\n");
    }

    a.status = APPT_CONFIRMED;
    a.isFollowup = 1;
    a.parentId = parentId;

    list[count] = a;
    count = count + 1;
    saveAppointments(list, count);

    printf("Follow-up scheduled (Appointment ID %d).\n", a.id);
}