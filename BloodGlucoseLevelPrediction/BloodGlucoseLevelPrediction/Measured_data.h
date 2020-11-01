#include <time.h>

#pragma once
namespace Measured_data 
{
	class Measured_data
	{
		private:
				const unsigned char* timestamp;
				double IG;

		public:
				Measured_data(const unsigned char* p_timestamp, double p_IG);
				const unsigned char* get_Timestamp();
				double get_IG();
	};
}

