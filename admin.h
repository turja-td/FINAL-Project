#pragma once

#include "common.h"

int loadAdmins(Admin list[]);
void saveAdmins(Admin list[], int count);
int loginAdmin(int *outAdminId);
void setDefaultAdmin(void);
void runAdminMenu(int adminId);

