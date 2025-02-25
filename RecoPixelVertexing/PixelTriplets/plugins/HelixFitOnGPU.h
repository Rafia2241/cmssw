#ifndef RecoPixelVertexing_PixelTriplets_plugins_HelixFitOnGPU_h
#define RecoPixelVertexing_PixelTriplets_plugins_HelixFitOnGPU_h

#include "CUDADataFormats/Track/interface/PixelTrackHeterogeneous.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DHeterogeneous.h"
#include "RecoPixelVertexing/PixelTrackFitting/interface/FitResult.h"

#include "CAConstants.h"

namespace riemannFit {
  // in case of memory issue can be made smaller
  constexpr uint32_t maxNumberOfConcurrentFits = caConstants::maxNumberOfTuples;
  constexpr uint32_t stride = maxNumberOfConcurrentFits;
  using Matrix3x4d = Eigen::Matrix<double, 3, 4>;
  using Map3x4d = Eigen::Map<Matrix3x4d, 0, Eigen::Stride<3 * stride, stride> >;
  using Matrix6x4f = Eigen::Matrix<float, 6, 4>;
  using Map6x4f = Eigen::Map<Matrix6x4f, 0, Eigen::Stride<6 * stride, stride> >;

  // hits
  template <int N>
  using Matrix3xNd = Eigen::Matrix<double, 3, N>;
  template <int N>
  using Map3xNd = Eigen::Map<Matrix3xNd<N>, 0, Eigen::Stride<3 * stride, stride> >;
  // errors
  template <int N>
  using Matrix6xNf = Eigen::Matrix<float, 6, N>;
  template <int N>
  using Map6xNf = Eigen::Map<Matrix6xNf<N>, 0, Eigen::Stride<6 * stride, stride> >;
  // fast fit
  using Map4d = Eigen::Map<Vector4d, 0, Eigen::InnerStride<stride> >;

}  // namespace riemannFit

class HelixFitOnGPU {
public:
  using HitsView = TrackingRecHit2DSOAView;

  using Tuples = pixelTrack::HitContainer;
  using OutputSoA = pixelTrack::TrackSoA;

  using TupleMultiplicity = caConstants::TupleMultiplicity;

  explicit HelixFitOnGPU(float bf, bool fitNas4) : bField_(bf), fitNas4_(fitNas4) {}
  ~HelixFitOnGPU() { deallocateOnGPU(); }

  void setBField(double bField) { bField_ = bField; }
  void launchRiemannKernels(HitsView const *hv, uint32_t nhits, uint32_t maxNumberOfTuples, cudaStream_t cudaStream);
  void launchBrokenLineKernels(HitsView const *hv, uint32_t nhits, uint32_t maxNumberOfTuples, cudaStream_t cudaStream);

  void launchRiemannKernelsOnCPU(HitsView const *hv, uint32_t nhits, uint32_t maxNumberOfTuples);
  void launchBrokenLineKernelsOnCPU(HitsView const *hv, uint32_t nhits, uint32_t maxNumberOfTuples);

  void allocateOnGPU(Tuples const *tuples, TupleMultiplicity const *tupleMultiplicity, OutputSoA *outputSoA);
  void deallocateOnGPU();

private:
  static constexpr uint32_t maxNumberOfConcurrentFits_ = riemannFit::maxNumberOfConcurrentFits;

  // fowarded
  Tuples const *tuples_ = nullptr;
  TupleMultiplicity const *tupleMultiplicity_ = nullptr;
  OutputSoA *outputSoa_;
  float bField_;

  const bool fitNas4_;
};

#endif  // RecoPixelVertexing_PixelTriplets_plugins_HelixFitOnGPU_h
