#include <d3d12.h>
#include <MyAccessHub.h>
#include "MeshManager.h"
#include "DXSampleHelper.h" 

void MeshManager::createPresetMeshData()
{
    HRESULT hr;
    ULONG indices[] =
    {
        1,0,2,
        0,3,2,
    };

    hr = AddIndexBuffer(L"Sprite", indices, sizeof(ULONG), 6);

    UINT stride = sizeof(SpriteVertex);
    UINT offset = 0;
    SpriteVertex vertices[] =                                               
    {
        { XMFLOAT3(-0.5f, -0.5f, 1.0f), XMFLOAT2(0, 1) },   
        { XMFLOAT3(0.5f, -0.5f, 1.0f), XMFLOAT2(1, 1) },    
        { XMFLOAT3(0.5f, 0.5f, 1.0f), XMFLOAT2(1, 0) },     
        { XMFLOAT3(-0.5f, 0.5f, 1.0f), XMFLOAT2(0, 0) },    
    };

    hr = AddVertexBuffer(L"Sprite", vertices, sizeof(SpriteVertex), 4);

}

void MeshManager::SetVertexBuffer(ID3D12GraphicsCommandList* m_cmdList, const std::wstring idName)
{
    if (m_crVertex == idName)
    {
        return; 
    }

    m_crVertex = idName;

    m_cmdList->IASetVertexBuffers(0, 1, &m_ViewContainers[idName]->vbView);
}

void MeshManager::SetIndexBuffer(ID3D12GraphicsCommandList* m_cmdList, const std::wstring idName)
{
    if (m_crIndex == idName)
    {
        return; 
    }

    m_crIndex = idName;

    m_cmdList->IASetIndexBuffer(&m_ViewContainers[idName]->ibView);
}

HRESULT MeshManager::AddVertexBuffer(const std::wstring idName, const void* initBuff, UINT vertexSize, UINT vertexCount)
{

    m_VertexBuffers[idName].reset();
    m_VertexBuffers[idName] = make_unique<BufferContainer>();
    m_VertexBuffers[idName]->dataCount = vertexCount;
    m_VertexBuffers[idName]->dataSize = vertexSize;
    m_VertexBuffers[idName]->pBuffer.Reset();

	HRESULT hr = MyAccessHub::getMyGameEngine()->CreateVertexBuffer(m_VertexBuffers[idName]->pBuffer.GetAddressOf(), initBuff, vertexSize, vertexCount);

    if (FAILED(hr))
        return hr;

    m_VertexBuffers[idName]->pBuffer->SetName(idName.c_str());

    if (m_ViewContainers.find(idName) == m_ViewContainers.end())
    {
        m_ViewContainers[idName].reset();
        m_ViewContainers[idName] = make_unique<ViewContainer>();
    }

    m_ViewContainers[idName]->vbView.BufferLocation = m_VertexBuffers[idName]->pBuffer->GetGPUVirtualAddress();
    m_ViewContainers[idName]->vbView.SizeInBytes = vertexSize * vertexCount;
    m_ViewContainers[idName]->vbView.StrideInBytes = vertexSize;

    return hr;

}

HRESULT MeshManager::AddIndexBuffer(const std::wstring idName, const void* initBuff, UINT valueSize, UINT indexCount)
{
    m_IndexBuffers[idName].reset();
    m_IndexBuffers[idName] = make_unique<BufferContainer>();
    m_IndexBuffers[idName]->dataSize = valueSize;
    m_IndexBuffers[idName]->dataCount = indexCount;
    m_IndexBuffers[idName]->pBuffer.Reset();

    HRESULT hr = MyAccessHub::getMyGameEngine()->CreateIndexBuffer(m_IndexBuffers[idName]->pBuffer.GetAddressOf(), initBuff, valueSize, indexCount);
    if (FAILED(hr))
        return hr;

    m_IndexBuffers[idName]->pBuffer->SetName(idName.c_str());

    if (m_ViewContainers.find(idName) == m_ViewContainers.end())
    {
        m_ViewContainers[idName].reset();
        m_ViewContainers[idName] = make_unique<ViewContainer>();
    }
    m_ViewContainers[idName]->ibView.BufferLocation = m_IndexBuffers[idName]->pBuffer->GetGPUVirtualAddress();
    m_ViewContainers[idName]->ibView.SizeInBytes = valueSize * indexCount;

    switch (valueSize)
    {
    case 4:
        m_ViewContainers[idName]->ibView.Format = DXGI_FORMAT_R32_UINT;
        break;

    default:
        m_ViewContainers[idName]->ibView.Format = DXGI_FORMAT_R16_UINT;
        break;
    }

    return hr;
}

D3D12_VERTEX_BUFFER_VIEW* MeshManager::GetVertexBufferView(const std::wstring idName)
{
	return &m_ViewContainers[idName]->vbView;
}

D3D12_INDEX_BUFFER_VIEW* MeshManager::GetIndexBufferView(const std::wstring idName)
{
	return &m_ViewContainers[idName]->ibView;
}

void MeshManager::resetMesh()
{
    m_crIndex = L"";
    m_crVertex = L"";
}

void MeshManager::removeVertexBuffer(const std::wstring id, bool withIndex)
{
    m_VertexBuffers.erase(id);

    if (withIndex)
        removeIndexBuffer(id);
}

void MeshManager::removeIndexBuffer(const std::wstring id)
{
    m_IndexBuffers.erase(id);

    m_ViewContainers.erase(id);
}
