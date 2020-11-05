#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <chrono>
#include <time.h>
#include <cstdlib>

#include "sqlite3/sqlite3.h"
#include "Measured_data.h"
#include "Database_interface.h"
#include "Net.h"


static bool compare_times_difference(int times_difference, int condition, int values_count)
{
    if (times_difference / 60 == (condition * values_count))
    {
        return true;
    }

    return false;
}

// https://courseware.zcu.cz/portal/studium/courseware/kiv/ppr/samostatna-prace/zadani.html

// Input parameters:
// 1.minute_prediction - X Minutes prediction of IG values.
// 2.sqlite_database_name - Name of database (relative path to root dir).
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

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    begin = std::chrono::steady_clock::now();

    Database_interface::Database_interface* db_interface = new Database_interface::Database_interface(sqlite_database_name);
    std::vector<Measured_data::Measured_data*> measured_data = db_interface->load_measured_data();

    end = std::chrono::steady_clock::now();
    std::cout << "Loading data from database = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    if (measured_data.size() == 0) {

        std::cout << "No measured data found.\n";
        exit(-1);
    }

    // Set srand seed using current time.
    time_t current_time;
    srand(time(&current_time));

    begin = std::chrono::steady_clock::now();

    NeuronNet::Net* neuron_net = new NeuronNet::Net();

    end = std::chrono::steady_clock::now();
    std::cout << "Creating neural network = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    int values_count = 0;
    uint64_t previous_segment_id = 0;
    uint64_t current_segment_id = 0;    
    time_t current_first_time;
    Measured_data::Measured_data* current_measure_data;
    std::vector<double> values;
    std::vector<double> target_values;
    bool time_spacing_ok = false;
    int time_difference_seconds = 0;

    begin = std::chrono::steady_clock::now();

    tm tmm;

    for (unsigned int i = 0; i < measured_data.size(); i++) 
    {    
        current_measure_data = measured_data.at(i);
        current_segment_id = current_measure_data->get_segment_id();

        tmm = *current_measure_data->get_time();
        current_time = mktime(&tmm);

        if (values_count == 8) 
        {
            neuron_net->feed_forward(values);
            

            // TODO: get 8 values 
            //neuron_net->back_propagation(values);

            values.clear();
            values_count = 0;
        }
        
        if (values_count == 0)
        {
            current_first_time = current_time;
            previous_segment_id = current_segment_id;
        }

        // Check segment id of current data and previous data.
        if (current_segment_id == previous_segment_id)
        {

            time_spacing_ok = false;
            
            time_difference_seconds = abs(difftime(current_first_time, current_time));            

            if (values_count < 7)
            {
                time_spacing_ok = compare_times_difference(time_difference_seconds, 5, values_count);

                if (values_count == 6 && time_spacing_ok)
                {
                    i++;
                }
            }
            else
            {
                // Calculate time difference for t-40 minutes. (values_count + 1) is for skipping 8th value and go for 9.
                time_spacing_ok = compare_times_difference(time_difference_seconds, 5, values_count + 1);
            }

            if (time_spacing_ok)
            {
                values.push_back(current_measure_data->get_IG());
                values_count++;
            }
            else
            {
                values.clear();
                values_count = 0;
            }
        }
        else
        {
            values.clear();
            values_count = 0;
        }        
        
    }

    end = std::chrono::steady_clock::now();
    std::cout << "Training neural network = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    std::cout << "End successfully, measured data size = " << measured_data.size() << "\n";
}


