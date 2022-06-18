#pragma once

enum ValueType
{
	ValueType_Bool	  = 1,
	ValueType_Int	  = ValueType_Bool,
	ValueType_Int16	  = 2,
	ValueType_Float	  = ValueType_Int16,
	ValueType_String  = 3,
	ValueType_Vec3    = 5,
	ValueType_Mat4	  = 8,
	ValueType_Unknown = 0
};