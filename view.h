#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <QGLWidget>
#include <QTime>
#include <QTimer>

#include <set>
#include <vector>
#include <Eigen/StdVector>

EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix2f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3i)

#include "Mesh2D.h"
#include "DeformMesh.h"
#include "gl/util/ResourceLoader.h"

namespace CS123 { namespace GL {
    class Shader;
}}


class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

    void setWireframe(bool w) {
        wireframe = w;
    }

    void setAnchorPoints(bool ac) {
        viewAnchorPoints = ac;
    }

    void setStep(int s) {
        switch(s) {
            case 0: step = DeformMesh::DEFORMSTEP::STEP1; break;
            case 1: step = DeformMesh::DEFORMSTEP::STEP2; break;
            case 2: step = DeformMesh::DEFORMSTEP::STEP3; break;
        }
        if (!m_mesh) return;
        oldVertices = m_mesh->getVertices();
        DeformMesh::initDeform(m_mesh, constrained, solve);
        DeformMesh::deformMesh(m_mesh, oldVertices, constrained, solve, step, m_meshStep2);
    }

    // Should really be using STL files but OBJs are easy (chops off z coord).
    void loadObj(QString file) {
        constrained.clear();
        m_mesh = ResourceLoader::loadOBJMesh(file.toStdString());
        m_meshStep2 = nullptr;
    }

protected:
    QTime time;
    QTimer timer;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    int width, height;
    bool held, wireframe, viewAnchorPoints;

    std::shared_ptr<Mesh2D> m_mesh, m_meshStep2;
    std::set<int> constrained;
    std::vector<Eigen::Vector2f> oldVertices;
    int lastConstrained = -1;
    Eigen::MatrixXf solve;
    DeformMesh::DEFORMSTEP step;

    std::shared_ptr<CS123::GL::Shader> m_defaultShader, m_wireframeShader, m_anchorPointShader;

private slots:
    void tick();

};

#endif // VIEW_H

