#pragma once

namespace jc::model
{
	static constexpr uint32_t INSTANCE = 0x0; // ModelInstance*
}

namespace jc::model_instance
{
	static constexpr uint32_t FLAGS = 0x4; // uint32_t
}

class ModelInstance
{
private:
public:

	void set_flags(uint32_t v);
	void add_flag(uint32_t v);
	void remove_flag(uint32_t v);

	uint32_t get_flags() const;
};

class Model
{
public:

	ModelInstance* get_instance() const;
};