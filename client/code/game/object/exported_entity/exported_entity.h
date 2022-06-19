#pragma once

namespace jc::exported_entity
{
	namespace fn
	{
		static constexpr uint32_t MARK_AS_LOADED	= 0x563CD0;
		static constexpr uint32_t IS_LOADED			= 0x563CF0;
	}
}

class ExportedEntity
{
public:

	bool is_loaded() const;
};