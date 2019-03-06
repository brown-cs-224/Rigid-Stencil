#include "ResourceLoader.h"
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QFileInfo>
#include <QGLWidget>
#include <iostream>
#include <set>

#define TINYOBJLOADER_IMPLEMENTATION
#include "util/tiny_obj_loader.h"

using namespace CS123::GL;
using namespace Eigen;

static std::unordered_map<std::string, CS123::GL::Texture2D> m_textures;


std::string ResourceLoader::loadResourceFileToString(const std::string &resourcePath) {
    QString vertFilePath = QString::fromStdString(resourcePath);
    QFile vertFile(vertFilePath);
    if (vertFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream vertStream(&vertFile);
        QString contents = vertStream.readAll();
        return contents.toStdString();
    }
    throw IOException("Could not open file: " + resourcePath);
}

GLuint ResourceLoader::createShaderProgram(const char *vertexFilePath,const char *fragmentFilePath) {
    // Create and compile the shaders.
    GLuint vertexShaderID = createShader(GL_VERTEX_SHADER, vertexFilePath);
    GLuint fragmentShaderID = createShader(GL_FRAGMENT_SHADER, fragmentFilePath);

    // Link the shader program.
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderID);
    glAttachShader(programId, fragmentShaderID);
    glLinkProgram(programId);

    // Print the info log.
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog(std::max(infoLogLength, int(1)));
        glGetProgramInfoLog(programId, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);
    }

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programId;
}

GLuint ResourceLoader::createShader(GLenum shaderType, const char *filepath) {
    GLuint shaderID = glCreateShader(shaderType);

    // Read shader file.
    std::string code;
    QString filepathStr = QString(filepath);
    QFile file(filepathStr);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        code = stream.readAll().toStdString();
    }

    // Compile shader code.
    printf("Compiling shader: %s\n", filepath);
    const char *codePtr = code.c_str();
    glShaderSource(shaderID, 1, &codePtr, NULL);
    glCompileShader(shaderID);

    // Print info log.
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog(infoLogLength);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);
    }

    return shaderID;
}

std::shared_ptr<Mesh2D> ResourceLoader::loadOBJMesh(const std::string &filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    QFileInfo info(QString((filePath).c_str()));
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                                info.absoluteFilePath().toStdString().c_str(), (info.absolutePath().toStdString() + "/").c_str(), true);

    if(!ret || !err.empty() || attrib.vertices.size() == 0) {
        std::cerr << "Failed to load/parse .obj file" << std::endl;
        std::cerr << err << std::endl;
        return nullptr;
    }

    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector3f> colors;
    std::vector<Vector2f> uvs;
    std::vector<int> materialIds;
    std::vector<Vector3i> faces;

    const int numVerts = attrib.vertices.size() / 3;
    vertices.resize(numVerts);
    normals.resize(numVerts);
    colors.resize(numVerts);
    uvs.resize(numVerts);

    for (unsigned int i = 0; i < shapes[0].mesh.indices.size(); i+=3) {
        faces.push_back(Vector3i(shapes[0].mesh.indices[i].vertex_index, shapes[0].mesh.indices[i+1].vertex_index, shapes[0].mesh.indices[i+2].vertex_index));
    }

    std::set<int> verts;
    for (unsigned int i = 0; i < shapes[0].mesh.indices.size(); i++) {

        const int vind = 3*shapes[0].mesh.indices[i].vertex_index;
        if (verts.find(vind) == verts.end()) {
            const int tind = 2*shapes[0].mesh.indices[i].texcoord_index;
            const int nind = 3*shapes[0].mesh.indices[i].normal_index;

            vertices[shapes[0].mesh.indices[i].vertex_index] = Vector3f(attrib.vertices[vind], attrib.vertices[vind+1], attrib.vertices[vind+2]);
            if (attrib.texcoords.size() > 0) uvs[shapes[0].mesh.indices[i].vertex_index] = Vector2f(attrib.texcoords[tind], attrib.texcoords[tind+1]);
            normals[shapes[0].mesh.indices[i].vertex_index] = Vector3f(attrib.normals[nind], attrib.normals[nind+1], attrib.normals[nind+2]);
            colors[shapes[0].mesh.indices[i].vertex_index] = Vector3f(0, 0, 0);

            verts.emplace(vind);
        }
    }

    std::vector<tinyobj::material_t> loadedMats;
    for (const tinyobj::material_t &mat : materials) {
        if (!mat.diffuse_texname.empty()) {
            if (m_textures.find(mat.diffuse_texname) == m_textures.end()) {
                QImage image = QImage(mat.diffuse_texname.data());
                image = QGLWidget::convertToGLFormat(image);

                if (image.isNull()) {
                    std::cerr << "Could not read file " << mat.diffuse_texname << std::endl;
                    continue;
                }

                Texture2D texture(image.bits(), image.width(), image.height());
                texture.bind();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                texture.unbind();
                m_textures.insert(std::make_pair(mat.diffuse_texname, std::move(texture)));
            }
            loadedMats.push_back(mat);
        }
    }

    std::shared_ptr<Mesh2D> res = std::make_shared<Mesh2D>(vertices,
                                                           normals,
                                                           uvs,
                                                           colors,
                                                           faces,
                                                           materialIds,
                                                           loadedMats);

    return res;
}

const CS123::GL::Texture2D& ResourceLoader::getTexture(const std::string &filePath) {
    return m_textures.at(filePath);
}

