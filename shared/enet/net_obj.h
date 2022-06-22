#pragma once

#ifndef JC_LOADER

// network id of an object
//
using NID = uint32_t;

static constexpr NID INVALID_NID = 0u;

#ifdef JC_SERVER
namespace enet
{
	void INIT_NIDS_POOL();

	NID GET_FREE_NID();

	void FREE_NID(NID nid);
}
#endif

enum NetObjectType : uint32_t
{
	NetObject_Invalid,
	NetObject_Player,
};

class NetObject
{
private:

	NID nid = INVALID_NID;

public:

#ifdef JC_SERVER
	NetObject();
	~NetObject();
#endif

	virtual uint32_t get_type() const = 0;

	template <typename T>
	T* cast() const
	{
		return (get_type() == T::TYPE() ? BITCAST(T*, this) : nullptr);
	}

	template <typename T>
	T* cast_safe() const
	{
		check(get_type() == T::TYPE(), "Wrong type, current instance type is {} but got {}", get_type(), T::TYPE());

		return BITCAST(T*, this);
	}

#ifdef JC_CLIENT
	void set_nid(NID v) { nid = v; }
#endif

	NID get_nid() const { return nid; }
};
#endif