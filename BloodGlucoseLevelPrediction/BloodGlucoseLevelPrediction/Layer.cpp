#include "Layer.h"

namespace NeuronNet
{
	Layer::Layer(double p_bias) 
	{
		bias = p_bias;
	}

	void Layer::add_neuron(Neuron* neuron)
	{
		neurons.push_back(neuron);
	}

	std::vector<Neuron*> Layer::get_neurons() 
	{
		return neurons;
	}

	double Layer::get_bias()
	{
		return bias;
	}
}