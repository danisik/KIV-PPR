#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <chrono>
#include <time.h>
#include <cstdlib>
#include <thread>

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

static void neural_network_training(int network_id, std::vector<Measured_data::Measured_data*> measured_data, int minute_prediction)
{    
    //std::chrono::steady_clock::time_point begin;
    //std::chrono::steady_clock::time_point end;

    // Set srand seed using current time.
    time_t current_time;
    srand(time(&current_time) * (network_id + 1));

    //begin = std::chrono::steady_clock::now();

    NeuronNet::Net* neuron_net = new NeuronNet::Net();

    //end = std::chrono::steady_clock::now();
    //std::cout << "Creating neural network = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    int values_count = 0;

    // Variables for feed forward values.
    uint64_t previous_segment_id = 0;
    uint64_t current_segment_id = 0;
    std::vector<double> values;
    time_t current_first_time;
    Measured_data::Measured_data* current_measure_data;
    bool current_time_spacing_ok = false;
    int current_time_difference_seconds = 0;

    // Variables for back propagation values.
    uint64_t previous_target_segment_id = 0;
    uint64_t current_target_segment_id = 0;
    std::vector<double> target_values;
    time_t target_first_time;
    Measured_data::Measured_data* target_measure_data;
    time_t current_target_time;
    bool target_time_spacing_ok = false;
    int target_time_difference_seconds = 0;

    //begin = std::chrono::steady_clock::now();

    tm tmm;

    int prediction_time_offset = minute_prediction / 5;

    long int counting = 0;

    for (unsigned int i = 0; i < measured_data.size(); i++)
    {
        current_measure_data = measured_data.at(i);
        current_segment_id = current_measure_data->get_segment_id();

        // Check if index of predicted value is lower that size of vector.
        if ((i + prediction_time_offset) >= measured_data.size())
        {
            break;
        }

        target_measure_data = measured_data.at(i + prediction_time_offset);
        current_target_segment_id = target_measure_data->get_segment_id();

        tmm = *current_measure_data->get_time();
        current_time = mktime(&tmm);

        tmm = *target_measure_data->get_time();
        current_target_time = mktime(&tmm);

        if (values_count == 8)
        {
            neuron_net->feed_forward(values);
            neuron_net->back_propagation(target_values);

            values.clear();
            target_values.clear();
            values_count = 0;

            counting++;
        }

        if (values_count == 0)
        {
            current_first_time = current_time;
            previous_segment_id = current_segment_id;

            target_first_time = current_target_time;
            previous_target_segment_id = current_target_segment_id;
        }

        // Check segment id of current data and previous data.
        if (current_segment_id == previous_segment_id && previous_target_segment_id == current_target_segment_id)
        {
            current_time_spacing_ok = false;
            target_time_spacing_ok = false;

            current_time_difference_seconds = abs(difftime(current_first_time, current_time));
            target_time_difference_seconds = abs(difftime(target_first_time, current_target_time));

            if (values_count < 7)
            {
                current_time_spacing_ok = compare_times_difference(current_time_difference_seconds, 5, values_count);
                target_time_spacing_ok = compare_times_difference(target_time_difference_seconds, 5, values_count);

                if (values_count == 6 && current_time_spacing_ok && target_time_spacing_ok)
                {
                    i++;
                }
            }
            else
            {
                // Calculate time difference for t-40 minutes. (values_count + 1) is for skipping 8th value and go for 9.
                current_time_spacing_ok = compare_times_difference(current_time_difference_seconds, 5, values_count + 1);
                target_time_spacing_ok = compare_times_difference(target_time_difference_seconds, 5, values_count + 1);
            }

            if (current_time_spacing_ok && target_time_spacing_ok)
            {
                values.push_back(current_measure_data->get_IG());
                target_values.push_back(target_measure_data->get_IG());
                values_count++;
            }
            else
            {
                values.clear();
                target_values.clear();
                values_count = 0;
            }
        }
        else
        {
            values.clear();
            target_values.clear();
            values_count = 0;
        }

    }

    //end = std::chrono::steady_clock::now();
    //std::cout << "Training neural network = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    std::cout << "Network " << network_id << ":" << " Error = " << (neuron_net->get_error() * 100) << "%\n";
    //std::cout << "End successfully, successful runs = " << counting << "\n";
}

// https://courseware.zcu.cz/portal/studium/courseware/kiv/ppr/samostatna-prace/zadani.html

// Input parameters:
// 1.minute_prediction - X Minutes prediction of IG values.
// 2.sqlite_database_name - Name of database (relative path to root dir).
int main(int argc, char** argv)
{    
    // First arguments is full path to program.
    if (argc <= 2) 
    {

        std::cout << "Invalid number of parameters (minute prediction + sqlite database name REQUIRED).\n";
        exit(-1);
    }

    // Parse input arguments.
    int minute_prediction = std::atoi(argv[1]);
    char* sqlite_database_name = argv[2];

    // Check if minute prediction have normal value.
    if (minute_prediction == 0) 
    {

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

    if (measured_data.size() == 0) 
    {
        std::cout << "No measured data found.\n";
        exit(-1);
    }

    std::cout << "Risk function used.\n";
    std::cout << "Multiclass classification.\n";

    std::vector<std::thread> threads;

    begin = std::chrono::steady_clock::now();


    // CUDA thread. 
    //neural_network_training<<< 10, 1 >>>(0, measured_data, minute_prediction);

    for (unsigned int i = 0; i < 10; i++)
    {
        threads.push_back(std::thread(neural_network_training, i, measured_data, minute_prediction));
    }

    for (std::thread& th : threads)
    {
        th.join();
    }

    end = std::chrono::steady_clock::now();
    std::cout << "Training neural networks = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}


