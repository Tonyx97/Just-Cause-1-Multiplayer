#pragma once

namespace jc::asset_texture
{
	static constexpr uint32_t DX9_TEXTURE = 0x44;
}

class AssetTexture
{
private:

public:

	IDirect3DTexture9* get_texture() const;
};