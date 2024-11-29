#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <map>
using namespace std;
class Doctor{
    private:
        int doctor_length;
        string doctor_id;
        string doctor_name;
        string doctor_address;
    public:
        string getID() {
            return doctor_id;
        }
        string getName() {
            return doctor_name;
        }
        string getAddress() {
            return doctor_address;
        }
        int getLength() {
            return doctor_length;
        }
        Doctor(string id, string name, string address) {
            doctor_id = id;
            doctor_name = name;
            doctor_address = address;
            doctor_length = doctor_id.length() + doctor_name.length() + doctor_address.length() + 2;
        }
        Doctor() {
            cin.ignore();
            cout << "Enter ID: ";
            getline(cin, doctor_id);
            cout << "Enter Name: ";
            getline(cin, doctor_name);
            cout << "Enter Address: ";
            getline(cin, doctor_address);
            doctor_length = doctor_id.length() + doctor_name.length() + doctor_address.length() + 2;
        }
};

class DoctorTable{
    private:
        int AVAILLIST = -1;
        fstream file;
        fstream filePrimaryIndex;
        fstream fileSecondaryIndex;
        void initializeFiles() {
            initializeRecordFile();
            initializePrimaryIndexFile();
        }
        void initializeRecordFile() {
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
        void initializePrimaryIndexFile() {
            // Create file if it doesn't exist
            ofstream output("doctorsPI.txt", ios::app);
            output.close();
            filePrimaryIndex.open("doctorsPI.txt", ios::in);
            string line;
            while (getline(filePrimaryIndex, line)) {
                string doc_id;
                string byteOffset;
                int i = 0;
                while (line[i] != ' ') {
                    doc_id += line[i];
                    i++;
                }
                i++;
                while (i < line.length()) {
                    byteOffset += line[i];
                    i++;
                }
                primaryIndex[doc_id] = stoi(byteOffset);
            }
        }
    public:
        map <string , int> primaryIndex;
        DoctorTable() {
            initializeFiles();
        }
        int getAVAILLIST() {
            return AVAILLIST;
        }
        void addDoctor(Doctor d) {
            int byteOffset = addDoctorRecord(d);
            addDoctorPrimaryIndex(d, byteOffset);
        }
        int addDoctorRecord(Doctor d) {
            int byteOffset;
            file.open("doctors.txt", ios::in | ios::out);
            file.seekp(0, ios::end);
            file << setw(2) << setfill('0') << right << d.getLength();
            file << d.getID() << '|' << d.getName() << '|' << d.getAddress() << '\n';
            byteOffset = file.tellp();
            byteOffset -= d.getLength() + 3;
            file.close();
            return byteOffset;
        }
        void addDoctorPrimaryIndex(Doctor d, int byteOffset) {
            filePrimaryIndex.open("doctorsPI.txt", ios::in | ios::out);
            filePrimaryIndex.seekp(0, ios::end);
            filePrimaryIndex << d.getID() << " " << byteOffset << "\n";
            filePrimaryIndex.close();
            primaryIndex[d.getID()] = byteOffset;
        }
        Doctor readDoctorRecord(int byteOffset) {
            string doc_id;
            string doc_name;
            string doc_address;
            file.open("doctors.txt", ios::in);
            file.seekg(byteOffset+2, ios::beg);
            getline(file, doc_id, '|');
            getline(file, doc_name, '|');
            getline(file, doc_address, '\n');
            file.close();
            return Doctor(doc_id, doc_name, doc_address);
        }
        void sortPrimaryIndex() {
            filePrimaryIndex.open("doctorsPI.txt", ios::in | ios::out);
            filePrimaryIndex.seekp(0, ios::beg);
            for (auto i = primaryIndex.begin(); i != primaryIndex.end(); i++) {
                filePrimaryIndex << i->first << '|' << i->second << '\n';
            }
            filePrimaryIndex.close();
        }
};
