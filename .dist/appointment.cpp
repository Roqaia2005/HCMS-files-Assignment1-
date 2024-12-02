#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <map>
#include "doctor.cpp"
using namespace std;

class Appointment{
    private:
        int app_length;
        string app_id;
        string app_date;
        string doctor_id;
    public:
        string getID() {
            return app_id;
        }
        string getDate() {
            return app_date;
        }
        string getDoctorID() {
            return doctor_id;
        }
        int getLength() {
            return app_length;
        }
        Appointment(string id, string date, string doc_id) {
            app_id = id;
            app_date = date;
            doctor_id = doc_id;
            app_length = app_id.length() + app_date.length() + doctor_id.length() + 2;
        }
        Appointment() {
            cin.ignore();
            cout << "Enter ID: ";
            getline(cin, app_id);
            cout << "Enter Date: ";
            getline(cin, app_date);
            cout << "Enter Doctor ID: ";
            getline(cin, doctor_id);
            app_length = app_id.length() + app_date.length() + doctor_id.length() + 2;
        }
};
class AppointmentFile {
    private:
        int AVAILLIST = -1;
        string fileName;
        fstream file;
    public:
        AppointmentFile() {

        }
        AppointmentFile(string fileName) {
            this->fileName = fileName;
        }
        int getAVAILLIST() {
            return AVAILLIST;
        }
        void setAVAILLIST(int n) {
            AVAILLIST = n;
            file.open("appointments.txt", ios::in | ios::out);
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
        int addAppointmentRecord(Appointment a) {
            int byteOffset;
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::end);
            file << setw(2) << setfill('0') << right << a.getLength();
            file << a.getID() << '|' << a.getDate() << '|' << a.getDoctorID() << '\n';
            byteOffset = file.tellp();
            byteOffset -= a.getLength() + 3;
            file.close();
            return byteOffset;
        }
        Appointment readAppointmentRecord(int byteOffset) {
            string app_id;
            string app_name;
            string doc_id;
            file.open(fileName, ios::in);
            file.seekg(byteOffset + 2, ios::beg);
            getline(file, app_id, '|');
            getline(file, app_name, '|');
            getline(file, doc_id, '\n');
            file.close();
            return Appointment(app_id, app_name, doc_id);
        }
};
class AppointmentPrimaryIndexFile {
    private:
        string fileName;
        fstream file;
    public:
        // Should be changed to a new class that can access primary index file using binary search
        // and sort file.
        map<string, int> primaryIndex;
        AppointmentPrimaryIndexFile() {

        }
        AppointmentPrimaryIndexFile(string fileName) {
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
                string app_id;
                string byteOffset;
                int i = 0;
                while (line[i] != '|') {
                    app_id += line[i];
                    i++;
                }
                i++;
                while (i < line.length()) {
                    byteOffset += line[i];
                    i++;
                }
                primaryIndex[app_id] = stoi(byteOffset);
            }
            file.close();
        }
        void addAppointmentPrimaryIndex(Appointment a, int byteOffset) {
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::end);
            file << a.getID() << '|' << byteOffset << '\n';
            file.close();
            primaryIndex[a.getID()] = byteOffset;
        }
        bool exists(string ID) {
            return primaryIndex.find(ID) != primaryIndex.end();
        }
};
class AppointmentSecondaryIndexFile {
    private:
        string fileName;
        fstream file;
    public:
        // Should be changed to another type that manages secondary index file
        map<string, vector<string>> secondaryIndexOnDoctorID;
        AppointmentSecondaryIndexFile() {

        }
        AppointmentSecondaryIndexFile(string fileName) {
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
                string doc_id;
                string app_id;
                int i = 0;
                while (line[i] != '|') {
                    doc_id += line[i];
                    i++;
                }
                i++;
                while (i < line.length()) {
                    app_id += line[i];
                    i++;
                }
                secondaryIndexOnDoctorID[doc_id].push_back(app_id);
            }
            file.close();
        }
        void addAppointmentSecondaryIndex(Appointment a) {
            file.open(fileName, ios::in | ios::out);
            file.seekp(0, ios::end);
            file << a.getDoctorID() << '|' << a.getID() << '\n';
            file.close();
            secondaryIndexOnDoctorID[a.getDoctorID()].push_back(a.getID());
        }
};
class AppointmentTable{
    private:
        AppointmentFile file;
        AppointmentPrimaryIndexFile filePrimaryIndex;
        AppointmentSecondaryIndexFile fileSecondaryIndex;
        DoctorTable* dt;
        void initializeFiles() {
            file.initializeRecordFile();
            filePrimaryIndex.initializePrimaryIndexFile();
            fileSecondaryIndex.initializeSecondaryIndexFile();
        }
    public:
        AppointmentTable(DoctorTable* dt) {
            this->dt = dt;
            file = AppointmentFile("appointments.txt");
            filePrimaryIndex = AppointmentPrimaryIndexFile("appointmentsPI.txt");
            fileSecondaryIndex = AppointmentSecondaryIndexFile("appointmentsSI.txt");
            initializeFiles();
        }

        AppointmentPrimaryIndexFile* getPrimaryIndexFile() {
            return &filePrimaryIndex;
        }

       AppointmentFile* getFile() {
            return &file;
        }
        void addAppointment(Appointment a) {
            if (filePrimaryIndex.exists(a.getID())) {
                cout << "Primary key Appointment ID already exists.\n";
                return;
            }
            if (!dt->getPrimaryIndexFile()->exists(a.getDoctorID())) {
                cout << "Secondary key Doctor ID doesn't exist.\n";
                return;
            }
            int byteOffset = file.addAppointmentRecord(a);
            filePrimaryIndex.addAppointmentPrimaryIndex(a , byteOffset);
            fileSecondaryIndex.addAppointmentSecondaryIndex(a);
        }
        void printAppointmentInfo() {
            string ID;
            cout << "Enter ID: ";
            cin >> ID;
            Appointment info = file.readAppointmentRecord(filePrimaryIndex.primaryIndex[ID]);
            cout << "Date: " << info.getDate() << ", Doctor ID: " << info.getDoctorID() << "\n";
        }
};
