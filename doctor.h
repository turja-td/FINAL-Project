#pragma once

#include "common.h"

int loadDoctors(Doctor list[]);
void saveDoctors(Doctor list[], int count);
int findDoctorById(Doctor list[], int count, int id);

int loginDoctor(int *outDoctorId);

void searchDoctors(void);
int pickDoctorForBooking(void);

void doctorEditProfile(int doctorId);
void runDoctorMenu(int doctorId);


