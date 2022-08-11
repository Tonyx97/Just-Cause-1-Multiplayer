#include <defs/standard.h>

#include "asset_rbm.h"

ref<AssetRBM> AssetRBM::get_ref()
{
	ref<AssetRBM> r;

	jc::this_call(jc::asset_rbm::fn::GET_REF, this, &r);

	return r;
}