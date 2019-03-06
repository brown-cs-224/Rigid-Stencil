#ifndef DEFORMMESH_H
#define DEFORMMESH_H

#include "Mesh2D.h"
#include <set>

class DeformMesh
{
public:

    enum DEFORMSTEP {
      STEP1, STEP2, STEP3
    };

    static void initDeform(const std::shared_ptr<Mesh2D> &mesh, const std::set<int> &constraints, Eigen::MatrixXf &solve);
    static void deformMesh(const std::shared_ptr<Mesh2D> &mesh, const std::vector<Eigen::Vector2f> &initialVertices, const std::set<int> &constraints, const Eigen::MatrixXf &solve, DEFORMSTEP step, std::shared_ptr<Mesh2D> &meshStep2);
};

#endif // DEFORMMESH_H
