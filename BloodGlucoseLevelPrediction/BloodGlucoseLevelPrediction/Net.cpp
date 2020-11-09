#include <time.h>

#include "Net.h"
#include "Neuron.h"

namespace NeuronNet
{
	Net::Net()
	{			
		Layer* input_layer = new Layer(1.0);
		Layer* hidden_layer_1 = new Layer(1.0);
		Layer* hidden_layer_2 = new Layer(1.0);
		Layer* output_layer = new Layer(1.0);

		// Create neurons for input layer.
		for (unsigned int i = 0; i < il_neurons_count; i++)
		{
		input_layer->add_neuron(new Neuron(i, 0.0, hl1_neurons_count));
		}

		layers.push_back(input_layer);

		// Create neurons for first hidden layer.
		for (unsigned int i = 0; i < hl1_neurons_count; i++)
		{
			hidden_layer_1->add_neuron(new Neuron(i, 0.0, hl2_neurons_count));
		}

		layers.push_back(hidden_layer_1);

		// Create neurons for second hidden layer.
		for (unsigned int i = 0; i < hl2_neurons_count; i++)
		{
			hidden_layer_2->add_neuron(new Neuron(i, 0.0, ol_neurons_count));
		}

		layers.push_back(hidden_layer_2);


		band_values.push_back(Low_Threshold);

		// Create neurons for output layer.
		for (unsigned int i = 0; i < ol_neurons_count; i++)
		{
			output_layer->add_neuron(new Neuron(i, 0.0, 0));

			if (i > 0 && i < (ol_neurons_count - 1))
			{
				band_values.push_back(Band_Index_To_Level(i));
			}
		}

		band_values.push_back(High_Threshold);

		layers.push_back(output_layer);
	}

	void Net::feed_forward(std::vector<double> input_values)
	{
		Layer* input_layer = layers.at(0);
		std::vector<Neuron*> input_layer_neurons = input_layer->get_neurons();

		for (unsigned int i = 0; i < input_layer_neurons.size(); i++)
		{
			input_layer_neurons.at(i)->set_value(risk(input_values.at(i)));
		}

		// Start feed forward for every neuron in hidden / output layer.
		for (unsigned int i = 1; i < layers.size(); i++) {

			bool last_layer = i == (layers.size() - 1) ? true : false;

			Layer* previous_layer = layers.at(i - 1);
			Layer* current_layer = layers.at(i);

			std::vector<Neuron*> previous_neurons = previous_layer->get_neurons();
			std::vector<Neuron*> current_neurons = current_layer->get_neurons();

			for (unsigned int j = 0; j < current_neurons.size(); j++)
			{
				current_neurons.at(j)->feed_forward(previous_neurons, previous_layer->get_bias(), current_layer->get_bias(), last_layer);
			}

			if (last_layer)
			{
				// TODO: uncomment when softmax iterative implemented.
				softmax(current_neurons, current_neurons.size());
			}
		}

		/*
		std::cout << "\nOutput:\n";
		Layer* output_layer = layers.at(3);
		for (int i = 0; i < output_layer->get_neurons().size(); i++)
		{
			std::cout << output_layer->get_neurons().at(i)->get_value() << "\n";
		}
		std::cout << "\n";
		*/
	}

	void Net::prepare_single_output_target_value(std::vector<double>& prepared_values, double target_value)
	{
		prepared_values.clear();

		int target_value_index = -1;
		bool index_found = false;

		for (unsigned int i = 0; i < ol_neurons_count; i++)
		{
			prepared_values.push_back(0.0);

			if (!index_found)
			{
				if (
					(i == 0 && target_value <= band_values.at(i))
					||
					(i > 0 && i < 30 && target_value > band_values.at(i - 1) && target_value <= band_values.at(i))
					||
					(i == 30 && target_value > band_values.at(i - 1) && target_value < band_values.at(i + 1))
					||
					(i == 31 && target_value >= band_values.at(i))
					)
				{
					index_found = true;
					target_value_index = i;
				}
			}
		}

		prepared_values.at(target_value_index) = target_value;

		int size = prepared_values.size();

		double neuron_value = 0.0;
		double maximum = -INFINITY;
		double sum = 0.0;

		for (unsigned int i = 0; i < size; i++) {

			neuron_value = prepared_values.at(i);

			if (maximum < neuron_value) {

				maximum = neuron_value;
			}
		}

		for (unsigned int i = 0; i < size; i++) {

			neuron_value = prepared_values.at(i);
			sum += exp(neuron_value - maximum);
		}

		double constant = maximum + log(sum);

		for (unsigned int i = 0; i < size; i++) {

			prepared_values.at(i) = exp(prepared_values.at(i) - constant);
		}
	}	

	void Net::back_propagation(std::vector<double> target_values)
	{
		std::vector<double> prepared_values;

		for (unsigned int target_value_index = 0; target_value_index < target_values.size(); target_value_index++)
		{
			prepare_single_output_target_value(prepared_values, target_values.at(target_value_index));

			Layer* output_layer = layers.at(layers.size() - 1);
			
			double current_error = 0.0;
			double relative_error = 0.0;

			for (unsigned int i = 0; i < output_layer->get_neurons().size(); i++)
			{
				double delta = prepared_values.at(i) - output_layer->get_neurons().at(i)->get_value();
				current_error += delta * delta;

				// abs(vypoèítaná hodnota - namìøená hodnota)/namìøená hodnota

				// TODO: uncomment -> zakomentováno protože to pak hází chybovost pøes 5000%.
				//relative_error += abs(output_layer->get_neurons().at(i)->get_value() - prepared_values.at(i)) / prepared_values.at(i);				
			}

			relative_error = relative_error / output_layer->get_neurons().size();
			current_error = current_error / output_layer->get_neurons().size();

			current_error = sqrt(current_error) + relative_error;

			error = ((error * smoothing_factor) + current_error) / (smoothing_factor + 1.0);

			// Calculate gradients in output layer.
			std::vector<Neuron*> output_neurons = output_layer->get_neurons();						
			for (unsigned int i = 0; i < output_neurons.size(); i++)
			{
				/*
				output_neurons.at(i)->set_gradient(
					(prepared_values.at(i) - output_neurons.at(i)->get_value())
					* 
					(1 - (output_neurons.at(i)->get_value() * output_neurons.at(i)->get_value()))
				);
				*/

				// Viz: https://stackoverflow.com/questions/57631507/how-can-i-take-the-derivative-of-the-softmax-output-in-back-prop.
				output_neurons.at(i)->set_gradient(prepared_values.at(i) - output_neurons.at(i)->get_value());
			}
						
			//softmax_derivative(output_neurons, output_neurons.size(), prepared_values);
			

			// Calculate gradients in (L-1, L-2, ..., 2) layers - hidden layers. 
			for (unsigned int i = (layers.size() - 2); i > 0; i--)
			{
				std::vector<Neuron*> current_hidden_neurons = layers.at(i)->get_neurons();
				std::vector<Neuron*> next_layer_neurons = layers.at(i + 1)->get_neurons();

				for (unsigned int j = 0; j < current_hidden_neurons.size(); j++)
				{
					current_hidden_neurons.at(j)->calculate_hidden_neuron_gradient(next_layer_neurons);
				}
			}

			// Set new weights in all connections.
			for (unsigned int i = 0; i < (layers.size() - 1); i++)
			{
				std::vector<Neuron*> current_neurons = layers.at(i)->get_neurons();
				std::vector<Neuron*> next_neurons = layers.at(i + 1)->get_neurons();

				for (unsigned int j = 0; j < current_neurons.size(); j++)
				{
					current_neurons.at(j)->update_connections_weight(next_neurons, eta, alpha);
				}
			}
		}
	}

	void Net::softmax(std::vector<Neuron*> output_neurons, unsigned int size)
	{
		// Calculate output neurons value by SoftMax function.
		double neuron_value = 0.0;
		double maximum = -INFINITY;
		double sum = 0.0;

		for (unsigned int i = 0; i < size; i++) {

			neuron_value = output_neurons.at(i)->get_value();

			if (maximum < neuron_value) {

				maximum = neuron_value;
			}
		}

		for (unsigned int i = 0; i < size; i++) {

			neuron_value = output_neurons.at(i)->get_value();
			sum += exp(neuron_value - maximum);
		}

		double constant = maximum + log(sum);

		for (unsigned int i = 0; i < size; i++) {

			Neuron* output_neuron = output_neurons.at(i);
			output_neuron->set_value(exp(output_neuron->get_value() - constant));
		}
	}

	void Net::softmax_derivative(std::vector<Neuron*> output_neurons, unsigned int size, std::vector<double> target_values)
	{
		double sum = 0.0;
		double xj_value = 0.0;

		for (unsigned int i = 0; i < size; i++) {

			sum += exp(output_neurons.at(i)->get_value());
		}

		for (unsigned int i = 0; i < size; i++) {

			Neuron* output_neuron = output_neurons.at(i);
			double delta = target_values.at(i) - output_neuron->get_value();

			// Calculate xi_value;
			double new_value = exp(output_neuron->get_value()) / sum;
			
			if (i == 0)
			{
				// When i = j, then xi_value = xj_value.
				xj_value = new_value;

				new_value = xj_value - (xj_value * new_value);
			}
			else
			{
				new_value = 0 - (xj_value * new_value);
			}

			output_neuron->set_gradient(delta * new_value);
		}
	}

	double Net::risk(const double bg) 
	{
		// DOI:  10.1080/10273660008833060
		// mmol/L.
		const double original_risk = 1.794 * (pow(log(bg), 1.026) - 1.861);
		return original_risk / 3.5;
	}

	double Net::Band_Index_To_Level(const size_t index) 
	{
		if (index == 0) return Low_Threshold - Half_Band_Size;
		if (index >= Band_Count - 1) return High_Threshold + Half_Band_Size;

		return Low_Threshold + static_cast<double>(index - 1) * Band_Size + Half_Band_Size;
	}

	double Net::get_error()
	{
		// Round error to 4 decimal values.
		int error_cut = error * 10000;
		error = (double) error_cut / 10000.0;

		return error;
	}
}