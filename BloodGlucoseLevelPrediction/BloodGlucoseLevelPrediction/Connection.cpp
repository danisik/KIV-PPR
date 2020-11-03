#include "Connection.h"

namespace NeuronNet
{
	Connection::Connection() 
	{
		weight = 0.0;
		delta_weight = 0.0;
	}

	void Connection::set_weight(double p_weight)
	{
		weight = p_weight;
	}

	void Connection::set_delta_weight(double p_delta_weight)
	{
		delta_weight = p_delta_weight;
	}

	double Connection::get_weight()
	{
		return weight;
	}

	double Connection::get_delta_weight()
	{
		return delta_weight;
	}

}
