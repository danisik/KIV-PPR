#include "Measured_data.h"
#include <ctime>

namespace Measured_data
{
	Measured_data::Measured_data(const unsigned char* p_timestamp, double p_IG)
	{
		timestamp = p_timestamp;
		IG = p_IG;
	}

	double Measured_data::get_IG()
	{
		return IG;
	}

	const unsigned char* Measured_data::get_Timestamp()
	{
		return timestamp;
	}
}