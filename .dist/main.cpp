#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <string>
#include <algorithm>
#include "appointment.cpp"
using namespace std;

string toLowerCase(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

class HealthCareManagementSystem {
private:
    DoctorTable* dt;
    AppointmentTable* at;

    void mainMenu() {
        cout << "1-Add new doctor\n";
        cout << "2-Add new appointment\n";
        cout << "3-Update Doctor Name (Doctor ID)\n";
        cout << "4-Update Appointment Date (Appointment ID)\n";
        cout << "5-Delete Appointment (Appointment ID)\n";
        cout << "6-Delete Doctor (Doctor ID)\n";
        cout << "7-Print Doctor Info (Doctor ID)\n";
        cout << "8-Print Appointment Info (Appointment ID)\n";
        cout << "9-Write Query\n";
        cout << "0-Exit\n";
    }

    void executeQuery(const string& query) {
        cout << "Received query: " << query << endl;

        regex selectAllDoctorsRegex(
                R"(SELECT\s+\*\s+FROM\s+Doctors\s+WHERE\s+DoctorID\s*=\s*'(.*?)'\;)",
                regex::icase);
        regex selectAllAppointmentsRegex(
                R"(SELECT\s+\*\s+FROM\s+Appointments\s+WHERE\s+DoctorID\s*=\s*'(.*?)'\;)",
                regex::icase);
        regex selectDoctorNameRegex(
                R"(SELECT\s+DoctorName\s+FROM\s+Doctors\s+WHERE\s+DoctorID\s*=\s*'(.*?)'\;)",
                regex::icase);
        regex selectAllDoctorsNoWhereRegex(
                R"(SELECT\s+\*\s+FROM\s+Doctors\s*\;)",
                regex::icase);
        regex selectAllAppointmentsNoWhereRegex(
                R"(SELECT\s+\*\s+FROM\s+Appointments\s*\;)",
                regex::icase);
        regex selectDoctorNamesRegex(
                R"(SELECT\s+DoctorName\s+FROM\s+Doctors\s*\;)",
                regex::icase);

        smatch matches; //smatch for regex match string

        if (regex_match(query, matches, selectAllDoctorsRegex)) {
            string doctorID = matches[1].str(); // after where at indx 0 there is the query itself
            // and in indx one the first capture group after where
            cout << "Querying all doctors with ID: " << doctorID << endl;
            queryDoctorsByPrimaryKey(doctorID);
        } else if (regex_match(query, matches, selectAllAppointmentsRegex)) {
            string doctorID = matches[1].str();
            cout << "Querying all appointments for doctor ID: " << doctorID << endl;
            queryAppointmentsByDoctorID(doctorID);
        } else if (regex_match(query, matches, selectDoctorNameRegex)) {
            string doctorID = matches[1].str();
            cout << "Querying doctor name with ID: " << doctorID << endl;
            queryDoctorNameByDoctorID(doctorID);
        } else if (regex_match(query, matches, selectAllDoctorsNoWhereRegex)) {
            cout << "Querying all doctors" << endl;
            queryAllDoctors();
        } else if (regex_match(query, matches, selectAllAppointmentsNoWhereRegex)) {
            cout << "Querying all appointments" << endl;
            queryAllAppointments();
        } else if (regex_match(query, matches, selectDoctorNamesRegex)) {
            cout << "Querying all doctor names" << endl;
            queryAllDoctorNames();
        } else {
            cout << "Invalid query format.\n";
        }
    }

    void queryDoctorsByPrimaryKey(const string& doctorID) {
        string lowerDoctorID = toLowerCase(doctorID);
        //iterates over the primary index map of the DoctorTable(dt pointer to it)
        for (const auto& [id, offset] : dt->getPrimaryIndexFile()->primaryIndex) {
            if (toLowerCase(id) == lowerDoctorID) {
                Doctor doc = dt->getFile()->readDoctorRecord(offset);
                cout << "Doctor Record: ID=" << doc.getID() << ", Name=" << doc.getName()
                     << ", Address=" << doc.getAddress() << endl;
                return;
            }
        }
        cout << "No doctor found with ID " << doctorID << ".\n";
    }

    void queryAppointmentsByDoctorID(const string& doctorID) {
        //vector to store the offsets of matching appointment records
        // as ID is foreign key here it can be repeated
        vector<int> offsets;
        string lowerDoctorID = toLowerCase(doctorID);
        //Iterates over the primary index map of the AppointmentTable(at pointer to it)
        for (const auto& [id, offset] : at->getPrimaryIndexFile()->primaryIndex) {
            Appointment app = at->getFile()->readAppointmentRecord(offset);
            if (toLowerCase(app.getDoctorID()) == lowerDoctorID) {
                //add the offset to the vector if the doctor ID matches
                offsets.push_back(offset);
            }
        }
        if (offsets.empty()) {
            cout << "No appointments found for Doctor ID " << doctorID << ".\n";
            return;
        }
        for (int offset : offsets) {
            //read the appointment record from the file using the offset
            Appointment app = at->getFile()->readAppointmentRecord(offset);
            cout << "Appointment Record: ID=" << app.getID()
                 << ", Date=" << app.getDate() << ", Doctor ID=" << app.getDoctorID() << endl;
        }
    }

    void queryDoctorNameByDoctorID(const string& doctorID) {
        string lowerDoctorID = toLowerCase(doctorID);
        //iterate over the primary index map of the DoctorTable
        for (const auto& [id, offset] : dt->getPrimaryIndexFile()->primaryIndex) {
            if (toLowerCase(id) == lowerDoctorID) {
                //read the doctor record from the file using the offset
                Doctor doc = dt->getFile()->readDoctorRecord(offset);
                cout << "Doctor Name: " << doc.getName() << endl;
                return;
            }
        }
        cout << "No doctor found with ID " << doctorID << ".\n";
    }

    void queryAllDoctors() {
        for (const auto& [id, offset] : dt->getPrimaryIndexFile()->primaryIndex) {
            Doctor doc = dt->getFile()->readDoctorRecord(offset);
            if (!doc.getID().empty() && !doc.getName().empty() && !doc.getAddress().empty())
            cout << "Doctor Record: ID=" << doc.getID() << ", Name=" << doc.getName()
                 << ", Address=" << doc.getAddress() << endl;
        }
    }

    void queryAllAppointments() {
        for (const auto& [id, offset] : at->getPrimaryIndexFile()->primaryIndex) {
            Appointment app = at->getFile()->readAppointmentRecord(offset);
            cout << "Appointment Record: ID=" << app.getID() << ", Date=" << app.getDate()
                 << ", Doctor ID=" << app.getDoctorID() << endl;
        }
    }

    void queryAllDoctorNames() {
        for (const auto& [id, offset] : dt->getPrimaryIndexFile()->primaryIndex) {
            Doctor doc = dt->getFile()->readDoctorRecord(offset);
            cout << "Doctor Name: " << doc.getName() << endl;
        }
    }

    void writeSQLQuery() {
        string query;
        cout << "Enter your SQL query (e.g., Select * from Doctors where DoctorID='1';):\n";
        cin.ignore();
        getline(cin, query);
        executeQuery(query);
    }

public:
    HealthCareManagementSystem() {
        dt = new DoctorTable();
        at = new AppointmentTable(dt);
    }
    DoctorTable* getDoctorTable() {
        return dt;
    }

    AppointmentTable* getAppointmentTable() {
        return at;
    }

    void run() {
        char choice;
        string input;
        while (true) {
            mainMenu();
            cin >> choice;
            if (choice == '0') {
                break;  // Exit the loop when choice is '0'
            }

            if (choice == '1') {
                dt->addDoctor(Doctor());
            }
            else if (choice == '2') {
                at->addAppointment(Appointment());
            }
            else if (choice == '3') { // Update doctor name
                cout << "Enter ID: ";
                cin >> input;
                cout << "Enter new name: ";
                string newname;
                cin >> newname;
                dt->updateDoctor(input, newname);
            }
            else if (choice == '4') { // Update appointment date
                cout << "Enter ID: ";
                cin >> input;
                cout << "Enter new date: ";
                string newdate;
                cin >> newdate;
                at->updateAppointment(input, newdate);
            }
            else if (choice == '5') { // Delete appointment
                cout << "Enter ID: ";
                cin >> input;
                at->deleteAppointment(input);
            }
            else if (choice == '6') { // Delete doctor
                cout << "Enter ID: ";
                cin >> input;
                dt->deleteDoctor(input);
            }
            else if (choice == '7') { // Print doctor info
                dt->printDoctorInfo();
            }
            else if (choice == '8') { // Print appointment info
                at->printAppointmentInfo();
            }
            else if (choice == '9') { // Execute SQL query
                writeSQLQuery();
            }
            else {
                cout << "Invalid choice! Please try again.\n";
            }
        }
    }

};

int main() {
    HealthCareManagementSystem* HCMS = new HealthCareManagementSystem();
    HCMS->run();
    return 0;
}
