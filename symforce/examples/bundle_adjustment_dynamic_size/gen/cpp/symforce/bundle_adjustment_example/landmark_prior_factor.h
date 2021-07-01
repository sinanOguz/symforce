// -----------------------------------------------------------------------------
// This file was autogenerated by symforce. Do NOT modify by hand.
// -----------------------------------------------------------------------------
#pragma once

#include <Eigen/Dense>

namespace bundle_adjustment_example {

/**
 * This function was autogenerated from a symbolic function. Do not modify by hand.
 *
 * Symbolic function: landmark_prior_residual
 * Arg type(s): Symbol, Symbol, Symbol, Symbol, Symbol
 * Return type(s): Matrix11
 *     geo.Matrix: Jacobian for args 0 (landmark)
 *     geo.Matrix: Hessian for args 0 (landmark)
 *     geo.Matrix: rhs for args 0 (landmark)
 */
template <typename Scalar>
void LandmarkPriorFactor(const Scalar landmark, const Scalar inverse_range_prior,
                         const Scalar weight, const Scalar sigma, const Scalar epsilon,
                         Eigen::Matrix<Scalar, 1, 1>* const res = nullptr,
                         Eigen::Matrix<Scalar, 1, 1>* const jacobian = nullptr,
                         Eigen::Matrix<Scalar, 1, 1>* const hessian = nullptr,
                         Eigen::Matrix<Scalar, 1, 1>* const rhs = nullptr) {
  // Total ops: 10

  // Input arrays

  // Intermediate terms (4)
  const Scalar _tmp0 = -inverse_range_prior + landmark;
  const Scalar _tmp1 = epsilon + sigma;
  const Scalar _tmp2 = weight / _tmp1;
  const Scalar _tmp3 = (weight * weight) / (_tmp1 * _tmp1);

  // Output terms (4)
  if (res != nullptr) {
    Eigen::Matrix<Scalar, 1, 1>& _res = (*res);

    _res(0, 0) = _tmp0 * _tmp2;
  }

  if (jacobian != nullptr) {
    Eigen::Matrix<Scalar, 1, 1>& _jacobian = (*jacobian);

    _jacobian(0, 0) = _tmp2;
  }

  if (hessian != nullptr) {
    Eigen::Matrix<Scalar, 1, 1>& _hessian = (*hessian);

    _hessian(0, 0) = _tmp3;
  }

  if (rhs != nullptr) {
    Eigen::Matrix<Scalar, 1, 1>& _rhs = (*rhs);

    _rhs(0, 0) = _tmp0 * _tmp3;
  }
}  // NOLINT(readability/fn_size)

// NOLINTNEXTLINE(readability/fn_size)
}  // namespace bundle_adjustment_example