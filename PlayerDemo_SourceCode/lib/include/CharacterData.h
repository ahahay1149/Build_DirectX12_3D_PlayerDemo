#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>

#include <wrl/client.h>

#include <PipeLineManager.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class GraphicsPipeLineObjectBase;

class CharacterData
{
protected:
	XMFLOAT3	position;			
	XMFLOAT3	rotation;
	XMFLOAT3	scale;

	std::vector<ComPtr<ID3D12Resource>> m_constantBuffers;

	GraphicsPipeLineObjectBase* m_pPipeLine = nullptr;

	UINT m_cbuffCount;

public:
	CharacterData()
	{
		setPosition(0.0f, 0.0f, 0.0f);
		setRotation(0.0f, 0.0f, 0.0f);
		setScale(1.0f, 1.0f, 1.0f);

		m_constantBuffers.clear();
		m_cbuffCount = 0;
	}

	void AddConstantBuffer(UINT buffSize, const void* initData);

	ID3D12Resource* GetConstantBuffer(UINT index)
	{
		if (m_cbuffCount > index)
		{
			return m_constantBuffers[index].Get();
		}

		return nullptr;
	}

	void setPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void setRotation(float x, float y, float z)
	{
		rotation.x = x;
		rotation.y = y;
		rotation.z = z;
	}

	void setScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	const XMFLOAT3 getPosition()
	{
		return position;
	}

	const XMFLOAT3 getRotation()
	{
		return rotation;
	}

	const XMFLOAT3 getScale()
	{
		return scale;
	}

	void SetGraphicsPipeLine(std::wstring pipelineName);

	GraphicsPipeLineObjectBase* GetPipeline()
	{
		return m_pPipeLine;
	}
};

