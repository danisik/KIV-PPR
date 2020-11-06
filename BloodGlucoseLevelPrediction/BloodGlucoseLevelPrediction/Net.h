#include <vector>
#include <iostream>
#include "Neuron.h"
#include "Layer.h"

#pragma once

namespace NeuronNet
{
	class Net
	{		
		static constexpr double smoothing_factor = 100.0;

		static constexpr int il_neurons_count = 8;
		static constexpr int hl1_neurons_count = 16; 
		static constexpr int hl2_neurons_count = 26;
		static constexpr int ol_neurons_count = 32;


		static constexpr double Low_Threshold = 3.0;            //mmol/L below which a medical attention is needed
		static constexpr double High_Threshold = 13.0;            //dtto above

		static constexpr double Band_Size = (High_Threshold - Low_Threshold) / static_cast<double>(ol_neurons_count);                        //must imply relative error <= 10%
		static constexpr double Inv_Band_Size = 1.0 / Band_Size;        //abs(Low_Threshold-Band_Size)/Low_Threshold
		static constexpr double Half_Band_Size = 0.5 / Inv_Band_Size;
		static constexpr size_t Band_Count = ol_neurons_count + 2;
		static constexpr double eta = 0.15;
		static constexpr double alpha = 0.5;

		private:
				std::vector<Layer*> layers;
				std::vector<double> band_values;
				double error;

		public:
				Net();
				void feed_forward(std::vector<double> input_values);
				void back_propagation(std::vector<double> target_values);
				double get_error();

		private:
				double risk(const double bg);
				double Band_Index_To_Level(const size_t index);
				void softmax(std::vector<Neuron*> output_neurons, unsigned int size);
				void prepare_single_output_target_value(std::vector<double>& prepared_values, double target_value);
	};
}

