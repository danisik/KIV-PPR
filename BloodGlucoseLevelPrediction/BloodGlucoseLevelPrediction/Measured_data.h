#include <time.h>
#include <cstdint>

#pragma once
namespace Measured_data 
{
	class Measured_data
	{
		private:
				tm* time;
				double IG;
				uint64_t segment_id;

		public:
				Measured_data(tm* p_time, double p_IG, uint64_t p_segment_id);
				tm* get_time();
				double get_IG();
				uint64_t get_segment_id();
	};
}

