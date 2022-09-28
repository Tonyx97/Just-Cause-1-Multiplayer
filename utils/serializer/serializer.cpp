#include <defs/standard.h>

#include "serializer.h"

std::set<std::string> serialized_objects;

void register_serialized_object(int t, const std::string& name)
{
	if (!serialized_objects.contains(name))
	{
		log(CYAN, "Serialized Object ({}): {}", t, name);

		serialized_objects.insert(name);
	}
}

void serialization_ctx::to_file(const std::string& filename)
{
	std::ofstream test_file(filename, std::ios::binary);

	test_file.write(std::bit_cast<char*>(data.data()), data.size());
}