#define GLM_FORCE_RADIANS

#include "view.h"
#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "gl/shaders/Shader.h"


using namespace Eigen;
using namespace CS123::GL;

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent), held(false), wireframe(false), viewAnchorPoints(false) {
    setFocusPolicy(Qt::StrongFocus);
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
}

View::~View()
{
}

void View::initializeGL() {
    /* Initialize glew */
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " << glewGetString( GLEW_VERSION ) << std::endl;

    glClearColor(1.f, 1.f, 1.f, 1);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(10.f);
    glLineWidth(2.f);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    m_defaultShader = std::make_shared<Shader>(
        Shader::from_files("shader.vert", "shader.frag"));
    m_wireframeShader = std::make_shared<Shader>(
        Shader::from_files("wireframe.vert", "wireframe.frag"));
    m_anchorPointShader = std::make_shared<Shader>(
        Shader::from_files_geom("anchorPoint.vert", "anchorPoint.geom", "anchorPoint.frag"));

    // Load default mesh
    loadObj("meshes/grid.obj");

    time.start();
    timer.start(1000 / 60);
}


void View::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (!m_mesh) return;
    m_mesh->update();

    m_defaultShader->bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_mesh->render(m_defaultShader);
    m_defaultShader->unbind();

    if (step == DeformMesh::DEFORMSTEP::STEP2 && m_meshStep2) {
        m_wireframeShader->bind();
        m_wireframeShader->setUniform("wireframeColor", glm::vec3(1.f, 0.5f, 0.1f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_meshStep2->render(m_wireframeShader);
        m_wireframeShader->unbind();
    }

    if (wireframe) {
        m_wireframeShader->bind();
        m_wireframeShader->setUniform("wireframeColor", glm::vec3(0.f, 0.f, 0.f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_mesh->render(m_wireframeShader);
        m_wireframeShader->unbind();
    }

    if (viewAnchorPoints) {
        m_anchorPointShader->bind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        m_mesh->render(m_anchorPointShader, GL_POINTS);
        m_anchorPointShader->unbind();
    }

}

void View::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    width = w;
    height = h;

    glm::mat4x4 p = glm::ortho(0.f, static_cast<float>(w)/h, 0.f, 1.f, 0.1f, 100.0f);
    glm::mat4x4 v = glm::lookAt(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4x4 m = glm::mat4x4(1.f);

    m_defaultShader->bind();
    m_defaultShader->setUniform("p", p);
    m_defaultShader->setUniform("v", v);
    m_defaultShader->setUniform("m", m);
    m_defaultShader->unbind();

    m_wireframeShader->bind();
    m_wireframeShader->setUniform("p", p);
    m_wireframeShader->setUniform("v", v);
    m_wireframeShader->setUniform("m", m);
    m_wireframeShader->unbind();

    m_anchorPointShader->bind();
    m_anchorPointShader->setUniform("p", p);
    m_anchorPointShader->setUniform("v", v);
    m_anchorPointShader->setUniform("m", m);
    m_anchorPointShader->setUniform("width", w);
    m_anchorPointShader->setUniform("height", h);
    m_anchorPointShader->unbind();
}

void View::mousePressEvent(QMouseEvent *event) {
    if (!m_mesh) return;

    if (event->button() == Qt::MouseButton::LeftButton) {
        Vector2f click = Vector2f(
            static_cast<float>(event->x())/height * this->devicePixelRatio(),
            1.f - static_cast<float>(event->y())/height * this->devicePixelRatio());
        lastConstrained = m_mesh->getClosestVertexIndex(click, 1.f);
        if(constrained.find(lastConstrained) == constrained.end())
            lastConstrained = -1;

        held = true;
    }
}

void View::mouseMoveEvent(QMouseEvent *event) {
    if (!m_mesh) return;

    if (held && lastConstrained >= 0) {
        Vector2f click = Vector2f(
            static_cast<float>(event->x())/height * this->devicePixelRatio(),
            1.f - static_cast<float>(event->y())/height * this->devicePixelRatio());
        m_mesh->setVertex(lastConstrained, click);
        DeformMesh::deformMesh(m_mesh, oldVertices, constrained, solve, step, m_meshStep2);
    }
}

void View::mouseReleaseEvent(QMouseEvent *event) {
    if (!m_mesh) return;

    if (event->button() == Qt::MouseButton::RightButton) {
        Vector2f click = Vector2f(
            static_cast<float>(event->x())/height * this->devicePixelRatio(),
            1.f - static_cast<float>(event->y())/height * this->devicePixelRatio());
        int closest = m_mesh->getClosestVertexIndex(click, 0.1f);
        if (closest >= 0) {
            if (constrained.find(closest) != constrained.end()) {
                constrained.erase(closest);
                m_mesh->setVertexColor(closest, Vector3f(0.0, 0.0, 0.0));
            }
            else {
                constrained.insert(closest);
                m_mesh->setVertexColor(closest, Vector3f(1.0, 0.0, 0.0));
            }
        }
        oldVertices = m_mesh->getVertices();
        DeformMesh::initDeform(m_mesh, constrained, solve);
    }
    held = false;
}

void View::keyPressEvent(QKeyEvent *){}

void View::keyReleaseEvent(QKeyEvent *) {}


void View::tick() {
    update();
}

