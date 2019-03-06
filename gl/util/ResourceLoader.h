#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <exception>
#include <string>
#include <memory>
#include <unordered_map>
#include "gl/textures/Texture2D.h"

#include "GL/glew.h"
#include "Mesh2D.h"

class IOException : public std::exception {
public:
    IOException(const std::string &what) : message(what) {}
    virtual ~IOException() throw() {}
    virtual const char* what() const throw() override { return message.c_str(); }

private:
    std::string message;
};

class ResourceLoader
{
public:
    static std::string loadResourceFileToString(const std::string &resourcePath);
    static GLuint createShaderProgram(const char * vertex_file_path,const char * fragment_file_path);
    static std::shared_ptr<Mesh2D> loadOBJMesh(const std::string &filePath);
    static const CS123::GL::Texture2D &getTexture(const std::string &filePath);

private:
    static GLuint createShader(GLenum shaderType, const char *filepath);
};

#endif // RESOURCELOADER_H
