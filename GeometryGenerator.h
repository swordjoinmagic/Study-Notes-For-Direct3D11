#pragma once

#include "D3DUtils.h"
#include <vector>

class GeometryGenerator {
public:
	struct Vertex {
		float3 pos;
		float3 normal;
		float3 tangent;
		float2 texcoord;

		Vertex() {}
		Vertex(
			const float3& _pos,
			const float3& _normal,
			const float3& _tangent,
			const float2& texcoord):
			pos(_pos),normal(_normal),
			tangent(_tangent),texcoord(texcoord) {}
	};
	struct MeshData {
		// 顶点集合
		std::vector<Vertex> vertices;
		// 索引集合
		std::vector<uint> indices;
	};
	/*
		创建一个width*height的网格,共分为m*n个quad,
	*/
	void CreateGrid(float width,float height,uint m,uint n,MeshData& mesh) {		
		// 顶点数
		uint vertexCount = m * n;
		// 三角面数
		uint faceCount = (m - 1)*(n - 1) * 2;

		float halfWidth = 0.5f*width;
		float halfHeight = 0.5f*height;

		// 每个四边形间隔距离
		float dx = width / (n - 1);
		float dy = height / (m - 1);
		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		// 初始化容量
		mesh.vertices.resize(vertexCount);

		// 设置顶点属性
		for (uint i = 0; i < m;i++) {
			float y = halfHeight - i * dy;
			for (uint j = 0; j < n;j++) {			
				// 设置第i行j列的网格
				float x = -halfWidth + j * dx;

				mesh.vertices[i*n + j].pos = float3(x,0,y);
				mesh.vertices[i*n + j].normal = float3(0,1,0);
				mesh.vertices[i*n + j].tangent = float3(1,0,0);

				// uv
				mesh.vertices[i*n + j].texcoord = float2(j*du,i*dv);

			}
		}

		mesh.indices.resize(faceCount*3);
		// 设置顶点索引
		uint k = 0;	
		for (uint i = 0; i < m - 1;i++) {
			for (uint j = 0; j < n - 1;j++) {
				mesh.indices[k] = i * n + j;
				mesh.indices[k + 1] = i * n + j + 1;
				mesh.indices[k + 2] = (i + 1)*n + j;
				mesh.indices[k + 3] = (i + 1)*n + j;
				mesh.indices[k + 4] = i * n + j + 1;
				mesh.indices[k + 5] = (i + 1)*n + j + 1;
				k += 6;
			}
		}
	}
};