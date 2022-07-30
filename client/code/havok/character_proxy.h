#pragma once

namespace jc::hk::character_proxy
{
	static constexpr uint32_t USERDATA				= 0xC;
	static constexpr uint32_t SIMPLE_SHAPE_PHANTOM	= 0x58;
	static constexpr uint32_t MASS					= 0xC4;
}

class hkSimpleShapePhantom;
class Character;

class hkCharacterProxy
{
private:
public:

	void set_mass(float v);

	float get_mass() const;

	Character* get_character() const;

	hkSimpleShapePhantom* get_shape() const;
};