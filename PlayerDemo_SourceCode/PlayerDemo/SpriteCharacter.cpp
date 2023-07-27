#include "SpriteCharacter.h"
#include "TextureManager.h"
#include "MyAccessHub.h"

void SpriteCharacter::setTextureId(const wchar_t* texId)
{
	if (texture_id != texId)
	{
		texture_id = texId;
	}
}

HRESULT SpriteCharacter::setSpritePatterns(XMUINT4* patterns, UINT numOfPatterns)
{
    auto texContainer = MyAccessHub::getMyGameEngine()->GetTextureManager()->GetTexture(texture_id);

    if (texContainer == nullptr)
        return E_FAIL;

    const XMUINT4* tempUint = nullptr;
    unique_ptr<SpritePattern> tempSp;

    m_uvArray.clear();
    m_uvArray.resize(numOfPatterns);

    for (UINT i = 0; i < numOfPatterns; i++)
    {
        tempUint = &patterns[i];

        tempSp = move(m_uvArray.at(i));
        tempSp.reset(new SpritePattern());

        tempSp->size.x = tempUint->z - tempUint->x;
        tempSp->size.y = tempUint->w - tempUint->y;

        tempSp->uv.x = tempUint->x / texContainer->fWidth;
        tempSp->uv.y = tempUint->y / texContainer->fHeight;

        tempSp->uv.z = tempUint->z / texContainer->fWidth;
        tempSp->uv.w = tempUint->w / texContainer->fHeight;

        m_uvArray.at(i) = move(tempSp);
    }

    return S_OK;
}

void SpriteCharacter::setColor(float r, float g, float b, float a)
{
    m_color.x = r;
    m_color.y = g;
    m_color.z = b;
    m_color.w = a;
}

void SpriteCharacter::setColorMix(COLOR_MIX_OP op)
{
    m_colorMix = (UINT)op;
}

void SpriteCharacter::setAlphaMix(COLOR_MIX_OP op)
{
    m_alphaMix = (UINT)op;
}

const XMFLOAT4* SpriteCharacter::getColor()
{
    return &m_color;
}

UINT SpriteCharacter::getColorMix()
{
    return m_colorMix;
}

UINT SpriteCharacter::getAlphaMix()
{
    return m_alphaMix;
}

