#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include <iostream>
#include <random>

class MultiVariateNormal
{
public:
  MultiVariateNormal(const Eigen::VectorXd mean, const Eigen::MatrixXd covariance)
  : mean_(mean), covariance_(covariance)
  {
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(covariance_);
    transform_ = eigen_solver.eigenvectors() * eigen_solver.eigenvalues().cwiseSqrt().asDiagonal();
  }
  double pdf(const Eigen::VectorXd & x) const
  {
    double n = x.rows();
    double a = (x - mean_).transpose() * covariance_.inverse() * (x - mean_);
    double b = std::pow(std::sqrt(2 * M_PI), n) * std::sqrt(covariance_.determinant());
    return std::exp(-0.5 * a) / b;
  }

  Eigen::VectorXd operator()() const
  {
    static std::random_device seed;
    static std::mt19937 engine(seed());
    static std::normal_distribution<> dist;

    return mean_ + transform_ * Eigen::VectorXd{mean_.size()}.unaryExpr([&](auto x) { return dist(engine); });
  }
  void setMean(const Eigen::VectorXd mean) { mean_ = mean; }
  void setCovariance(const Eigen::MatrixXd covariance) { covariance_ = covariance; }

private:
  Eigen::VectorXd mean_;
  Eigen::MatrixXd covariance_;
  Eigen::MatrixXd transform_;
};

int main(int argc, char ** argv)
{
  Eigen::VectorXd mean(2);
  Eigen::MatrixXd covariance(2, 2);

  mean << 0.0, 0.0;
  covariance << 1.0, 0.1, 0.1, 1.0;

  MultiVariateNormal multi_variate_normal(mean, covariance);

  Eigen::VectorXd x(2);
  x << 0.2, 0.3;
  std::cout << multi_variate_normal.pdf(x) << std::endl;

  return 0;
}
