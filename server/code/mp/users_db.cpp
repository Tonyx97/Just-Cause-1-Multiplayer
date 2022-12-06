#include <defs/standard.h>
#include <defs/json.h>

#include <crypto/sha512.h>

#include <timer/timer.h>

#include "users_db.h"

bool UsersDB::init()
{
	if (!std::filesystem::is_regular_file(USERS_DB_FILE()))
		std::ofstream(USERS_DB_FILE(), std::ios::trunc);

	file = std::fstream(USERS_DB_FILE());
	
	if (!file)
		return false;

	load();

	return true;
}

bool UsersDB::register_user(const std::string& name, const std::string& pass)
{
	if (db.contains(name))
		return false;

	auto& entry = db[name];

	entry["pass"] = crypto::sha512(pass);
	entry["acl"] = "none";

	save();

	return true;
}

bool UsersDB::verify_user(const std::string& name, const std::string& pass, UserInfo& info)
{
	if (!db.contains(name))
		return false;

	auto& entry = db[name];

	const std::string& password = entry["pass"];

	if (info.invalid_pass = (password != crypto::sha512(pass)))
		return false;

	info.username = name;
	info.acl = util::string::split(std::string(entry["acl"]), ',');

	return true;
}

bool UsersDB::login_user(PlayerClient* pc, const std::string& name, const std::string& pass)
{
	// make sure the user name is not already registered and that the player client
	// is not logged in yet

	if (logged_users.contains(name) || pc_users.contains(pc))
		return false;

	UserInfo info;

	// make sure the user info exists

	if (!verify_user(name, pass, info))
		return false;

	pc_users.insert({ pc, info });
	logged_users.insert(name);

	return true;
}

bool UsersDB::logout_user(PlayerClient* pc)
{
	if (const auto it = pc_users.find(pc); it != pc_users.end())
	{
		logged_users.erase(it->second.username);

		pc_users.erase(it);

		return true;
	}

	return false;
}

bool UsersDB::is_logged_in(PlayerClient* pc)
{
	const auto it = pc_users.find(pc);
	return it != pc_users.end();
}

bool UsersDB::has_acl(PlayerClient* pc, const std::string& acl_name)
{
	const auto it = pc_users.find(pc);
	return it != pc_users.end() ? it->second.has_acl(acl_name) : false;
}

void UsersDB::destroy()
{
	save();

	file.close();
}

void UsersDB::load()
{
	jc_json::from_file(db, USERS_DB_FILE());
}

void UsersDB::save()
{
	file << std::setw(4) << db << std::endl;
}