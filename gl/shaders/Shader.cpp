#include "Shader.h"

#include <algorithm>
#include <iostream>
#include <utility>

#define GLM_FORCE_RADIANS
#define NDEBUG

#include "glm/gtc/type_ptr.hpp"

#include "gl/GLDebug.h"
#include "gl/textures/Texture2D.h"
#include "gl/util/ResourceLoader.h"

namespace CS123 { namespace GL {

Shader::Shader(const std::string &vertexSource, const std::string &fragmentSource)
{
    createProgramID();
    std::vector<GLuint> shaders;
    shaders.push_back(createVertexShaderFromSource(vertexSource));
    shaders.push_back(createFragmentShaderFromSource(fragmentSource));
    buildShaderProgramFromShaders(shaders);
    discoverShaderData();

    #ifndef NDEBUG
    // Print out which uniforms and textures shader has picked up.
    std::cout << "**** "  << std::endl;
    for(const auto& kv : m_uniforms){
      std::cout << kv.first << " --> " << kv.second << std::endl;
    }
    std::cout << "*** TEX TEX "  << std::endl;
    for(const auto& kv : m_textureLocations){
      std::cout << kv.first << " --> " << kv.second << std::endl;
    }
    #endif

}

Shader::Shader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource) {
    createProgramID();
    std::vector<GLuint> shaders;
    shaders.push_back(createVertexShaderFromSource(vertexSource));
    shaders.push_back(createGeometryShaderFromSource(geometrySource));
    shaders.push_back(createFragmentShaderFromSource(fragmentSource));
    buildShaderProgramFromShaders(shaders);
    discoverShaderData();
}

Shader::~Shader()
{
    this->clear();
}

Shader::Shader(Shader &&that) :
    m_programID(that.m_programID),
    m_attributes(std::move(that.m_attributes)),
    m_uniforms(std::move(that.m_uniforms)),
    m_uniformArrays(std::move(that.m_uniformArrays)),
    m_textureLocations(std::move(that.m_textureLocations)),
    m_textureSlots(std::move(that.m_textureSlots))
{
    that.m_programID = 0;
}

Shader& Shader::operator=(Shader &&that) {
    this->clear();

    m_programID = that.m_programID;
    m_attributes = std::move(that.m_attributes);
    m_uniforms = std::move(that.m_uniforms);
    m_uniformArrays = std::move(that.m_uniformArrays);
    m_textureLocations = std::move(that.m_textureLocations);
    m_textureSlots = std::move(that.m_textureSlots);

    that.m_programID = 0;
    return *this;
}

void Shader::clear() {
    glDeleteProgram(m_programID);
}

Shader Shader::from_files(const std::string& vert, const std::string& frag, const std::string& dir){
    return {
        ResourceLoader::loadResourceFileToString(":/" + dir + "/" + vert),
        ResourceLoader::loadResourceFileToString(":/" + dir + "/" + frag)
    };
}

Shader Shader::from_files_geom(const std::string& vert,
                               const std::string& geom,
                               const std::string& frag,
                               const std::string& dir){
    return {
        ResourceLoader::loadResourceFileToString(":/" + dir + "/" + vert),
        ResourceLoader::loadResourceFileToString(":/" + dir + "/" + geom),
        ResourceLoader::loadResourceFileToString(":/" + dir + "/" + frag)
    };
}

void Shader::bind() {
    glUseProgram(m_programID);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::setUniform(const std::string &name, float f) {
    glUniform1f(m_uniforms[name] - 1, f);
}

void Shader::setUniform(const std::string &name, const glm::vec2 &vec2) {
    glUniform2fv(m_uniforms[name] - 1, 1, glm::value_ptr(vec2));
}

void Shader::setUniform(const std::string &name, const glm::vec3 &vec3) {
    glUniform3fv(m_uniforms[name] - 1, 1, glm::value_ptr(vec3));
}

void Shader::setUniform(const std::string &name, const glm::vec4 &vec4) {
    glUniform4fv(m_uniforms[name] - 1, 1, glm::value_ptr(vec4));
}

void Shader::setUniform(const std::string &name, int i) {
    glUniform1i(m_uniforms[name] - 1, i);
}

void Shader::setUniform(const std::string &name, const glm::ivec2 &ivec2) {
    glUniform2iv(m_uniforms[name] - 1, 1, glm::value_ptr(ivec2));
}

void Shader::setUniform(const std::string &name, const glm::ivec3 &ivec3) {
    glUniform3iv(m_uniforms[name] - 1, 1, glm::value_ptr(ivec3));
}

void Shader::setUniform(const std::string &name, const glm::ivec4 &ivec4) {
    glUniform4iv(m_uniforms[name] - 1, 1, glm::value_ptr(ivec4));
}

void Shader::setUniform(const std::string &name, bool b) {
    glUniform1i(m_uniforms[name] - 1, static_cast<GLint>(b));
}

void Shader::setUniform(const std::string &name, const glm::bvec2 &bvec2) {
    glUniform2iv(m_uniforms[name] - 1, 1, glm::value_ptr(glm::ivec2(bvec2)));
}

void Shader::setUniform(const std::string &name, const glm::bvec3 &bvec3) {
    glUniform3iv(m_uniforms[name] - 1, 1, glm::value_ptr(glm::ivec3(bvec3)));
}

void Shader::setUniform(const std::string &name, const glm::bvec4 &bvec4) {
    glUniform4iv(m_uniforms[name] - 1, 1, glm::value_ptr(glm::ivec4(bvec4)));
}

void Shader::setUniform(const std::string &name, const glm::mat2 &mat2) {
    glUniformMatrix2fv(m_uniforms[name] - 1, 1, GL_FALSE, glm::value_ptr(mat2));
}

void Shader::setUniform(const std::string &name, const glm::mat3 &mat3) {
    glUniformMatrix3fv(m_uniforms[name] - 1, 1, GL_FALSE, glm::value_ptr(mat3));
}

void Shader::setUniform(const std::string &name, const glm::mat4 &mat4) {
    glUniformMatrix4fv(m_uniforms[name] - 1, 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::setUniformArrayByIndex(const std::string &name, float f, size_t index) {
    glUniform1f(m_uniformArrays[std::make_tuple(name, index)] - 1, f);
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::vec2 &vec2, size_t index) {
    glUniform2fv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(vec2));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::vec3 &vec3, size_t index) {
    glUniform3fv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(vec3));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::vec4 &vec4, size_t index) {
    glUniform4fv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(vec4));
}

void Shader::setUniformArrayByIndex(const std::string &name, int i, size_t index) {
    glUniform1i(m_uniformArrays[std::make_tuple(name, index)] - 1, i);
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::ivec2 &ivec2, size_t index) {
    glUniform2iv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(ivec2));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::ivec3 &ivec3, size_t index) {
    glUniform3iv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(ivec3));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::ivec4 &ivec4, size_t index) {
    glUniform4iv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(ivec4));
}

void Shader::setUniformArrayByIndex(const std::string &name, bool b, size_t index) {
    glUniform1i(m_uniformArrays[std::make_tuple(name, index)] - 1, static_cast<GLint>(b));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::bvec2 &bvec2, size_t index) {
    glUniform2iv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(glm::ivec2(bvec2)));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::bvec3 &bvec3, size_t index) {
    glUniform3iv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(glm::ivec3(bvec3)));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::bvec4 &bvec4, size_t index) {
    glUniform4iv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, glm::value_ptr(glm::ivec4(bvec4)));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::mat2 &mat2, size_t index) {
    glUniformMatrix2fv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, GL_FALSE, glm::value_ptr(mat2));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::mat3 &mat3, size_t index) {
    glUniformMatrix3fv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, GL_FALSE, glm::value_ptr(mat3));
}

void Shader::setUniformArrayByIndex(const std::string &name, const glm::mat4 &mat4, size_t index) {
    glUniformMatrix4fv(m_uniformArrays[std::make_tuple(name, index)] - 1, 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::setTexture(const std::string &, const Texture1D &) {}

void Shader::setTexture(const std::string &name, const Texture2D &t) {

  if (m_textureLocations.find(name) != m_textureLocations.end()) {
      GLint location = m_textureLocations.at(name);
      GLint slot = m_textureSlots.at(location);
      // fprintf(stderr, "Location %d, slot %d\n", location, slot);
      glActiveTexture(GL_TEXTURE0 + slot);
      glUniform1i(location, slot);
      t.bind();
  } else {
    std::cout << "Inactive texture bind: " << name << std::endl;
  }
}

bool Shader::doesTextureExist(const std::string &name) {
    return m_textureLocations.find(name) != m_textureLocations.end();
}

void Shader::setTexture(const std::string &, const Texture3D &) {}

void Shader::setTexture(const std::string &, const TextureCube &) {}

void Shader::attachShaders(const std::vector<GLuint> &shaders) {
    std::for_each(shaders.begin(), shaders.end(), [this](int s){ glAttachShader(m_programID, s); });
}

void Shader::buildShaderProgramFromShaders(const std::vector<GLuint> &shaders) {
    attachShaders(shaders);
    linkShaderProgram();
    detachShaders(shaders);
    deleteShaders(shaders);
}

GLuint Shader::createFragmentShaderFromSource(const std::string &source) {
   return createShaderFromSource(source, GL_FRAGMENT_SHADER);
}

GLuint Shader::createGeometryShaderFromSource(const std::string &source) {
    return createShaderFromSource(source, GL_GEOMETRY_SHADER);
}

void Shader::compileShader(GLuint handle, const std::string &source) {
    const GLchar* codeArray[] = { source.c_str() };
    glShaderSource(handle, 1, codeArray, nullptr);
    glCompileShader(handle);
}

GLuint Shader::createVertexShaderFromSource(const std::string &source) {
    return createShaderFromSource(source, GL_VERTEX_SHADER);
}

GLuint Shader::createShaderFromSource(const std::string &source, GLenum shaderType) {
    GLuint shaderHandle = glCreateShader(shaderType);
    compileShader(shaderHandle, source);
    CS123::GL::checkShaderCompilationStatus(shaderHandle);
    return shaderHandle;
}

void Shader::createProgramID() {
    m_programID = glCreateProgram();
}

void Shader::detachShaders(const std::vector<GLuint> &shaders) {
    std::for_each(shaders.begin(), shaders.end(), [this](int s){ glDetachShader(m_programID, s); });
}

void Shader::deleteShaders(const std::vector<GLuint> &shaders) {
    std::for_each(shaders.begin(), shaders.end(), [this](int s){ glDeleteShader(s); });
}

void Shader::linkShaderProgram() {
    glLinkProgram(m_programID);
    CS123::GL::checkShaderLinkStatus(m_programID);
}

void Shader::discoverShaderData() {
    discoverAttributes();
    discoverUniforms();
}

void Shader::discoverAttributes() {
    bind();
    GLint attribCount;
    glGetProgramiv(m_programID, GL_ACTIVE_ATTRIBUTES, &attribCount);
    for (int i = 0; i < attribCount; i++) {
        const GLsizei bufSize = 256;
        GLsizei nameLength = 0;
        GLint arraySize = 0;
        GLenum type;
        GLchar name[bufSize];
        glGetActiveAttrib(m_programID, i, bufSize, &nameLength, &arraySize, &type, name);
        name[std::min(nameLength, bufSize - 1)] = 0;
        m_attributes[std::string(name)] = glGetAttribLocation(m_programID, name);
    }
    unbind();
}

void Shader::discoverUniforms() {
    bind();
    GLint uniformCount;
    glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &uniformCount);
    for (int i = 0; i < uniformCount; i++) {
        const GLsizei bufSize = 256;
        GLsizei nameLength = 0;
        GLint arraySize = 0;
        GLenum type;
        GLchar name[bufSize];
        glGetActiveUniform(m_programID, i, bufSize, &nameLength, &arraySize, &type, name);
        name[std::min(nameLength, bufSize - 1)] = 0;

        std::string strname(name);
        if (isUniformArray(name, nameLength)) {
            addUniformArray(strname, arraySize);
        } else if (isTexture(type)) {
            addTexture(strname);
        } else {
            addUniform(strname);
        }
    }
    unbind();
}

bool Shader::isUniformArray(const GLchar *name, GLsizei nameLength) {
    // Check if the last 3 characters are '[0]'
    return (name[nameLength - 3] == '[') &&
           (name[nameLength - 2] == '0') &&
           (name[nameLength - 1] == ']');
}

bool Shader::isTexture(GLenum type) {
    return (type == GL_SAMPLER_2D) ||
           (type == GL_SAMPLER_CUBE);
}

void Shader::addUniformArray(const std::string &name, size_t size) {
    std::string cleanName = name.substr(0, name.length() - 3);
    for (auto i = static_cast<size_t>(0); i < size; i++) {
        std::string enumeratedName = name;
        enumeratedName[enumeratedName.length() - 2] = static_cast<char>('0' + i);
        std::tuple< std::string, size_t > nameIndexTuple = std::make_tuple(cleanName, i);
        m_uniformArrays[nameIndexTuple] = glGetUniformLocation(m_programID, enumeratedName.c_str()) + 1;
    }
}

void Shader::addTexture(const std::string &name) {
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    m_textureLocations[name] = location;
    GLint slot = m_textureSlots.size();
    m_textureSlots[location] = slot; // Assign slots in increasing order.
}

void Shader::addUniform(const std::string &name) {
    m_uniforms[name] = glGetUniformLocation(m_programID, name.c_str()) + 1;
}

}}
