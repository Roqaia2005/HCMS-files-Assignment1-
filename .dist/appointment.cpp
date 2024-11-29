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

class AppointmentTable{
    private:
        int AVAILLIST = -1;
        fstream file;
        fstream filePrimaryIndex;
        fstream fileSecondaryIndex;
        DoctorTable* dt;

        void initializeFiles() {
            initializeRecordFile();
            initializePrimaryIndexFile();
        }

        void initializeRecordFile() {
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

        void initializePrimaryIndexFile() {
            // Create file if it doesn't exist
            ofstream output("appointmentsPI.txt", ios::app);
            output.close();

            // Read primary index into the map
            filePrimaryIndex.open("appointmentsPI.txt", ios::in);
            string line;
            while (getline(filePrimaryIndex, line)) {
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
            filePrimaryIndex.close();
        }
    public:
        // Should be changed to a new class that can access primary index file using binary search
        // and sort file.
        map<string, int> primaryIndex;
        AppointmentTable(DoctorTable* dt) {
            this->dt = dt;
            initializeFiles();
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
        void addAppointment(Appointment a) {
            if (dt->primaryIndex.find(a.getDoctorID()) == dt->primaryIndex.end()) {
                cout << "Secondary key Doctor ID doesn't exist.";
                return;
            }
            int byteOffset = addAppointmentRecord(a);
            addAppointmentPrimaryIndex(a , byteOffset);
        }
        int addAppointmentRecord(Appointment a) {
            int byteOffset;
            file.open("appointments.txt", ios::in | ios::out);
            file.seekp(0, ios::end);
            file << setw(2) << setfill('0') << right << a.getLength();
            file << a.getID() << '|' << a.getDate() << '|' << a.getDoctorID() << '\n';
            byteOffset = file.tellp();
            byteOffset -= a.getLength() + 3;
            file.close();
            return byteOffset;
        }
        void addAppointmentPrimaryIndex(Appointment a, int byteOffset) {
            filePrimaryIndex.open("appointmentsPI.txt", ios::in | ios::out);
            filePrimaryIndex.seekp(0, ios::end);
            filePrimaryIndex << a.getID() << '|' << byteOffset << '\n';
            filePrimaryIndex.close();
            primaryIndex[a.getID()] = byteOffset;
        }
        Appointment readAppointmentRecord(int byteOffset) {
            string app_id;
            string app_name;
            string doc_id;
            file.open("appointments.txt", ios::in);
            file.seekg(byteOffset + 2, ios::beg);
            getline(file, app_id, '|');
            getline(file, app_name, '|');
            getline(file, doc_id, '\n');
            file.close();
            return Appointment(app_id, app_name, doc_id);
        }
        void sortPrimaryIndex() {
            // Code to sort primary index file
        }
        void printAppointmentInfo() {
            string ID;
            cout << "Enter ID: ";
            cin >> ID;
            Appointment info = readAppointmentRecord(primaryIndex[ID]);
            cout << "Date: " << info.getDate() << ", Doctor ID: " << info.getDoctorID() << "\n";
        }
};