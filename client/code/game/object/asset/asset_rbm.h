#pragma once

namespace jc::asset_rbm
{
	namespace fn
	{
		static constexpr uint32_t GET_REF = 0x5C3570;
	}
}

class AssetRBM
{
private:

public:

	AssetRBM() {}

	ref<AssetRBM> get_ref();
};