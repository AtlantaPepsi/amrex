#ifndef AMREX_MultiFabUtil_H_
#define AMREX_MultiFabUtil_H_
#include <AMReX_Config.H>

#include <AMReX_MultiFab.H>
#include <AMReX_iMultiFab.H>
#include <AMReX_LayoutData.H>
#include <AMReX_MFIter.H>
#include <AMReX_Array.H>
#include <AMReX_Vector.H>
#include <AMReX_MultiFabUtil_C.H>

#include <AMReX_MultiFabUtilI.H>

namespace amrex
{
    //! Average nodal-based MultiFab onto cell-centered MultiFab.
    void average_node_to_cellcenter (MultiFab& cc, int dcomp,
                                     const MultiFab& nd, int scomp,
                                     int ncomp, int ngrow = 0);

    //! Average edge-based MultiFab onto cell-centered MultiFab. This fills in
    //! ngrow ghost cells in the cell-centered MultiFab. Both cell centered and
    //! edge centered MultiFabs need to have ngrow ghost values
    void average_edge_to_cellcenter (MultiFab& cc, int dcomp,
                                     const Vector<const MultiFab*>& edge,
                                     int ngrow = 0);

    //! Average face-based MultiFab onto cell-centered MultiFab.
    void average_face_to_cellcenter (MultiFab& cc, int dcomp,
                                     const Vector<const MultiFab*>& fc,
                                     int ngrow = 0);
    void average_face_to_cellcenter (MultiFab& cc, int dcomp,
                                     const Array<const MultiFab*,AMREX_SPACEDIM>& fc,
                                     int ngrow = 0);
    void average_face_to_cellcenter (MultiFab& cc,
                                     const Vector<const MultiFab*>& fc,
                                     const Geometry& geom);
    void average_face_to_cellcenter (MultiFab& cc,
                                     const Array<const MultiFab*,AMREX_SPACEDIM>& fc,
                                     const Geometry& geom);

    //! Average cell-centered MultiFab onto face-based MultiFab.
    void average_cellcenter_to_face (const Vector<MultiFab*>& fc,
                                     const MultiFab& cc,
                                     const Geometry& geom,
                                     int ncomp = 1,
                                     bool use_harmonic_averaging = false);
    void average_cellcenter_to_face (const Array<MultiFab*,AMREX_SPACEDIM>& fc,
                                     const MultiFab& cc,
                                     const Geometry& geom,
                                     int ncomp = 1,
                                     bool use_harmonic_averaging = false);

    //! Average fine face-based MultiFab onto crse face-based MultiFab.
    template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int> = 0>
    void average_down_faces (const Vector<const MF*>& fine,
                             const Vector<MF*>& crse,
                             const IntVect& ratio,
                             int ngcrse = 0);
    template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int> = 0>
    void average_down_faces (const Vector<const MF*>& fine,
                             const Vector<MF*>& crse,
                             int ratio,
                             int ngcrse = 0);
    template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int> = 0>
    void average_down_faces (const Array<const MF*,AMREX_SPACEDIM>& fine,
                             const Array<MF*,AMREX_SPACEDIM>& crse,
                             const IntVect& ratio,
                             int ngcrse = 0);
    template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int> = 0>
    void average_down_faces (const Array<const MF*,AMREX_SPACEDIM>& fine,
                             const Array<MF*,AMREX_SPACEDIM>& crse,
                             int ratio,
                             int ngcrse = 0);
    //! This version does average down for one direction.
    //! It uses the IndexType of MultiFabs to determine the direction.
    //! It is expected that one direction is nodal and the rest are cell-centered.
    template <typename FAB>
    void average_down_faces (const FabArray<FAB>& fine, FabArray<FAB>& crse,
                             const IntVect& ratio, int ngcrse=0);

    //  This version takes periodicity into account.
    template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int> = 0>
    void average_down_faces (const Array<const MF*,AMREX_SPACEDIM>& fine,
                             const Array<MF*,AMREX_SPACEDIM>& crse,
                             const IntVect& ratio, const Geometry& crse_geom);
    //  This version takes periodicity into account.
    template <typename FAB>
    void average_down_faces (const FabArray<FAB>& fine, FabArray<FAB>& crse,
                             const IntVect& ratio, const Geometry& crse_geom);

    //! Average fine edge-based MultiFab onto crse edge-based MultiFab.
    void average_down_edges (const Vector<const MultiFab*>& fine,
                             const Vector<MultiFab*>& crse,
                             const IntVect& ratio,
                             int ngcrse = 0);
    void average_down_edges (const Array<const MultiFab*,AMREX_SPACEDIM>& fine,
                             const Array<MultiFab*,AMREX_SPACEDIM>& crse,
                             const IntVect& ratio,
                             int ngcrse = 0);
    //! This version does average down for one direction.
    //! It uses the IndexType of MultiFabs to determine the direction.
    //! It is expected that one direction is cell-centered and the rest are nodal.
    void average_down_edges (const MultiFab& fine, MultiFab& crse,
                             const IntVect& ratio, int ngcrse=0);

    //! Average fine node-based MultiFab onto crse node-centered MultiFab.
    template <typename FAB>
    void average_down_nodal (const FabArray<FAB>& S_fine,
                             FabArray<FAB>& S_crse,
                             const IntVect& ratio,
                             int ngcrse = 0,
                             bool mfiter_is_definitely_safe=false);

    //! Average fine cell-based MultiFab onto crse cell-centered MultiFab using
    //! volume-weighting. This routine DOES NOT assume that the crse BoxArray is
    //! a coarsened version of the fine BoxArray.
    void average_down (const MultiFab& S_fine, MultiFab& S_crse,
                       const Geometry& fine_geom, const Geometry& crse_geom,
                       int scomp, int ncomp, const IntVect& ratio);
    void average_down (const MultiFab& S_fine, MultiFab& S_crse,
                       const Geometry& fine_geom, const Geometry& crse_geom,
                       int scomp, int ncomp, const int ratio);

    //! Average MultiFab onto crse MultiFab without volume weighting. This
    //! routine DOES NOT assume that the crse BoxArray is a coarsened version of
    //! the fine BoxArray. Work for both cell-centered and nodal MultiFabs.
    template<typename FAB>
    void average_down (const FabArray<FAB>& S_fine, FabArray<FAB>& S_crse,
                       int scomp, int ncomp, const IntVect& ratio);
    template<typename FAB>
    void average_down (const FabArray<FAB>& S_fine, FabArray<FAB>& S_crse,
                       int scomp, int ncomp, int ratio);

    //! Add a coarsened version of the data contained in the S_fine MultiFab to
    //! S_crse, including ghost cells.
    void sum_fine_to_coarse (const MultiFab& S_Fine, MultiFab& S_crse,
                             int scomp, int ncomp,
                             const IntVect& ratio,
                             const Geometry& cgeom, const Geometry& fgeom);

    //! Output state data for a single zone
    void print_state (const MultiFab& mf, const IntVect& cell, const int n=-1,
                      const IntVect& ng = IntVect::TheZeroVector());

    //! Write each fab individually
    void writeFabs (const MultiFab& mf, const std::string& name);
    void writeFabs (const MultiFab& mf, int comp, int ncomp, const std::string& name);

    //! Extract a slice from the given cell-centered MultiFab at coordinate
    //! "coord" along direction "dir".
    std::unique_ptr<MultiFab> get_slice_data(int dir, Real coord,
                                             const MultiFab& cc,
                                             const Geometry& geom, int fstart, int ncomp,
                                             bool interpolate=false);

    //! Return an iMultiFab that has the same BoxArray and DistributionMapping
    //! as the coarse MultiFab cmf. Cells covered by the coarsened fine grids
    //! are set to fine_value, whereas other cells are set to crse_value.
    template <typename FAB>
    iMultiFab makeFineMask (const FabArray<FAB>& cmf, const BoxArray& fba, const IntVect& ratio,
                            int crse_value = 0, int fine_value = 1);
    iMultiFab makeFineMask (const BoxArray& cba, const DistributionMapping& cdm,
                            const BoxArray& fba, const IntVect& ratio,
                            int crse_value = 0, int fine_value = 1);
    template <typename FAB>
    iMultiFab makeFineMask (const FabArray<FAB>& cmf, const BoxArray& fba, const IntVect& ratio,
                            Periodicity const& period, int crse_value, int fine_value);
    iMultiFab makeFineMask (const BoxArray& cba, const DistributionMapping& cdm,
                            const IntVect& cnghost, const BoxArray& fba, const IntVect& ratio,
                            Periodicity const& period, int crse_value, int fine_value);
    template <typename FAB>
    iMultiFab makeFineMask (const FabArray<FAB>& cmf, const FabArray<FAB>& fmf,
                            const IntVect& cnghost, const IntVect& ratio,
                            Periodicity const& period, int crse_value, int fine_value);
    template <typename FAB>
    iMultiFab makeFineMask (const FabArray<FAB>& cmf, const FabArray<FAB>& fmf,
                            const IntVect& cnghost, const IntVect& ratio,
                            Periodicity const& period, int crse_value, int fine_value,
                            LayoutData<int>& has_cf);

    MultiFab makeFineMask (const BoxArray& cba, const DistributionMapping& cdm,
                           const BoxArray& fba, const IntVect& ratio,
                           Real crse_value, Real fine_value);

    //! Computes divergence of face-data stored in the umac MultiFab.
    void computeDivergence (MultiFab& divu, const Array<MultiFab const*,AMREX_SPACEDIM>& umac,
                            const Geometry& geom);

    //! Computes gradient of face-data stored in the umac MultiFab.
    void computeGradient (MultiFab& grad, const Array<MultiFab const*,AMREX_SPACEDIM>& umac,
                          const Geometry& geom);

    //! Convert iMultiFab to MultiFab
    MultiFab ToMultiFab (const iMultiFab& imf);
    //! Convert iMultiFab to Long
    FabArray<BaseFab<Long> > ToLongMultiFab (const iMultiFab& imf);

    //! Periodic shift MultiFab
    MultiFab periodicShift (MultiFab const& mf, IntVect const& offset,
                            Periodicity const& period);

    //! example: auto mf = amrex::cast<MultiFab>(imf);
    template <typename T, typename U>
    T cast (U const& mf_in)
    {
        T mf_out(mf_in.boxArray(), mf_in.DistributionMap(), mf_in.nComp(), mf_in.nGrowVect());

#ifdef AMREX_USE_OMP
#pragma omp parallel if (Gpu::notInLaunchRegion())
#endif
        for (MFIter mfi(mf_in); mfi.isValid(); ++mfi)
        {
            const Long n = mfi.fabbox().numPts() * mf_in.nComp();
            auto pdst = mf_out[mfi].dataPtr();
            auto psrc = mf_in [mfi].dataPtr();
            AMREX_HOST_DEVICE_PARALLEL_FOR_1D ( n, i,
            {
                pdst[i] = static_cast<typename U::value_type>(psrc[i]);
            });
        }
        return mf_out;
    }

    /**
     * \brief Sum MultiFab data to line
     *
     * Return a HostVector that contains the sum of the given MultiFab data in the plane
     * with the given normal direction.  The size of the vector is
     * domain.length(direction) x ncomp.  The vector is actually a 2D array, where the
     * element for component icomp at spatial index k is at [icomp*ncomp+k].
     *
     * \param mf MultiFab data for summing
     * \param icomp starting component
     * \param ncomp number of components
     * \param domain the domain
     * \param direction the direction of the line
     * \param local If false, reduce across MPI processes.
     */
    Gpu::HostVector<Real> sumToLine (MultiFab const& mf, int icomp, int ncomp,
                                     Box const& domain, int direction, bool local = false);

    /** \brief Volume weighted sum for a vector of MultiFabs
     *
     * Return a volume weighted sum of MultiFabs of AMR data.  The sum is
     * perform on a single component of the data.  If the MultiFabs are
     * built with EB Factories, the cut cell volume fraction will be
     * included in the weight.
     */
    Real volumeWeightedSum (Vector<MultiFab const*> const& mf, int icomp,
                            Vector<Geometry> const& geom,
                            Vector<IntVect> const& ratio,
                            bool local = false);

    /**
     * \brief Fourth-order interpolation from fine to coarse level.
     *
     * This is for high-order "average-down" of finite-difference data.  If
     * ghost cell data are used, it's the caller's responsibility to fill
     * the ghost cells before calling this function.
     *
     * \param cmf   coarse data
     * \param scomp starting component
     * \param ncomp number of component
     * \param fmf   fine data
     * \param ratio refinement ratio.
     */
    void FourthOrderInterpFromFineToCoarse (MultiFab& cmf, int scomp, int ncomp,
                                            MultiFab const& fmf,
                                            IntVect const& ratio);
}

namespace amrex {

template <typename FAB>
iMultiFab
makeFineMask (const FabArray<FAB>& cmf, const BoxArray& fba, const IntVect& ratio,
              int crse_value, int fine_value)
{
    return makeFineMask(cmf.boxArray(), cmf.DistributionMap(), cmf.nGrowVect(),
                        fba, ratio, Periodicity::NonPeriodic(), crse_value, fine_value);
}

template <typename FAB>
iMultiFab
makeFineMask (const FabArray<FAB>& cmf, const BoxArray& fba, const IntVect& ratio,
              Periodicity const& period, int crse_value, int fine_value)
{
    return makeFineMask(cmf.boxArray(), cmf.DistributionMap(), cmf.nGrowVect(),
                        fba, ratio, period, crse_value, fine_value);
}

template <typename FAB>
iMultiFab
makeFineMask (const FabArray<FAB>& cmf, const FabArray<FAB>& fmf,
              const IntVect& cnghost, const IntVect& ratio,
              Periodicity const& period, int crse_value, int fine_value)
{
    iMultiFab mask(cmf.boxArray(), cmf.DistributionMap(), 1, cnghost);
    mask.setVal(crse_value);

    iMultiFab foo(amrex::coarsen(fmf.boxArray(),ratio), fmf.DistributionMap(),
                  1, 0, MFInfo().SetAlloc(false));
    const FabArrayBase::CPC& cpc = mask.getCPC(cnghost,foo,IntVect::TheZeroVector(),period);
    mask.setVal(fine_value, cpc, 0, 1);

    return mask;
}

template <typename FAB>
iMultiFab
makeFineMask (const FabArray<FAB>& cmf, const FabArray<FAB>& fmf,
              const IntVect& cnghost, const IntVect& ratio,
              Periodicity const& period, int crse_value, int fine_value,
              LayoutData<int>& has_cf)
{
    iMultiFab mask(cmf.boxArray(), cmf.DistributionMap(), 1, cnghost);
    mask.setVal(crse_value);

    iMultiFab foo(amrex::coarsen(fmf.boxArray(),ratio), fmf.DistributionMap(),
                  1, 0, MFInfo().SetAlloc(false));
    const FabArrayBase::CPC& cpc = mask.getCPC(cnghost,foo,IntVect::TheZeroVector(),period);
    mask.setVal(fine_value, cpc, 0, 1);

    has_cf = mask.RecvLayoutMask(cpc);

    return mask;
}

//! Average fine node-based MultiFab onto crse node-based MultiFab.
//! This routine assumes that the crse BoxArray is a coarsened version of the fine BoxArray.
template <typename FAB>
void average_down_nodal (const FabArray<FAB>& fine, FabArray<FAB>& crse,
                         const IntVect& ratio, int ngcrse, bool mfiter_is_definitely_safe)
{
    AMREX_ASSERT(fine.is_nodal());
    AMREX_ASSERT(crse.is_nodal());
    AMREX_ASSERT(crse.nComp() == fine.nComp());

    int ncomp = crse.nComp();
    using value_type = typename FAB::value_type;

    if (mfiter_is_definitely_safe || isMFIterSafe(fine, crse))
    {
#ifdef AMREX_USE_OMP
#pragma omp parallel if (Gpu::notInLaunchRegion())
#endif
        for (MFIter mfi(crse,TilingIfNotGPU()); mfi.isValid(); ++mfi)
        {
            const Box& bx = mfi.growntilebox(ngcrse);
            Array4<value_type> const& crsearr = crse.array(mfi);
            Array4<value_type const> const& finearr = fine.const_array(mfi);

            AMREX_LAUNCH_HOST_DEVICE_LAMBDA ( bx, tbx,
            {
                amrex_avgdown_nodes(tbx,crsearr,finearr,0,0,ncomp,ratio);
            });
        }
    }
    else
    {
        FabArray<FAB> ctmp(amrex::coarsen(fine.boxArray(),ratio), fine.DistributionMap(),
                           ncomp, ngcrse);
        average_down_nodal(fine, ctmp, ratio, ngcrse);
        crse.ParallelCopy(ctmp,0,0,ncomp,ngcrse,ngcrse);
    }
}

// *************************************************************************************************************

// Average fine cell-based MultiFab onto crse cell-centered MultiFab.
// We do NOT assume that the coarse layout is a coarsened version of the fine layout.
// This version does NOT use volume-weighting
template<typename FAB>
void average_down (const FabArray<FAB>& S_fine, FabArray<FAB>& S_crse, int scomp, int ncomp, int rr)
{
    average_down(S_fine,S_crse,scomp,ncomp,rr*IntVect::TheUnitVector());
}

template<typename FAB>
void average_down (const FabArray<FAB>& S_fine, FabArray<FAB>& S_crse,
                   int scomp, int ncomp, const IntVect& ratio)
{
    BL_PROFILE("amrex::average_down");
    AMREX_ASSERT(S_crse.nComp() == S_fine.nComp());
    AMREX_ASSERT((S_crse.is_cell_centered() && S_fine.is_cell_centered()) ||
                 (S_crse.is_nodal()         && S_fine.is_nodal()));

    using value_type = typename FAB::value_type;

    bool is_cell_centered = S_crse.is_cell_centered();

    //
    // Coarsen() the fine stuff on processors owning the fine data.
    //
    BoxArray crse_S_fine_BA = S_fine.boxArray(); crse_S_fine_BA.coarsen(ratio);

    if (crse_S_fine_BA == S_crse.boxArray() && S_fine.DistributionMap() == S_crse.DistributionMap())
    {
#ifdef AMREX_USE_GPU
        if (Gpu::inLaunchRegion() && S_crse.isFusingCandidate()) {
            auto const& crsema = S_crse.arrays();
            auto const& finema = S_fine.const_arrays();
            if (is_cell_centered) {
                ParallelFor(S_crse, IntVect(0), ncomp,
                            [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k, int n) noexcept
                            {
                                amrex_avgdown(i,j,k,n,crsema[box_no],finema[box_no],scomp,scomp,ratio);
                            });
            } else {
                ParallelFor(S_crse, IntVect(0), ncomp,
                            [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k, int n) noexcept
                            {
                                amrex_avgdown_nodes(i,j,k,n,crsema[box_no],finema[box_no],scomp,scomp,ratio);
                            });
            }
            Gpu::streamSynchronize();
        } else
#endif
        {
#ifdef AMREX_USE_OMP
#pragma omp parallel if (Gpu::notInLaunchRegion())
#endif
            for (MFIter mfi(S_crse,TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                //  NOTE: The tilebox is defined at the coarse level.
                const Box& bx = mfi.tilebox();
                Array4<value_type> const& crsearr = S_crse.array(mfi);
                Array4<value_type const> const& finearr = S_fine.const_array(mfi);

                if (is_cell_centered) {
                    AMREX_HOST_DEVICE_PARALLEL_FOR_4D(bx, ncomp, i, j, k, n,
                                                      {
                                                          amrex_avgdown(i,j,k,n,crsearr,finearr,scomp,scomp,ratio);
                                                      });
                } else {
                    AMREX_HOST_DEVICE_PARALLEL_FOR_4D(bx, ncomp, i, j, k, n,
                                                      {
                                                          amrex_avgdown_nodes(i,j,k,n,crsearr,finearr,scomp,scomp,ratio);
                                                      });
                }
            }
        }
    }
    else
    {
        FabArray<FAB> crse_S_fine(crse_S_fine_BA, S_fine.DistributionMap(), ncomp, 0, MFInfo(),DefaultFabFactory<FAB>());

#ifdef AMREX_USE_GPU
        if (Gpu::inLaunchRegion() && crse_S_fine.isFusingCandidate()) {
            auto const& crsema = crse_S_fine.arrays();
            auto const& finema = S_fine.const_arrays();
            if (is_cell_centered) {
                ParallelFor(crse_S_fine, IntVect(0), ncomp,
                            [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k, int n) noexcept
                            {
                                amrex_avgdown(i,j,k,n,crsema[box_no],finema[box_no],0,scomp,ratio);
                            });
            } else {
                ParallelFor(crse_S_fine, IntVect(0), ncomp,
                            [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k, int n) noexcept
                            {
                                amrex_avgdown_nodes(i,j,k,n,crsema[box_no],finema[box_no],0,scomp,ratio);
                            });
            }
            Gpu::streamSynchronize();
        } else
#endif
        {
#ifdef AMREX_USE_OMP
#pragma omp parallel if (Gpu::notInLaunchRegion())
#endif
            for (MFIter mfi(crse_S_fine,TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                //  NOTE: The tilebox is defined at the coarse level.
                const Box& bx = mfi.tilebox();
                Array4<value_type> const& crsearr = crse_S_fine.array(mfi);
                Array4<value_type const> const& finearr = S_fine.const_array(mfi);

                //  NOTE: We copy from component scomp of the fine fab into component 0 of the crse fab
                //        because the crse fab is a temporary which was made starting at comp 0, it is
                //        not part of the actual crse multifab which came in.

                if (is_cell_centered) {
                    AMREX_HOST_DEVICE_PARALLEL_FOR_4D(bx, ncomp, i, j, k, n,
                                                      {
                                                          amrex_avgdown(i,j,k,n,crsearr,finearr,0,scomp,ratio);
                                                      });
                } else {
                    AMREX_HOST_DEVICE_PARALLEL_FOR_4D(bx, ncomp, i, j, k, n,
                                                      {
                                                          amrex_avgdown_nodes(i,j,k,n,crsearr,finearr,0,scomp,ratio);
                                                      });
                }
            }
        }

        S_crse.ParallelCopy(crse_S_fine,0,scomp,ncomp);
    }
}





   /**
    * \brief Returns part of a norm based on two MultiFabs
    * The MultiFabs MUST have the same underlying BoxArray.
    * The function f is applied elementwise as f(x(i,j,k,n),y(i,j,k,n))
    * inside the summation (subject to a valid mask entry pf(mask(i,j,k,n)
    */

template <typename F>
Real
NormHelper (const MultiFab& x, int xcomp,
            const MultiFab& y, int ycomp,
            F && f,
            int numcomp, IntVect nghost, bool local)
{
    BL_ASSERT(x.boxArray() == y.boxArray());
    BL_ASSERT(x.DistributionMap() == y.DistributionMap());
    BL_ASSERT(x.nGrowVect().allGE(nghost) && y.nGrowVect().allGE(nghost));

    Real sm = Real(0.0);
#ifdef AMREX_USE_GPU
    if (Gpu::inLaunchRegion()) {
        auto const& xma = x.const_arrays();
        auto const& yma = y.const_arrays();
        sm = ParReduce(TypeList<ReduceOpSum>{}, TypeList<Real>{}, x, nghost,
        [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k) noexcept -> GpuTuple<Real>
        {
            Real t = Real(0.0);
            auto const& xfab = xma[box_no];
            auto const& yfab = yma[box_no];
            for (int n = 0; n < numcomp; ++n) {
                t += f(xfab(i,j,k,xcomp+n) , yfab(i,j,k,ycomp+n));
            }
            return t;
        });
    } else
#endif
    {
#ifdef AMREX_USE_OMP
#pragma omp parallel if (!system::regtest_reduction) reduction(+:sm)
#endif
        for (MFIter mfi(x,true); mfi.isValid(); ++mfi)
        {
            Box const& bx = mfi.growntilebox(nghost);
            Array4<Real const> const& xfab = x.const_array(mfi);
            Array4<Real const> const& yfab = y.const_array(mfi);
            AMREX_LOOP_4D(bx, numcomp, i, j, k, n,
            {
                sm += f(xfab(i,j,k,xcomp+n) , yfab(i,j,k,ycomp+n));
            });
        }
    }

    if (!local) {
        ParallelAllReduce::Sum(sm, ParallelContext::CommunicatorSub());
    }

    return sm;
}

   /**
    * \brief Returns part of a norm based on three MultiFabs
    * The MultiFabs MUST have the same underlying BoxArray.
    * The Predicate pf is used to test the mask
    * The function f is applied elementwise as f(x(i,j,k,n),y(i,j,k,n))
    * inside the summation (subject to a valid mask entry pf(mask(i,j,k,n)
    */

template <typename MMF, typename Pred, typename F>
Real
NormHelper (const MMF& mask,
               const MultiFab& x, int xcomp,
               const MultiFab& y, int ycomp,
               Pred && pf,
               F && f,
               int numcomp, IntVect nghost, bool local)
{
    BL_ASSERT(x.boxArray() == y.boxArray());
    BL_ASSERT(x.boxArray() == mask.boxArray());
    BL_ASSERT(x.DistributionMap() == y.DistributionMap());
    BL_ASSERT(x.DistributionMap() == mask.DistributionMap());
    BL_ASSERT(x.nGrowVect().allGE(nghost) && y.nGrowVect().allGE(nghost));
    BL_ASSERT(mask.nGrowVect().allGE(nghost));

    Real sm = Real(0.0);
#ifdef AMREX_USE_GPU
    if (Gpu::inLaunchRegion()) {
        auto const& xma = x.const_arrays();
        auto const& yma = y.const_arrays();
        auto const& mma = mask.const_arrays();
        sm = ParReduce(TypeList<ReduceOpSum>{}, TypeList<Real>{}, x, nghost,
        [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k) noexcept -> GpuTuple<Real>
        {
            Real t = Real(0.0);
            if (pf(mma[box_no](i,j,k))) {
                auto const& xfab = xma[box_no];
                auto const& yfab = yma[box_no];
                for (int n = 0; n < numcomp; ++n) {
                    t += f(xfab(i,j,k,xcomp+n) , yfab(i,j,k,ycomp+n));
                }
            }
            return t;
        });
    } else
#endif
    {
#ifdef AMREX_USE_OMP
#pragma omp parallel if (!system::regtest_reduction) reduction(+:sm)
#endif
        for (MFIter mfi(x,true); mfi.isValid(); ++mfi)
        {
            Box const& bx = mfi.growntilebox(nghost);
            Array4<Real const> const& xfab = x.const_array(mfi);
            Array4<Real const> const& yfab = y.const_array(mfi);
            auto const& mfab = mask.const_array(mfi);
            AMREX_LOOP_4D(bx, numcomp, i, j, k, n,
            {
                if (pf(mfab(i,j,k))) {
                    sm += f(xfab(i,j,k,xcomp+n) , yfab(i,j,k,ycomp+n));
                }
            });
        }
    }

    if (!local) {
        ParallelAllReduce::Sum(sm, ParallelContext::CommunicatorSub());
    }

    return sm;
}

template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int>>
void average_down_faces (const Vector<const MF*>& fine,
                         const Vector<MF*>& crse,
                         const IntVect& ratio, int ngcrse)
{
    AMREX_ASSERT(fine.size() == AMREX_SPACEDIM && crse.size() == AMREX_SPACEDIM);
    average_down_faces(Array<const MF*,AMREX_SPACEDIM>
                               {{AMREX_D_DECL(fine[0],fine[1],fine[2])}},
                       Array<MF*,AMREX_SPACEDIM>
                               {{AMREX_D_DECL(crse[0],crse[1],crse[2])}},
                       ratio, ngcrse);
}

template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int>>
void average_down_faces (const Vector<const MF*>& fine,
                         const Vector<MF*>& crse, int ratio, int ngcrse)
{
    average_down_faces(fine,crse,IntVect{ratio},ngcrse);
}

template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int>>
void average_down_faces (const Array<const MF*,AMREX_SPACEDIM>& fine,
                         const Array<MF*,AMREX_SPACEDIM>& crse,
                         int ratio, int ngcrse)
{
    average_down_faces(fine,crse,IntVect{ratio},ngcrse);
}

template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int>>
void average_down_faces (const Array<const MF*,AMREX_SPACEDIM>& fine,
                         const Array<MF*,AMREX_SPACEDIM>& crse,
                         const IntVect& ratio, int ngcrse)
{
    for (int idim = 0; idim < AMREX_SPACEDIM; ++idim) {
        average_down_faces(*fine[idim], *crse[idim], ratio, ngcrse);
    }
}

template <typename FAB>
void average_down_faces (const FabArray<FAB>& fine, FabArray<FAB>& crse,
                         const IntVect& ratio, int ngcrse)
{
    BL_PROFILE("average_down_faces");

    AMREX_ASSERT(crse.nComp() == fine.nComp());
    AMREX_ASSERT(fine.ixType() == crse.ixType());
    const auto type = fine.ixType();
    int dir;
    for (dir = 0; dir < AMREX_SPACEDIM; ++dir) {
        if (type.nodeCentered(dir)) break;
    }
    auto tmptype = type;
    tmptype.unset(dir);
    if (dir >= AMREX_SPACEDIM || !tmptype.cellCentered()) {
        amrex::Abort("average_down_faces: not face index type");
    }
    const int ncomp = crse.nComp();
    if (isMFIterSafe(fine, crse))
    {
#ifdef AMREX_USE_GPU
        if (Gpu::inLaunchRegion() && crse.isFusingCandidate()) {
            auto const& crsema = crse.arrays();
            auto const& finema = fine.const_arrays();
            ParallelFor(crse, IntVect(ngcrse), ncomp,
            [=] AMREX_GPU_DEVICE (int box_no, int i, int j, int k, int n) noexcept
            {
                amrex_avgdown_faces(i,j,k,n, crsema[box_no], finema[box_no], 0, 0, ratio, dir);
            });
            Gpu::streamSynchronize();
        } else
#endif
        {
#ifdef AMREX_USE_OMP
#pragma omp parallel if (Gpu::notInLaunchRegion())
#endif
            for (MFIter mfi(crse,TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                const Box& bx = mfi.growntilebox(ngcrse);
                auto const& crsearr = crse.array(mfi);
                auto const& finearr = fine.const_array(mfi);
                AMREX_HOST_DEVICE_PARALLEL_FOR_4D(bx, ncomp, i, j, k, n,
                {
                    amrex_avgdown_faces(i,j,k,n, crsearr, finearr, 0, 0, ratio, dir);
                });
            }
        }
    }
    else
    {
        FabArray<FAB> ctmp(amrex::coarsen(fine.boxArray(),ratio), fine.DistributionMap(),
                      ncomp, ngcrse, MFInfo(), DefaultFabFactory<FAB>());
        average_down_faces(fine, ctmp, ratio, ngcrse);
        crse.ParallelCopy(ctmp,0,0,ncomp,ngcrse,ngcrse);
    }
}

template <typename MF, std::enable_if_t<IsFabArray<MF>::value,int>>
void average_down_faces (const Array<const MF*,AMREX_SPACEDIM>& fine,
                         const Array<MF*,AMREX_SPACEDIM>& crse,
                         const IntVect& ratio, const Geometry& crse_geom)
{
    for (int idim = 0; idim < AMREX_SPACEDIM; ++idim) {
        average_down_faces(*fine[idim], *crse[idim], ratio, crse_geom);
    }
}

template <typename FAB>
void average_down_faces (const FabArray<FAB>& fine, FabArray<FAB>& crse,
                         const IntVect& ratio, const Geometry& crse_geom)
{
    FabArray<FAB> ctmp(amrex::coarsen(fine.boxArray(),ratio), fine.DistributionMap(),
                  crse.nComp(), 0);
    average_down_faces(fine, ctmp, ratio, 0);
    crse.ParallelCopy(ctmp,0,0,crse.nComp(),0,0,crse_geom.periodicity());
}

}

#endif
