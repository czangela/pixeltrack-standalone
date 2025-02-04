#ifndef RecoLocalTracker_SiPixelRecHits_plugins_gpuPixelDoublets_h
#define RecoLocalTracker_SiPixelRecHits_plugins_gpuPixelDoublets_h

#include "gpuPixelDoubletsAlgos.h"

namespace KOKKOS_NAMESPACE {
  namespace gpuPixelDoublets {

    using CellNeighbors = CAConstants::CellNeighbors;
    using CellTracks = CAConstants::CellTracks;
    using CellNeighborsVector = CAConstants::CellNeighborsVector;
    using CellTracksVector = CAConstants::CellTracksVector;

    constexpr auto getDoubletsFromHistoMaxBlockSize = 64;  // for both x and y
    constexpr auto getDoubletsFromHistoMinBlocksPerMP = 16;

    constexpr int nPairs = 13 + 2 + 4;
    static_assert(nPairs <= CAConstants::maxNumberOfLayerPairs());

    constexpr int16_t phi0p05 = 522;  // round(521.52189...) = phi2short(0.05);
    constexpr int16_t phi0p06 = 626;  // round(625.82270...) = phi2short(0.06);
    constexpr int16_t phi0p07 = 730;  // round(730.12648...) = phi2short(0.07);A

    class getDoubletsFromHisto {
    private:
      const uint8_t layerPairs[2 * nPairs] = {
          0, 1, 0, 4, 0, 7,              // BPIX1 (3)
          1, 2, 1, 4, 1, 7,              // BPIX2 (5)
          4, 5, 7, 8,                    // FPIX1 (8)
          2, 3, 2, 4, 2, 7, 5, 6, 8, 9,  // BPIX3 & FPIX2 (13)
          0, 2, 1, 3,                    // Jumping Barrel (15)
          0, 5, 0, 8,                    // Jumping Forward (BPIX1,FPIX2)
          4, 6, 7, 9                     // Jumping Forward (19)
      };
      const int16_t phicuts[nPairs] = {phi0p05,
                                       phi0p07,
                                       phi0p07,
                                       phi0p05,
                                       phi0p06,
                                       phi0p06,
                                       phi0p05,
                                       phi0p05,
                                       phi0p06,
                                       phi0p06,
                                       phi0p06,
                                       phi0p05,
                                       phi0p05,
                                       phi0p05,
                                       phi0p05,
                                       phi0p05,
                                       phi0p05,
                                       phi0p05,
                                       phi0p05};
      float const minz[nPairs] = {
          -20., 0., -30., -22., 10., -30., -70., -70., -22., 15., -30, -70., -70., -20., -22., 0, -30., -70., -70.};
      float const maxz[nPairs] = {
          20., 30., 0., 22., 30., -10., 70., 70., 22., 30., -15., 70., 70., 20., 22., 30., 0., 70., 70.};
      float const maxr[nPairs] = {20., 9., 9., 20., 7., 7., 5., 5., 20., 6., 6., 5., 5., 20., 20., 9., 9., 9., 9.};

      // Need to be by value because the object if this struct is transferred to the device
      Kokkos::View<GPUCACell*, KokkosDeviceMemSpace, Restrict> cells;
      Kokkos::View<uint32_t, KokkosDeviceMemSpace, Restrict> nCells;
      Kokkos::View<CAConstants::CellNeighborsVector, KokkosDeviceMemSpace, Restrict>
          cellNeighbors;                                                                       // not used at the moment
      Kokkos::View<CAConstants::CellTracksVector, KokkosDeviceMemSpace, Restrict> cellTracks;  // not used at the moment
      TrackingRecHit2DSOAView const* __restrict__ hhp;
      Kokkos::View<GPUCACell::OuterHitOfCell*, KokkosDeviceMemSpace, Restrict> isOuterHitOfCell;
      int nActualPairs;
      bool ideal_cond;
      bool doClusterCut;
      bool doZ0Cut;
      bool doPtCut;
      uint32_t maxNumOfDoublets;
      const int stride;

    public:
      getDoubletsFromHisto(
          const Kokkos::View<GPUCACell*, KokkosDeviceMemSpace, Restrict>& cells,
          const Kokkos::View<uint32_t, KokkosDeviceMemSpace, Restrict>& nCells,
          const Kokkos::View<CAConstants::CellNeighborsVector, KokkosDeviceMemSpace, Restrict>&
              cellNeighbors,  // not used at the moment
          const Kokkos::View<CAConstants::CellTracksVector, KokkosDeviceMemSpace, Restrict>&
              cellTracks,  // not used at the moment
          TrackingRecHit2DSOAView const* __restrict__ hhp,
          const Kokkos::View<GPUCACell::OuterHitOfCell*, KokkosDeviceMemSpace, Restrict>& isOuterHitOfCell,
          int nActualPairs,
          bool ideal_cond,
          bool doClusterCut,
          bool doZ0Cut,
          bool doPtCut,
          uint32_t maxNumOfDoublets,
          const int stride)
          : cells(cells),
            nCells(nCells),
            cellNeighbors(cellNeighbors),
            cellTracks(cellTracks),
            hhp(hhp),
            isOuterHitOfCell(isOuterHitOfCell),
            nActualPairs(nActualPairs),
            ideal_cond(ideal_cond),
            doClusterCut(doClusterCut),
            doZ0Cut(doZ0Cut),
            doPtCut(doPtCut),
            maxNumOfDoublets(maxNumOfDoublets),
            stride(stride) {}

      KOKKOS_INLINE_FUNCTION void operator()(const Kokkos::TeamPolicy<KokkosExecSpace>::member_type& teamMember) const {
        auto const& __restrict__ hh = *hhp;
        doubletsFromHisto(layerPairs,
                          nActualPairs,
                          cells,
                          nCells,
                          cellNeighbors,
                          cellTracks,
                          hh,
                          isOuterHitOfCell,
                          phicuts,
                          minz,
                          maxz,
                          maxr,
                          ideal_cond,
                          doClusterCut,
                          doZ0Cut,
                          doPtCut,
                          maxNumOfDoublets,
                          stride,
                          teamMember);
      }
    };
  }  // namespace gpuPixelDoublets
}  // namespace KOKKOS_NAMESPACE

#endif  // RecoLocalTracker_SiPixelRecHits_plugins_gpuPixelDouplets_h
