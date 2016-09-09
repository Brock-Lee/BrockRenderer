#include "stdafx.h"
#include "ThirdParty/Assimp/scene.h"
#include "ThirdParty/Assimp/Importer.hpp"
#include "ThirdParty/Assimp/postprocess.h"
Scene* g_scene;
Camera* g_camera;
using namespace std;
void Scene::AddPoint( const vec3 &p )
{
	m_points.push_back(p);
}

void Scene::AddTriangle(const std::string &texture,const Triangle &triangle)
{
	m_triangles[texture].push_back(triangle);
	m_triangleCount ++ ;
}

void Scene::LoadModel(const std::string &filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,aiProcessPreset_TargetRealtime_MaxQuality|aiProcess_FixInfacingNormals); 
	assert(scene != nullptr);
	for(int i=0;i<scene->mNumMeshes;i++)
	{
		const aiMesh* mesh = scene->mMeshes[i]; 
		const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		//Texture
		aiString temp;
		mat->GetTexture(aiTextureType_DIFFUSE,0,&temp);
		string texName(temp.C_Str());
		if(!mesh->HasTextureCoords(0) || texName == "")
			m_textures[texName = ""] = nullptr;
		else
		{
			texName = filename.substr(0, filename.rfind('/')+1) + texName;
			if(m_textures.find(texName) == m_textures.end())
				LoadTexture(texName);
		}
		//Materials
		aiColor3D diffuse, ambient, specular;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE,diffuse);
		mat->Get(AI_MATKEY_COLOR_AMBIENT,ambient);
		mat->Get(AI_MATKEY_COLOR_SPECULAR,specular);
		m_materials[texName] = new Material();
		m_materials[texName]->materialAmbient = vec3((float*)&ambient);
		m_materials[texName]->materialDiffuse = vec3((float*)&diffuse);
		m_materials[texName]->materialSpecular = vec3((float*)&specular);

		// Vertex attributes
		unsigned int nBefore = m_triangles[texName].size();
		
		m_triangles[texName].reserve(nBefore + mesh->mNumFaces);
		
		for (unsigned int i=0; i<mesh->mNumFaces; i++){
			Triangle temp;
			int index = 0;
			for(; index<3; index++)
			{
				unsigned int pVertex = mesh->mFaces[i].mIndices[index];
				if(pVertex >= mesh->mNumVertices)
					break;
				aiVector3D &p = mesh->mVertices[pVertex];
				
				temp.v[index].position = vec3(p.x, p.y, p.z);
				//m_triangles[texName].at(nTri).v[index].position = vec3(p.x, p.y, p.z);
				m_aabb.Update(vec3(p.x, p.y, p.z));
				aiVector3D &n = mesh->mNormals[pVertex];
				temp.v[index].normal = vec3(n.x, n.y, n.z);
				if(mesh->HasTextureCoords(0))
				{
					aiVector3D &uv = mesh->mTextureCoords[0][pVertex];
					temp.v[index].uv = vec2(uv.x, uv.y);
				}
			}
			if(index == 3)
			{
				temp.GenFaceNormal();
				AddTriangle(texName, temp);
			}
		}
		
	}
}

void Scene::LoadTexture( const std::string &filename )
{
	Texture* pTex = new Texture(filename);
	m_textures[filename] = pTex;
}
