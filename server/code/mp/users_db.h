#pragma once

class PlayerClient;

class UsersDB
{
public:

	struct UserInfo
	{
		std::vector<std::string> acl;

		std::string username;
		
		bool invalid_pass = false;

		bool has_acl(const std::string& acl_name) const
		{
			return std::find(acl.begin(), acl.end(), acl_name) != acl.end();
		}
	};

private:

	std::unordered_set<std::string> logged_users;

	std::unordered_map<PlayerClient*, UserInfo> pc_users;

	std::fstream file;

	json db;

	void load();
	void save();

public:

	static constexpr auto USERS_DB_FILE() { return "users_db.json"; }

	bool init();
	bool register_user(const std::string& name, const std::string& pass);
	bool verify_user(const std::string& name, const std::string& pass, UserInfo& info);
	bool login_user(PlayerClient* pc, const std::string& name, const std::string& pass);
	bool logout_user(PlayerClient* pc);
	bool is_logged_in(PlayerClient* pc);
	bool has_acl(PlayerClient* pc, const std::string& acl_name);

	void destroy();
};