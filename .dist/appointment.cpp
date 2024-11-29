#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
using namespace std;

class Appointment{
    private:
        int app_length;
        char app_id[15] = {' '};
        char app_date[30] = {' '};
        char doctor_id[15] = {' '};
    public:
        char* getID() {
            return app_id;
        }
        char* getDate() {
            return app_date;
        }
        char* getDoctorID() {
            return doctor_id;
        }
        int getLength() {
            return app_length;
        }
        Appointment(string id, string date, string doc_id) {
            strcpy(app_id, id.c_str());
            strcpy(app_date, date.c_str());
            strcpy(doctor_id, doc_id.c_str());
            app_length = strlen(app_id) + strlen(app_date) + strlen(doctor_id) + 2;
        }
        Appointment() {
            cin.ignore();
            cout << "Enter ID: ";
            cin.getline(app_id, 15);
            cout << "Enter Date: ";
            cin.getline(app_date, 30);
            cout << "Enter Doctor ID: ";
            cin.getline(doctor_id, 15);
            app_length = strlen(app_id) + strlen(app_date) + strlen(doctor_id) + 2;
        }
};

class AppointmentTable{
    private:
        int AVAILLIST = -1;
        fstream file;
        fstream filePrimaryIndex;
        fstream fileSecondaryIndex;
    public:
        AppointmentTable() {
            // Create file if it doesn't exist
            ofstream output("appointments.txt", ios::app);
            output.close();
            char READAVAILLIST[5];
            // Read AVAILLIST if it exists
            file.open("appointments.txt", ios::in);
            file.getline(READAVAILLIST, 5);
            if (strlen(READAVAILLIST) > 0) {
                AVAILLIST = stoi(READAVAILLIST);
            }
            file.close();
            // Write AVAILLIST if it doesn't exist
            file.open("appointments.txt", ios::in | ios::out);
            file.seekp(0, ios::beg);
            file << setw(5) << setfill(' ') << left << AVAILLIST << '\n';
            file.close();
        }
        int getAVAILLIST() {
            return AVAILLIST;
        }
        void AddAppointment(Appointment a) {
            file.open("appointments.txt", ios::in | ios::out);
            file.seekp(0, ios::end);
            file << setw(2) << setfill('0') << right << a.getLength();
            file << a.getID() << '|' << a.getDate() << '|' << a.getDoctorID() << '\n';
            file.close();
        }
        Appointment ReadAppointmentRecord(int byteOffset) {
            char app_id[15] = {' '};
            char app_name[30] = {' '};
            char doc_id[15] = {' '};
            char strlength[2];
            int length;
            file.open("appointments.txt", ios::in);
            file.seekg(byteOffset, ios::beg);
            file.read(strlength, 2);
            length = stoi(strlength);
            file.getline(app_id, 15, '|');
            file.getline(app_name, 30, '|');
            file.getline(doc_id, 15, '\n');
            file.close();
            return Appointment(app_id, app_name, doc_id);
        }
};