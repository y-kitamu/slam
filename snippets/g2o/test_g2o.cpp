/*
 * test_g2o.cpp
 *
 * Create Date : 2022-08-05 16:27:33
 * Copyright (c) 2019- Yusuke Kitamura <ymyk6602@gmail.com>
 */
#include <filesystem>
#include <iostream>

#include <g2o/core/auto_differentiation.h>
#include <g2o/core/base_unary_edge.h>
#include <g2o/core/base_vertex.h>
#include <g2o/core/optimization_algorithm_factory.h>
#include <g2o/core/sparse_optimizer.h>
#include <g2o/stuff/command_args.h>
#include <g2o/stuff/sampler.h>
#include <glog/logging.h>
#include <gperftools/profiler.h>


namespace fs = std::filesystem;

G2O_USE_OPTIMIZATION_LIBRARY(dense);

double errorOfSolution(int numPoints, Eigen::Vector2d* points, const Eigen::Vector3d& circle) {
    Eigen::Vector2d center = circle.head<2>();
    double radius = circle(2);
    double error = 0.;
    for (int i = 0; i < numPoints; i++) {
        double d = (points[i] - center).norm() - radius;
        error += d * d;
    }
    return error;
}


class VertexCircle : public g2o::BaseVertex<3, Eigen::Vector3d> {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    VertexCircle() {}

    virtual bool read(std::istream&) { return false; }

    virtual bool write(std::ostream&) const { return false; }

    virtual void setToOriginImpl() {
        std::cerr << __PRETTY_FUNCTION__ << " not implemented yet" << std::endl;
    }

    virtual void oplusImpl(const double* update) {
        Eigen::Vector3d::ConstMapType v(update);
        _estimate += v;
    }
};


class EdgePointOnCircle : public g2o::BaseUnaryEdge<1, Eigen::Vector2d, VertexCircle> {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    EdgePointOnCircle() {}

    virtual bool read(std::istream&) { return false; }
    virtual bool write(std::ostream&) const { return false; }

    template <typename T>
    bool operator()(const T* circle, T* error) const {
        typename g2o::VectorN<2, T>::ConstMapType center(circle);
        const T& radius = circle[2];

        error[0] = (measurement().cast<T>() - center).norm() - radius;
        return true;
    }

    G2O_MAKE_AUTO_AD_FUNCTIONS;
};

int main(int argc, char** argv) {
    // logging codes
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    fs::path save_dir("./prof");
    if (!fs::exists(save_dir)) {
        fs::create_directories(save_dir);
    }
    auto save_path =
        (save_dir / fs::path((fs::path(argv[0])).stem().string() + ".prof")).generic_string();
    ProfilerStart(save_path.c_str());
    std::cout << "Profiling to " << save_path << std::endl;
    // end logging codes

    int numPoints;
    int maxIterations;
    bool verbose;
    g2o::CommandArgs arg;
    arg.param("numPoints", numPoints, 100, "number of points sampled from circle");
    arg.param("i", maxIterations, 10, "perform n iterations");
    arg.param("v", verbose, false, "verbose output of the optimization process");
    arg.parseArgs(argc, argv);

    Eigen::Vector2d center(4.0, 2.0);
    double radius = 2.0;
    Eigen::Vector2d* points = new Eigen::Vector2d[numPoints];


    g2o::Sampler::seedRand();
    for (int i = 0; i < numPoints; i++) {
        double r = g2o::Sampler::gaussRand(radius, 0.05);
        double angle = g2o::Sampler::uniformRand(0.0, 2.0 * M_PI);
        points[i].x() = center.x() + r * cos(angle);
        points[i].y() = center.y() + r * sin(angle);
    }

    g2o::SparseOptimizer optimizer;
    optimizer.setVerbose(false);

    g2o::OptimizationAlgorithmProperty solverProperty;
    optimizer.setAlgorithm(
        g2o::OptimizationAlgorithmFactory::instance()->construct("lm_dense", solverProperty));

    VertexCircle* circle = new VertexCircle();
    circle->setId(0);
    circle->setEstimate(Eigen::Vector3d(3.0, 3.0, 3.0));
    optimizer.addVertex(circle);

    for (int i = 0; i < numPoints; i++) {
        EdgePointOnCircle* e = new EdgePointOnCircle();
        e->setInformation(Eigen::Matrix<double, 1, 1>::Identity());
        e->setVertex(0, circle);
        e->setMeasurement(points[i]);
        optimizer.addEdge(e);
    }

    optimizer.initializeOptimization();
    optimizer.setVerbose(verbose);
    optimizer.optimize(maxIterations);

    if (verbose) {
        std::cout << std::endl;
    }

    std::cout << "Iterative least squares solution" << std::endl;
    std::cout << "center of the circle " << circle->estimate().head<2>().transpose() << std::endl;
    std::cout << "radius of the cirlce " << circle->estimate()(2) << std::endl;
    std::cout << "error " << errorOfSolution(numPoints, points, circle->estimate()) << std::endl;
    std::cout << std::endl;

    Eigen::MatrixXd A(numPoints, 3);
    Eigen::VectorXd b(numPoints);
    for (int i = 0; i < numPoints; i++) {
        A(i, 0) = -2 * points[i].x();
        A(i, 1) = -2 * points[i].y();
        A(i, 2) = 1;
        b(i) = -std::pow(points[i].x(), 2) - std::pow(points[i].y(), 2);
    }

    Eigen::Vector3d solution = (A.transpose() * A).ldlt().solve(A.transpose() * b);
    solution(2) = std::sqrt(pow(solution(0), 2) + std::pow(solution(1), 2) - solution(2));

    std::cout << "Linear least squares solution" << std::endl;
    std::cout << "center of the circle " << solution.head<2>().transpose() << std::endl;
    std::cout << "radius of the cirlce " << solution(2) << std::endl;
    std::cout << "error " << errorOfSolution(numPoints, points, solution) << std::endl;

    // clean up
    delete[] points;

    ProfilerStop();
}
