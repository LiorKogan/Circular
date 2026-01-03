// ==========================================================================
// Copyright (C) 2011 Lior Kogan (koganlior1@gmail.com)
// ==========================================================================

#include "stdafx.h"

#include <chrono>
#include <iostream>                 // cout
#include <fstream>                  // ofstream
#include <numbers>                  // std::numbers::pi
#include <random>                   // random number generators 

#include <ranges>                   // std::views::iota
#include <execution>                // std::execution::par
#include <syncstream>               // std::osyncstream

#include "CircVal.h"                // CircVal, CircValTester
#include "CircArc.h"                // CircArcLen, CircArc, CircArcTester
#include "CircStat.h"               // CircAverage, WeightedCircAverage, CAvrgSampledCircSignal, CircMedian
#include "CircHelper.h"             // Sqr, Mod
#include "TruncNormalDist.h"        // truncated_normal_distribution
#include "WrappedNormalDist.h"      // wrapped_normal_distribution
#include "WrappedTruncNormalDist.h" // wrapped_truncated_normal_distribution

// ==========================================================================
int _tmain(int argc, _TCHAR* argv[])
{
    auto Time0 = chrono::system_clock::now();

    // ------------------------------------------------------
    {
        CircVal   <UnsignedDegRange> Start0 ( 10.                );
        CircArcLen<UnsignedRadRange> Length0(std::numbers::pi    );
        CircArc   <SignedDegRange  > a0     (Start0, Length0     ); // construct by CircVal, CircArcLen

        CircVal   <UnsignedRadRange> Start1 (std::numbers::pi/2  );
        CircVal   <UnsignedRadRange> End1   (std::numbers::pi/2*3);
        CircArc   <UnsignedDegRange> a1     (Start1, End1        ); // construct by CircVal, CircVal

        CircVal   <UnsignedDegRange> Start2 ( 10.                );
        CircVal   <UnsignedRadRange> End2   (std::numbers::pi    );
        CircArc   <SignedDegRange  > a2     (Start2, End2        ); // construct by CircVal, CircVal (of different types)

        CircArc   <UnsignedDegRange> a3     (100, 200            ); // construct by double, double
        CircArc   <UnsignedRadRange> a4     (a3                  ); // construct by CircArc (of different type)
        a4 = a3;                                                    // assignment operator

        bool b1 = a3.Contains( 50 );
        bool b2 = a3.Contains(100 );
        bool b3 = a3.Contains(150 );
        bool b4 = a3.Contains(200 );
        bool b5 = a3.Contains(250 );
        bool b6 = a3.Contains(300 );
        bool b7 = a3.Contains(Start1);

        CircArc<UnsignedDegRange> Arc3(a3);
        CircArc<UnsignedRadRange> Arc4(a3);
        CircArc<UnsignedRadRange> Arc5 = a3;
        Arc5 = a4;

        bool b = (Arc4 == Arc3);
        double d = Arc4.GetL();

        bool c1 = a4.Contains(Arc3);
        bool c2 = CircArc<SignedDegRange  >(-170., 360.).Contains (CircArc<SignedDegRange  >(-180., 360.)); // both are full circles
        bool c3 = CircArc<UnsignedDegRange>(   0., 100.).Intersect(CircArc<UnsignedDegRange>( 100., 100.));
    }

    // todo: assure ArcLength is equal

    // ------------------------------------------------------
    // testing correctness of CircVal class implementation
    {
        CircValTester<SignedDegRange  > testA;
        CircValTester<UnsignedDegRange> testB;
        CircValTester<SignedRadRange  > testC;
        CircValTester<UnsignedRadRange> testD;

        CircValTester<TestRange0      > test0;
        CircValTester<TestRange1      > test1;
        CircValTester<TestRange2      > test2;
        CircValTester<TestRange3      > test3;
    }

    // ------------------------------------------------------
    // testing correctness of CircArc class implementation
    {
        CircArcTester<SignedDegRange  > testA;
        CircArcTester<UnsignedDegRange> testB;
        CircArcTester<SignedRadRange  > testC;
        CircArcTester<UnsignedRadRange> testD;

        CircArcTester<TestRange0      > test0;
        CircArcTester<TestRange1      > test1;
        CircArcTester<TestRange2      > test2;
        CircArcTester<TestRange3      > test3;
    }

    // ------------------------------------------------------
    // sample code: basic circular math operations
    {
        CircVal<SignedDegRange  > c1 =  10. ;
        CircVal<UnsignedRadRange> c2 =   0.2;
        CircVal<SignedDegRange  > c3 = c1+c2;

        c1 = c2;

        c1 += 355.;
        double d = c1;

        d  = sin(c1) / cos(c2) + tan(c3);
        c1 = asin<SignedDegRange>(0.5); // don't do d1= asin(0.5) !!!
    }

    // ------------------------------------------------------
    // sample code: generating wrapped normal distributed random value
    {
        std::default_random_engine rand_engine;
        std::random_device         rnd_device ;
        rand_engine.seed(rnd_device()); // reseed engine

        double fAvrg  =    0.;
        double fSigma =   45.;
        double fL     = -180.; // wrapping-range lower-bound
        double fH     =  180.; // wrapping-range upper-bound

        wrapped_normal_distribution<double> r_wrp(fAvrg, fSigma, fL, fH);
        double r1 = r_wrp(rand_engine); // random value
    }

    // ------------------------------------------------------
    // sample code: generating truncated normal distributed random value
    {
        std::default_random_engine rand_engine;
        std::random_device         rnd_device ;
        rand_engine.seed(rnd_device()); // reseed engine

        double fAvrg  =    0.;
        double fSigma =   45.;
        double fA     =  -40.; // truncation-range lower-bound
        double fB     =   40.; // truncation-range upper-bound

        truncated_normal_distribution<double> r_trn(fAvrg, fSigma, fA, fB);
        double r2 = r_trn(rand_engine); // random value
    }

    // ------------------------------------------------------
    // sample code: generating wrapped truncated normal distributed random value
    {
        std::default_random_engine rand_engine;
        std::random_device         rnd_device ;
        rand_engine.seed(rnd_device()); // reseed engine

        // normal distribution is first truncated, and then wrapped

        const double fAvrg  =    0.;
        const double fSigma =  100.;
        const double fA     = -500.; // truncation-range lower-bound
        const double fB     =  500.; // truncation-range upper-bound
        const double fL     =    0.; // wrapping  -range lower-bound
        const double fH     =  360.; // wrapping  -range upper-bound

        wrapped_truncated_normal_distribution<double> r_wrp_trn(fAvrg, fSigma, fA, fB, fL, fH);
        double d = r_wrp_trn(rand_engine); // random value
    }

    // ------------------------------------------------------
    {
        std::default_random_engine rand_engine;
        std::random_device         rnd_device ;
        rand_engine.seed(rnd_device()); // reseed engine
        uniform_real_distribution<double> ud(0., 360.);

        const size_t count = 100;
        vector<CircVal<UnsignedDegRange>> Angles2(count);

        auto Time0 = chrono::system_clock::now();
        for (size_t i = 0; i < 100000; i++)
        {
            for (size_t j = 0; j < count; ++j)
                Angles2[j] = ud(rand_engine);

            auto y = CircAverage (Angles2);
        }

        auto Duration = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - Time0).count();
        cout << Duration << endl;

        Time0 = chrono::system_clock::now();
        for (int i = 0; i < 100000; i++)
        {
            for (size_t j = 0; j < count; ++j)
                Angles2[j] = ud(rand_engine);

            auto z = CircAverage2(Angles2);
        }

        Duration = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - Time0).count();
        cout << Duration << endl;

        cout << "=================" << endl;
    }

    // ------------------------------------------------------
    // code used to collect data for graphs that demonstrate average of circular values
    {
        vector<CircVal<UnsignedDegRange>> Angles2 = {30., 130., 230., 330.};
        auto y = CircAverage (Angles2);
        auto z = CircAverage2(Angles2);

        ofstream f0("log0.txt");

        for (double x = 0.; x <= 360.; x += 0.1)
        {
            double fSum = 0;
            for (const auto& a : Angles2)
                fSum += Sqr(__min(abs(x-a), 360.-abs(x-a)));

            f0 << x << "\t" << fSum << endl;
        }
    }

    // ------------------------------------------------------
    // sample code: calculate median, average and weighted-average set of circular values
    {
        std::default_random_engine rand_engine;
        std::random_device         rnd_device ;
        rand_engine.seed(rnd_device()); // reseed engine

        uniform_real_distribution<double> ud(UnsignedDegRange::L, UnsignedDegRange::H);

        vector<     CircVal<UnsignedDegRange>        > angles1;
        vector<pair<CircVal<UnsignedDegRange>,double>> angles2;

        for (size_t i = 0; i < 3; ++i)
        {
            double m = ud(rand_engine);
            angles1.emplace_back(m     );
            angles2.emplace_back(m, 0.3);
        }

        auto Medn  = CircMedian         (angles1);
        auto Avrg1 = CircAverage        (angles1);
        auto Avrg2 = WeightedCircAverage(angles2);
    }

    // ------------------------------------------------------
    // sample code: estimate average of a sampled continuous-time circular signal, using circular linear interpolation
    {
        CAvrgSampledCircSignal<UnsignedDegRange> A1;
        A1.AddMeasurement(CircVal<UnsignedDegRange>(200.), 1);
        A1.AddMeasurement(CircVal<UnsignedDegRange>(300.), 2);
        A1.AddMeasurement(CircVal<UnsignedDegRange>( 20.), 6);

        CircVal<UnsignedDegRange> ad1;
        A1.GetAvrg(ad1);
    }

    // ------------------------------------------------------
    // code used to collect data for RMS error of average estimation based on noisy measurements
    {
        std::default_random_engine rand_engine;
        std::random_device         rnd_device ;
        rand_engine.seed(rnd_device()); // reseed engine

        ofstream f1("log1.txt");

        auto numbers = std::views::iota(1, 101); // for each value of standard-deviation: 1..100
        std::for_each(std::execution::par, numbers.begin(), numbers.end(), [&rand_engine, &f1](int nStdDev) 
        {
            std::osyncstream(std::cout) << "StdDev: " << nStdDev << endl;

            uniform_real_distribution<double> ud(0., 360.);

            double fSumSqrErr1 = 0.;
            double fSumSqrErr2 = 0.;

            const size_t nTrails  = 50000;                  // number of trails
            const size_t nSamples =  1000;                  // number of observations per trail

            vector<CircVal<UnsignedDegRange>> vInput(nSamples);

            const double fAvrg = ud(rand_engine);           // our const parameter for this trail
            wrapped_normal_distribution          <double> r_wnd1(fAvrg, nStdDev,                       0., 360.);
         // wrapped_truncated_normal_distribution<double> r_wnd1(fAvrg, nStdDev, fAvrg-45., fAvrg+45., 0., 360.);

            for (size_t t = 0; t < nTrails; ++t)
            {
                for (auto& Sample : vInput)
                    Sample = r_wnd1(rand_engine);           // generate "noisy" observation

                set<CircVal<UnsignedDegRange>> sAvrg1 = CircAverage(vInput);                   // avrg - method 1 (new method)

                double fSigSin = 0.;
                double fSigCos = 0.;

                for (const auto& Sample : vInput)
                {
                    fSigSin += sin(Sample);
                    fSigCos += cos(Sample);
                }

                CircVal<UnsignedDegRange> Avrg2 = atan2<UnsignedDegRange>(fSigSin, fSigCos);   // avrg - method 2 (conventional method)

                const double fErr1 = CircVal<UnsignedDegRange>::Sdist(*sAvrg1.begin(), fAvrg); // error of estimate - method 1
                const double fErr2 = CircVal<UnsignedDegRange>::Sdist(Avrg2          , fAvrg); // error of estimate - method 2

                fSumSqrErr1 += Sqr(fErr1);
                fSumSqrErr2 += Sqr(fErr2);
            }

            const double fRMS1 = sqrt(fSumSqrErr1 / (nTrails-1));  // root mean square error - method 1
            const double fRMS2 = sqrt(fSumSqrErr2 / (nTrails-1));  // root mean square error - method 2

            std::osyncstream(f1) << nStdDev << "\t" << fRMS1 << "\t" << fRMS2 << endl; // save RMS results to file
        } );
    }

    // -----------------------------------
    auto Duration = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - Time0).count();
    cout << Duration << endl;

    // system ("pause");

    return 0;
}
