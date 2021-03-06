
#ifndef MESH_H
#define MESH_H

#include <vector>
#include "graphics_headers.h"

class Scene {
public:
	struct Mesh {

		GLuint VBO, IBO;

		std::vector<Vertex> 		vertices;
		std::vector<unsigned int> 	indices; 
	};

	struct Texture {

		GLuint handle;
	};

	Scene();
	~Scene();

	// load model mesh from file - loads only first mesh
	bool LoadModel(std::string file, int idx);
	// load texture from file to be associated with mesh
	bool LoadTexture(std::string file);

	// render the scene (model + texture)
		// setup the model matrix BEFOREHAND
	void Render();
	// free mesh asset
	void DeleteMesh();
	// free texture asset
	void DeleteTexture();
	// get reference to mesh representation
	Mesh& getMesh();

private:

	Mesh mesh;
	Texture texture;
};

#endif // MESH_H
