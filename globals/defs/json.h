#pragma once

#include <json.hpp>

using json = nlohmann::ordered_json;

namespace jc_json
{
	template <typename T>
	inline std::pair<T, bool> get_field(json& key, const std::string& name)
	{
		try { return { key.at(name), true }; }
		catch (...) { return { {}, false }; }
	}

	inline bool from_file(json& j, const std::string& filename)
	{
		if (auto fd = std::ifstream(filename))
		{
			try
			{
				fd >> j;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		return false;
	}
}