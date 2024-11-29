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
            // Create file if it doesn't exist
            ofstream output("doctors.txt", ios::app);
            output.close();
            char READAVAILLIST[5];
            // Read AVAILLIST if it exists
            file.open("doctors.txt", ios::in);
            file.getline(READAVAILLIST, 5);
            if (strlen(READAVAILLIST) > 0) {
                AVAILLIST = stoi(READAVAILLIST);
            }
            file.close();
            // Write AVAILLIST if it doesn't exist
            file.open("doctors.txt", ios::in | ios::out);
            file.seekp(0, ios::beg);
            file << setw(5) << setfill(' ') << left << AVAILLIST << '\n';
            file.close();
        }
        int getAVAILLIST() {
            return AVAILLIST;
        }
        void AddDoctor(Doctor d) {
            file.open("doctors.txt", ios::in | ios::out);
            file.seekp(0, ios::end);
            file << setw(2) << setfill('0') << right << d.getLength();
            file << d.getID() << '|' << d.getName() << '|' << d.getAddress() << '\n';
            file.close();
        }
        Doctor ReadDoctorRecord(int byteOffset) {
            char doc_id[15] = {' '};
            char doc_name[30] = {' '};
            char doc_address[30] = {' '};
            char strlength[2];
            int length;
            file.open("doctors.txt", ios::in);
            file.seekg(byteOffset, ios::beg);
            file.read(strlength, 2);
            length = stoi(strlength);
            file.getline(doc_id, 15, '|');
            file.getline(doc_name, 30, '|');
            file.getline(doc_address, 30, '\n');
            file.close();
            return Doctor(doc_id, doc_name, doc_address);
        }
};
