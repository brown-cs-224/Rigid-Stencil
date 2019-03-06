#include "Mesh2D.h"

#include <QImage>
#include <QGLWidget>
#include <iostream>
#include "gl/util/ResourceLoader.h"

using namespace Eigen;

Mesh2D::Mesh2D(const std::vector<Vector3f> &vertices,
                const std::vector<Vector3f> &normals,
                const std::vector<Vector2f> &uvs,
                const std::vector<Vector3f> &colors,
                const std::vector<Vector3i> &faces,
                const std::vector<int> &materialIds,
                const std::vector<tinyobj::material_t> &materials,
                bool normalize) : needsUpdate(false)
{
    std::vector<Vector2f> verts;
    verts.reserve(vertices.size());
    for (const Vector3f &v : vertices) {
        verts.push_back(v.head<2>());
    }

    initialize(verts, normals, uvs, colors, faces, materialIds, materials, normalize);
}

Mesh2D::Mesh2D(const std::vector<Vector2f> &vertices,
               const std::vector<Vector3f> &normals,
               const std::vector<Vector2f> &uvs,
               const std::vector<Vector3f> &colors,
               const std::vector<Vector3i> &faces,
               const std::vector<int> &materialIds,
               const std::vector<tinyobj::material_t> &materials,
               bool normalize) : needsUpdate(false)
{
    initialize(vertices, normals, uvs, colors, faces, materialIds, materials, normalize);
}

void Mesh2D::initialize(const std::vector<Vector2f> &vertices,
                        const std::vector<Vector3f> &,
                        const std::vector<Vector2f> &uvs,
                        const std::vector<Vector3f> &colors,
                        const std::vector<Vector3i> &faces,
                        const std::vector<int> &,
                        const std::vector<tinyobj::material_t> &materials, bool normalize) {

    m_vertices = vertices;

    // Normalize mesh to [0, 1]
    if (normalize) {
        float minx = std::numeric_limits<float>::max();
        float maxx = std::numeric_limits<float>::min();
        float miny = std::numeric_limits<float>::max();
        float maxy = std::numeric_limits<float>::min();
        for (const Vector2f &v : vertices) {
            minx = glm::min(minx, v.x());
            maxx = glm::max(maxx, v.x());
            miny = glm::min(miny, v.y());
            maxy = glm::max(maxy, v.y());
        }
        for (Vector2f &v : m_vertices) {
            v = Vector2f((v.x() - minx) / (maxx - minx), (v.y() - miny) / (maxy - miny));
            //v = 0.85f * Vector2f((v.x() - minx) / (maxx - minx), (v.y() - miny) / (maxy - miny)) + Vector2f(0.15f, 0.075f);
        }
    }

    m_colors = colors;
    m_uvs = uvs;
    m_faces = faces;
    m_materials = materials;

    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);
    glGenVertexArrays(1, &m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (m_vertices.size() * 2 + m_uvs.size() * 2 + m_colors.size() * 3), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_vertices.size() * 2, static_cast<const void *>(m_vertices.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size() * 2, sizeof(float) * m_uvs.size() * 2, static_cast<const void *>(m_uvs.data()));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size() * 2 + sizeof(float) * m_uvs.size() * 2, sizeof(float) * m_colors.size() * 3, static_cast<const void *>(m_colors.data()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * m_faces.size(), static_cast<const void *>(m_faces.data()), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Should be double/triple buffered for max efficiency
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, static_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid *>(sizeof(float) * m_vertices.size() * 2));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid *>(sizeof(float) * (m_vertices.size() * 2 + m_uvs.size() * 2)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh2D::~Mesh2D()
{
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Mesh2D::render(std::shared_ptr<CS123::GL::Shader> &shader, GLenum mode) const {

    if (m_materials.size() > 0 && !m_materials.at(0).diffuse_texname.empty() && shader->doesTextureExist("tex")) {
        const CS123::GL::Texture2D &tex = ResourceLoader::getTexture(m_materials.at(0).diffuse_texname);
        shader->setUniform("useTexture", 1);
        shader->setTexture("tex", tex);
    } else {
        shader->setUniform("useTexture", 0);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(mode, m_faces.size() * 3, GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}

// Can be improved with an acceleration data structure
int Mesh2D::getClosestVertexIndex(const Vector2f &v, float maxDistance) {
    int index = -1;
    float max = std::numeric_limits<float>::max();

    for (unsigned int i = 0; i < m_vertices.size(); i++) {
        const Vector2f &c = m_vertices.at(i);
        const float test = (c - v).norm();
        if (test < maxDistance && test < max) {
            index = i;
            max = test;
        }
    }

    return index;
}

void Mesh2D::update() {
    if (needsUpdate) {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_vertices.size() * 2, static_cast<const void *>(m_vertices.data()));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size() * 2 + sizeof(float) * m_uvs.size() * 2, sizeof(float) * m_colors.size() * 3, static_cast<const void *>(m_colors.data()));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        needsUpdate = false;
    }
}

std::vector<Eigen::Vector2f> Mesh2D::getVertices() const {
    return m_vertices;
}

void Mesh2D::setVertex(int index, const Vector2f &v) {
    m_vertices.at(index) = v;
    needsUpdate = true;
}

void Mesh2D::setVertices(const std::vector<Eigen::Vector2f> &verts) {
    m_vertices = verts;
    needsUpdate = true;
}

const std::vector<Eigen::Vector3i>& Mesh2D::getFaces() const {
    return m_faces;
}

void Mesh2D::setVertexColor(int index, Vector3f color) {
    m_colors.at(index) = color;
    needsUpdate = true;
}
