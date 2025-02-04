#ifndef KokkosCore_kokkosConfig_h
#define KokkosCore_kokkosConfig_h

#include <Kokkos_Core.hpp>
#include "KokkosCore/kokkosConfigCommon.h"

namespace cms::kokkos {
  template <typename Space>
  struct MemSpaceTraits {
    using HostSpace = Kokkos::HostSpace;
  };
#ifdef KOKKOS_ENABLE_CUDA
  template <>
  struct MemSpaceTraits<Kokkos::CudaSpace> {
    using HostSpace = Kokkos::CudaHostPinnedSpace;
  };
#endif
}  // namespace cms::kokkos

#ifdef KOKKOS_BACKEND_SERIAL
using KokkosExecSpace = Kokkos::Serial;
#define KOKKOS_NAMESPACE kokkos_serial
#elif defined KOKKOS_BACKEND_PTHREAD
using KokkosExecSpace = Kokkos::Threads;
#define KOKKOS_NAMESPACE kokkos_pthread
#elif defined KOKKOS_BACKEND_CUDA
using KokkosExecSpace = Kokkos::Cuda;
#define KOKKOS_NAMESPACE kokkos_cuda
#elif defined KOKKOS_BACKEND_HIP
using KokkosExecSpace = Kokkos::Experimental::HIP;
#define KOKKOS_NAMESPACE kokkos_hip
#else
#error "Unsupported Kokkos backend"
#endif

using KokkosDeviceMemSpace = KokkosExecSpace::memory_space;
using KokkosHostMemSpace = cms::kokkos::MemSpaceTraits<KokkosDeviceMemSpace>::HostSpace;

// this is meant mostly for unit tests
template <typename ExecSpace>
struct KokkosBackend;
template <>
struct KokkosBackend<Kokkos::Serial> {
  static constexpr auto value = kokkos_common::InitializeScopeGuard::Backend::SERIAL;
};
#ifdef KOKKOS_ENABLE_THREADS
template <>
struct KokkosBackend<Kokkos::Threads> {
  static constexpr auto value = kokkos_common::InitializeScopeGuard::Backend::PTHREAD;
};
#endif
#ifdef KOKKOS_ENABLE_CUDA
template <>
struct KokkosBackend<Kokkos::Cuda> {
  static constexpr auto value = kokkos_common::InitializeScopeGuard::Backend::CUDA;
};
#endif
#ifdef KOKKOS_ENABLE_HIP
template <>
struct KokkosBackend<Kokkos::Experimental::HIP> {
  static constexpr auto value = kokkos_common::InitializeScopeGuard::Backend::HIP;
};
#endif

// trick to force expanding KOKKOS_NAMESPACE before stringification inside DEFINE_FWK_MODULE
#define DEFINE_FWK_KOKKOS_MODULE2(name) DEFINE_FWK_MODULE(name)
#define DEFINE_FWK_KOKKOS_MODULE(name) DEFINE_FWK_KOKKOS_MODULE2(KOKKOS_NAMESPACE::name)

#define DEFINE_FWK_KOKKOS_EVENTSETUP_MODULE2(name) DEFINE_FWK_EVENTSETUP_MODULE(name)
#define DEFINE_FWK_KOKKOS_EVENTSETUP_MODULE(name) DEFINE_FWK_KOKKOS_EVENTSETUP_MODULE2(KOKKOS_NAMESPACE::name)

#endif
