#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
using namespace std;
class Doctor{
    private:
        int doctor_length;
        char doctor_id[15] = {' '};
        char doctor_name[30] = {' '};
        char doctor_address[30] = {' '};
    public:
        char* getID() {
            return doctor_id;
        }
        char* getName() {
            return doctor_name;
        }
        char* getAddress() {
            return doctor_address;
        }
        int getLength() {
            return doctor_length;
        }
        Doctor(string id, string name, string address) {
            strcpy(doctor_id, id.c_str());
            strcpy(doctor_name, name.c_str());
            strcpy(doctor_address, address.c_str());
            doctor_length = strlen(doctor_id) + strlen(doctor_name) + strlen(doctor_address) + 2;
        }
        Doctor() {
            cin.ignore();
            cout << "Enter ID: ";
            cin.getline(doctor_id, 15);
            cout << "Enter Name: ";
            cin.getline(doctor_name, 30);
            cout << "Enter Address: ";
            cin.getline(doctor_address, 30);
            doctor_length = strlen(doctor_id) + strlen(doctor_name) + strlen(doctor_address) + 2;
        }
};

class DoctorTable{
    private:
        int AVAILLIST = -1;
        fstream file;
        fstream filePrimaryIndex;
        fstream fileSecondaryIndex;
    public:
        DoctorTable() {
            ofstream output("doctors.txt", ios::app);
            output.close();
            char READAVAILLIST[5];
            file.open("doctors.txt", ios::in);
            file.getline(READAVAILLIST, 5);
            if (strlen(READAVAILLIST) > 0) {
                AVAILLIST = stoi(READAVAILLIST);
            }
            file.close();
            file.open("doctors.txt", ios::in | ios::out);
            file.seekp(0, ios::beg);
            file << setw(5) << setfill(' ') << left << AVAILLIST << '\n';
            file.close();
        }
        int getAVAILLIST() {
            return AVAILLIST;
        }
        void AddDoctor(Doctor d) {
            file.open("doctors.txt", ios::app | ios::out);
            file << setw(2) << setfill('0') << d.getLength();
            file << d.getID() << '|' << d.getName() << '|' << d.getAddress() << '\n';
            file.close();
        }
};
