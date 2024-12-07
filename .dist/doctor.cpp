#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>
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
      void setName(string newName) {
            doctor_name = newName;
            // Recalculate the record length when the name is updated
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

            char READAVAILLIST[6];
            // Read AVAILLIST if it exists
            file.open(fileName, ios::in);
            file.getline(READAVAILLIST, 6);
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
            int offset = -1;
            int prevOffset;
            char prevAVAIL[6];
            char currAVAIL[6];
            char prevRecordLength[3];
            char currRecordLength[3];

            file.open(fileName, ios::in | ios::out);
            if (getAVAILLIST() == -1) {
                offset = -1;
                file.close();
            }
            else {
                file.seekg(getAVAILLIST() + 1);
                file.getline(currAVAIL, 6, '|');
                file.getline(currRecordLength, 3, '|');
                if (d.getLength() <= stoi(currRecordLength)) {
                    offset = getAVAILLIST();
                    file.close();
                    setAVAILLIST(stoi(currAVAIL));
                }
                else {
                    prevOffset = getAVAILLIST() + 1;
                    strcpy(prevAVAIL, currAVAIL);
                    strcpy(prevRecordLength, currRecordLength);
                    while (true) {
                        if (stoi(currAVAIL) == -1) {
                            file.close();
                            break;
                        }
                        file.seekg(stoi(currAVAIL) + 1);
                        file.getline(currAVAIL, 6, '|');
                        file.getline(currRecordLength, 3, '|');
                        if (d.getLength() <= stoi(currRecordLength)) {
                            offset = stoi(prevAVAIL);
                            file.seekp(prevOffset);
                            file << currAVAIL << "|" << prevRecordLength << "|";
                            file.close();
                            break;
                        }
                        prevOffset = stoi(prevAVAIL) + 1;
                        strcpy(prevAVAIL, currAVAIL);
                        strcpy(prevRecordLength, currRecordLength);
                    }
                }
            }

            file.open(fileName, ios::in | ios::out);
            if (offset != -1) {
                file.seekp(offset, ios::beg);
                file << setw(2) << setfill('0') << right << currRecordLength;
                file << d.getID() << '|' << d.getName() << '|' << d.getAddress();
                for (int i = 0; i < stoi(currRecordLength) - d.getLength(); i++)
                    file << ' ';
                byteOffset = file.tellp();
                byteOffset -= stoi(currRecordLength) + 2;
            }
            else {
                file.seekp(0, ios::end);
                file << setw(2) << setfill('0') << right << d.getLength();
                file << d.getID() << '|' << d.getName() << '|' << d.getAddress() << '\n';
                byteOffset = file.tellp();
                byteOffset -= d.getLength() + 3;
            }
            file.close();
            return byteOffset;
        }
        void deleteDoctorRecord(int byteOffset) {
            char recordLength[3];
            file.open(fileName, ios::in | ios::out);
            file.seekg(byteOffset, ios::beg);
            file.read(recordLength, 2);
            file.seekp(byteOffset, ios::beg);
            file << '*' << getAVAILLIST() << '|' << recordLength << '|';
            file.close();
            setAVAILLIST(byteOffset);
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
            while (*(doc_address.end() - 1) == ' ') {
                doc_address.erase(doc_address.end() - 1);
            }
            return Doctor(doc_id, doc_name, doc_address);
        }
      void updateDoctorRecord(int byteOffset, Doctor updatedDoctor) {
            // Open the file in read-write mode
            file.open(fileName, ios::in | ios::out);
            if (!file) {
                cout << "Error: Unable to open file for updating.\n";
                return;
            }

            // Seek to the byte offset where the record is located
            file.seekp(byteOffset + 2, ios::beg);

            // Write the updated doctor record
            file << updatedDoctor.getID() << '|' << updatedDoctor.getName() << '|' << updatedDoctor.getAddress();

            // Close the file
            file.close();
        }
};

class DoctorPrimaryIndexFile {
    private:
        string fileName;
        fstream file;
        void fixPrimaryIndexFile() {
            file.open(fileName, ios::out);
            for (const auto& [ID, offset] : primaryIndex) {
                file << ID << "|" << offset << "\n";
            }
            file.close();
        }
    public:
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
            primaryIndex[d.getID()] = byteOffset;
            fixPrimaryIndexFile();
        }
        void deleteDoctorPrimaryIndex(string ID) {
            primaryIndex.erase(ID);
            fixPrimaryIndexFile();
        }
        bool exists(string ID) {
            return primaryIndex.find(ID) != primaryIndex.end();
        }
 int getByteOffset(string ID) {
            // Check if the ID exists and return the offset if found
            if (exists(ID)) {
                return primaryIndex[ID];
            }
            else {
                cout << "it doesn't exsist \n";
            }
        }
};

class DoctorSecondaryIndexFile {
    private:
        string fileName;
        fstream file;
        void fixSecondaryIndexFile() {
            file.open(fileName, ios::out);
            for (const auto& [Name, IDs] : secondaryIndexOnName) {
                file << Name << "|";
                for (const auto& ID : IDs) {
                    file << ID << "|";
                }
                file << "\n";
            }
            file.close();
        }
    public:
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
                    if (line[i] == '|') {
                        secondaryIndexOnName[doc_name].push_back(doc_id);
                        doc_id = "";
                    }
                    else {
                        doc_id += line[i];
                    }
                    i++;
                }
            }
            file.close();
        }
        void addDoctorSecondaryIndex(Doctor d) {
            secondaryIndexOnName[d.getName()].push_back(d.getID());
            fixSecondaryIndexFile();
        }
        void deleteDoctorSecondaryIndex(string Name,string ID) {
            secondaryIndexOnName[Name].erase(find(secondaryIndexOnName[Name].begin(), secondaryIndexOnName[Name].end(), ID));
            if (secondaryIndexOnName[Name].empty()) {
                secondaryIndexOnName.erase(Name);
            }
            fixSecondaryIndexFile();
        }
        void updateDoctorSecondaryIndex(Doctor d, string oldName) {
            // Remove from the current name's entry if it exists
            if (secondaryIndexOnName.find(oldName) != secondaryIndexOnName.end()) {
                secondaryIndexOnName[oldName].erase(find(secondaryIndexOnName[oldName].begin(), secondaryIndexOnName[oldName].end(), d.getID()));
                if (secondaryIndexOnName[oldName].empty()) {
                    secondaryIndexOnName.erase(oldName);
                }
            }
            // Add to the new name entry
            secondaryIndexOnName[d.getName()].push_back(d.getID());
            fixSecondaryIndexFile();
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
        void deleteDoctor(string ID) {
            if (!filePrimaryIndex.exists(ID)) {
                cout << "Doctor ID doesn't exist.\n";
                return;
            }
            Doctor d = file.readDoctorRecord(filePrimaryIndex.primaryIndex[ID]);
            file.deleteDoctorRecord(filePrimaryIndex.primaryIndex[ID]);
            fileSecondaryIndex.deleteDoctorSecondaryIndex(d.getName() ,ID);
            filePrimaryIndex.deleteDoctorPrimaryIndex(ID);
        }
       void updateDoctor(string ID, string newName) {
            if (!filePrimaryIndex.exists(ID)) {
                cout << "Doctor ID doesn't exist.\n";
                return;
            }

            // Read the current Doctor record
            int byteOffset = filePrimaryIndex.primaryIndex[ID];
            Doctor currentDoctor = file.readDoctorRecord(byteOffset);
            string oldName = currentDoctor.getName();
            int oldLength = currentDoctor.getLength();

            // Update the Doctor record in the file
            currentDoctor.setName(newName);
            int newLength = currentDoctor.getLength();

            if (newLength != oldLength) {
                deleteDoctor(currentDoctor.getID());
                addDoctor(currentDoctor);
            }
            else {
                file.updateDoctorRecord(byteOffset, currentDoctor);
                fileSecondaryIndex.updateDoctorSecondaryIndex(currentDoctor, oldName);
            }
        }
        Doctor getDoctor(string ID) {
            return file.readDoctorRecord(filePrimaryIndex.primaryIndex[ID]);
        }
        void printDoctorInfo() {
            string ID;
            cout << "Enter ID: ";
            cin >> ID;
            Doctor info = this->file.readDoctorRecord(this->filePrimaryIndex.primaryIndex[ID]);
            cout << "Name: " << info.getName() << ", Address: " << info.getAddress() << "\n";
        }
};

