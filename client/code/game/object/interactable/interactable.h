#pragma once

namespace jc::interactable
{
	static constexpr uint32_t TARGET = 0x3C;		// ObjectBase*

	namespace vt
	{
		static constexpr uint32_t INTERACT_WITH = 1;
	}

	namespace hook
	{
		void apply();
		void undo();
	}
}

class ObjectBase;

class Interactable
{
public:

	void interact_with(Character* character);

	ObjectBase* get_target() const;
};