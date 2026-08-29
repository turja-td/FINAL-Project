#pragma once

#include "common.h"

int loadPatients(Patient list[]);
void savePatients(Patient list[], int count);
int findPatientById(Patient list[], int count, int id);
void registerPatient(void);
int  loginPatient(int *outPatientId);
void patientEditProfile(int patientId);
void runPatientMenu(int patientId);
