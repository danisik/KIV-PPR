#include <vector>
#include <string>

#include "Measured_data.h"
#include "sqlite3/sqlite3.h"


#pragma once
namespace Database_interface
{
	class Database_interface
	{
		private:
				sqlite3* db;

		public:
				Database_interface(char* sqlite_database_name);
				bool close_database();
				std::vector<Measured_data::Measured_data*> load_measured_data();

		private:				
				bool create_statement(const char* query, sqlite3_stmt** stmt);
	};
}

