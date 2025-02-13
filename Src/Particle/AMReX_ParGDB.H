#ifndef AMREX_ParGDB_H_
#define AMREX_ParGDB_H_
#include <AMReX_Config.H>

#include <AMReX_Array.H>
#include <AMReX_Vector.H>
#include <AMReX_Geometry.H>
#include <AMReX_MultiFab.H>

namespace amrex {

class ParGDBBase
{
public:

    ParGDBBase () noexcept = default;
    virtual ~ParGDBBase () = default;
    ParGDBBase (ParGDBBase const&) noexcept = default;
    ParGDBBase (ParGDBBase &&) noexcept = default;
    ParGDBBase& operator= (ParGDBBase const&) noexcept = default;
    ParGDBBase& operator= (ParGDBBase &&) noexcept = default;

    [[nodiscard]] virtual const Geometry& ParticleGeom (int level) const = 0;
    [[nodiscard]] virtual const Geometry& Geom (int level) const = 0;

    [[nodiscard]] virtual const Vector<Geometry>& ParticleGeom () const = 0;
    [[nodiscard]] virtual const Vector<Geometry>&         Geom () const = 0;

    [[nodiscard]] virtual const DistributionMapping& ParticleDistributionMap (int level) const = 0;
    [[nodiscard]] virtual const DistributionMapping&         DistributionMap (int level) const = 0;

    [[nodiscard]] virtual const Vector<DistributionMapping>& ParticleDistributionMap () const = 0;
    [[nodiscard]] virtual const Vector<DistributionMapping>&         DistributionMap () const = 0;

    [[nodiscard]] virtual const BoxArray& ParticleBoxArray (int level) const = 0;
    [[nodiscard]] virtual const BoxArray&         boxArray (int level) const = 0;

    [[nodiscard]] virtual const Vector<BoxArray>& ParticleBoxArray () const = 0;
    [[nodiscard]] virtual const Vector<BoxArray>&         boxArray () const = 0;

    virtual void SetParticleBoxArray (int level, const BoxArray& new_ba) = 0;
    virtual void SetParticleDistributionMap (int level, const DistributionMapping& new_dm) = 0;
    virtual void SetParticleGeometry (int level, const Geometry& new_geom) = 0;

    virtual void ClearParticleBoxArray (int level) = 0;
    virtual void ClearParticleDistributionMap (int level) = 0;
    virtual void ClearParticleGeometry (int level) = 0;

    [[nodiscard]] virtual bool LevelDefined (int level) const = 0;
    [[nodiscard]] virtual int finestLevel () const = 0;
    [[nodiscard]] virtual int maxLevel () const = 0;

    [[nodiscard]] virtual IntVect refRatio (int level) const = 0;
    [[nodiscard]] virtual int MaxRefRatio (int level) const = 0;

    [[nodiscard]] virtual Vector<IntVect> refRatio () const = 0;

    template <class MF>
    [[nodiscard]] bool OnSameGrids (int level, const MF& mf) const;
};

//
//! we use this for non-Amr particle code
class ParGDB
    : public ParGDBBase
{
public:

    ParGDB () = default;

    ParGDB (const Geometry            & geom,
            const DistributionMapping & dmap,
            const BoxArray            & ba);

    ParGDB (const Vector<Geometry>            & geom,
            const Vector<DistributionMapping> & dmap,
            const Vector<BoxArray>            & ba,
            const Vector<int>                 & rr);

    ParGDB (const Vector<Geometry>            & geom,
            const Vector<DistributionMapping> & dmap,
            const Vector<BoxArray>            & ba,
            const Vector<IntVect>             & rr);

    [[nodiscard]] const Geometry& ParticleGeom (int level) const override;
    [[nodiscard]] const Geometry& Geom (int level) const override;

    [[nodiscard]] const Vector<Geometry>& ParticleGeom () const override;
    [[nodiscard]] const Vector<Geometry>&         Geom () const override;

    [[nodiscard]] const DistributionMapping& ParticleDistributionMap
                                             (int level) const override;
    [[nodiscard]] const DistributionMapping&         DistributionMap
                                             (int level) const override;

    [[nodiscard]] const Vector<DistributionMapping>& ParticleDistributionMap () const override;
    [[nodiscard]] const Vector<DistributionMapping>&         DistributionMap () const override;

    [[nodiscard]] const BoxArray& ParticleBoxArray (int level) const override;
    [[nodiscard]] const BoxArray&         boxArray (int level) const override;

    [[nodiscard]] const Vector<BoxArray>& ParticleBoxArray () const override;
    [[nodiscard]] const Vector<BoxArray>&         boxArray () const override;

    void SetParticleBoxArray (int level, const BoxArray& new_ba) override;
    void SetParticleDistributionMap (int level,        const DistributionMapping& new_dm) override;
    void SetParticleGeometry (int level, const Geometry& new_geom) override;

    void ClearParticleBoxArray (int level) override;
    void ClearParticleDistributionMap (int level) override;
    void ClearParticleGeometry (int level) override;

    [[nodiscard]] bool LevelDefined (int level) const override;
    [[nodiscard]] int finestLevel () const override;
    [[nodiscard]] int maxLevel () const override;

    [[nodiscard]] IntVect refRatio (int level) const override;
    [[nodiscard]] int MaxRefRatio (int level) const override;

    [[nodiscard]] Vector<IntVect> refRatio () const override;

protected:

    Vector<Geometry>            m_geom;
    Vector<DistributionMapping> m_dmap;
    Vector<BoxArray>            m_ba;
    Vector<IntVect>             m_rr;
    int                         m_nlevels;
};

template <class MF>
bool
ParGDBBase::OnSameGrids (int level, const MF& mf) const
{
    return (mf.DistributionMap() == ParticleDistributionMap(level) &&
            mf.boxArray().CellEqual(ParticleBoxArray(level)));
}

inline
ParGDB::ParGDB (const Geometry            & geom,
                const DistributionMapping & dmap,
                const BoxArray            & ba)
    :
    m_geom(1,geom),
    m_dmap(1,dmap),
    m_ba(1,ba),
    m_nlevels(1)
{ }

inline
ParGDB::ParGDB (const Vector<Geometry>            & geom,
                const Vector<DistributionMapping> & dmap,
                const Vector<BoxArray>            & ba,
                const Vector<IntVect>             & rr)
    :
    m_geom(geom),
    m_dmap(dmap),
    m_ba(ba),
    m_rr(rr),
    m_nlevels(static_cast<int>(ba.size()))
{ }

inline
ParGDB::ParGDB (const Vector<Geometry>            & geom,
                const Vector<DistributionMapping> & dmap,
                const Vector<BoxArray>            & ba,
                const Vector<int>                 & rr)
    :
    m_geom(geom),
    m_dmap(dmap),
    m_ba(ba),
    m_nlevels(static_cast<int>(ba.size()))
{
    for (int level = 0; level < static_cast<int>(rr.size()); ++level)
    {
        m_rr.push_back(rr[level]*IntVect::TheUnitVector());
    }
}

inline
const Geometry&
ParGDB::Geom (int level) const
{
    return m_geom[level];
}

inline
const Geometry&
ParGDB::ParticleGeom (int level) const
{
    return m_geom[level];
}

inline
const Vector<Geometry>&
ParGDB::Geom () const
{
    return m_geom;
}

inline
const Vector<Geometry>&
ParGDB::ParticleGeom () const
{
    return m_geom;
}

inline
const DistributionMapping&
ParGDB::ParticleDistributionMap (int level) const
{
    return m_dmap[level];
}

inline
const DistributionMapping&
ParGDB::DistributionMap (int level) const
{
    return m_dmap[level];
}

inline
const Vector<DistributionMapping>&
ParGDB::ParticleDistributionMap () const
{
    return m_dmap;
}

inline
const Vector<DistributionMapping>&
ParGDB::DistributionMap () const
{
    return m_dmap;
}

inline
const BoxArray&
ParGDB::ParticleBoxArray (int level) const
{
    return m_ba[level];
}

inline
const BoxArray&
ParGDB::boxArray (int level) const
{
    return m_ba[level];
}

inline
const Vector<BoxArray>&
ParGDB::ParticleBoxArray () const
{
    return m_ba;
}

inline
const Vector<BoxArray>&
ParGDB::boxArray () const
{
    return m_ba;
}

inline
void
ParGDB::SetParticleBoxArray (int level, const BoxArray& new_ba)
{
    AMREX_ASSERT(level < m_nlevels);
    m_ba[level] = new_ba;
}

inline
void
ParGDB::SetParticleDistributionMap (int level, const DistributionMapping& new_dm)
{
    AMREX_ASSERT(level < m_nlevels);
    m_dmap[level] = new_dm;
}

inline
void
ParGDB::SetParticleGeometry (int level, const Geometry& new_geom)
{
    AMREX_ASSERT(level < m_nlevels);
    m_geom[level] = new_geom;
}

inline
void
ParGDB::ClearParticleBoxArray (int level)
{
    AMREX_ASSERT(level < m_nlevels);
    m_ba[level] = BoxArray();
}

inline
void
ParGDB::ClearParticleDistributionMap (int level)
{
    AMREX_ASSERT(level < m_nlevels);
    m_dmap[level] = DistributionMapping();
}

inline
void
ParGDB::ClearParticleGeometry (int level)
{
    AMREX_ASSERT(level < m_nlevels);
    m_geom[level] = Geometry();
}

inline
bool
ParGDB::LevelDefined (int level) const
{
    return (level < m_nlevels);
}

inline
int
ParGDB::finestLevel () const
{
    return m_nlevels-1;
}

inline
int
ParGDB::maxLevel () const
{
    return m_nlevels-1;
}

inline
IntVect
ParGDB::refRatio (int level) const
{
    return m_rr[level]*IntVect::TheUnitVector();
}

inline
int
ParGDB::MaxRefRatio (int /*level*/) const
{
    int max_ref_ratio = 0;
    for (int lev = 0; lev < m_nlevels-1; lev++)
        max_ref_ratio = std::max(max_ref_ratio, m_rr[lev].max());
    return max_ref_ratio;
}

inline
Vector<IntVect>
ParGDB::refRatio () const
{
    return m_rr;
}

}

#endif
