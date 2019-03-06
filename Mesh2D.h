#ifndef MESH2D_H
#define MESH2D_H

#include <Eigen/Dense>
#include <util/tiny_obj_loader.h>
#include <vector>
#include <memory>
#include "GL/glew.h"
#include "gl/shaders/Shader.h"


class Mesh2D
{
public:

    Mesh2D(const std::vector<Eigen::Vector3f> &vertices,
         const std::vector<Eigen::Vector3f> &normals,
         const std::vector<Eigen::Vector2f> &uvs,
         const std::vector<Eigen::Vector3f> &colors,
         const std::vector<Eigen::Vector3i> &faces,
         const std::vector<int> &materialIds,
         const std::vector<tinyobj::material_t> &materials,
         bool normalize = true);
    Mesh2D(const std::vector<Eigen::Vector2f> &vertices,
         const std::vector<Eigen::Vector3f> &normals,
         const std::vector<Eigen::Vector2f> &uvs,
         const std::vector<Eigen::Vector3f> &colors,
         const std::vector<Eigen::Vector3i> &faces,
         const std::vector<int> &materialIds,
         const std::vector<tinyobj::material_t> &materials,
         bool normalize = true);
    ~Mesh2D();

    void render(std::shared_ptr<CS123::GL::Shader> &shader, GLenum mode = GL_TRIANGLES) const;

    int getClosestVertexIndex(const Eigen::Vector2f &v, float maxDistance);

    void update();

    std::vector<Eigen::Vector2f> getVertices() const;

    void setVertex(int index, const Eigen::Vector2f &v);

    void setVertices(const std::vector<Eigen::Vector2f> &verts);

    const std::vector<Eigen::Vector3i>& getFaces() const;

    void setVertexColor(int index, Eigen::Vector3f color);

    std::vector<tinyobj::material_t> getMaterials() { return m_materials; }

private:

    void initialize(const std::vector<Eigen::Vector2f> &vertices,
                    const std::vector<Eigen::Vector3f> &normals,
                    const std::vector<Eigen::Vector2f> &uvs,
                    const std::vector<Eigen::Vector3f> &colors,
                    const std::vector<Eigen::Vector3i> &faces,
                    const std::vector<int> &materialIds,
                    const std::vector<tinyobj::material_t> &materials, bool normalize = true);

    // Properties from the .obj file
    std::vector<Eigen::Vector2f> m_vertices;
    std::vector<Eigen::Vector3f> m_colors;
    std::vector<Eigen::Vector2f> m_uvs;
    std::vector<Eigen::Vector3i> m_faces;
    std::vector<tinyobj::material_t> m_materials;
    bool needsUpdate;

    GLuint m_VAO, m_VBO, m_IBO;
};

#endif // MESH2D_H
