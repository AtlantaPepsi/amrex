#ifndef AMREX_ARRAY4_H_
#define AMREX_ARRAY4_H_
#include <AMReX_Config.H>

#include <AMReX.H>
#include <AMReX_IntVect.H>
#include <AMReX_GpuPrint.H>

#include <iostream>
#include <sstream>

namespace amrex {

    template <typename T>
    struct CellData // Data in a single cell
    {
        T* AMREX_RESTRICT p = nullptr;
        Long stride = 0;
        int ncomp = 0;

        AMREX_GPU_HOST_DEVICE
        constexpr CellData (T* a_p, Long a_stride, int a_ncomp)
            : p(a_p), stride(a_stride), ncomp(a_ncomp)
            {}

        template <class U=T,
                  std::enable_if_t<std::is_const<U>::value,int> = 0>
        AMREX_GPU_HOST_DEVICE
        constexpr CellData (CellData<typename std::remove_const<T>::type> const& rhs) noexcept
            : p(rhs.p), stride(rhs.stride), ncomp(rhs.ncomp)
            {}

        AMREX_GPU_HOST_DEVICE
        explicit operator bool() const noexcept { return p != nullptr; }

        AMREX_GPU_HOST_DEVICE
        int nComp() const noexcept { return ncomp; }

        template <class U=T,
                  std::enable_if_t<!std::is_void<U>::value,int> = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        U& operator[] (int n) const noexcept {
#if defined(AMREX_DEBUG) || defined(AMREX_BOUND_CHECK)
            if (n < 0 || n >= ncomp) {
#if AMREX_DEVICE_COMPILE
                AMREX_DEVICE_PRINTF(" %d is out of bound (0:%d)", n, ncomp-1);
#else
                std::stringstream ss;
                ss << " " << n << " is out of bound: (0:" << ncomp-1 << ")";
                amrex::Abort(ss.str());
#endif
            }
#endif
            return p[n*stride];
        }
    };

    template <typename T>
    struct Array4
    {
        T* AMREX_RESTRICT p;
        Long jstride = 0;
        Long kstride = 0;
        Long nstride = 0;
        Dim3 begin{1,1,1};
        Dim3 end{0,0,0};  // end is hi + 1
        int  ncomp=0;

        AMREX_GPU_HOST_DEVICE
        constexpr Array4 () noexcept : p(nullptr) {}

        template <class U=T, typename std::enable_if<std::is_const<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE
        constexpr Array4 (Array4<typename std::remove_const<T>::type> const& rhs) noexcept
            : p(rhs.p),
              jstride(rhs.jstride),
              kstride(rhs.kstride),
              nstride(rhs.nstride),
              begin(rhs.begin),
              end(rhs.end),
              ncomp(rhs.ncomp)
            {}

        AMREX_GPU_HOST_DEVICE
        constexpr Array4 (T* a_p, Dim3 const& a_begin, Dim3 const& a_end, int a_ncomp) noexcept
            : p(a_p),
              jstride(a_end.x-a_begin.x),
              kstride(jstride*(a_end.y-a_begin.y)),
              nstride(kstride*(a_end.z-a_begin.z)),
              begin(a_begin),
              end(a_end),
              ncomp(a_ncomp)
            {}

        template <class U,
                  typename std::enable_if
                  <std::is_same<typename std::remove_const<T>::type,
                                typename std::remove_const<U>::type>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE
        constexpr Array4 (Array4<U> const& rhs, int start_comp) noexcept
            : p((T*)(rhs.p+start_comp*rhs.nstride)),
              jstride(rhs.jstride),
              kstride(rhs.kstride),
              nstride(rhs.nstride),
              begin(rhs.begin),
              end(rhs.end),
              ncomp(rhs.ncomp-start_comp)
        {}

        template <class U,
                  typename std::enable_if
                  <std::is_same<typename std::remove_const<T>::type,
                                typename std::remove_const<U>::type>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE
        constexpr Array4 (Array4<U> const& rhs, int start_comp, int num_comps) noexcept
            : p((T*)(rhs.p+start_comp*rhs.nstride)),
              jstride(rhs.jstride),
              kstride(rhs.kstride),
              nstride(rhs.nstride),
              begin(rhs.begin),
              end(rhs.end),
              ncomp(num_comps)
        {}

        AMREX_GPU_HOST_DEVICE
        explicit operator bool() const noexcept { return p != nullptr; }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        U& operator() (int i, int j, int k) const noexcept {
#if defined(AMREX_DEBUG) || defined(AMREX_BOUND_CHECK)
            index_assert(i,j,k,0);
#endif
            return p[(i-begin.x)+(j-begin.y)*jstride+(k-begin.z)*kstride];
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        U& operator() (int i, int j, int k, int n) const noexcept {
#if defined(AMREX_DEBUG) || defined(AMREX_BOUND_CHECK)
            index_assert(i,j,k,n);
#endif
            return p[(i-begin.x)+(j-begin.y)*jstride+(k-begin.z)*kstride+n*nstride];
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        T* ptr (int i, int j, int k) const noexcept {
#if defined(AMREX_DEBUG) || defined(AMREX_BOUND_CHECK)
            index_assert(i,j,k,0);
#endif
            return p + ((i-begin.x)+(j-begin.y)*jstride+(k-begin.z)*kstride);
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        T* ptr (int i, int j, int k, int n) const noexcept {
#if defined(AMREX_DEBUG) || defined(AMREX_BOUND_CHECK)
            index_assert(i,j,k,0);
#endif
            return p + ((i-begin.x)+(j-begin.y)*jstride+(k-begin.z)*kstride+n*nstride);
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        U& operator() (IntVect const& iv) const noexcept {
#if (AMREX_SPACEDIM == 1)
            return this->operator()(iv[0],0,0);
#elif (AMREX_SPACEDIM == 2)
            return this->operator()(iv[0],iv[1],0);
#else
            return this->operator()(iv[0],iv[1],iv[2]);
#endif
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        U& operator() (IntVect const& iv, int n) const noexcept {
#if (AMREX_SPACEDIM == 1)
            return this->operator()(iv[0],0,0,n);
#elif (AMREX_SPACEDIM == 2)
            return this->operator()(iv[0],iv[1],0,n);
#else
            return this->operator()(iv[0],iv[1],iv[2],n);
#endif
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        T* ptr (IntVect const& iv) const noexcept {
#if (AMREX_SPACEDIM == 1)
            return this->ptr(iv[0],0,0);
#elif (AMREX_SPACEDIM == 2)
            return this->ptr(iv[0],iv[1],0);
#else
            return this->ptr(iv[0],iv[1],iv[2]);
#endif
        }

        template <class U=T, typename std::enable_if<!std::is_void<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        T* ptr (IntVect const& iv, int n) const noexcept {
#if (AMREX_SPACEDIM == 1)
            return this->ptr(iv[0],0,0,n);
#elif (AMREX_SPACEDIM == 2)
            return this->ptr(iv[0],iv[1],0,n);
#else
            return this->ptr(iv[0],iv[1],iv[2],n);
#endif
        }

        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        T* dataPtr () const noexcept {
            return this->p;
        }

        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        std::size_t size () const noexcept {
            return this->nstride * this->ncomp;
        }

        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        int nComp () const noexcept { return ncomp; }

        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        bool contains (int i, int j, int k) const noexcept {
            return (i>=begin.x && i<end.x && j>=begin.y && j<end.y && k>=begin.z && k<end.z);
        }

#if defined(AMREX_DEBUG) || defined(AMREX_BOUND_CHECK)
        AMREX_GPU_HOST_DEVICE inline
        void index_assert (int i, int j, int k, int n) const
        {
            if (i<begin.x || i>=end.x || j<begin.y || j>=end.y || k<begin.z || k>=end.z
                || n < 0 || n >= ncomp) {
#if AMREX_DEVICE_COMPILE
                AMREX_DEVICE_PRINTF(" (%d,%d,%d,%d) is out of bound (%d:%d,%d:%d,%d:%d,0:%d)\n",
                                    i, j, k, n, begin.x, end.x-1, begin.y, end.y-1,
                                    begin.z, end.z-1, ncomp-1);
                amrex::Abort();
#else
                std::stringstream ss;
                ss << " (" << i << "," << j << "," << k << "," <<  n
                   << ") is out of bound ("
                   << begin.x << ":" << end.x-1 << ","
                   << begin.y << ":" << end.y-1 << ","
                   << begin.z << ":" << end.z-1 << ","
                   << "0:" << ncomp-1 << ")";
                amrex::Abort(ss.str());
#endif
            }
        }
#endif

        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        CellData<T> cellData (int i, int j, int k) const noexcept {
            return CellData<T>{this->ptr(i,j,k), nstride, ncomp};
        }
    };

    template <class Tto, class Tfrom>
    AMREX_GPU_HOST_DEVICE
    Array4<Tto> ToArray4 (Array4<Tfrom> const& a_in) noexcept
    {
        return Array4<Tto>((Tto*)(a_in.p), a_in.begin, a_in.end, a_in.ncomp);
    }

    template <class T>
    AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
    Dim3 lbound (Array4<T> const& a) noexcept
    {
        return a.begin;
    }

    template <class T>
    AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
    Dim3 ubound (Array4<T> const& a) noexcept
    {
        return Dim3{a.end.x-1,a.end.y-1,a.end.z-1};
    }

    template <class T>
    AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
    Dim3 length (Array4<T> const& a) noexcept
    {
        return Dim3{a.end.x-a.begin.x,a.end.y-a.begin.y,a.end.z-a.begin.z};
    }

    template <typename T>
    std::ostream& operator<< (std::ostream& os, const Array4<T>& a) {
        os << "((" << lbound(a) << ',' << ubound(a) << ")," << a.ncomp << ')';
        return os;
    }

    //
    // Type traits for detecting if a class has a size() constexpr function.
    //
    template <class A, class Enable = void> struct HasMultiComp : std::false_type {};
    //
    template <class B>
    struct HasMultiComp<B, typename std::enable_if<B().size() >= 1>::type>
        : std::true_type {};

    //
    // PolymorphicArray4 can be used to access both AoS and SoA with
    // (i,j,k,n).  Here SoA refers multi-component BaseFab, and AoS refers
    // to single-component BaseFab of multi-component GpuArray.
    //
    template <typename T>
    struct PolymorphicArray4
        : public Array4<T>
    {
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        PolymorphicArray4 (Array4<T> const& a)
            : Array4<T>{a} {}

        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        T& operator() (int i, int j, int k) const noexcept {
            return this->Array4<T>::operator()(i,j,k);
        }

        template <class U=T, typename std::enable_if< amrex::HasMultiComp<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        typename U::reference_type
        operator() (int i, int j, int k, int n) const noexcept {
            return this->Array4<T>::operator()(i,j,k,0)[n];
        }

        template <class U=T, typename std::enable_if<!amrex::HasMultiComp<U>::value,int>::type = 0>
        AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
        U& operator() (int i, int j, int k, int n) const noexcept {
            return this->Array4<T>::operator()(i,j,k,n);
        }
    };

    template <typename T>
    PolymorphicArray4<T>
    makePolymorphic (Array4<T> const& a)
    {
        return PolymorphicArray4<T>(a);
    }
}

#endif
