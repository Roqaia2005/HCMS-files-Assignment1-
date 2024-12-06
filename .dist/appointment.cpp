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
   void setDate(string newDate) {
            app_date = newDate;
            // Recalculate the record length when the date is updated
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
        int addAppointmentRecord(Appointment a) {
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
                if (a.getLength() <= stoi(currRecordLength)) {
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
                        if (a.getLength() <= stoi(currRecordLength)) {
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
                file << a.getID() << '|' << a.getDate() << '|' << a.getDoctorID();
                for (int i = 0; i < stoi(currRecordLength) - a.getLength(); i++)
                    file << ' ';
                byteOffset = file.tellp();
                byteOffset -= stoi(currRecordLength) + 2;
            }
            else {
                file.seekp(0, ios::end);
                file << setw(2) << setfill('0') << right << a.getLength();
                file << a.getID() << '|' << a.getDate() << '|' << a.getDoctorID() << '\n';
                byteOffset = file.tellp();
                byteOffset -= a.getLength() + 3;
            }
            
            file.close();
            return byteOffset;
        }
        void deleteAppointmentRecord(int byteOffset) {
            char recordLength[3];
            file.open(fileName, ios::in | ios::out);
            file.seekg(byteOffset, ios::beg);
            file.read(recordLength, 2);
            file.seekp(byteOffset, ios::beg);
            file << '*' << getAVAILLIST() << '|' << recordLength << '|';
            file.close();
            setAVAILLIST(byteOffset);
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
            while (*(doc_id.end() - 1) == ' ') {
                doc_id.erase(doc_id.end() - 1);
            }
            return Appointment(app_id, app_name, doc_id);
        }
        void updateAppointmentRecord(int byteOffset, Appointment updatedAppointment) {
            // Open the file in read-write mode
            file.open(fileName, ios::in | ios::out);
            if (!file) {
                cout << "Error: Unable to open file for updating.\n";
                return;
            }

            // Seek to the byte offset where the record is located
            file.seekp(byteOffset + 2, ios::beg);

            // Write the updated appointment record
            file << setw(2) << setfill('0') << right << updatedAppointment.getLength();
            file << updatedAppointment.getID() << '|' << updatedAppointment.getDate();

            // Ensure the record has the correct length by padding with spaces if needed
            int paddingLength = updatedAppointment.getLength() - (updatedAppointment.getID().length() + updatedAppointment.getDate().length() + 2);
            for (int i = 0; i < paddingLength; i++) {
                file << ' ';
            }

            // Close the file
            file.close();
        }
};
class AppointmentPrimaryIndexFile {
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
            primaryIndex[a.getID()] = byteOffset;
            fixPrimaryIndexFile();
        }
        void deleteAppointmentPrimaryIndex(string ID) {
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
class AppointmentSecondaryIndexFile {
    private:
        string fileName;
        fstream file;
        void fixSecondaryIndexFile() {
            file.open(fileName, ios::out);
            for (const auto& [Doc_ID, IDs] : secondaryIndexOnDoctorID) {
                file << Doc_ID << "|";
                for (const auto& ID : IDs) {
                    file << ID << "|";
                }
                file << "\n";
            }
            file.close();
        }
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
                    if (line[i] == '|') {
                        secondaryIndexOnDoctorID[doc_id].push_back(app_id);
                        app_id = "";
                    }
                    else {
                        app_id += line[i];
                    }
                    i++;
                }
            }
            file.close();
        }
        void addAppointmentSecondaryIndex(Appointment a) {
            secondaryIndexOnDoctorID[a.getDoctorID()].push_back(a.getID());
            fixSecondaryIndexFile();
        }
        void deleteAppointmentSecondaryIndex(string Doctor_ID,string ID) {
            secondaryIndexOnDoctorID[Doctor_ID].erase(find(secondaryIndexOnDoctorID[Doctor_ID].begin(), secondaryIndexOnDoctorID[Doctor_ID].end(), ID));
            if (secondaryIndexOnDoctorID[Doctor_ID].empty()) {
                secondaryIndexOnDoctorID.erase(Doctor_ID);
            }
            fixSecondaryIndexFile();
        }
   void updateAppointmentSecondaryIndex(Appointment appointment, string newDate) {
            // Remove from the current date's entry if it exists
            if (secondaryIndexOnDate.find(appointment.getDate()) != secondaryIndexOnDate.end()) {
                secondaryIndexOnDate[appointment.getDate()].erase(
                    find(secondaryIndexOnDate[appointment.getDate()].begin(),
                        secondaryIndexOnDate[appointment.getDate()].end(),
                        appointment.getID()));
                if (secondaryIndexOnDate[appointment.getDate()].empty()) {
                    secondaryIndexOnDate.erase(appointment.getDate());
                }
            }
            // Add to the new date entry
            secondaryIndexOnDate[newDate].push_back(appointment.getID());
            fixSecondaryIndexFile();
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
        void deleteAppointment(string ID) {
            if (!filePrimaryIndex.exists(ID)) {
                cout << "Appointment ID doesn't exist.\n";
                return;
            }
            Appointment a = file.readAppointmentRecord(filePrimaryIndex.primaryIndex[ID]);
            file.deleteAppointmentRecord(filePrimaryIndex.primaryIndex[ID]);
            fileSecondaryIndex.deleteAppointmentSecondaryIndex(a.getDoctorID(), ID);
            filePrimaryIndex.deleteAppointmentPrimaryIndex(ID);
        }
        void updateAppointment(string ID, string newDate) {
            if (!filePrimaryIndex.exists(ID)) {
                cout << "Appointment ID doesn't exist.\n";
                return;
            }

            // Read the current Appointment record
            int byteOffset = filePrimaryIndex.primaryIndex[ID];
            Appointment currentAppointment = file.readAppointmentRecord(byteOffset);

            // Update the Appointment record's date in the file
            currentAppointment.setDate(newDate);
            file.updateAppointmentRecord(byteOffset, currentAppointment);

            // Update the primary index (optional if the ID does not change)
            filePrimaryIndex.updateAppointmentPrimaryIndex(ID, byteOffset);

            // Update the secondary index
            fileSecondaryIndex.updateAppointmentSecondaryIndex(currentAppointment, newDate);
        }
        Appointment getAppointment(string ID) {
            return file.readAppointmentRecord(filePrimaryIndex.primaryIndex[ID]);
        }
        void printAppointmentInfo() {
            string ID;
            cout << "Enter ID: ";
            cin >> ID;
            Appointment info = file.readAppointmentRecord(filePrimaryIndex.primaryIndex[ID]);
            cout << "Date: " << info.getDate() << ", Doctor ID: " << info.getDoctorID() << "\n";
        }
};
