// ==========================================================================
// Copyright (C) 2013 Lior Kogan (koganlior1@gmail.com)
// ==========================================================================
// classes defined here:
// CircArc            - circular arc
// CircArcTester      - tester for CircArc class
// ==========================================================================

#pragma once

#include <cmath>
#include <assert.h>

#include "CircVal.h" // CircVal, CircValTypeDef

// ==========================================================================
// circular arc length
// Type should be defined using the CircValTypeDef macro
template <typename Type>
class CircArcLen
{
    double l; // the arc length [0, Type::R]

public:
    // ---------------------------------------------
    operator double() const
    {
        return l;
    }

    // ---------------------------------------------
    // construction based on a floating-point value
    // floating-point is truncated into the range [0, Type::R]
    CircArcLen(double r) : l(__min(__max(0., r), Type::R))
    {
    }

    // construction based on a circular arc length of the same type
    CircArcLen(const CircArcLen& c) : l(c.l)
    {
    }

    // construction based on a circular arc length another type
    // sample use: CircArcLen<SignedRadRange> c= c2;   -or-   CircArcLen<SignedRadRange> c(c2);
    template<typename Type2>
    CircArcLen(const CircArcLen<Type2>& c) : l((c == Type2::R) ? Type::R : Type::R / Type2::R * c) // ... to avoid rounding errors
    {
    }

    // ---------------------------------------------
    // assignment from a floating-point value
    // floating-point is truncated into the range [0, Type::R]
    CircArcLen& operator= (double r)
    {
        l = __min(__max(0., r), Type::R);
        return *this;
    }

    // assignment from a circular arc length of the same type
    CircArcLen& operator=(const CircArcLen& c)
    {
        l = c.l;
        return *this;
    }    

    // assignment from another type of circular arc length
    template<typename Type2>
    CircArcLen& operator= (const CircArcLen<Type2>& c)
    {
        l = (c == Type2::R) ? Type::R : Type::R / Type2::R * c; // ... to avoid rounding errors
        return *this;
    }
};

// ==========================================================================
// circular arc
// Type should be defined using the CircValTypeDef macro
template <typename Type>
class CircArc
{
    // the arc [c1,c1+l] is defined by the shortest increasing walk from c1 to c1+l, unless l=Type::R where the arc is defined as the whole circle
    CircVal   <Type> c1; // the arc start-point [Type::L, Type::H)
    CircVal   <Type> c2; // the arc end  -point [Type::L, Type::H). Note that c2=c1 in two cases: (a) l=0 (b) l=Type::R
    CircArcLen<Type> l ; // the arc length      [0      , Type::R]

    // ---------------------------------------------
public:
    CircVal   <Type> GetC1() const { return c1; } // the arc start-point [Type::L, Type::H)
    CircVal   <Type> GetC2() const { return c2; } // the arc end  -point [Type::L, Type::H). Note that c2=c1 in two cases: (a) l=0 (b) l=Type::R
    CircArcLen<Type> GetL () const { return l ; } // the arc length      [0      , Type::R]

    // ---------------------------------------------
    CircArc() : c1(Type::Z), c2(Type::Z), l(0)
    {
    }

    // "reference" construction:
    // construction based on CircVal (arc start point) and CircArcLen (arc length)
    template<typename Type2, typename Type3>
    CircArc(const CircVal<Type2>& _c1, const CircArcLen<Type3>& _l) : c1(_c1), c2((double)CircVal<Type>(c1)+(double)CircArcLen<Type>(_l)), l(_l)
    {
    }

    // construction based on two floating-point values (arc start-point, arc length)
    // c1 is wrapped into the range, r is truncated into the range
    CircArc(double fc1, double fl) : c1(fc1), c2(fc1+CircArcLen<Type>(fl)), l(fl)
    {
    }

    // construction based on two circular values of same/different type (arc start-point, arc end-point)
    // note that if c1==c2, the arc length will be 0
    template<typename Type2, typename Type3>
    CircArc(const CircVal<Type2>& _c1, const CircVal<Type3>& _c2) : c1(_c1), c2(_c2), l(CircVal<Type>::Pdist(c1, c2))
    {
    }

    // construction based on another circular arc of same/different circular-value type
    // sample use: CircArc<SignedRadRange> a = a2;   -or-   CircArc<SignedRadRange> a(a2);
    template<typename Type2>
    CircArc(const CircArc<Type2>& a) : c1(a.GetC1()), c2(a.GetC2()), l(a.GetL())
    {
    }

    // ---------------------------------------------
    // assignment from a circular arc of same/different circular arc type
    template<typename Type2>
    CircArc& operator= (const CircArc<Type2>& a)
    {
        c1 = a.GetC1();
        c2 = a.GetC2();
        l  = a.GetL ();
        return *this;
    }

    // ---------------------------------------------
    bool operator==(const CircArc& a) const
    {
        if ((l == Type::R) && (a.l == Type::R)) // both are full-circle; start-point doesn't matter
            return true;

        return std::equal_to<decltype(c1)>{}(c1, a.c1) && std::equal_to<decltype(l)>{}(l, a.l); // std::equal_to instead of == to avoid triggering -Wfloat-equal
    }

    bool operator!=(const CircArc& a) const
    {
        return !(*this == a);
    }

    // check if this arc contains a circular value (note that arc contains its endpoints)
    bool Contains(const CircVal<Type>& c) const
    {
        return l - CircVal<Type>::Pdist(c1, c) > -1e-12;
    }

    // check if this arc contains another circular arc (note that arcs contain their endpoints)
    bool Contains(const CircArc& a) const
    {
        if (  l == Type::R) return true ; // full-circle
        if (a.l == Type::R) return false; // full-circle

        // ensure order: c1 --- a.c1 --- a.c2 --- c2
        const double l1 = CircVal<Type>::Pdist(c1, a.c1);
        const double l2 = CircVal<Type>::Pdist(c1, a.c2);
        return (l2 - l1 > -1e-12) && (l - l2 > -1e-12);
    }

    // check if two circular arcs intersect (note that arcs contain their endpoints)
    bool Intersect(const CircArc& a) const
    {
        return Contains(a.c1) || a.Contains(c1); // one of them contains the start of the other
    }
};

// ==========================================================================
// circular arcs
// Type should be defined using the CircValTypeDef macro
template <typename Type>
class CircArcs
{
};

// CircArcs Union       (const CircArc& a);
// CircArcs Intersection(const CircArc& a);
// CircArcs Diff        (const CircArc& a);

// ==========================================================================
// tester for CircVal class
template <typename Type>
class CircArcTester
{
public:
    CircArcTester()
    {
        Test();
    }

    static void Test()
    {
        const unsigned nSteps = 36              ;
        const double   fStep  = Type::R / nSteps;

        unsigned m = 0, n = 0, p = 0, q[nSteps+1];

        for (unsigned i = 0; i<nSteps; ++i)
            q[i] = 0;

        for (unsigned i = 0; i < nSteps; ++i)
            for (unsigned j = 0; j <= nSteps; ++j)
            {
                CircArc<Type> a1(Type::L + i*fStep, j*fStep); // 1st arc: start-point, length

                for (unsigned k = 0; k < nSteps; ++k)
                    for (unsigned l = 0; l <= nSteps; ++l)
                    {
                        CircArc<Type> a2(Type::L + k*fStep, l*fStep); // 2nd arc: start-point, length

                        bool b1 = a1.Contains(a2); if (b1) ++m;       // if a2 is a sub-arc of a1
                        bool b2 = a2.Contains(a1); if (b2) ++n;       // if a1 is a sub-arc of a2

                        if   (a1 == a2) { assert(  b1 && b2 ); ++p; } // if identical
                        else              assert(!(b1 && b2));

                        if (a1.Intersect(a2)) ++q[j];
                    }
            }

        assert (p == 2*nSteps*nSteps                                     ); // number of identical arcs
        assert (m ==   nSteps*nSteps * (nSteps*nSteps + 9*nSteps + 8) / 6); // number of times a2 is a sub-arc of a1
        assert (m == n                                                   ); // number of times a1 is a sub-arc of a2 shuould be identical
//      assert (q == ...)                                                   // number of intersecting arcs

        // --------------
    }
};
