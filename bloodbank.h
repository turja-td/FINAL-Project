#pragma once

void initializeBloodStock(void);

void donateBlood(int donorId);
void requestBlood(int requesterId, int requesterRole, int forPatientId);

void viewMyDonations(int patientId);
void viewMyBloodRequestsPatient(int patientId);
void viewMyBloodRequestsDoctor(int doctorId);

void viewBloodInventory(void);

void adminReviewBloodRequests(void);
void adminReviewDonations(void);


