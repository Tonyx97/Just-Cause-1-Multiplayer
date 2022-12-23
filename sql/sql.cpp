#include <defs/standard.h>

#include "sql.h"

/*bool MySQL::print_error()
{
	last_error = sql_handle ? std::string(mysql_error(sql_handle)) : "MySQL connection is closed";
	printf("%s\n", last_error.c_str());
	return false;
}

void MySQL::disconnect()
{
	if (sql_handle)
	{
		mysql_close(sql_handle);
		initialized = false;
		sql_handle = nullptr;
	}
}

bool MySQL::connect(const char* host, const char* db, const char* user, const char* pass, const bool& reconnect, const uint32_t& port)
{
	if (!initialized)
	{
		if (!(sql_handle = mysql_init(nullptr)))
			return print_error();

		initialized = true;
	}
	
	strcpy_s(host_r, host);
	strcpy_s(user_r, user);
	strcpy_s(pass_r, pass);
	strcpy_s(db_r, db);

	sql_handle->options.reconnect = reconnect;

	if (!mysql_real_connect(sql_handle, host, user, pass, db, port, nullptr, 0))
		return print_error();

	return true;
}

bool MySQL::select_db(const char* database)
{
	if (!sql_handle)
		return print_error();

	if (mysql_select_db(sql_handle, database))
		return print_error();

	return true;
}

std::unique_ptr<query_result, std::function<void(query_result*)>> MySQL::query_internal(const std::string& query_str)
{
	std::lock_guard<std::mutex> lock(mtx);

	if (mysql_real_query(sql_handle, query_str.c_str(), static_cast<uint32_t>(query_str.length())))
	{
		print_error();
		return nullptr;
	}
	
	std::unique_ptr<query_result, std::function<void(query_result*)>> ret_res = nullptr;

	if (MYSQL_RES* result = mysql_store_result(sql_handle))
	{
		ret_res = std::unique_ptr<query_result, std::function<void(query_result*)>>(new query_result(mysql_num_fields(result)), [&](query_result* ptr) { delete ptr; });
		
		uint32_t num_results = 0;

		do {
			MYSQL_FIELD* field = nullptr;

			for (uint32_t i = 0; i < ret_res->get_columns(); ++i)
			{
				mysql_field_seek(result, i);
				field = mysql_fetch_field(result);
				ret_res->add_column(col_data(field->type, field->name, field->length));
			}

			MYSQL_ROW row;

			while (row = mysql_fetch_row(result))
			{
				SQLValues* args = new SQLValues();

				uint32_t i = 0;

				std::for_each(ret_res->columns.begin(), ret_res->columns.end(), [&](const col_data& column)
				{
					if (row[i])
					{
						switch (column.type)
						{
						case MYSQL_TYPE_TIMESTAMP:
						case MYSQL_TYPE_TIME:
						case MYSQL_TYPE_DATETIME:
						case MYSQL_TYPE_DATE:
						case MYSQL_TYPE_STRING:
						case MYSQL_TYPE_VARCHAR:
						case MYSQL_TYPE_BLOB:
						case MYSQL_TYPE_VAR_STRING: args->add_value(std::string(row[i]));					break;
						case MYSQL_TYPE_FLOAT:		args->add_value(static_cast<float>(std::atof(row[i]))); break;
						case MYSQL_TYPE_LONGLONG:	args->add_value((int64_t)std::atoll(row[i]));			break;
						case MYSQL_TYPE_LONG:		args->add_value((int32_t)std::atoll(row[i]));			break;
						case MYSQL_TYPE_SHORT:		args->add_value((int16_t)std::atoll(row[i]));			break;
						case MYSQL_TYPE_TINY:		args->add_value((int8_t)std::atoll(row[i]));			break;
						default:					printf_s("[1] Unknown mysql type: %i\n", column.type);	break;
						}
					}
					else
					{
						switch (column.type)
						{
						case MYSQL_TYPE_TIMESTAMP:
						case MYSQL_TYPE_TIME:
						case MYSQL_TYPE_DATETIME:
						case MYSQL_TYPE_DATE:
						case MYSQL_TYPE_STRING:
						case MYSQL_TYPE_VARCHAR:
						case MYSQL_TYPE_BLOB:
						case MYSQL_TYPE_VAR_STRING: args->add_value(std::string {});						break;
						case MYSQL_TYPE_FLOAT:		args->add_value(0.f);									break;
						case MYSQL_TYPE_LONGLONG:	args->add_value(0ll);									break;
						case MYSQL_TYPE_LONG:		args->add_value(0l);									break;
						case MYSQL_TYPE_SHORT:		args->add_value((int16_t)0);							break;
						case MYSQL_TYPE_TINY:		args->add_value((int8_t)0);								break;
						default:					printf_s("[2] Unknown mysql type: %i\n", column.type);	break;
						}
					}
					
					++i;
				});

				ret_res->add_row(args);
			}

			mysql_free_result(result);

			if (ret_res->rows.empty())
			{
				ret_res.reset();
				return nullptr;
			}

		} while (mysql_next_result(sql_handle) != -1);

		ret_res->set_results(++num_results);
	}

	return ret_res;
}

bool MySQL::query_bool_internal(const std::string& query_str)
{
	return !!query(query_str);
}

bool MySQL::update_internal(const std::string& update_str)
{
	std::lock_guard<std::mutex> lock(mtx);

	if (mysql_real_query(sql_handle, update_str.c_str(), static_cast<uint32_t>(update_str.length())))
		return print_error();

	return true;
}*/