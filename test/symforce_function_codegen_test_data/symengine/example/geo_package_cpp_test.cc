//  -----------------------------------------------------------------------------
// This file was autogenerated by symforce. Do NOT modify by hand.
//
// Instead modify:
//     symforce/codegen/cpp_templates/example/geo_package_cpp_test.cc.jinja
//
// And then run `symforce_gen_codegen_test --update`.
// -----------------------------------------------------------------------------
/**
 * Tests for C++ geometry types. Mostly checking all the templates compile since
 * the math is tested comprehensively in symbolic form.
 */

#include <iostream>

#include <sym/pose2.h>
#include <sym/pose3.h>
#include <sym/rot2.h>
#include <sym/rot3.h>
#include <symforce/opt/util.h>

#include "../cpp/symforce/sym/tangent_d_storage_pose2.h"
#include "../cpp/symforce/sym/tangent_d_storage_pose3.h"
#include "../cpp/symforce/sym/tangent_d_storage_rot2.h"
#include "../cpp/symforce/sym/tangent_d_storage_rot3.h"
#include "catch.hpp"

TEST_CASE("Test Rot3", "[geo_package]") {
  // Make a random rotation
  std::mt19937 gen(42);
  const sym::Rot3f rot = sym::Rot3f::Random(gen);

  // Cast
  const sym::Rot3d rotd = rot.Cast<double>();
  CHECK(rotd.IsApprox(rot.Cast<double>(), 1e-6));
  CHECK(rotd.Cast<float>().IsApprox(rot, 1e-6));

  // Convert to Eigen rotation representations
  const Eigen::Quaternionf quat = rot.Quaternion();
  const Eigen::AngleAxisf aa = rot.AngleAxis();
  const Eigen::Matrix<float, 3, 3> mat = rot.ToRotationMatrix();
  const Eigen::Matrix<float, 3, 1> ypr = rot.YawPitchRoll();

  // Rotate a point
  const Eigen::Vector3f point = sym::Random<Eigen::Vector3f>(gen);
  CHECK((quat * point).isApprox(aa * point, 1e-6));
  CHECK((quat * point).isApprox(mat * point, 1e-6));
  CHECK((quat * point).isApprox(rot * point, 1e-6));

  // Rotate a point as an initializer expression
  CHECK((quat * Eigen::Vector3f::UnitX()).isApprox(rot * Eigen::Vector3f::UnitX(), 1e-6));

  // Construct back from Eigen rotation representations
  CHECK(sym::Rot3f(quat).IsApprox(rot, 1e-6));
  CHECK(sym::Rot3f(aa).IsApprox(rot, 1e-6));
  CHECK(sym::Rot3f::FromRotationMatrix(mat).IsApprox(rot, 1e-6));
  CHECK(sym::Rot3f::FromYawPitchRoll(ypr).ToPositiveReal().IsApprox(rot, 1e-6));

  // Make a pose
  sym::Pose3f pose(sym::Rot3f(aa), point);
  CHECK(pose.Rotation().IsApprox(rot, 1e-6));
  CHECK(pose.Position() == point);

  const sym::Pose3f pose_inv = pose.Inverse();
  CHECK(pose_inv.Rotation().IsApprox(rot.Inverse(), 1e-6));

  // Transform a point with a pose
  CHECK((pose_inv * point).norm() < 1e-6);

  // Transform a point as an initializer expression
  CHECK((pose * Eigen::Vector3f::UnitX().eval()).isApprox(pose * Eigen::Vector3f::UnitX(), 1e-6));

  // Check zero comparison
  CHECK(sym::Rot3f(Eigen::Vector4f::Zero()).IsApprox(sym::Rot3f(Eigen::Vector4f::Zero()), 1e-9));
  CHECK(!sym::Rot3f().IsApprox(sym::Rot3f(Eigen::Vector4f::Zero()), 1e-9));

  // Check that the log returns vectors with norm less than pi, and is the inverse of exp
  for (int i = 0; i < 1000; i++) {
    const sym::Rot3d rot = sym::Rot3d::Random(gen);
    const Eigen::Vector3d log = rot.ToTangent();
    CHECK(log.norm() <= M_PI);
    const sym::Rot3d exp_log_rot = sym::Rot3d::FromTangent(log);

    // The quaternion might not be equal, it might be negated, but the matrix should be equal
    CHECK(rot.ToRotationMatrix().isApprox(exp_log_rot.ToRotationMatrix(), 1e-9));
  }
}

TEST_CASE("Test Pose3", "[geo_package]") {
  // Make a random pose
  std::mt19937 gen(42);
  const sym::Pose3d pose = sym::Random<sym::Pose3d>(gen);

  // Test InverseCompose
  const Eigen::Vector3d point = sym::Random<Eigen::Vector3d>(gen);
  CHECK(pose.InverseCompose(point).isApprox(pose.Inverse() * point, 1e-9));
}

TEST_CASE("Test Rot2 and Pose2", "[geo_package]") {
  std::mt19937 gen(42);
  const sym::Rot2f rot = sym::Random<sym::Rot2f>(gen);
  const Eigen::Vector2f pos = sym::Random<Eigen::Vector2f>(gen);

  // Cast
  const sym::Rot2d rotd = rot.Cast<double>();
  CHECK(rotd.IsApprox(rot.Cast<double>(), 1e-6));
  CHECK(rotd.Cast<float>().IsApprox(rot, 1e-6));

  // Make a pose
  const sym::Pose2f pose(rot, pos);
  CHECK(pose.Rotation().IsApprox(rot, 1e-6));
  CHECK(pose.Position() == pos);

  const sym::Pose2f pose_inv = pose.Inverse();
  CHECK(pose_inv.Rotation().IsApprox(rot.Inverse(), 1e-9));

  // Test InverseCompose
  const Eigen::Vector2f point = sym::Random<Eigen::Vector2f>(gen);
  CHECK(pose.InverseCompose(point).isApprox(pose.Inverse() * point, 1e-6));

  // Test FromAngle and angle constructor
  const float angle = rot.ToTangent()(0);
  CHECK(rot.IsApprox(sym::Rot2f(angle), 1e-6));
  CHECK(rot.IsApprox(sym::Rot2f::FromAngle(angle), 1e-6));
}

TEMPLATE_TEST_CASE("Test Storage ops", "[geo_package]", sym::Rot2<double>, sym::Rot2<float>,
                   sym::Pose2<double>, sym::Pose2<float>, sym::Rot3<double>, sym::Rot3<float>,
                   sym::Pose3<double>, sym::Pose3<float>) {
  using T = TestType;

  using Scalar = typename sym::StorageOps<T>::Scalar;

  const T value{};
  std::cout << "*** Testing StorageOps: " << value << " ***" << std::endl;

  constexpr int32_t storage_dim = sym::StorageOps<T>::StorageDim();
  CHECK(value.Data().rows() == storage_dim);
  CHECK(value.Data().cols() == 1);

  std::vector<Scalar> vec;
  vec.resize(storage_dim);
  sym::StorageOps<T>::ToStorage(value, vec.data());
  CHECK(vec.size() > 0);
  CHECK(vec.size() == storage_dim);
  for (int i = 0; i < vec.size(); ++i) {
    CHECK(vec[i] == value.Data()[i]);
  }

  const T value2 = sym::StorageOps<T>::FromStorage(vec.data());
  CHECK(value.Data() == value2.Data());
  vec[0] = 2.1;
  vec[vec.size() - 1] = 1.2;
  const T value3 = sym::StorageOps<T>::FromStorage(vec.data());
  CHECK(value.Data() != value3.Data());
}

TEMPLATE_TEST_CASE("Test Scalar storage ops", "[geo_package]", double, float) {
  using T = TestType;

  using Scalar = typename sym::StorageOps<T>::Scalar;

  const T value{};
  std::cout << "*** Testing StorageOps: " << value << " ***" << std::endl;

  constexpr int32_t storage_dim = sym::StorageOps<T>::StorageDim();
  CHECK(storage_dim == 1);

  std::vector<Scalar> vec;
  vec.resize(storage_dim);
  sym::StorageOps<T>::ToStorage(value, vec.data());
  CHECK(vec.size() == storage_dim);
  CHECK(vec[0] == value);

  const T value2 = sym::StorageOps<T>::FromStorage(vec.data());
  CHECK(value == value2);
  vec[0] = 2.1;
  const T value3 = sym::StorageOps<T>::FromStorage(vec.data());
  CHECK(value != value3);
}

TEMPLATE_TEST_CASE("Test Matrix storage ops", "[geo_package]", sym::Vector1<double>,
                   sym::Vector1<float>, sym::Vector2<double>, sym::Vector2<float>,
                   sym::Vector3<double>, sym::Vector3<float>, sym::Vector4<double>,
                   sym::Vector4<float>, sym::Vector5<double>, sym::Vector5<float>,
                   sym::Vector6<double>, sym::Vector6<float>, sym::Vector7<double>,
                   sym::Vector7<float>, sym::Vector8<double>, sym::Vector8<float>,
                   sym::Vector9<double>, sym::Vector9<float>) {
  using T = TestType;

  using Scalar = typename sym::StorageOps<T>::Scalar;

  const T value = T::Zero();
  std::cout << "*** Testing Matrix StorageOps: " << value.transpose() << " ***" << std::endl;

  constexpr int32_t storage_dim = sym::StorageOps<T>::StorageDim();
  CHECK(storage_dim == T::RowsAtCompileTime);

  std::vector<Scalar> vec;
  vec.resize(storage_dim);
  sym::StorageOps<T>::ToStorage(value, vec.data());
  CHECK(vec.size() == storage_dim);
  for (int i = 0; i < vec.size(); ++i) {
    CHECK(vec[i] == value[i]);
  }

  const T value2 = sym::StorageOps<T>::FromStorage(vec.data());
  CHECK(value == value2);
  vec[0] = 2.1;
  const T value3 = sym::StorageOps<T>::FromStorage(vec.data());
  CHECK(value != value3);
}

TEMPLATE_TEST_CASE("Test Group ops", "[geo_package]", sym::Rot2<double>, sym::Rot2<float>,
                   sym::Pose2<double>, sym::Pose2<float>, sym::Rot3<double>, sym::Rot3<float>,
                   sym::Pose3<double>, sym::Pose3<float>) {
  using T = TestType;

  const T identity{};
  std::cout << "*** Testing GroupOps: " << identity << " ***" << std::endl;

  // TODO(hayk): Make sym::StorageOps<T>::IsApprox that uses ToStorage to compare, then
  // get rid of the custom scalar version below.
  CHECK(identity.IsApprox(sym::GroupOps<T>::Identity(), 1e-9));
  CHECK(identity.IsApprox(sym::GroupOps<T>::Compose(identity, identity), 1e-9));
  CHECK(identity.IsApprox(sym::GroupOps<T>::Inverse(identity), 1e-9));
  CHECK(identity.IsApprox(sym::GroupOps<T>::Between(identity, identity), 1e-9));
}

TEMPLATE_TEST_CASE("Test Scalar group ops", "[geo_package]", double, float) {
  using T = TestType;

  const T identity{};
  std::cout << "*** Testing GroupOps: " << identity << " ***" << std::endl;

  CHECK(identity == sym::GroupOps<T>::Identity());
  CHECK(identity == sym::GroupOps<T>::Compose(identity, identity));
  CHECK(identity == sym::GroupOps<T>::Inverse(identity));
  CHECK(identity == sym::GroupOps<T>::Between(identity, identity));
}

TEMPLATE_TEST_CASE("Test Matrix group ops", "[geo_package]", sym::Vector1<double>,
                   sym::Vector1<float>, sym::Vector2<double>, sym::Vector2<float>,
                   sym::Vector3<double>, sym::Vector3<float>, sym::Vector4<double>,
                   sym::Vector4<float>, sym::Vector5<double>, sym::Vector5<float>,
                   sym::Vector6<double>, sym::Vector6<float>, sym::Vector7<double>,
                   sym::Vector7<float>, sym::Vector8<double>, sym::Vector8<float>,
                   sym::Vector9<double>, sym::Vector9<float>) {
  using T = TestType;

  const T identity = T::Zero();
  std::cout << "*** Testing Matrix GroupOps: " << identity.transpose() << " ***" << std::endl;

  CHECK(identity == sym::GroupOps<T>::Identity());
  CHECK(identity == sym::GroupOps<T>::Compose(identity, identity));
  CHECK(identity == sym::GroupOps<T>::Inverse(identity));
  CHECK(identity == sym::GroupOps<T>::Between(identity, identity));
}

TEMPLATE_TEST_CASE("Test Lie group ops", "[geo_package]", sym::Rot2<double>, sym::Rot2<float>,
                   sym::Pose2<double>, sym::Pose2<float>, sym::Rot3<double>, sym::Rot3<float>,
                   sym::Pose3<double>, sym::Pose3<float>) {
  using T = TestType;

  using Scalar = typename sym::StorageOps<T>::Scalar;
  using TangentVec = Eigen::Matrix<Scalar, sym::LieGroupOps<T>::TangentDim(), 1>;
  constexpr const int32_t storage_dim = sym::StorageOps<T>::StorageDim();
  using StorageVec = Eigen::Matrix<Scalar, storage_dim, 1>;
  using SelfJacobian = typename sym::GroupOps<T>::SelfJacobian;
  const Scalar epsilon = 1e-7;

  const T identity = sym::GroupOps<T>::Identity();
  std::cout << "*** Testing LieGroupOps: " << identity << " ***" << std::endl;

  constexpr int32_t tangent_dim = sym::LieGroupOps<T>::TangentDim();
  CHECK(tangent_dim > 0);
  CHECK(tangent_dim <= sym::StorageOps<T>::StorageDim());

  std::mt19937 gen(24362);
  const TangentVec pertubation = sym::Random<TangentVec>(gen);
  const T value = sym::LieGroupOps<T>::FromTangent(pertubation, epsilon);

  const TangentVec recovered_pertubation = sym::LieGroupOps<T>::ToTangent(value, epsilon);
  CHECK(pertubation.isApprox(recovered_pertubation, std::sqrt(epsilon)));

  const T recovered_identity = sym::LieGroupOps<T>::Retract(value, -recovered_pertubation, epsilon);
  CHECK(recovered_identity.IsApprox(identity, std::sqrt(epsilon)));

  const TangentVec pertubation_zero =
      sym::LieGroupOps<T>::LocalCoordinates(identity, recovered_identity, epsilon);
  CHECK(pertubation_zero.norm() < std::sqrt(epsilon));

  SelfJacobian inverse_jacobian;
  sym::GroupOps<T>::InverseWithJacobian(identity, &inverse_jacobian);
  CHECK(inverse_jacobian.isApprox(-SelfJacobian::Identity(), epsilon));

  // Test perturbing one axis at a time by sqrt(epsilon)
  // Makes sure special cases of one-axis perturbations are handled correctly, and that distortion
  // due to epsilon doesn't extend too far away from 0
  {
    TangentVec small_perturbation = TangentVec::Zero();
    for (size_t i = 0; i < sym::LieGroupOps<T>::TangentDim(); i++) {
      small_perturbation(i) = std::sqrt(epsilon);
      const T value = sym::LieGroupOps<T>::FromTangent(small_perturbation, epsilon);
      const TangentVec recovered_perturbation = sym::LieGroupOps<T>::ToTangent(value, epsilon);
      CHECK(small_perturbation.isApprox(recovered_perturbation, 10 * epsilon));
      small_perturbation(i) = 0;
    }
  }

  // Test tangent_D_storage generated from the symbolic version in SymForce against numerical
  // derivatives
  for (size_t i = 0; i < 10000; i++) {
    const T a = sym::Random<T>(gen);

    StorageVec storage;
    sym::StorageOps<T>::ToStorage(a, storage.data());
    const Eigen::Matrix<Scalar, tangent_dim, storage_dim> numerical_tangent_D_storage =
        sym::NumericalDerivative(
            [epsilon, &a](const StorageVec& storage_perturbed) {
              return sym::LieGroupOps<T>::LocalCoordinates(
                  a, sym::StorageOps<T>::FromStorage(storage_perturbed.data()), epsilon);
            },
            storage, epsilon, std::sqrt(epsilon));

    const Eigen::Matrix<Scalar, tangent_dim, storage_dim> symforce_tangent_D_storage =
        sym::Tangent_D_Storage(a, epsilon);

    CHECK(
        numerical_tangent_D_storage.isApprox(symforce_tangent_D_storage, 10 * std::sqrt(epsilon)));
  }

  // Test ComposeWithJacobians against numerical derivatives
  for (size_t i = 0; i < 10000; i++) {
    const T a = sym::Random<T>(gen);
    const T b = sym::Random<T>(gen);

    const SelfJacobian numerical_jacobian =
        sym::NumericalDerivative(std::bind(&sym::GroupOps<T>::Compose, std::placeholders::_1, b), a,
                                 epsilon, std::sqrt(epsilon));

    SelfJacobian symforce_jacobian;
    sym::GroupOps<T>::ComposeWithJacobians(a, b, &symforce_jacobian, nullptr);

    CHECK(numerical_jacobian.isApprox(symforce_jacobian, 10 * std::sqrt(epsilon)));
  }
}

TEMPLATE_TEST_CASE("Test Scalar Lie group ops", "[geo_package]", double, float) {
  using T = TestType;

  using Scalar = typename sym::StorageOps<T>::Scalar;
  using TangentVec = Eigen::Matrix<Scalar, sym::LieGroupOps<T>::TangentDim(), 1>;
  const Scalar epsilon = 1e-7;

  const T identity = sym::GroupOps<T>::Identity();
  std::cout << "*** Testing LieGroupOps: " << identity << " ***" << std::endl;

  constexpr int32_t tangent_dim = sym::LieGroupOps<T>::TangentDim();
  CHECK(tangent_dim > 0);
  CHECK(tangent_dim <= sym::StorageOps<T>::StorageDim());

  std::mt19937 gen(42);
  const TangentVec pertubation = sym::Random<TangentVec>(gen);
  const T value = sym::LieGroupOps<T>::FromTangent(pertubation, epsilon);

  const TangentVec recovered_pertubation = sym::LieGroupOps<T>::ToTangent(value, epsilon);
  CHECK(pertubation.isApprox(recovered_pertubation, std::sqrt(epsilon)));

  const T recovered_identity = sym::LieGroupOps<T>::Retract(value, -recovered_pertubation, epsilon);
  CHECK(fabs(recovered_identity - identity) < std::sqrt(epsilon));

  const TangentVec pertubation_zero =
      sym::LieGroupOps<T>::LocalCoordinates(identity, recovered_identity, epsilon);
  CHECK(pertubation_zero.norm() < std::sqrt(epsilon));
}

TEMPLATE_TEST_CASE("Test Matrix Lie group ops", "[geo_package]", sym::Vector1<double>,
                   sym::Vector1<float>, sym::Vector2<double>, sym::Vector2<float>,
                   sym::Vector3<double>, sym::Vector3<float>, sym::Vector4<double>,
                   sym::Vector4<float>, sym::Vector5<double>, sym::Vector5<float>,
                   sym::Vector6<double>, sym::Vector6<float>, sym::Vector7<double>,
                   sym::Vector7<float>, sym::Vector8<double>, sym::Vector8<float>,
                   sym::Vector9<double>, sym::Vector9<float>) {
  using T = TestType;

  using Scalar = typename sym::StorageOps<T>::Scalar;
  using TangentVec = Eigen::Matrix<Scalar, sym::LieGroupOps<T>::TangentDim(), 1>;
  const Scalar epsilon = 1e-7;

  const T identity = sym::GroupOps<T>::Identity();
  std::cout << "*** Testing Matrix LieGroupOps: " << identity.transpose() << " ***" << std::endl;

  constexpr int32_t tangent_dim = sym::LieGroupOps<T>::TangentDim();
  CHECK(tangent_dim > 0);
  CHECK(tangent_dim <= sym::StorageOps<T>::StorageDim());

  std::mt19937 gen(42);
  const TangentVec pertubation = sym::Random<TangentVec>(gen);
  const T value = sym::LieGroupOps<T>::FromTangent(pertubation, epsilon);

  const TangentVec recovered_pertubation = sym::LieGroupOps<T>::ToTangent(value, epsilon);
  CHECK(pertubation.isApprox(recovered_pertubation, std::sqrt(epsilon)));

  const T recovered_identity = sym::LieGroupOps<T>::Retract(value, -recovered_pertubation, epsilon);
  CHECK(recovered_identity.isApprox(identity, std::sqrt(epsilon)));

  const TangentVec pertubation_zero =
      sym::LieGroupOps<T>::LocalCoordinates(identity, recovered_identity, epsilon);
  CHECK(pertubation_zero.norm() < std::sqrt(epsilon));
}
