#pragma once
#include <DirectXMath.h>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "..\Content\ShaderStructures.h"

using namespace DirectX;
using namespace std;

	class MeshLoader
	{
	private:
		//vector < XMFLOAT3 > out_vertices;
		//vector < XMFLOAT2 > out_uvs;
		//vector < XMFLOAT3 > out_normals;

	public:
		// Constructor with path later.
		vector < GX2_HWProject::VertexPositionColor > fullVertex;
		vector<unsigned int> vIndices;
		
		MeshLoader();
		unsigned int GetNumVerts();
		bool LoadMeshFromFile(char *path);	// Make bool later

	};

