#include <defs/standard.h>

#include "asset_texture.h"

IDirect3DTexture9* AssetTexture::get_texture() const
{
	return jc::read<IDirect3DTexture9*>(this, jc::asset_texture::DX9_TEXTURE);
}