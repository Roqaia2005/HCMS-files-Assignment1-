#include <iostream>
#include "doctor.cpp"
#include "appointment.cpp"
using namespace std;
class HealthCareManagementSystem{
    private:
        DoctorTable* dt;
        AppointmentTable* at;
        void mainMenu(){
            cout<<"1-Add new doctor\n";
            cout<<"2-Add new appointment\n";
            cout<<"3-Update Doctor Name (Doctor ID)\n";
            cout<<"4-Update Appointment Date (Appointment ID)\n";
            cout<<"5-Delete Appointment (Appointment ID)\n";
            cout<<"6-Delete Doctor (Doctor ID)\n";
            cout<<"7-Print Doctor Info (Doctor ID)\n";
            cout<<"8-Print Appointment Info (Appointment ID)\n";
            cout<<"9-Write Query\n";
            cout<<"0-Exit\n";
        } 
    public:
        HealthCareManagementSystem() {
            dt = new DoctorTable();
            at = new AppointmentTable();
        }
        
        void Run() {
            char input;
            while (true) {
                mainMenu();
                cin >> input;
                if (input == '0') {
                    break;
                }
                switch (input) {
                    case '1':
                        dt->AddDoctor(Doctor());
                        break;
                    case '2':
                        at->AddAppointment(Appointment());
                        break;
                    case 't':
                        dt->AddDoctor(dt->ReadDoctorRecord(7));
                        break;
                }
            }
        }
};


int main() {
    HealthCareManagementSystem HCMS;
    HCMS.Run();

    return 0;
}