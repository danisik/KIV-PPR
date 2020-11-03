

#pragma once

namespace NeuronNet
{
	class Connection
	{
		private:
				double weight;
				double delta_weight;

		public:
				Connection();
				void set_weight(double p_weight);
				void set_delta_weight(double p_delta_weight);

				double get_weight();
				double get_delta_weight();
	};
}

