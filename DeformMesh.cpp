#include "DeformMesh.h"

using namespace Eigen;


void DeformMesh::initDeform(const std::shared_ptr<Mesh2D> &mesh, const std::set<int> &constraints, Eigen::MatrixXf &solve) {
    if (constraints.size() == 0) return;

    const std::vector<Vector2f> &initialVertices = mesh->getVertices();
    const std::vector<Vector3i> &faces = mesh->getFaces();

    // TODO: Step 1 and assign solve
}

// Feel free to use "step" to visualize the different steps and "meshStep2" to draw the step 2 fitted triangles.
void DeformMesh::deformMesh(const std::shared_ptr<Mesh2D> &mesh, const std::vector<Eigen::Vector2f> &initialVertices, const std::set<int> &constraints, const Eigen::MatrixXf &solve, DEFORMSTEP step, std::shared_ptr<Mesh2D> &meshStep2) {
    if (constraints.size() == 0) return;

    const std::vector<Vector3i> &faces = mesh->getFaces();
    std::vector<Vector2f> newVertices = mesh->getVertices();

    // TODO: Finish Step 1

    // TODO: Step 2

    // TODO: Step 3

    mesh->setVertices(newVertices);
}
