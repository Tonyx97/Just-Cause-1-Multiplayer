#include <defs/libs.h>
#include <defs/standard.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/SceneCombiner.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct Buffer
{
	std::vector<uint8_t> data;

	Buffer() {}

	template <typename T>
	void insert(const T& v, size_t size)
	{
		const auto ptr = BITCAST(uint8_t*, &v);

		data.insert(data.end(), ptr, ptr + size);
	}

	template <typename T>
	void insert(T* v, size_t size)
	{
		data.insert(data.end(), v, v + size);
	}

	template <typename T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_array_v<T>>* = nullptr>
	void add(const T& v)
	{
		insert(v, sizeof(v));
	}

	void add(const std::string& v)
	{
		insert(v.length(), sizeof(size_t));
		insert(v.c_str(), v.length());
	}

	void to_file(const std::string& filename)
	{
		std::ofstream test_file(filename, std::ios::binary);

		test_file.write(BITCAST(char*, data.data()), data.size());
	}
};

int main()
{
	Assimp::Importer imp;

	auto scene = imp.ReadFile(
		"test.fbx",
		//aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs |
		//aiProcess_FlipWindingOrder |
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_ForceGenNormals |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights);

	log(RED, "meshes: {}", scene->mNumMeshes);

	Buffer out {};

	out.add("RBMDL");
	out.add(1);
	out.add(3);
	out.add(1);
	out.add(-vec3(10.f, 10.f, 10.f));
	out.add(vec3(10.f, 10.f, 10.f));
	out.add(scene->mNumMeshes); // block count

	for (int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
	{
		out.add("DiffuseVertexColors");

		const auto mesh = scene->mMeshes[mesh_index];
		const auto material = scene->mMaterials[mesh->mMaterialIndex];

		check(material->GetTextureCount(aiTextureType_DIFFUSE) < 2, "Invalid amount of textures");

		aiString name;

		check(material->GetTexture(aiTextureType_DIFFUSE, 0, &name) == aiReturn_SUCCESS, "Could not get a mesh texture");

		log(RED, "{}", name.C_Str());

		out.add(name.C_Str());

		for (int i = 0; i < 20; ++i) out.add(0.f);
		
		out.add(3); // primitive type
		out.add(mesh->mNumVertices); // vertex count

		struct VertexInfo
		{
			vec3 position;
			vec3 normal;
			vec2 uv;
			uint32_t color;
		};

		std::vector<VertexInfo> vertices;
		std::vector<uint16_t> indices;

		for (int i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices.push_back(
			{
				.position = *(vec3*)&mesh->mVertices[i],
				.normal = *(vec3*)&mesh->mNormals[i],
				.uv = *(vec2*)&mesh->mTextureCoords[0][i],
				.color = 0xffffffff,
			});
		}

		for (int face_index = 0; face_index < mesh->mNumFaces; ++face_index)
		{
			const auto face = &mesh->mFaces[face_index];

			check(face->mNumIndices == 3, "Wrong number of indices");

			indices.push_back(static_cast<uint16_t>(face->mIndices[0]));
			indices.push_back(static_cast<uint16_t>(face->mIndices[1]));
			indices.push_back(static_cast<uint16_t>(face->mIndices[2]));
		}

		log(GREEN, "primitive type: {}", int(mesh->mPrimitiveTypes));
		log(GREEN, "vertices: {}", vertices.size());
		log(GREEN, "indices: {}", indices.size());

		for (const auto& vertex : vertices)
			out.add(vertex.position);

		out.add(vertices.size());

		for (const auto& vertex : vertices)
		{
			out.add(vertex.uv);
			out.add(vertex.color);
			out.add(util::pack::pack_float<int8_t>(vertex.normal.x, 127.f));
			out.add(util::pack::pack_float<int8_t>(vertex.normal.y, 127.f));
			out.add(util::pack::pack_float<int8_t>(vertex.normal.z, 127.f));
			out.add(0ui8);
		}

		out.add(0);
		out.add(indices.size());

		for (uint16_t i = 0; auto index : indices)
			out.add(index);

		out.add(0x89ABCDEF);
	}

	out.to_file("test.rbm");

	//return EXIT_SUCCESS;
	return std::cin.get();
}