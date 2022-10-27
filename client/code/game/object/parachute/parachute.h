#pragma once

namespace jc::parachute
{
	static constexpr uint32_t MODEL			= 0x0;
	static constexpr uint32_t TRANSFORM		= 0x4;
	static constexpr uint32_t STAGE			= 0x44;
	static constexpr uint32_t IS_CLOSED		= 0x48;

	namespace fn
	{
		static constexpr uint32_t LOAD_MODEL	= 0x603530;
	}
}

class Model;

class Parachute
{
private:
public:

	void load_model(const char* model = "Characters\\Equipment\\EQUI_10_PARACHUTE_2.rbm");
	void set_closed(bool v);

	bool is_closed() const;

	int get_stage() const;

	Model* get_model() const;

	Transform* get_transform() const;
};