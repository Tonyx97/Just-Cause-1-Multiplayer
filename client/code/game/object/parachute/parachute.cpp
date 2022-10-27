#include <defs/standard.h>

#include "parachute.h"

void Parachute::load_model(const char* model)
{
	jc::this_call(jc::parachute::fn::LOAD_MODEL, this, model);
}

void Parachute::set_closed(bool v)
{
	jc::write(v, this, jc::parachute::IS_CLOSED);
}

bool Parachute::is_closed() const
{
	return jc::read<bool>(this, jc::parachute::IS_CLOSED);
}

int Parachute::get_stage() const
{
	return jc::read<int>(this, jc::parachute::STAGE);
}

Model* Parachute::get_model() const
{
	return jc::read<Model*>(this, jc::parachute::MODEL);
}

Transform* Parachute::get_transform() const
{
	return REF(Transform*, this, jc::parachute::TRANSFORM);
}