#pragma once

namespace jc::exported_entity
{
	namespace fn
	{
		static constexpr uint32_t LOAD_CLASS_PROPERTIES = 0x563780;
		static constexpr uint32_t TAKE_CLASS_PROPERTY	= 0x5638C0;
		static constexpr uint32_t MARK_AS_LOADED		= 0x563CD0;
		static constexpr uint32_t IS_LOADED				= 0x563CF0;
	}
}

struct object_base_map;

class ExportedEntity
{
public:

	bool load_class_properties(object_base_map*& map);
	bool take_class_property(std::string* class_name, object_base_map*& map);
	bool is_loaded() const;
};