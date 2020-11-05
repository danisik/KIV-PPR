#include "Measured_data.h"
#include <ctime>

namespace Measured_data
{
	Measured_data::Measured_data(tm* p_time, double p_IG, uint64_t p_segment_id)
	{
		time = p_time;
		IG = p_IG;
		segment_id = p_segment_id;
	}

	double Measured_data::get_IG()
	{
		return IG;
	}

	uint64_t Measured_data::get_segment_id()
	{
		return segment_id;
	}

	tm* Measured_data::get_time()
	{
		return time;
	}
}