#include "pch.h"
#include "MeshLoader.h"


MeshLoader::MeshLoader()
{
}

//Make bool for return check
bool MeshLoader::LoadMeshFromFile(char *path)
{
	// TODO: Load mesh data and send it to the graphics card.

	// OpenGL-Tutorial.org based loader
	// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading

	if (*path != 0)
	{
		vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		vector< XMFLOAT3 > tempVertices;
		vector< XMFLOAT2 > tempUVs;
		vector< XMFLOAT3 > tempNormals;


		FILE * file;
		fopen_s(&file, path, "r");
		if (file == NULL)
		{
			return false;
		}
		//_splitpath_s(path, NULL, NULL, NULL, NULL, &filename[0], NULL, NULL, NULL);

		while (file)
		{
			char lineHeader[128];
			int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader));
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0)
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tempVertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				tempUVs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				//float temp;
				//temp = normal.y;
				//normal.y = normal.z;
				//normal.z *= temp;  // flipping Z (might not be needed)
				//normal.z *= -1;  // flipping Z (might not be needed)
				tempNormals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				//string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[2], &uvIndex[2], &normalIndex[2], 
																			 &vertexIndex[1], &uvIndex[1], &normalIndex[1], 
																			 &vertexIndex[0], &uvIndex[0], &normalIndex[0]);

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		for (unsigned int i = 0; i < vertexIndices.size(); ++i)
		{
			// use a map for better perfomance later
			XMFLOAT3 vertex = tempVertices[vertexIndices[i] - 1];
			//out_vertices.push_back(vertex);
			XMFLOAT2 UV = tempUVs[uvIndices[i] - 1];
			//out_uvs.push_back(UV);
			XMFLOAT3 normals = tempNormals[normalIndices[i] - 1];
			//out_normals.push_back(normals);
			
			GX2_HWProject::VertexPositionColor tempVert;
			tempVert.pos = vertex;
			//tempVert.color = XMFLOAT3(0.1f, 0.1f, 0.1f);
			tempVert.color = XMFLOAT3(UV.x, UV.y, 1.0f);
			tempVert.norm = normals;

			bool match = false;
			unsigned int j = 0;
			if (vIndices.size() != 0)
			{
				for (; j < fullVertex.size(); j++)
				{
					if (tempVert.pos.x == fullVertex[j].pos.x		&&
						tempVert.pos.y == fullVertex[j].pos.y		&&
						tempVert.pos.z == fullVertex[j].pos.z		&&

						tempVert.color.x == fullVertex[j].color.x	&&
						tempVert.color.y == fullVertex[j].color.y	&&
						tempVert.color.z == fullVertex[j].color.z	&&

						tempVert.norm.x == fullVertex[j].norm.x		&&
						tempVert.norm.y == fullVertex[j].norm.y		&&
						tempVert.norm.z == fullVertex[j].norm.z)
					{
						match = true;
						break;
					}
					else
					{
						continue;
					}
					//tempVert.color		= XMFLOAT3(UV.x, UV.y, 1.0f);
				}
			}

			if (!match)
			{
				fullVertex.push_back(tempVert);
				vIndices.push_back(fullVertex.size()-1);
			}
			else
			{
				vIndices.push_back(j);
			}
		}
		//for (unsigned int i = 0; i < uvIndices.size(); ++i)
		//{
		//
		//}
		//
		//for (unsigned int i = 0; i < normalIndices.size(); ++i)
		//{
		//}
	}

	int test = 0;

}
unsigned int MeshLoader::GetNumVerts()
{
	return fullVertex.size();
}

