#include <defs/standard.h>

#include "model.h"

void ModelInstance::set_flags(uint32_t v)
{
	jc::write(v, this, jc::model_instance::FLAGS);
}

void ModelInstance::add_flag(uint32_t v)
{
	set_flags(get_flags() | v);
}

void ModelInstance::remove_flag(uint32_t v)
{
	set_flags(get_flags() & ~v);
}

uint32_t ModelInstance::get_flags() const
{
	return jc::read<uint32_t>(this, jc::model_instance::FLAGS);
}

ModelInstance* Model::get_instance() const
{
	return jc::read<ModelInstance*>(this, jc::model::INSTANCE);
}