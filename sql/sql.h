#pragma once

#ifndef SQL_H
#define SQL_H

/*#include <mysql.h>

#include <vector>
#include <mutex>
#include <functional>
#include <any>
#include <format>

#define SQL_FORMAT(t, ...) std::vformat(t, std::make_format_args(__VA_ARGS__))

using SQL_TS = int64_t;

class SQLValues
{
private:

public:

	std::vector<std::any> args;

	SQLValues()							{}
	~SQLValues()						{}

	template <typename T>
	void add_value(const T& val)		{ args.push_back(val); }

	int32_t get_num_args() const		{ return static_cast<int32_t>(args.size()); }

	template <typename T>
	T get_value(uint32_t column) const	{ return std::any_cast<T>(args[column]); }
};

struct col_data
{
	enum_field_types type;
	std::string name;
	int32_t size;

	col_data(enum_field_types eType, std::string strName, int32_t size) : type(eType), name(strName), size(size) {}
};

class query_result
{
private:

	uint32_t num_columns = 0u,
			 results = 0u;

public:

	std::vector<SQLValues*> rows;
	std::vector<col_data> columns;

	query_result(uint32_t columns)							{ num_columns = columns; }
	~query_result()											{ for (auto row : rows) delete row; }
	
	void add_row(SQLValues* args)							{ rows.push_back(args); }
	void add_column(const col_data& column)					{ columns.push_back(column); }

	void set_results(uint32_t val)							{ results = val; }

	uint32_t get_results() const							{ return results; }
	uint32_t get_columns() const							{ return num_columns; }

	template <typename T>
	T get_value(uint32_t row, uint32_t column) const		{ return std::any_cast<T>(rows[row]->args[column]); }
};

class MySQL
{
private:

	std::string last_error;

	std::mutex mtx;

	MYSQL* sql_handle = nullptr;

	bool initialized = false;

	char host_r[128] { 0 },
		 user_r[128] { 0 },
		 pass_r[128] { 0 },
		 db_r[128] { 0 };

	bool print_error();

public:

	MySQL()														{}
	~MySQL()													{}

	void reset_last_error()										{ last_error.clear(); }
	void disconnect();

	bool connect(const char* host, const char* db, const char* user, const char* pass, const bool& reconnect = true, const uint32_t& port = 0);
	bool select_db(const char* db);

	template <typename... A>
	std::unique_ptr<query_result, std::function<void(query_result*)>> query(const std::string& query_str, A&&... args)	{ return query_internal(SQL_FORMAT(query_str, args...)); }
	std::unique_ptr<query_result, std::function<void(query_result*)>> query_internal(const std::string& query_str);

	template <typename... A>
	bool query_bool(const std::string query_str, A&&... args)	{ return query_bool_internal(SQL_FORMAT(query_str, args...)); }
	bool query_bool_internal(const std::string& query_str);

	template <typename... A>
	bool update(const std::string update_str, A&&... args)		{ return update_internal(SQL_FORMAT(update_str, args...)); }
	bool update_internal(const std::string& update_str);

	std::string get_last_error()								{ return last_error; }
};

using query_res = std::unique_ptr<query_result, std::function<void(query_result*)>>;

inline std::unique_ptr<MySQL> g_mysql;*/

#endif