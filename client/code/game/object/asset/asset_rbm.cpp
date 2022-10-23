#include <defs/standard.h>

#include "asset_rbm.h"

shared_ptr<AssetRBM> AssetRBM::get_ref()
{
	shared_ptr<AssetRBM> r;

	jc::this_call(jc::asset_rbm::fn::GET_REF, this, &r);

	return r;
}