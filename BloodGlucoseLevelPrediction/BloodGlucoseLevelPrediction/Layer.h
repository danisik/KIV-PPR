#include "Neuron.h"
#include <vector>

#pragma once
namespace NeuronNet 
{
	class Layer
	{
		private:
				std::vector<Neuron*> neurons;
				double bias;

		public:
				Layer(double p_bias);
				void add_neuron(Neuron* neuron);
				std::vector<Neuron*> get_neurons();
				double get_bias();
	};
}
