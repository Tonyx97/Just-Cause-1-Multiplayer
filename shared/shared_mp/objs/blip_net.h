#pragma once

#include <shared_mp/objs/net_object.h>

class BlipNetObject : public NetObject
{
private:

#ifdef JC_CLIENT
	class UIMapIcon* obj = nullptr;
#endif

public:

	static constexpr NetObjectType TYPE() { return NetObject_Blip; }

	NetObjectType get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	BlipNetObject(NID nid, const TransformTR& transform);

	class ObjectBase* get_object_base() override;
#else
	BlipNetObject(SyncType sync_type, const TransformTR& transform);
#endif
	~BlipNetObject();

	void on_sync() override;
	void on_net_var_change(NetObjectVarType var_type) override;

	bool spawn() override;
};

#define CREATE_BLIP_NET_OBJECT(...)	JC_ALLOC(BlipNetObject, __VA_ARGS__)