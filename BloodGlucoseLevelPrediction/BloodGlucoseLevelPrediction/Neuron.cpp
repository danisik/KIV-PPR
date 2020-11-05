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

	double Neuron::get_gradient()
	{
		return gradient;
	}

	void Neuron::set_gradient(double p_gradient)
	{
		gradient = p_gradient;
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
			value = tanh(sum);
		} 
		else 
		{
			value = tanh(sum);
		}
	}

	void Neuron::calculate_hidden_neuron_gradient(std::vector<Neuron*> next_neurons)
	{
		// Sum DOW.
		double sum = 0.0;
		for (unsigned int i = 0; i < next_neurons.size(); i++)
		{
			sum += connections.at(i)->get_weight() * next_neurons.at(i)->get_gradient();
		}

		gradient = sum * (1 - value * value);
	}

	double Neuron::calculate_output_neuron_gradient(double target_value)
	{
		double v_gradient = target_value - value;

		// TODO: USE DERIVATION OF SOFTMAX.
		v_gradient = v_gradient * (1 - value * value);

		gradient = v_gradient;

		return v_gradient;
	}

	void Neuron::update_connections_weight(std::vector<Neuron*> next_neurons, double eta, double alpha)
	{
		for (unsigned int i = 0; i < connections.size(); i++)
		{
			Connection* connection = connections.at(i);
			double delta_weight = connection->get_delta_weight();

			delta_weight = (eta * value * next_neurons.at(i)->get_gradient()) + (alpha * delta_weight);

			connection->set_weight(connection->get_weight() + delta_weight);
			connection->set_delta_weight(delta_weight);
		}
	}
}