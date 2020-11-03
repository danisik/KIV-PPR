#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "sqlite3/sqlite3.h"
#include "Measured_data.h"
#include "Database_interface.h"
#include "Net.h"

// https://courseware.zcu.cz/portal/studium/courseware/kiv/ppr/samostatna-prace/zadani.html

int main(int argc, char** argv)
{
    // First arguments is full path to program.
    if (argc <= 2) {

        std::cout << "Invalid number of parameters (minute prediction + sqlite database name REQUIRED).\n";
        exit(-1);
    }

    // Parse input arguments.
    int minute_prediction = std::atoi(argv[1]);
    char* sqlite_database_name = argv[2];

    // Check if minute prediction have normal value.
    if (minute_prediction == 0) {

        std::cout << "Invalid minute prediction value.\n";
        exit(-1);
    }

    Database_interface::Database_interface* db_interface = new Database_interface::Database_interface(sqlite_database_name);
    std::vector<Measured_data::Measured_data*> measured_data = db_interface->load_measured_data();

    if (measured_data.size() == 0) {

        std::cout << "No measured data found.\n";
        exit(-1);
    }

    // Set srand seed using current time.
    time_t current_time;
    srand(time(&current_time));

    NeuronNet::Net* neuron_net = new NeuronNet::Net();

    std::vector<double> values;

    values.push_back(11.05);
    values.push_back(10.88);
    values.push_back(10.88);
    values.push_back(10.88);
    values.push_back(10.77);
    values.push_back(10.66);
    values.push_back(10.49);
    values.push_back(10.32);

    std::cout << "\nInput:\n";
    for (int i = 0; i < values.size(); i++) {

        std::cout << values.at(i) << "\n";
    }
    std::cout << "\n";

    neuron_net->feed_forward(values);

    std::cout << "End successfully, measured data size = " << measured_data.size() << "\n";
}


