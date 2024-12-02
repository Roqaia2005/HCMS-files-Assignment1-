#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
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

class DoctorFile {
    private:
        int AVAILLIST = -1;
        string fileName;
        fstream file;
    public:
        DoctorFile() {

        }
        DoctorFile(string fileName) {
            this->fileName = fileName;
        }
        int getAVAILLIST() {
            return AVAILLIST;
        }
        void setAVAILLIST(int n) {
            AVAILLIST = n;
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::beg);
            file << setw(5) << setfill(' ') << left << AVAILLIST << '\n';
            file.close();
        }
        void initializeRecordFile() {
            // Create file if it doesn't exist
            ofstream output(fileName, ios::app);
            output.close();

            char READAVAILLIST[5];
            // Read AVAILLIST if it exists
            file.open(fileName, ios::in);
            file.getline(READAVAILLIST, 5);
            if (strlen(READAVAILLIST) > 0) {
                AVAILLIST = stoi(READAVAILLIST);
            }
            file.close();

            // Write AVAILLIST if it doesn't exist
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::beg);
            file << setw(5) << setfill(' ') << left << AVAILLIST << '\n';
            file.close();
        }
        int addDoctorRecord(Doctor d) {
            int byteOffset;
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::end);
            file << setw(2) << setfill('0') << right << d.getLength();
            file << d.getID() << '|' << d.getName() << '|' << d.getAddress() << '\n';
            byteOffset = file.tellp();
            byteOffset -= d.getLength() + 3;
            file.close();
            return byteOffset;
        }
        Doctor readDoctorRecord(int byteOffset) {
            string doc_id;
            string doc_name;
            string doc_address;
            file.open(fileName, ios::in);
            file.seekg(byteOffset + 2, ios::beg);
            getline(file, doc_id, '|');
            getline(file, doc_name, '|');
            getline(file, doc_address, '\n');
            file.close();
            return Doctor(doc_id, doc_name, doc_address);
        }
};

class DoctorPrimaryIndexFile {
    private:
        string fileName;
        fstream file;
    public:
        // Should be changed to a new class that can manage accessing primary index file using binary search
        // and sorting it.
        map<string, int> primaryIndex;
        DoctorPrimaryIndexFile() {

        }
        DoctorPrimaryIndexFile(string fileName) {
            this->fileName = fileName;
        }
        void initializePrimaryIndexFile() {
            // Create file if it doesn't exist
            ofstream output(fileName, ios::app);
            output.close();

            // Read primary index into the map
            file.open(fileName, ios::in);
            string line;
            while (getline(file, line)) {
                string doc_id;
                string byteOffset;
                int i = 0;
                while (line[i] != '|') {
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
            file.close();
        }
        void addDoctorPrimaryIndex(Doctor d, int byteOffset) {
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::end);
            file << d.getID() << "|" << byteOffset << "\n";
            file.close();
            primaryIndex[d.getID()] = byteOffset;
        }
        bool exists(string ID) {
            return primaryIndex.find(ID) != primaryIndex.end();
        }
};

class DoctorSecondaryIndexFile {
    private:
        string fileName;
        fstream file;
    public:
        // Should be changed to a new type that manages secondary index file
        map<string, vector<string>> secondaryIndexOnName;
        DoctorSecondaryIndexFile() {

        }
        DoctorSecondaryIndexFile(string fileName) {
            this->fileName = fileName;
        }
        void initializeSecondaryIndexFile() {
            // Create file if it doesn't exist
            ofstream output(fileName, ios::app);
            output.close();

            // Read secondary index into the map
            file.open(fileName, ios::in);
            string line;
            while (getline(file, line)) {
                string doc_name;
                string doc_id;
                int i = 0;
                while (line[i] != '|') {
                    doc_name += line[i];
                    i++;
                }
                i++;
                while (i < line.length()) {
                    doc_id += line[i];
                    i++;
                }
                secondaryIndexOnName[doc_name].push_back(doc_id);
            }
            file.close();
        }
        void addDoctorSecondaryIndex(Doctor d) {
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::end);
            file << d.getName() << "|" << d.getID() << "\n";
            file.close();
            secondaryIndexOnName[d.getName()].push_back(d.getID());
        }
};

class DoctorTable{
    private:
        DoctorFile file;
        DoctorPrimaryIndexFile filePrimaryIndex;
        DoctorSecondaryIndexFile fileSecondaryIndex;
        void initializeFiles() {
            file.initializeRecordFile();
            filePrimaryIndex.initializePrimaryIndexFile();
            fileSecondaryIndex.initializeSecondaryIndexFile();
        }
    public:
        DoctorTable() {
            file = DoctorFile("doctors.txt");
            filePrimaryIndex = DoctorPrimaryIndexFile("doctorsPI.txt");
            fileSecondaryIndex = DoctorSecondaryIndexFile("doctorsSI.txt");
            initializeFiles();
        }
        DoctorPrimaryIndexFile* getPrimaryIndexFile() {
            return &filePrimaryIndex;
        }

       DoctorFile* getFile() {
           return &file;
       }

        void addDoctor(Doctor d) {
            if (filePrimaryIndex.exists(d.getID())) {
                cout << "Primary key Doctor ID already exists.\n";
                return;
            }
            int byteOffset = file.addDoctorRecord(d);
            filePrimaryIndex.addDoctorPrimaryIndex(d, byteOffset);
            fileSecondaryIndex.addDoctorSecondaryIndex(d);
        }
        void printDoctorInfo() {
            string ID;
            cout << "Enter ID: ";
            cin >> ID;
            Doctor info = this->file.readDoctorRecord(this->filePrimaryIndex.primaryIndex[ID]);
            cout << "Name: " << info.getName() << ", Address: " << info.getAddress() << "\n";
        }
};

