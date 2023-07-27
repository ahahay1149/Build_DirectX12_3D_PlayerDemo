#include <MyAccessHub.h>
#include <MyGameEngine.h>
#include "CharacterData.h"

void CharacterData::AddConstantBuffer(UINT buffSize, const void* initData)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	if (m_constantBuffers.size() <= m_cbuffCount)
	{
		m_constantBuffers.resize(m_cbuffCount + 5);
	}
	m_constantBuffers[m_cbuffCount].Reset();
	
	engine->CreateConstantBuffer(m_constantBuffers[m_cbuffCount].GetAddressOf(), initData, buffSize);

	m_cbuffCount++;
}

void CharacterData::SetGraphicsPipeLine(std::wstring pipelineName)
{
	PipeLineManager* plMng = MyAccessHub::getMyGameEngine()->GetPipelineManager();

	m_pPipeLine = plMng->GetPipeLineObject(pipelineName);
}
