#include <defs/libs.h>
#include <defs/standard.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/SceneCombiner.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

static constexpr std::string_view INPUT_DIR = "input\\";
static constexpr std::string_view OUTPUT_DIR = "output\\";

int rbms_done = 0;

bool convert_to_rbm(const std::string& path, const std::string& filename, const std::string& ext)
{
	struct VertexInfo
	{
		vec3 position;
		vec3 normal;
		vec2 uv;
		uint32_t color;
	};

	Assimp::Importer imp;

	auto scene = imp.ReadFile(
		path,
		aiProcess_FlipUVs |
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_ForceGenNormals |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights);

	if (!scene)
		return false;

	Buffer out {};

	// add header

	out.add("RBMDL", 1, 3, 1, -vec3(10.f, 10.f, 10.f), vec3(10.f, 10.f, 10.f), scene->mNumMeshes);

	for (uint32_t mesh_index = 0u; mesh_index < scene->mNumMeshes; ++mesh_index)
	{
		const auto mesh = scene->mMeshes[mesh_index];
		const auto material = scene->mMaterials[mesh->mMaterialIndex];

		check(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE, "Only triangle meshes supported");

		auto get_texture_by_type = [&](aiTextureType type, int index = 0)
		{
			check(material->GetTextureCount(type) < 2, "Invalid amount of textures");

			aiString name;

			check(material->GetTexture(type, 0, &name) == aiReturn_SUCCESS, "Could not get a mesh texture");

			return std::string(name.C_Str());
		};

		const auto texture = get_texture_by_type(aiTextureType_DIFFUSE);

		std::vector<VertexInfo> vertices;
		std::vector<uint16_t> indices;

		for (uint32_t i = 0u; i < mesh->mNumVertices; ++i)
		{
			vertices.push_back(
			{
				.position = *(vec3*)&mesh->mVertices[i],
				.normal = *(vec3*)&mesh->mNormals[i],
				.uv = *(vec2*)&mesh->mTextureCoords[0][i],
				.color = 0xffffffff,
			});
		}

		for (uint32_t face_index = 0u; face_index < mesh->mNumFaces; ++face_index)
		{
			const auto face = &mesh->mFaces[face_index];

			check(face->mNumIndices == 3, "Wrong number of indices");

			indices.push_back(static_cast<uint16_t>(face->mIndices[0]));
			indices.push_back(static_cast<uint16_t>(face->mIndices[1]));
			indices.push_back(static_cast<uint16_t>(face->mIndices[2]));
		}

		// write the data

		out.add("DiffuseVertexColors");
		out.add(texture);

		for (int i = 0; i < 20; ++i) out.add(0.f);

		out.add(3); // triangle list
		out.add(vertices.size()); // vertex count

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

		for (auto index : indices)
			out.add(index);

		out.add(0x89ABCDEF);
	}

	out.to_file(std::string(OUTPUT_DIR) + filename + ".rbm");

	++rbms_done;

	return true;
}

int main()
{
	jc::prof::init("JC:MP Asset Converter");

	bool close = false;

	if (close = !std::filesystem::is_directory(INPUT_DIR))
		std::filesystem::create_directory(INPUT_DIR);

	if (close = close || !std::filesystem::is_directory(OUTPUT_DIR))
		std::filesystem::create_directory(OUTPUT_DIR);

	if (close)
		return EXIT_SUCCESS;

	for (auto e : std::filesystem::directory_iterator(INPUT_DIR))
	{
		const auto p = e.path();
		const auto path = p.string();
		const auto filename = p.stem().string();
		const auto ext = p.extension().string();

		bool ok = false;

		if (ext == ".fbx" || ext == ".3ds" || ext == ".obj")
			ok = convert_to_rbm(path, filename, ext);

		if (!ok)
			log(RED, "Could not convert file '{}'", path);
	}

	log(GREEN, "*** Conversion completed ***");
	log(GREEN, "RBMs converted: {}", rbms_done);

	std::cin.get();

	jc::prof::close_console(true);

	return EXIT_SUCCESS;
}