#include <iostream>
using namespace std;
class HealthCareManagementSystem{
    void mainMenu(){
        while (true) {
            cout<<"1- Welcome to Healthcare mangement system";
            cout<<"2- Add new doctor";
            cout<<"3- Add new appointment";
            cout<<"4- Update Doctor Name (Doctor ID)";
            cout<<"5- Update Appointment Date (Appointment ID)";
            cout<<"6- Delete Appointment (Appointment ID)";
            cout<<"7- Delete Doctor (Doctor ID)";
            cout<<"8- Print Doctor Info (Doctor ID)";
            cout<<"9- Print Appointment Info (Appointment ID)";
            cout<<"10- Write Query";
            cout<<"0- Exit";
        
        }
    }

};
