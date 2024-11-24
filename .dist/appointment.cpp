#include <iostream>
#include <string>
using namespace std;
class Appointment{
    string app_id;
    string app_date;
    string doctor_id;
    

    Appointment(string app_id, string app_date, string doctor_id){
        this->app_id=app_id;
        this->app_date=app_date;
        this->doctor_id=doctor_id;
    }

};