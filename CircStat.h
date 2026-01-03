// ==========================================================================
// Copyright (C) 2011 Lior Kogan (koganlior1@gmail.com)
// ==========================================================================
// classes defined here:
// CircAverage            - calculate average set of circular values
// WeightedCircAverage    - calculate weighted-average set of circular values
// CAvrgSampledCircSignal - estimate the average of a sampled continuous-time circular signal, using circular linear interpolation
// CircMedian             - calculate median set of circular values
// ==========================================================================

#pragma once

#include <cmath>
#include <assert.h>
#include <set>
#include <vector>
#include <algorithm>    // sort

#include "CircHelper.h" // Sqr

using namespace std;

// ==========================================================================
// calculate average set of circular values
// return set of average values
// T is a circular value type defined with the CircValTypeDef macro
template<typename T>
set<CircVal<T>> CircAverage(vector<CircVal<T>> const& A)
{
    // ----------------------------------------------
    // all vars: UnsignedDegRange [0,360)
    double          fSum           = 0.; // of all elements of A
    double          fSumSqr        = 0.; // of all elements of A
    double          fMinSumSqrDiff     ; // minimal sum of squares of differences
    vector<double>  LowerAngles        ; // ascending   [  0,180)
    vector<double>  UpperAngles        ; // descending  (360,180)
    double          fTestAvrg          ;
    vector<double>  MinAvrgVals        ; // results set

    // ----------------------------------------------
    // local functions - implemented as lambdas
    // ----------------------------------------------

    // calc sum(dist(180, Bi)^2) - all values are in set B
    // dist(180,Bi)= |180-Bi|
    // sum(dist(x, Bi)^2) = sum((180-Bi)^2) = sum(180^2-2*180*Bi + Bi^2) = 180^2*A.size - 360*sum(Ai) + sum(Ai^2)
    auto SumSqr = [&]() -> double
    {
        return 32400.*A.size() - 360.*fSum + fSumSqr;
    };

    // calc sum(dist(x, Ai)^2). A=B+C; set D is empty
    // dist(x,Bi)= |x-Bi|
    // dist(x,Ci)= 360-(Ci-x)
    // sum(dist(x, Bi)^2)= sum(     (x-Bi) ^2)= sum(        Bi^2 + x^2                      - 2*Bi*x)
    // sum(dist(x, Ci)^2)= sum((360-(Ci-x))^2)= sum(360^2 + Ci^2 + x^2 - 2*360*Ci + 2*360*x - 2*Ci*x)
    // sum(dist(x, Bi)^2) + sum(dist(x, Ci)^2) = nCountC*360^2 + sum(Ai^2) + nCountA*x^2 - 2*360*sum(Ci) + nCountC*2*360*x - 2*x*sum(Ai)
    auto SumSqrC = [&](double x, size_t nCountC, double fSumC) -> double
    {
        return x*(A.size()*x - 2*fSum) + fSumSqr - 2*360.*fSumC + nCountC*( 2*360.*x + 360.*360.);
    };

    // calc sum(dist(x, Ai)^2). A=B+D; set C is empty
    // dist(x,Bi)= |x-Bi|
    // dist(x,Di)= 360-(x-Di)
    // sum(dist(x,Bi)^2)= sum(    (x-Bi)^2)= sum(        Bi^2 + x^2                      - 2*Bi*x)
    // sum(dist(x,Di)^2)= sum(360-(x-Di)^2)= sum(360^2 + Di^2 + x^2 + 2*360*Di - 2*360*x - 2*Di*x)
    // sum(dist(x, Bi)^2) + sum(dist(x, Di)^2) = nCountD*360^2 + sum(Ai^2) + nCountA*x^2 + 2*360*sum(Di) - nCountD*2*360*x - 2*x*sum(Ai)
    auto SumSqrD = [&](double x, size_t nCountD, double fSumD) -> double
    {
        return x * (A.size()*x - 2*fSum) + fSumSqr + 2*360.*fSumD + nCountD*(-2*360.*x + 360.*360.);
    };

    // update MinAvrgAngles if lower/equal fMinSumSqrDiff found
    auto TestSum = [&](double fTestAvrg, double fTestSumDiffSqr) -> void
    {
        if (fTestSumDiffSqr < fMinSumSqrDiff)
        {
            MinAvrgVals.clear();
            MinAvrgVals.emplace_back(fTestAvrg);
            fMinSumSqrDiff = fTestSumDiffSqr;
        }
        else if (fTestSumDiffSqr == fMinSumSqrDiff)
            MinAvrgVals.emplace_back(fTestAvrg);
    };

    // ----------------------------------------------
    for (const auto& a : A)
    {
        double v = CircVal<UnsignedDegRange>(a); // convert to [0.360)
        fSum    +=     v ;
        fSumSqr += Sqr(v);
             if (v < 180.) LowerAngles.emplace_back(v);
        else if (v > 180.) UpperAngles.emplace_back(v);
    }

    sort(LowerAngles.begin(), LowerAngles.end()                   ); // ascending   [  0,180)
    sort(UpperAngles.begin(), UpperAngles.end(), greater<double>()); // descending  (360,180)

    // ----------------------------------------------
    // start with avrg= 180, sets c,d are empty
    // ----------------------------------------------
    MinAvrgVals.clear();
    MinAvrgVals.emplace_back(180.);
    fMinSumSqrDiff = SumSqr();

    // ----------------------------------------------
    // average in (180,360), set D: values in range [0,avrg-180)
    // ----------------------------------------------
    double fLowerBound = 0.; // of current sector
    double fSumD       = 0.; // of elements of set D

    auto iter = LowerAngles.begin();
    for (size_t d = 0; d < LowerAngles.size(); ++d)
    {
        // 1st  iteration : average in (                 180, lowerAngles[0]+180]
        // next iterations: average in (lowerAngles[i-1]+180, lowerAngles[i]+180]
        // set D          : lowerAngles[0..d]

        fTestAvrg = (fSum + 360.*d)/A.size(); // average for sector, that minimizes SumDiffSqr

        if ((fTestAvrg > fLowerBound+180.) && (fTestAvrg <= *iter+180.))  // if fTestAvrg is within sector
            TestSum(fTestAvrg, SumSqrD(fTestAvrg, d, fSumD));             // check if fTestAvrg generates lower SumSqr

        fLowerBound  = *iter      ;
        fSumD       += fLowerBound;
        ++iter;
    }

    // last sector : average in [lowerAngles[lastIdx]+180, 360)
    fTestAvrg = (fSum + 360.*LowerAngles.size())/A.size(); // average for sector, that minimizes SumDiffSqr

    if ((fTestAvrg < 360.) && (fTestAvrg > fLowerBound))                   // if fTestAvrg is within sector
        TestSum(fTestAvrg, SumSqrD(fTestAvrg, LowerAngles.size(), fSumD)); // check if fTestAvrg generates lower SumSqr

    // ----------------------------------------------
    // average in [0,180); set C: values in range (avrg+180, 360)
    // ----------------------------------------------
    double fUpperBound = 360.; // of current sector
    double fSumC       =   0.; // of elements of set C

    iter = UpperAngles.begin();
    for (size_t c = 0; c < UpperAngles.size(); ++c)
    {
        // 1st  iteration : average in [upperAngles[0]-180, 360                 )
        // next iterations: average in [upperAngles[i]-180, upperAngles[i-1]-180)
        // set C          : upperAngles[0..c]  (descendingly sorted)

        fTestAvrg = (fSum - 360.*c)/A.size(); // average for sector, that minimizes SumDiffSqr

        if ((fTestAvrg >= *iter-180.) && (fTestAvrg < fUpperBound-180.))   // if fTestAvrg is within sector
            TestSum(fTestAvrg, SumSqrC(fTestAvrg, c, fSumC));              // check if fTestAvrg generates lower SumSqr

        fUpperBound  = *iter      ;
        fSumC       += fUpperBound;
        ++iter;
    }

    // last sector : average in [0, upperAngles[lastIdx]-180)
    fTestAvrg = (fSum - 360.*UpperAngles.size())/A.size(); // average for sector, that minimizes SumDiffSqr

    if ((fTestAvrg >= 0.) && (fTestAvrg < fUpperBound))                    // if fTestAvrg is within sector
        TestSum(fTestAvrg, SumSqrC(fTestAvrg, UpperAngles.size(), fSumC)); // check if fTestAvrg generates lower SumSqr

    // ----------------------------------------------
    set<CircVal<T>> MinAvrgCircVals;
    for (const auto& v : MinAvrgVals)
        MinAvrgCircVals.emplace(CircVal<UnsignedDegRange>(v)); // convert from [0.360)

    return MinAvrgCircVals;
}

// ==========================================================================
// calculate average set of circular values
// return set of average values
// T is a circular value type defined with the CircValTypeDef macro
template<typename T>
set<CircVal<T>> CircAverage2(vector<CircVal<T>> const& A)
{
    const size_t   count         = A.size();
    double         fSum          = 0.      ; // of all elements of Angles
    double         fSumSqr       = 0.      ; // of all elements of Angles
    vector<double> Angles(count)           ; // UnsignedDegRange [0,360), ascendingly sorted

    for (size_t i = 0; i<count; ++i)
    {
        Angles[i]  = CircVal<UnsignedDegRange>(A[i]); // convert to [0,360)
        fSum      +=     Angles[i] ;
        fSumSqr   += Sqr(Angles[i]);
    }

    sort(Angles.begin(), Angles.end()); // ascending

    // ----------------------------------------------
    // calc sum of squares of differences for the initial order
    double fMinSumSqrDiff = fSumSqr - Sqr(fSum)/count;
    vector<size_t> MinShiftIdx = {0}; // indices of shift with minimal avrg

    // calc sum for each order, and test if new minimum found
    for (size_t i = 1; i<count; ++i)
    {
        fSumSqr += 720.*Angles[i-1];
        const double fTestSumDiffSqr = fSumSqr + 360.*360.*i - Sqr(fSum+360.*i)/count;

        if (fTestSumDiffSqr < fMinSumSqrDiff)       // new minimum found?
        {                                                               
            MinShiftIdx    = {i};
            fMinSumSqrDiff = fTestSumDiffSqr;
        }
        else if (fTestSumDiffSqr == fMinSumSqrDiff) // same minimum?
            MinShiftIdx.emplace_back(i);
    }

    // ----------------------------------------------
    set<CircVal<T>> MinAvrgCircVals;
    for (const auto& i : MinShiftIdx)
        MinAvrgCircVals.emplace(CircVal<UnsignedDegRange>((fSum+360.*i) / count)); // avrg from shift index

    return MinAvrgCircVals;
}

// ==========================================================================
// calculate weighted-average set of circular values
// return set of average values
// T is a circular value type defined with the CircValTypeDef macro
template<typename T>
set<CircVal<T>> WeightedCircAverage(vector<pair<CircVal<T>,double>> const& A) // vector <value,weight>
{
    set<CircVal<T>>              MinAvrgVals        ; // results set

    // ----------------------------------------------
    // all vars: UnsignedDegRange [0,360)
    double                       fASumW         = 0.; // sum(Wi     ) of all elements of A
    double                       fASumWA        = 0.; // sum(Wi*Ai  ) of all elements of A
    double                       fASumWA2       = 0.; // sum(Wi*Ai^2) of all elements of A
    double                       fMinSumSqrDiff     ; // minimal sum of squares of differences
    vector<pair<double, double>> LowerAngles        ; // ascending   [  0,180)  <angle,weight>
    vector<pair<double, double>> UpperAngles        ; // descending  (360,180)  <angle,weight>
    double                       fTestAvrg          ;

    // ----------------------------------------------
    // local functions - implemented as lambdas
    // ----------------------------------------------

    // calc sum(Wi*dist(180, Bi)^2) - all values are in set B
    // dist(180,Bi)= |180-Bi|
    // sum(Wi*dist(x, Bi)^2) = sum(Wi*(180-Bi)^2) = sum(Wi*(180^2-2*180*Bi + Bi^2)) = 180^2*fSumW - 360*sum(Wi*Ai) + sum(Wi*Ai^2)
    auto SumSqr = [&]() -> double
    {
        return 32400.*fASumW - 360.*fASumWA + fASumWA2;
    };

    // calc sum(Wi*dist(x, Ai)^2). A=B+C; set D is empty
    // dist(x,Bi)= |x-Bi|
    // dist(x,Ci)= 360-(Ci-x)
    // sum(Wi*dist(x,Bi)^2)= sum(Wi*(     (x-Bi) ^2))= sum(Wi*(        Bi^2 + x^2                      - 2*Bi*x)) +
    // sum(Wi*dist(x,Ci)^2)= sum(Wi*((360-(Ci-x))^2))= sum(Wi*(360^2 + Ci^2 + x^2 - 2*360*Ci + 2*360*x - 2*Ci*x))
    //                                                 ==========================================================
    //                                                 sum(Wi*(        Ai^2 + x^2                      - 2*Ai*x))
    auto SumSqrC = [&](double x      ,
                       double fCSumW ,            // sum(Wi   ) of all elements of C
                       double fCSumWC ) -> double // sum(Wi*Ci) of all elements of C
    {
        return fASumWA2 + x*x*fASumW -2*x*fASumWA - 720*fCSumWC + (129600+720*x)*fCSumW;
    };

    // calc sum(Wi*dist(x, Ai)^2). A=B+D; set C is empty
    // dist(x,Bi)= |x-Bi|
    // dist(x,Di)= 360-(x-Di)
    // sum(Wi*dist(x,Bi)^2)= sum(Wi*(     (x-Bi) ^2))= sum(Wi*(        Bi^2 + x^2                      - 2*Bi*x))
    // sum(Wi*dist(x,Di)^2)= sum(Wi*((360-(x-Di))^2))= sum(Wi*(360^2 + Di^2 + x^2 + 2*360*Di - 2*360*x - 2*Di*x))
    //                                                 ==========================================================
    //                                                 sum(Wi*(        Ai^2 + x^2                      - 2*Ai*x))
    auto SumSqrD = [&](double x      ,
                       double fDSumW ,            // sum(Wi   ) of all elements of D
                       double fDSumWD ) -> double // sum(Wi*Di) of all elements of D
    {
        return fASumWA2 + x*x*fASumW -2*x*fASumWA + 720*fDSumWD + (129600-720*x)*fDSumW;
    };

    // update MinAvrgAngles if lower/equal fMinSumSqrDiff found
    auto TestSum = [&](double fTestAvrg, double fTestSumDiffSqr) -> void
    {
        if (fTestSumDiffSqr < fMinSumSqrDiff)
        {
            MinAvrgVals.clear();
            MinAvrgVals.emplace(CircVal<UnsignedDegRange>(fTestAvrg));
            fMinSumSqrDiff= fTestSumDiffSqr;
        }
        else if (fTestSumDiffSqr == fMinSumSqrDiff)
            MinAvrgVals.emplace(CircVal<UnsignedDegRange>(fTestAvrg));
    };

    // ----------------------------------------------
    for (const auto& a : A)
    {
        double v  = CircVal<UnsignedDegRange>(a.first); // convert to [0.360)
        double w  = a.second;                           // weight
        fASumW   += w    ;
        fASumWA  += w*v  ;
        fASumWA2  = w*v*v;

             if (v < 180.) LowerAngles.emplace_back(pair<double,double>(v,w));
        else if (v > 180.) UpperAngles.emplace_back(pair<double,double>(v,w));
    }

    sort(LowerAngles.begin(), LowerAngles.end()                                ); // ascending   [  0,180)
    sort(UpperAngles.begin(), UpperAngles.end(), greater<pair<double,double>>()); // descending  (360,180)

    // ----------------------------------------------
    // start with avrg= 180, sets c,d are empty
    // ----------------------------------------------
    MinAvrgVals.clear();
    MinAvrgVals.emplace(CircVal<UnsignedDegRange>(180.));
    fMinSumSqrDiff = SumSqr();

    // ----------------------------------------------
    // average in (180,360), set D: values in range [0,avrg-180)
    // ----------------------------------------------
    double fLowerBound = 0.; // of current sector
    double fDSumW      = 0.; // sum(Wi   ) of all elements of D
    double fDSumWD     = 0.; // sum(Wi*Di) of all elements of D

    auto iter = LowerAngles.begin();
    for (size_t d = 0; d < LowerAngles.size(); ++d)
    {
        // 1st  iteration : average in (                 180, lowerAngles[0]+180]
        // next iterations: average in (lowerAngles[i-1]+180, lowerAngles[i]+180]
        // set D          : lowerAngles[0..d]

        fTestAvrg = (fASumWA + 360.*fDSumW)/fASumW; // average for sector, that minimizes SumDiffSqr

        if ((fTestAvrg > fLowerBound+180.) && (fTestAvrg <= (*iter).first+180.)) // if fTestAvrg is within sector
            TestSum(fTestAvrg, SumSqrD(fTestAvrg, fDSumW, fDSumWD));             // check if fTestAvrg generates lower SumSqr

        fLowerBound  = (*iter).first                 ;
        fDSumW      += (*iter).second                ;
        fDSumWD     += (*iter).second * (*iter).first;
        ++iter;
    }

    // last sector : average in [lowerAngles[lastIdx]+180, 360)
    fTestAvrg = (fASumWA + 360.*fDSumW)/fASumW; // average for sector, that minimizes SumDiffSqr

    if ((fTestAvrg < 360.) && (fTestAvrg > fLowerBound))                         // if fTestAvrg is within sector
        TestSum(fTestAvrg, SumSqrD(fTestAvrg, fDSumW, fDSumWD));                 // check if fTestAvrg generates lower SumSqr

    // ----------------------------------------------
    // average in [0,180); set C: values in range (avrg+180, 360)
    // ----------------------------------------------
    double fUpperBound = 360.; // of current sector
    double fCSumW      =   0.; // sum(Wi   ) of all elements of C
    double fCSumWC     =   0.; // sum(Wi*Ci) of all elements of C

    iter = UpperAngles.begin();
    for (size_t c = 0; c < UpperAngles.size(); ++c)
    {
        // 1st  iteration : average in [upperAngles[0]-180, 360                 )
        // next iterations: average in [upperAngles[i]-180, upperAngles[i-1]-180)
        // set C          : upperAngles[0..c]  (descendingly sorted)

        fTestAvrg = (fASumWA - 360.*fCSumW)/fASumW; // average for sector, that minimizes SumDiffSqr

        if ((fTestAvrg >= (*iter).first-180.) && (fTestAvrg < fUpperBound-180.)) // if fTestAvrg is within sector
            TestSum(fTestAvrg, SumSqrC(fTestAvrg, fCSumW, fCSumWC));             // check if fTestAvrg generates lower SumSqr

        fUpperBound  = (*iter).first                 ;
        fCSumW      += (*iter).second                ;
        fCSumWC     += (*iter).second * (*iter).first;
        ++iter;
    }

    // last sector : average in [0, upperAngles[lastIdx]-180)
    fTestAvrg = (fASumWA - 360.*fCSumW)/fASumW; // average for sector, that minimizes SumDiffSqr

    if ((fTestAvrg >= 0.) && (fTestAvrg < fUpperBound))                          // if fTestAvrg is within sector
        TestSum(fTestAvrg, SumSqrC(fTestAvrg, fCSumW, fCSumWC));                 // check if fTestAvrg generates lower SumSqr

    // ----------------------------------------------
    return MinAvrgVals;
}

// ==========================================================================
// estimate the average of a sampled continuous-time circular signal, using circular linear interpolation
// T is a circular value type defined with the CircValTypeDef macro
template<typename T>
class CAvrgSampledCircSignal
{
    size_t                           m_nSamples ;
    CircVal<T>                       m_PrevC    ; // previous value
    double                           m_fPrevTime; // previous time
    vector<pair<CircVal<T>, double>> m_Intervals; // vector of (avrg,weight) for each interval

public:
    CAvrgSampledCircSignal()
    {
        m_nSamples = 0;
    }

    void AddMeasurement(CircVal<T> C, double fTime)
    {
        if (m_nSamples)
        {
            assert(fTime > m_fPrevTime);

            double fIntervalAvrg   = CircVal<T>::Wrap((double)m_PrevC + CircVal<T>::Sdist(m_PrevC, C) / 2.);
            double fIntervalWeight = fTime - m_fPrevTime                                                   ;
            m_Intervals.emplace_back(fIntervalAvrg, fIntervalWeight);
        }

        m_PrevC     = C    ;
        m_fPrevTime = fTime;
        ++m_nSamples;
    }

    // calculate the weighted average for all intervals
    bool GetAvrg(CircVal<T>& Avrg)
    {
        switch (m_nSamples)
        {
        case 0:
            Avrg = CircVal<T>::GetZ();
            return false;

        case 1:
            Avrg = m_PrevC;
            return true;

        default:
            Avrg = *WeightedCircAverage(m_Intervals).begin();
            return true;
        }
    }
};

// ==========================================================================
// calculate median set of circular values
// return set of median values
// T is a circular value type defined with the CircValTypeDef macro
template<typename T>
set<CircVal<T>> CircMedian(vector<CircVal<T>> const& A)
{
    set <CircVal<T>> X;           // results set

    // ----------------------------------------------
    set<CircVal<T>> B;
    if (A.size() % 2 == 0)        // even number of values
    {
        auto S = A;
        sort(S.begin(), S.end()); // A, sorted

        for (size_t m = 0; m < S.size(); ++m)
        {
            size_t n = m+1; if (n == S.size()) n = 0;
            double d = CircVal<T>::Sdist(S[m], S[n]);

            // insert average set of each two circular-consecutive values
            B.emplace((double)S[m] + d / 2.);
            if (d == -CircVal<T>::GetR() / 2.)
                B.emplace((double)S[n] + d / 2.);
        }
    }
    else                          // odd number of values
        for (size_t m = 0; m < A.size(); ++m)
            B.emplace(A[m]);      // convert vector to set - remove duplicates

    // ----------------------------------------------
    double fMinSum = numeric_limits<double>::max();

    for (const auto& b : B)
    {
        double fSum = 0.;         // sum(|Sdist(a, b)|)
        for (const auto& a : A)
            fSum += abs(CircVal<T>::Sdist(b, a));

             if (fSum == fMinSum)              X.emplace(b);
        else if (fSum <  fMinSum) { X.clear(); X.emplace(b); fMinSum = fSum; }
    }

    // ----------------------------------------------
    return X;
}
