#include "Database_interface.h"

namespace Database_interface
{
	Database_interface::Database_interface(char* sqlite_database_name)
	{
		int return_code = sqlite3_open(sqlite_database_name, &db);
		if (return_code) {

			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(-1);
		}
	}

	bool Database_interface::close_database()
	{
		if (sqlite3_close(db) == SQLITE_BUSY)
		{
			return false;
		}

		return true;
	}

	bool Database_interface::create_statement(const char* query, sqlite3_stmt** stmt)
	{
		// Compile sql statement to binary.
		if (sqlite3_prepare_v2(db, query, -1, stmt, NULL) != SQLITE_OK)
		{

			fprintf(stderr, "Error while compiling sql: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			sqlite3_finalize(*stmt);
			
			return false;
		}

		return true;
	}

	std::vector<Measured_data::Measured_data*> Database_interface::load_measured_data()
	{
		sqlite3_stmt* stmt = 0;
		std::vector<Measured_data::Measured_data*> data_vector;
		const unsigned char* measure_dat;
		double IG = 0;
		int return_code = 0;

		std::string query = "select measuredat, ist from measuredvalue;";

		if (create_statement(query.c_str(), &stmt)) {

			// Iterate through results.
			while ((return_code = sqlite3_step(stmt)) == SQLITE_ROW)
			{
				measure_dat = sqlite3_column_text(stmt, 0);
				IG = sqlite3_column_double(stmt, 1);

				Measured_data::Measured_data* data = new Measured_data::Measured_data(measure_dat, IG);

				data_vector.push_back(data);
			}
		}		

		return data_vector;
	}
}
