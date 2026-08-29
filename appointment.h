#pragma once
#include "common.h"

int  loadAppointments(Appointment list[]);
void saveAppointments(Appointment list[], int count);
int  findAppointmentById(Appointment list[], int count, int id);
const char *appointmentStatusText(int status);
int  bookAppointment(int patientId, int doctorId);
void viewPatientAppointments(int patientId);
void doctorManageAppointments(int doctorId);
void scheduleFollowup(int doctorId, int patientId, int parentId);

