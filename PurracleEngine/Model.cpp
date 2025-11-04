#include "Model.h"

Model::Model(const std::string& path)
{
	LoadModel(path);
}

Model::~Model()
{
	for (auto mesh : meshes)
		delete mesh;
	meshes.clear();
}
void Model::Render(Shader* myShader) 
{
	myShader->SetMatrix4(GetMatrix(), "transform");
	for (auto mesh : meshes)
		mesh->Render(myShader);

}

void Model::LoadModel(const std::string& path)
{
    ufbx_load_opts opts = { 0 };
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.target_unit_meters = 1.0f;

    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(path.c_str(), &opts, &error);

    if (!scene)
    {
        std::cerr << "ERROR::UFBX:: Failed to load: " << path << std::endl;
        std::cerr << "Error: " << error.description.data << std::endl;
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Process all meshes in the scene
    for (size_t i = 0; i < scene->meshes.count; i++)
    {
        ufbx_mesh* mesh = scene->meshes.data[i];

        size_t num_triangles = mesh->num_triangles;

        // Reserve space for efficiency
        size_t vertex_offset = vertices.size();

        // Process each triangle
        for (size_t tri = 0; tri < num_triangles; tri++)
        {
            // Get the three vertices of this triangle
            for (size_t corner = 0; corner < 3; corner++)
            {
                uint32_t index = mesh->vertex_indices.data[tri * 3 + corner];

                Vertex vertex;

                // Position
                ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, index);
                vertex.Position = glm::vec3(pos.x, pos.y, pos.z);

                // Normal
                if (mesh->vertex_normal.exists)
                {
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                    vertex.Normal = glm::vec3(normal.x, normal.y, normal.z);
                }
                else
                {
                    vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }

                // UV coordinates
                if (mesh->vertex_uv.exists)
                {
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                    vertex.TexCoords = glm::vec2(uv.x, uv.y);
                }
                else
                {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
                indices.push_back(static_cast<unsigned int>(vertex_offset + tri * 3 + corner));
            }
        }

        meshes.push_back(new Mesh(vertices, indices));
    }

    ufbx_free_scene(scene);
}