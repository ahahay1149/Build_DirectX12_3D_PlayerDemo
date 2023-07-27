#pragma once
#include <Windows.h>
#include <MyAccessHub.h>
#include <CharacterData.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <wrl/client.h>

#include <fbxsdk.h>

#include "HitShapes.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class FBXDataContainer;

struct FbxVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

struct FbxSkinAnimeParams
{
	XMUINT4		indices0;	 
	XMUINT4		indices1;	 
	XMFLOAT4	weight0;	 
	XMFLOAT4	weight1;	 
};

struct FbxSkinAnimeVertex
{
	FbxVertex			vertex;			
	FbxSkinAnimeParams	skinvalues;		
};
enum class FBX_TEXTURE_TYPE
{
	FBX_DIFFUSE,
	FBX_NORMAL,
	FBX_SPECUAR,
	FBX_FALLOFF,
	FBX_REFLECTIONMAP,

	FBX_UNKNOWN
};

class MeshContainer
{
protected:

	FbxMesh* m_mesh;
	FBXDataContainer* m_animeFbxCont;

	bool m_uniqueMesh = false;

	const char* m_meshNodeName;
	int	m_parentNodeId;

	FbxAMatrix	m_IBaseMatrix;

	UINT		m_skinCount = 0;
public:
	std::wstring m_MaterialId = L"";
	std::wstring m_MeshId = L"";

	UINT	m_vertexCount = 0;

	std::vector<FbxVertex>	m_vertexData;
	std::vector<ULONG>		m_indexData;

	XMFLOAT3				m_vtxMin;
	XMFLOAT3				m_vtxMax;

	~MeshContainer();

	void setFbxMesh(FbxMesh* mesh);

	const char* getMeshNodeName()
	{
		return m_meshNodeName;
	}

	FbxMesh* getFbxMesh()
	{
		return m_mesh;
	}

	void setUniqueFlag(bool mesh)
	{
		m_uniqueMesh = mesh;
	}

	UINT GetSkinCount()
	{
		return m_skinCount;
	}
	void InitSkinList(int skinCount);
	void SetBoneIdList(FBXDataContainer* animeFbxCount);
};

class MaterialContainer
{
private:
	bool	m_uniqueTextures = false;

public:
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float alpha;
	std::vector<std::wstring>	m_diffuseTextures;
	std::vector<std::wstring>	m_normalTextures;
	std::vector<std::wstring>	m_specularTextures;
	std::vector<std::wstring>	m_falloffTextures;
	std::vector<std::wstring>	m_reflectionMapTextures;

	MaterialContainer()
	{
		for (int i = 0; i < 4; i++)
		{
			ambient[i] = 1.0f;
			diffuse[i] = 1.0f;
			specular[i] = 1.0f;
		}

		alpha = 1.0f;

		m_diffuseTextures.clear();
		m_normalTextures.clear();
		m_specularTextures.clear();
		m_falloffTextures.clear();
		m_reflectionMapTextures.clear();
	}

	void setUniqueTextureFlag(bool flg)
	{
		m_uniqueTextures = flg;
	}

	~MaterialContainer();

	void setAmbient(float r, float g, float b, float factor)
	{
		ambient[0] = r;
		ambient[1] = g;
		ambient[2] = b;
		ambient[3] = factor;
	}

	void setDiffuse(float r, float g, float b, float factor)
	{
		diffuse[0] = r;
		diffuse[1] = g;
		diffuse[2] = b;
		diffuse[3] = factor;
	}

	void setSpecular(float r, float g, float b, float factor)
	{
		specular[0] = r;
		specular[1] = g;
		specular[2] = b;
		specular[3] = factor;
	}

};

class FBXDataContainer
{
private:

	FbxScene*	m_pFbxScene;	
	
	LONG		m_animeFrames;	
	double		m_startTime;	
	double		m_endTime;		
	double		m_timePeriod;	

	FbxAnimStack* m_animeStack;	
	int					m_clusterCount;		
	int					m_cbuffIndex;		
	FBXDataContainer*	m_currentAnimeCont;	

	std::vector<const char*>	m_boneNameList;	
	std::vector<int>			m_boneIdList;	
	std::vector<FbxAMatrix>		m_IboneMatrix;	
	std::vector<XMFLOAT4X4>		m_F4X4Matrix;	
	std::vector<const char*> m_nodeNameList;
	std::vector<unique_ptr<MeshContainer>> m_pMeshContainer;
	std::unordered_map<std::wstring, unique_ptr<MaterialContainer>> m_pMaterialContainer;

	XMFLOAT3	m_vtxTotalMin;
	XMFLOAT3	m_vtxTotalMax;

	FBX_TEXTURE_TYPE GetTextureType(const fbxsdk::FbxBindingTableEntry& entryTable);

	HRESULT ReadFbxToMeshContainer(const std::wstring id, FbxMesh* pMesh);
	HRESULT LoadMaterial(const std::wstring id, FbxSurfaceMaterial* material);

	HRESULT LoadTextureFromMaterial(const std::wstring matName, const std::wstring id, FBX_TEXTURE_TYPE texType, const FbxProperty* fbxProp);

	int GetClusterId(FbxCluster* pCluster);
	int GetClusterId(FbxNode* pNode);
public:
	~FBXDataContainer()
	{
		if (m_pFbxScene != nullptr)
		{
			m_pFbxScene->Destroy();
			m_pFbxScene = nullptr;
		}

		m_boneNameList.clear();
		m_IboneMatrix.clear();
		m_boneIdList.clear();
		m_F4X4Matrix.clear();
		m_pMeshContainer.clear();
		m_pMaterialContainer.clear();
	}

	HRESULT LoadFBX(const std::wstring fileName, const std::wstring id);

	XMFLOAT3 GetFbxMin()
	{
		return m_vtxTotalMin;
	}

	XMFLOAT3 GetFbxMax()
	{
		return m_vtxTotalMax;
	}

	MeshContainer* GetMeshContainer(int index)
	{
		if (m_pMeshContainer.size() <= index)
		{
			return nullptr;
		}

		return m_pMeshContainer[index].get();
	}

	int GetMeshCount()
	{
		return m_pMeshContainer.size();
	}

	MaterialContainer* GetMaterialContainer(const std::wstring& matName)
	{
		if (m_pMaterialContainer[matName] != nullptr)
			return m_pMaterialContainer[matName].get();

		return nullptr;
	}

	void SetMeshUniqueFlag(bool meshFlag, bool materialFlag);
	void SetTextureUniqueFlag(bool texFlag);

	int GetNodeId(const char* nodeName);

	int GetMeshId(const char* meshName);
	FbxNode* GetMeshNode(int id);

	fbxsdk::FbxManager* GetFbxManager()
	{
		static fbxsdk::FbxManager* m_spFbxManager = nullptr;

		if (m_spFbxManager == nullptr)
		{
			m_spFbxManager = fbxsdk::FbxManager::Create();
		}

		return m_spFbxManager;
	}

	void SetAnimationFbx(FBXDataContainer* animeCont);	
	void UpdateAnimation(const FbxTime& animeTime);		
	const XMFLOAT4X4* GetAnimatedMatrix();				

	FbxScene* GetFbxScene() { return m_pFbxScene; }

	double GetStartTime() { return m_startTime; }
	double GetEndTime() { return m_endTime; }
	double GetPeriodTime() { return m_timePeriod; }

	LONG GetAnimeFrames() { return m_animeFrames; }
	FbxAnimStack* GetAnimeStack() { return m_animeStack; }

	int GetClusterCount()
	{
		return m_clusterCount;	
	}

	void SetCBuffIndex(int index)
	{
		m_cbuffIndex = index;
	}

	int GetCBuffIndex()
	{
		return m_cbuffIndex;
	}
};

class FBXCharacterData : public CharacterData
{
private:

	std::unique_ptr<FBXDataContainer>	m_mainFbx;

	std::unordered_map<std::wstring, std::unique_ptr<FBXDataContainer>>	m_animeFbxMap;

	std::wstring	m_currentAnimeLabel;	
	LONG			m_animeTime;			
public:

	HRESULT LoadMainFBX(const std::wstring fileName, const std::wstring id);

	FBXDataContainer* GetMainFbx()
	{
		return m_mainFbx.get();
	}

	HRESULT LoadAnimationFBX(const std::wstring filename, const std::wstring id);	
	FBXDataContainer* GetAnimeFbx(const std::wstring filename);						
	void SetAnime(std::wstring animeLabel);		
	void UpdateAnimation();							
	void UpdateAnimation(int frameCount);			

	void ClearAnimeFBX()							
	{
		m_animeFbxMap.clear();
	}

	
};