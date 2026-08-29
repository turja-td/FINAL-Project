#ifndef COMMON_H
#define COMMON_H


// Size Macros
#define MAX_PATIENTS 100
#define MAX_DOCTORS 30
#define MAX_ADMINS 5
#define MAX_APPOINTMENTS 100
#define MAX_PRESCRIPTIONS 100
#define MAX_MEDRECORDS 200
#define MAX_RATINGS 100
#define MAX_AMBULANCE 50
#define MAX_DONATIONS 200
#define MAX_BLOODREQ 200


// Length Macros
#define NAME_LEN 100
#define USERNAME_LEN 30
#define PASSWORD_LEN 20
#define CONTACT_LEN 20
#define GENDER_LEN 10
#define ADDRESS_LEN 100
#define BLOODGRP_LEN 10
#define SPEC_LEN 50
#define DATE_LEN 20
#define TIME_LEN 20
#define TEXT_LEN 300 
#define LOCATION_LEN 150



// File Macros
#define FILE_PATIENTS "data/patients.dat"
#define FILE_DOCTORS "data/doctors.dat"
#define FILE_ADMINS "data/admins.dat"
#define FILE_APPOINTMENTS "data/appointments.dat"
#define FILE_PRESCRIPTIONS "data/prescriptions.dat"
#define FILE_MEDRECORDS "data/medrecords.dat"
#define FILE_RATINGS "data/ratings.dat"
#define FILE_AMBULANCE "data/ambulance.dat"
#define FILE_DONATIONS "data/donations.dat"
#define FILE_BLOODREQ "data/bloodrequests.dat"
#define FILE_BLOODSTOCK "data/bloodstock.dat"


// Login Macros
#define ROLE_PATIENT 1
#define ROLE_DOCTOR 2
#define ROLE_ADMIN 3

// Appointment Status Macros
#define APPT_PENDING 0
#define APPT_CONFIRMED 1
#define APPT_COMPLETED 2
#define APPT_CANCELLED 3

// Ambulance Status Macros
#define AMBULANCE_PENDING 0
#define AMBULANCE_DISPATCHED 1

// Blood Donation Status Macros
#define DONATE_PENDING 0
#define DONATE_CONFIRMED 1
#define DONATE_REJECTED 2

// Blood Donation Request Macros
#define REQ_PENDING 0
#define REQ_APPROVED 1
#define REQ_REJECTED 2

// Global Structures
typedef struct {
    int id;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char name[NAME_LEN];
    int age;
    char gender[GENDER_LEN];
    char contact[CONTACT_LEN];
    char address[ADDRESS_LEN];
    char bloodGroup[BLOODGRP_LEN];
    int active;  // 1 = active account, 0 = deactivated 
} Patient;


typedef struct {
    int id;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char name[NAME_LEN];
    char specialization[SPEC_LEN];
    char contact[CONTACT_LEN];
    float avgRating;
    int ratingCount;
    int active; // 1 = active account, 0 = deactivated 
} Doctor;


typedef struct {
    int id;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char name[NAME_LEN];
} Admin;


typedef struct {
    int id;
    int patientId;
    int doctorId;
    char date[DATE_LEN];
    char time[TIME_LEN];
    int status;
    int isFollowup;
    int parentId;  //ID of the original appointment
} Appointment;


typedef struct {
    int id;
    int appointmentId;
    int patientId;
    int doctorId;
    char medicines[TEXT_LEN];
    char tests[TEXT_LEN];
    char notes[TEXT_LEN];
    char date[DATE_LEN];
} Prescription;


typedef struct {
    int id;
    int patientId;
    int doctorId;
    int appointmentId;
    char diagnosis[TEXT_LEN];
    char date[DATE_LEN];
} MedRecord;


typedef struct {
    int id;
    int doctorId;
    int patientId;
    int appointmentId;
    int stars;
    char comment[TEXT_LEN];
    char date[DATE_LEN];
} Rating;


typedef struct {
    int id;
    int patientId;
    char location[LOCATION_LEN];
    char contact[CONTACT_LEN];
    int status;
    char requestTime[TIME_LEN];
} Ambulance;


typedef struct {
    int id;
    int donorId;
    char bloodGroup[BLOODGRP_LEN];
    char date[DATE_LEN];
    int status;
} Donation;


typedef struct {
    int id;
    int requesterId; // Patient ID or Doctor ID
    int requesterRole; // ROLE_PATIENT or ROLE_DOCTOR
    int forPatientId; // Blood Reciever
    char bloodGroup[BLOODGRP_LEN];
    int units;
    int status;
    char date[DATE_LEN];
} BloodRequest;


typedef struct {
    char bloodGroup[BLOODGRP_LEN];
    int units;
} BloodStock;



#endif