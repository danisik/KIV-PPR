#include "Connection.h"
#include <vector>
#include <iostream>

#pragma once
namespace NeuronNet 
{
	class Neuron
	{
		private:
			int index;
			double value;
			double gradient;
			std::vector<Connection*> connections;

		public:
				Neuron(int p_index, double p_value, int connections_count);
				int get_index();
				void set_value(double p_value);
				double get_value();
				double get_gradient();
				void set_gradient(double p_gradient);
				std::vector<Connection*> get_connections();
				double generate_random_weight();
				void feed_forward(std::vector<Neuron*> previous_neurons, double previous_bias, double bias, bool last_layer);
				void calculate_hidden_neuron_gradient(std::vector<Neuron*> next_neurons);
				double calculate_output_neuron_gradient(double target_value);
				void update_connections_weight(std::vector<Neuron*> next_neurons, double eta, double alpha);
	};
}

