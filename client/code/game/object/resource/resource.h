#pragma once

namespace jc::resource
{
	namespace fn
	{
		static constexpr uint32_t PUSH_TO_QUEUE = 0x5C0CE0;
		static constexpr uint32_t FREE			= 0x8195E0;
	}

	namespace vt
	{
		static constexpr uint32_t FREE		= 2;
		static constexpr uint32_t FREE_DATA = 3;
	}
}

class ExportedEntityResource;

class Resource
{
public:

	void free();

	bool push(const jc::stl::string& file_to_load);
};