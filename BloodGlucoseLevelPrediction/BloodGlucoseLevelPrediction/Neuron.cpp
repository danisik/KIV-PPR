#include "Neuron.h"

namespace NeuronNet
{
	Neuron::Neuron(int p_index, double p_value, int connections_count)
	{
		index = p_index;
		value = p_value;

		// Initialize weight.
		for (int i = 0; i < connections_count; i++) 
		{
			Connection* connection = new Connection();
			connection->set_weight(generate_random_weight());
			connections.push_back(connection);
		}
	}

	int Neuron::get_index()
	{
		return index;
	}

	void Neuron::set_value(double p_value)
	{
		value = p_value;
	}

	double Neuron::get_value()
	{
		return value;
	}

	std::vector<Connection*> Neuron::get_connections()
	{
		return connections;
	}

	double Neuron::generate_random_weight()
	{		
		return (double)rand() / RAND_MAX;;
	}

	void Neuron::feed_forward(std::vector<Neuron*> previous_neurons, double previous_bias, double bias, bool last_layer)
	{
		// Calculate neuron value by TanH function.
		double sum = 0.0;

		for (unsigned int i = 0; i < previous_neurons.size(); i++)
		{
			Neuron* previous_neuron = previous_neurons.at(i);
			sum += previous_neuron->value * previous_neuron->get_connections().at(index)->get_weight();
		}

		sum += bias * previous_bias;

		if (last_layer)
		{
			value = sum;
		} 
		else 
		{
			value = tanh(sum);
		}
	}
}