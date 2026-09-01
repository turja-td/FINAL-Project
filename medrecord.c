#include <stdio.h>
#include <string.h>
#include "medrecord.h"
#include "common.h"
#include "utils.h"
#include "appointment.h"
#include "patient.h"

static int loadMedRecords(MedRecord list[]) 
{
    FILE *fp = fopen(FILE_MEDRECORDS, "rb");
    if(fp == NULL) 
    {
        return 0;
    }
    int count = fread(list, sizeof(MedRecord), MAX_MEDRECORDS, fp);
    fclose(fp);
    return count;
}

static void saveMedRecords(MedRecord list[], int count)
 {
    FILE *fp = fopen(FILE_MEDRECORDS, "wb");
    if(fp == NULL) 
    {
        printf("Error!could not save medical record data.\n");
        return;
    }
    fwrite(list, sizeof(MedRecord), count, fp);
    fclose(fp);
}

static int loadPrescriptions(Prescription list[])
{
    FILE *fp = fopen(FILE_PRESCRIPTIONS, "rb");
    if(fp == NULL) 
    {
        return 0;
    }
    int count = fread(list, sizeof(Prescription), MAX_PRESCRIPTIONS, fp);
    fclose(fp);
    return count;
}

static void savePrescriptions(Prescription list[], int count) 
{
    FILE *fp = fopen(FILE_PRESCRIPTIONS, "wb");
    if(fp == NULL) 
    {
        printf("Error: could not save prescription data.\n");
        return;
    }
    fwrite(list, sizeof(Prescription), count, fp);
    fclose(fp);
}

void doctorPrescribeAndComplete(int doctorId) 
{
    Appointment appts[MAX_APPOINTMENTS];
    int acount = loadAppointments(appts);
    int i;

    printf("\n--- My Confirmed Appointments (ready to complete) ---\n");
    int found = 0;
    for(i = 0; i < acount; i++){
        if (appts[i].doctorId == doctorId && appts[i].status == APPT_CONFIRMED) {
            printf("ID %d | Patient #%d | %s %s\n", appts[i].id, appts[i].patientId, appts[i].date, appts[i].time);
            found = 1;
        }
    }
    if(found == 0){
        printf("(none - an appointment must be CONFIRMED before it can be completed)\n");
        return;
    }

    int apptId = readInt("\nEnter appointment ID to treat and complete (0 to cancel): ");
    if(apptId == 0) return;

    int index = findAppointmentById(appts, acount, apptId);
    if(index == -1 || appts[index].doctorId != doctorId || appts[index].status != APPT_CONFIRMED) {
        printf("Invalid appointment, or it isn't CONFIRMED yet.\n");
        return;
    }

    char today[DATE_LEN];
    getToday(today);

    // Diagnosis writing
    MedRecord recs[MAX_MEDRECORDS];
    int rcount = loadMedRecords(recs);
    MedRecord m;
    m.id = rcount + 1;
    m.patientId = appts[index].patientId;
    m.doctorId = doctorId;
    m.appointmentId = apptId;
    readString("Diagnosis / notes on the visit: ", m.diagnosis, TEXT_LEN);
    strcpy(m.date, today);
    recs[rcount] = m;
    rcount = rcount + 1;
    saveMedRecords(recs, rcount);

    // Prescription writing
    char answer[8];
    readString("Prescribe medicines and/or tests? (y/n): ", answer, sizeof(answer));
    if (answer[0] == 'y' || answer[0] == 'Y') {
        Prescription pres[MAX_PRESCRIPTIONS];
        int pcount = loadPrescriptions(pres);
        Prescription p;
        p.id = pcount + 1;
        p.appointmentId = apptId;
        p.patientId = appts[index].patientId;
        p.doctorId = doctorId;
        readString("Medicines (comma-separated, or leave blank): ", p.medicines, TEXT_LEN);
        readString("Tests (comma-separated, or leave blank): ", p.tests, TEXT_LEN);
        readString("Additional notes: ", p.notes, TEXT_LEN);
        strcpy(p.date, today);
        pres[pcount] = p;
        pcount = pcount + 1;
        savePrescriptions(pres, pcount);
        printf("Prescription saved.\n");
    }

    
    appts[index].status = APPT_COMPLETED;
    saveAppointments(appts, acount);
    printf("\nAppointment #%d marked COMPLETED.\n", apptId);

    char followup[8];
    readString("Schedule a follow-up for this patient now? (y/n): ", followup, sizeof(followup));
    if (followup[0] == 'y' || followup[0] == 'Y') {
        scheduleFollowup(doctorId, appts[index].patientId, apptId);
    }
}

void doctorViewPatientHistory(void) 
{
    int patientId = readInt("Enter Patient ID to view history: ");

    Patient patients[MAX_PATIENTS];
    int pcount = loadPatients(patients);
    int pindex = findPatientById(patients, pcount, patientId);
    if(pindex == -1){
        printf("No patient found with that ID.\n");
        return;
    }

    printf("\n--- Medical History for %s (Patient #%d) ---\n", patients[pindex].name, patientId);

    MedRecord recs[MAX_MEDRECORDS];
    int rcount = loadMedRecords(recs);
    int found = 0;
    int i;
    for(i = 0; i < rcount; i++){
        if(recs[i].patientId == patientId){
            printf("[%s] Dr#%d -- %s\n", recs[i].date, recs[i].doctorId, recs[i].diagnosis);
            found = 1;
        }
    }
    
    if(found == 0){
        printf("(no medical records yet)\n");
    }    

    printf("\n--- Prescriptions ---\n");
    Prescription pres[MAX_PRESCRIPTIONS];
    int prcount = loadPrescriptions(pres);
    found = 0;
    for(i = 0; i < prcount; i++){
        if(pres[i].patientId == patientId){
            printf("[%s] Dr#%d -- Meds: %s | Tests: %s\n", pres[i].date, pres[i].doctorId, pres[i].medicines, pres[i].tests);
            found = 1;
        }
    }
    if(found == 0){
        printf("(no prescriptions yet)\n");
    } 
}

void viewPatientPrescriptions(int patientId) 
{
    Prescription pres[MAX_PRESCRIPTIONS];
    int count = loadPrescriptions(pres);

    printf("\n--- My Prescriptions and Tests ---\n");
    int found = 0;
    int i;
    for(i = 0; i < count; i++){
        if(pres[i].patientId == patientId){
            printf("[%s] Dr#%d\n  Medicines: %s\n  Tests: %s\n  Notes: %s\n", pres[i].date, pres[i].doctorId, pres[i].medicines, pres[i].tests, pres[i].notes);
            found = 1;
        }
    }
    if (found == 0){
        printf("(none yet)\n");
    }
}

void viewPatientMedicalHistory(int patientId) {
    MedRecord recs[MAX_MEDRECORDS];
    int count = loadMedRecords(recs);

    printf("\n--- My Medical History ---\n");
    int found = 0;
    int i;
    for(i = 0; i < count; i++){
        if(recs[i].patientId == patientId){
            printf("[%s] Dr#%d -- %s\n", recs[i].date, recs[i].doctorId, recs[i].diagnosis);
            found = 1;
        }
    }
    if (found == 0){
        printf("(none yet)\n");
    } 
}