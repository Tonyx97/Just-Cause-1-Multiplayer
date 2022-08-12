#include <defs/libs.h>
#include <defs/standard.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/GltfMaterial.h>
#include <assimp/SceneCombiner.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

int main()
{
	Assimp::Importer imp;

	auto scene = imp.ReadFile(
		"test.fbx",
		//aiProcess_ConvertToLeftHanded |
		//aiProcess_FlipWindingOrder |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_Triangulate |
		aiProcess_ForceGenNormals |
		aiProcess_GenNormals);

	log(RED, "meshes: {}", scene->mNumMeshes);


	return std::cin.get();
}