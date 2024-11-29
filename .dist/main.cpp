#include <iostream>
#include "doctor.cpp"
using namespace std;
class HealthCareManagementSystem{
    private:
        DoctorTable dt;
        void mainMenu(){
            cout<<"1-Welcome to Healthcare mangement system\n";
            cout<<"2-Add new doctor\n";
            cout<<"3-Add new appointment\n";
            cout<<"4-Update Doctor Name (Doctor ID)\n";
            cout<<"5-Update Appointment Date (Appointment ID)\n";
            cout<<"6-Delete Appointment (Appointment ID)\n";
            cout<<"7-Delete Doctor (Doctor ID)\n";
            cout<<"8-Print Doctor Info (Doctor ID)\n";
            cout<<"9-Print Appointment Info (Appointment ID)\n";
            cout<<"10-Write Query\n";
            cout<<"0-Exit\n";
        } 
    public:
        HealthCareManagementSystem() {
            dt = DoctorTable();
        }
        
        void Run() {
            int input;
            while (true) {
                mainMenu();
                cin >> input;
                if (input == 0) {
                    break;
                }
                switch (input) {
                    case 2:
                        dt.AddDoctor(Doctor());
                }
            }
        }
};


int main() {
    HealthCareManagementSystem HCMS;
    HCMS.Run();

    return 0;
}