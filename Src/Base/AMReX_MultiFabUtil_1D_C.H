#ifndef AMREX_MULTIFAB_UTIL_1D_C_H_
#define AMREX_MULTIFAB_UTIL_1D_C_H_
#include <AMReX_Config.H>

#include <AMReX_Gpu.H>
#include <AMReX_Geometry.H>
#include <AMReX_FArrayBox.H>
#include <AMReX_IArrayBox.H>
#include <cmath>

namespace amrex {

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avg_nd_to_cc (int i, int, int, int n,
                         Array4<Real      > const& cc,
                         Array4<Real const> const& nd,
                         int cccomp, int ndcomp) noexcept
{
    cc(i,0,0,n+cccomp) = Real(0.5)*(nd(i,0,0,n+ndcomp)+nd(i+1,0,0,n+ndcomp));
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avg_eg_to_cc (int i, int, int,
                         Array4<Real      > const& cc,
                         Array4<Real const> const& Ex,
                         int cccomp) noexcept
{
    cc(i,0,0,cccomp) = Ex(i,0,0);
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avg_fc_to_cc (int i, int, int,
                         Array4<Real      > const& cc,
                         Array4<Real const> const& fx,
                         int cccomp, GeometryData const& gd) noexcept
{
    const int coord_type = gd.Coord();

    switch (coord_type)
    {
    case 0:
    {
        cc(i,0,0,cccomp) = Real(0.5) * ( fx(i,0,0) + fx(i+1,0,0) );
        break;
    }
    case 1:
    {
        const Real problo = gd.ProbLo(0);
        const Real dx = gd.CellSize(0);
        Real rlo = problo + i*dx;
        Real rhi = problo + (i+1)*dx;
        Real rcen = Real(0.5)*(rlo+rhi);
        cc(i,0,0,cccomp) = Real(0.5) * ( rlo*fx(i,0,0) + rhi*fx(i+1,0,0) ) / rcen;
        break;
    }
    case 2:
    {
        const Real problo = gd.ProbLo(0);
        const Real dx = gd.CellSize(0);
        Real rlo = problo + i*dx;
        Real rhi = problo + (i+1)*dx;
        Real rcen = Real(0.5)*(rlo+rhi);
        cc(i,0,0,cccomp) = Real(0.5) * ( rlo*rlo*fx(i,0,0) + rhi*rhi*fx(i+1,0,0) ) / (rcen*rcen);
        break;
    }
    default:
        amrex::Abort("amrex_avg_fc_to_cc: wrong coord_type");
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avg_cc_to_fc (int i, int, int, int n, Box const& xbx,
                         Array4<Real> const& fx,
                         Array4<Real const> const& cc,
                         GeometryData const& gd, bool use_harmonic_averaging) noexcept
{
    if (xbx.contains(i,0,0)) {
        const int coord_type = gd.Coord();
        switch (coord_type)
        {
        case 0:
        {
            if (use_harmonic_averaging)
            {
                if (cc(i-1,0,0,n) == Real(0.0) || cc(i,0,0,n) == Real(0.0)) {
                    fx(i,0,0,n) = Real(0.0);
                } else {
                    fx(i,0,0,n) = Real(2.0) / ( Real(1.0) / cc(i-1,0,0,n) + Real(1.0) / cc(i,0,0,n) );
                }
            } else {
                fx(i,0,0,n) = Real(0.5)*(cc(i-1,0,0,n) + cc(i,0,0,n));
            }
            break;
        }
        case 1:
        {
            const Real problo = gd.ProbLo(0);
            const Real dx = gd.CellSize(0);

            Real rlo = problo + (i-Real(0.5))*dx;
            Real rhi = problo + (i+Real(0.5))*dx;
            Real rcen = Real(0.5)*(rlo+rhi);
            fx(i,0,0,n) = Real(0.5)*(rlo*cc(i-1,0,0,n) + rhi*cc(i,0,0,n)) / rcen;
            break;
        }
        case 2:
        {
            const Real problo = gd.ProbLo(0);
            const Real dx = gd.CellSize(0);

            Real rlo = problo + (i-Real(0.5))*dx;
            Real rhi = problo + (i+Real(0.5))*dx;
            Real rcen = Real(0.5)*(rlo+rhi);
            fx(i,0,0,n) = Real(0.5)*(rlo*rlo*cc(i-1,0,0,n) + rhi*rhi*cc(i,0,0,n)) / (rcen*rcen);
            break;
        }
        default:
            amrex::Abort("amrex_avg_cc_to_fc: wrong coord_type");
        };
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_faces (Box const& bx, Array4<T> const& crse,
                          Array4<T const> const& fine,
                          int ccomp, int fcomp, int ncomp,
                          IntVect const& ratio, int /*idir*/) noexcept
{
    const auto clo = lbound(bx);
    const auto chi = ubound(bx);

    const int facx = ratio[0];

    for (int n = 0; n < ncomp; ++n) {
        for (int i = clo.x; i <= chi.x; ++i) {
            crse(i,0,0,n+ccomp) = fine(facx*i,0,0,n+fcomp);
        }
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_faces (int i, int, int, int n, Array4<T> const& crse,
                          Array4<T const> const& fine,
                          int ccomp, int fcomp, IntVect const& ratio, int /*idir*/) noexcept
{
    crse(i,0,0,n+ccomp) = fine(ratio[0]*i,0,0,n+fcomp);
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_edges (Box const& bx, Array4<Real> const& crse,
                          Array4<Real const> const& fine,
                          int ccomp, int fcomp, int ncomp,
                          IntVect const& ratio, int /*idir*/) noexcept
{
    const auto clo = lbound(bx);
    const auto chi = ubound(bx);

    const int facx = ratio[0];
    Real facInv = Real(1.)/facx;

    for (int n = 0; n < ncomp; ++n) {
        for (int i = clo.x; i <= chi.x; ++i) {
            Real c = 0.;
            for (int iref = 0; iref < facx; ++iref) {
                c += fine(facx*i+iref,0,0,n+fcomp);
            }
            crse(i,0,0,n+ccomp) = c * facInv;
        }
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_edges (int i, int, int, int n, Array4<Real> const& crse,
                          Array4<Real const> const& fine,
                          int ccomp, int fcomp, IntVect const& ratio, int /*idir*/) noexcept
{
    const int facx = ratio[0];
    const Real facInv = Real(1.)/facx;
    Real c = 0.;
    for (int iref = 0; iref < facx; ++iref) {
        c += fine(facx*i+iref,0,0,n+fcomp);
    }
    crse(i,0,0,n+ccomp) = c * facInv;
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown (Box const& bx, Array4<T> const& crse,
                    Array4<T const> const& fine,
                    int ccomp, int fcomp, int ncomp,
                    IntVect const& ratio) noexcept
{
    const auto clo = lbound(bx);
    const auto chi = ubound(bx);

    const int facx = ratio[0];
    const T volfrac = T(1.0)/T(facx);

    for (int n = 0; n < ncomp; ++n) {
        for (int i = clo.x; i <= chi.x; ++i) {
            int ii = i*facx;
            T c = 0;
            for (int iref = 0; iref < facx; ++iref) {
                c += fine(ii+iref,0,0,n+fcomp);
            }
            crse(i,0,0,n+ccomp) = volfrac * c;
        }
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown (int i, int, int, int n, Array4<T> const& crse,
                    Array4<T const> const& fine,
                    int ccomp, int fcomp, IntVect const& ratio) noexcept
{
    const int facx = ratio[0];
    const T volfrac = T(1.0)/T(facx);
    const int ii = i*facx;
    T c = 0;
    for (int iref = 0; iref < facx; ++iref) {
        c += fine(ii+iref,0,0,n+fcomp);
    }
    crse(i,0,0,n+ccomp) = volfrac * c;
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_with_vol (int i, int, int, int n, Array4<T> const& crse,
                             Array4<T const> const& fine,
                             Array4<T const> const& fv,
                             int ccomp, int fcomp, IntVect const& ratio) noexcept
{
    const int facx = ratio[0];
    const int ii = i*facx;
    T cd = 0, cv = 0;
    for (int iref = 0; iref < facx; ++iref) {
        cv +=                           fv(ii+iref,0,0);
        cd += fine(ii+iref,0,0,fcomp+n)*fv(ii+iref,0,0);
    }
    crse(i,0,0,ccomp+n) = cd/cv;
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_nodes (Box const& bx, Array4<T> const& crse,
                          Array4<T const> const& fine,
                          int ccomp, int fcomp, int ncomp,
                          IntVect const& ratio) noexcept
{
    const auto clo = lbound(bx);
    const auto chi = ubound(bx);

    const int facx = ratio[0];

    for (int n = 0; n < ncomp; ++n) {
        AMREX_PRAGMA_SIMD
        for (int i = clo.x; i <= chi.x; ++i) {
            crse(i,0,0,n+ccomp) = fine(i*facx,0,0,n+fcomp);
        }
    }
}

template<typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void amrex_avgdown_nodes (int i, int, int, int n, Array4<T> const& crse,
                          Array4<T const> const& fine,
                          int ccomp, int fcomp, IntVect const& ratio) noexcept
{
    crse(i,0,0,n+ccomp) = fine(i*ratio[0],0,0,n+fcomp);
}

AMREX_GPU_HOST_DEVICE
inline
void amrex_compute_divergence (Box const& bx, Array4<Real> const& divu,
                               Array4<Real const> const& u,
                               GpuArray<Real,AMREX_SPACEDIM> const& dxinv) noexcept
{
    const auto lo = lbound(bx);
    const auto hi = ubound(bx);
    const Real dxi = dxinv[0];

    for     (int n = 0; n < divu.ncomp; ++n) {
        AMREX_PRAGMA_SIMD
        for (int i = lo.x; i <= hi.x; ++i) {
            divu(i,0,0,n) = dxi * (u(i+1,0,0,n)-u(i,0,0,n));
        }
    }
}

AMREX_GPU_HOST_DEVICE
inline
void amrex_compute_gradient (Box const& bx, Array4<Real> const& grad,
                             Array4<Real const> const& u,
                             GpuArray<Real,AMREX_SPACEDIM> const& dxinv) noexcept
{
    const auto lo = lbound(bx);
    const auto hi = ubound(bx);
    const Real dxi = dxinv[0];

    AMREX_PRAGMA_SIMD
    for (int i = lo.x; i <= hi.x; ++i) {
        grad(i,0,0) = dxi * (u(i+1,0,0)-u(i,0,0));
    }
}

}

#endif
