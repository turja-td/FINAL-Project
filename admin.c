#include <stdio.h>
#include <string.h>
#include "admin.h"
#include "common.h"
#include "utils.h"
#include "doctor.h"
#include "bloodbank.h"


int loadAdmins(Admin list[]){
    FILE *fp = fopen(FILE_ADMINS, "rb");

    if(fp == NULL){
        return 0;
    }

    int count = fread(list, sizeof(Admin), MAX_ADMINS, fp);

    fclose(fp);

    return count;
}


void saveAdmins(Admin list[], int count){
    FILE *fp = fopen(FILE_ADMINS, "wb");

    if(fp == NULL){
        printf("Error: could not save admin data.\n");
        return;
    }

    fwrite(list, sizeof(Admin), count, fp);

    fclose(fp);
}


void setDefaultAdmin(void){
    Admin list[MAX_ADMINS];
    int count = loadAdmins(list);

    if(count > 0){
        return;
    }

    Admin a;

    a.id = 1;

    strcpy(a.username, "admin");
    strcpy(a.password, "admin123");
    strcpy(a.name, "Default Admin");

    list[0] = a;

    saveAdmins(list, 1);

    printf("[First run] Created a default admin login -> username: admin, password: admin123\n\n");
}


int loginAdmin(int *outAdminId){
    Admin list[MAX_ADMINS];
    int count = loadAdmins(list);

    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];

    readString("Username: ", username, USERNAME_LEN);
    readString("Password: ", password, PASSWORD_LEN);

    int i;

    for(i = 0; i < count; i++){
        if(strcmp(list[i].username, username) == 0){
            if(strcmp(list[i].password, password) == 0){
                *outAdminId = list[i].id;
                return 1;
            }

            printf("Incorrect username or password.\n");
            return 0;
        }
    }

    printf("No admin found with that username.\n");

    return 0;
}


static void addDoctor(void){
    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    if(count >= MAX_DOCTORS){
        printf("Sorry, the doctor list is full.\n");
        return;
    }

    Doctor d;

    d.id = count + 1;

    readString("Doctor username: ", d.username, USERNAME_LEN);
    readString("Initial password: ", d.password, PASSWORD_LEN);
    readString("Doctor name: ", d.name, NAME_LEN);
    readString("Specialization: ", d.specialization, SPEC_LEN);
    readString("Contact number: ", d.contact, CONTACT_LEN);

    d.avgRating = 0.0f;
    d.ratingCount = 0;
    d.active = 1;

    list[count] = d;

    count = count + 1;

    saveDoctors(list, count);

    printf("\nDoctor added. They can log in with username \"%s\".\n", d.username);
}


static void listDoctors(void){
    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    printf("\n--- All Doctors ---\n");

    if(count == 0){
        printf("(none yet)\n");
        return;
    }

    int i;

    for(i = 0; i < count; i++){
        printf("ID %d | %s | Dr. %-20s | %-15s | %s | %s | Rating: %.1f (%d)\n",
               list[i].id,
               list[i].username,
               list[i].name,
               list[i].specialization,
               list[i].contact,
               list[i].active ? "ACTIVE" : "REMOVED",
               list[i].avgRating,
               list[i].ratingCount);
    }
}


static void removeDoctor(void){
    listDoctors();

    int id = readInt("\nEnter Doctor ID to remove (0 to cancel): ");

    if(id == 0){
        return;
    }

    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    int index = findDoctorById(list, count, id);

    if(index == -1){
        printf("No doctor found with that ID.\n");
        return;
    }

    if(list[index].active == 0){
        printf("That doctor is already removed.\n");
        return;
    }

    list[index].active = 0;

    saveDoctors(list, count);

    printf("Doctor %s removed. Their history is kept for records.\n",
           list[index].name);
}


static void reactivateDoctor(void){
    listDoctors();

    int id = readInt("\nEnter Doctor ID to reactivate (0 to cancel): ");

    if(id == 0){
        return;
    }

    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    int index = findDoctorById(list, count, id);

    if(index == -1){
        printf("No doctor found with that ID.\n");
        return;
    }

    if(list[index].active == 1){
        printf("That doctor is already active.\n");
        return;
    }

    list[index].active = 1;

    saveDoctors(list, count);

    printf("Doctor %s reactivated.\n", list[index].name);
}


static void updateDoctor(void){
    listDoctors();

    int id = readInt("\nEnter Doctor ID to update (0 to cancel): ");

    if(id == 0){
        return;
    }

    Doctor list[MAX_DOCTORS];
    int count = loadDoctors(list);

    int index = findDoctorById(list, count, id);

    if(index == -1){
        printf("No doctor found with that ID.\n");
        return;
    }

    printf("\n--- Update Doctor (leave blank to keep the current value) ---\n");

    char temp[NAME_LEN];

    printf("Name [%s]: ", list[index].name);
    readLine(temp, sizeof(temp));

    if(strlen(temp) > 0){
        strcpy(list[index].name, temp);
    }

    printf("Specialization [%s]: ", list[index].specialization);
    readLine(temp, sizeof(list[index].specialization));

    if(strlen(temp) > 0){
        strcpy(list[index].specialization, temp);
    }

    printf("Contact [%s]: ", list[index].contact);
    readLine(temp, sizeof(list[index].contact));

    if(strlen(temp) > 0){
        strcpy(list[index].contact, temp);
    }

    saveDoctors(list, count);

    printf("Doctor info updated.\n");
}


void runAdminMenu(int adminId){
    Admin list[MAX_ADMINS];
    int count = loadAdmins(list);

    int index = -1;
    int i;

    for(i = 0; i < count; i++){
        if(list[i].id == adminId){
            index = i;
            break;
        }
    }

    if(index == -1){
        printf("Admin profile not found.\n");
        return;
    }

    char name[NAME_LEN];
    strcpy(name, list[index].name);

    int running = 1;

    while(running){
        printf("\n===== Admin Menu (%s) =====\n", name);
        printf("1. Add a new doctor\n");
        printf("2. Remove a doctor\n");
        printf("3. Reactivate a removed doctor\n");
        printf("4. Update doctor information\n");
        printf("5. List all doctors\n");
        printf("6. Review pending blood requests\n");
        printf("7. Review pending blood donations\n");
        printf("8. View blood bank inventory\n");
        printf("9. Logout\n");

        int choice = readIntRange("Choice: ", 1, 9);

        if(choice == 1){
            addDoctor();
        }
        else if(choice == 2){
            removeDoctor();
        }
        else if(choice == 3){
            reactivateDoctor();
        }
        else if(choice == 4){
            updateDoctor();
        }
        else if(choice == 5){
            listDoctors();
        }
        else if(choice == 6){
            adminReviewBloodRequests();
        }
        else if(choice == 7){
            adminReviewDonations();
        }
        else if(choice == 8){
            viewBloodInventory();
        }
        else if(choice == 9){
            printf("Logging out...\n");
            running = 0;
        }
    }
}