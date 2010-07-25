/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/* Contains code written by Christoph Bobeth */

#include <src/rare-b-decays/charm-loops.hh>
#include <src/utils/exception.hh>

#include <cmath>
#include <complex>

#include <gsl/gsl_sf_dilog.h>

namespace wf
{
    using std::atan;
    using std::complex;
    using std::log;
    using std::pow;
    using std::sqrt;

    complex<double>
    CharmLoops::h(const double & mu, const double & s)
    {
        // cf. [BFS2001], Eq. (11), p. 4 in the limit m_q -> 0
        return 4.0 / 9.0 * complex<double>(2.0 / 3.0 + 2.0 * std::log(mu) - std::log(s), M_PI);
    }

    complex<double>
    CharmLoops::h(const double & mu, const double & s, const double & m_q)
    {
        if (m_q < 1e-4)
            return h(mu, s);

        const double z = 4.0 * m_q * m_q / s;
        if (z < 1e-10)
            return complex<double>(-4.0/9.0 * (2.0 * std::log(m_q / mu) + 1.0), 0.0);

        const double sqrt1z = std::sqrt(std::abs(z - 1.0));

        double a = 2.0 * std::log(m_q / mu) - 2.0 / 3.0 - z;
        double b = (2.0 + z) * sqrt1z;
        double rc, ic;
        if (z > 1.0)
        {
            ic = 0.0;
            rc = std::atan(1.0 / sqrt1z);
        }
        else
        {
            ic = -M_PI / 2.0;
            rc = std::log((1.0 + sqrt1z) / std::sqrt(z));
        }

        // cf. [BFS2001], Eq. (11), p. 4
        return -4.0 / 9.0 * (a + b * complex<double>(rc, ic));
    }

    complex<double>
    CharmLoops::A(const double & mu, const double & s, const double & m_b)
    {
            double s_hat = s / m_b / m_b, s_hat2 = s_hat * s_hat, denom = 1 - s_hat;
            double ln = log(s_hat), ln2 = ln * ln, li_2 = gsl_sf_dilog(s_hat);
            double z = 4.0 / s_hat, sqrt1z = sqrt(z - 1.0);

            double a = -104.0 / 243.0 * 2.0 * log(m_b / mu);
            double b = +4.0 * s_hat / 27.0 / denom * (li_2 + ln * log(1.0 - s_hat));
            complex<double> c = +1.0 / 729.0 / pow(denom, 2) * complex<double>(
                    6.0 * s_hat * (29.0 - 47.0 * s_hat) * ln + 785.0 - 1600.0 * s_hat + 833.0 * s_hat2,
                    6.0 * M_PI * (20.0 - 49.0 * s_hat + 47.0 * s_hat2));
            complex<double> d = -2.0 / 243.0 / pow(denom, 3) * complex<double>(
                    2.0 * sqrt1z * (-4.0 + 9.0 * s_hat - 15.0 * s_hat2 + 4.0 * s_hat * s_hat2) * (M_PI / 2.0 - atan(sqrt1z))
                    + 9.0 * s_hat * s_hat2 * ln2,
                    18.0 * M_PI * s_hat * (1.0 - 2.0 * s_hat) * ln);
            double e = +2.0 * s_hat / 243.0 / pow(denom, 4) * (36.0 * pow(M_PI / 2.0 - atan(sqrt1z), 2) +
                    M_PI * M_PI * (-4.0 + 9.0 * s_hat - 9.0 * s_hat2 + 3.0 * s_hat * s_hat2));

            // cf. [S2004], Eq. (29), p. 8
            complex<double> result = a + b + c + d + e;

            return result;
    }

    complex<double>
    CharmLoops::B(const double & mu, const double & s, const double & m_b)
    {
            double s_hat = s / m_b / m_b, s_hat2 = s_hat * s_hat, denom = 1 - s_hat;
            double ln = log(s_hat), ln2 = ln * ln, li_2 = gsl_sf_dilog(s_hat);
            double z = 4.0 / s_hat, sqrt1z = sqrt(z - 1.0);
            double lnmu = 2.0 * log(m_b / mu);

            complex<double> x1(0.5, 0.5 * sqrt1z), x2(0.5, -0.5 * sqrt1z), x3(0.5, 0.5 / sqrt1z), x4(0.5, -0.5 / sqrt1z);
            gsl_sf_result rp, ip;
            gsl_sf_complex_dilog_e(1.0, arg(-x2 / x1), &rp, &ip);
            complex<double> dilog(rp.val, ip.val);
            complex<double> lx1 = log(x1), lx2 = log(x2), lx3 = log(x3), lx4 = log(x4);


            complex<double> a = 8.0 / 243.0 / s_hat * (complex<double>(4.0 - 34.0 * s_hat, -17.0 * M_PI * s_hat) * lnmu
                    + 8.0 * s_hat * lnmu * lnmu + 17.0 * s_hat * ln * lnmu);
            complex<double> b = (2.0 + s_hat) * sqrt1z / 729.0 / s_hat * (
                        -48.0 * lnmu * (M_PI / 2.0 - atan(sqrt1z)) - 18.0 * M_PI * 2.0 * log(sqrt1z) -12.0 * M_PI * (2.0 * lx1 + lx3 + lx4)
                        + complex<double>(0.0, 1.0) * (
                            3.0 * 4.0 * pow(log(sqrt1z), 2.0) - 5.0 * M_PI * M_PI - 24.0 * dilog
                            +6.0 * (-9.0 * pow(lx1, 2.0) + pow(lx2, 2.0) - 2.0 * pow(lx4, 2.0) + 6.0 * lx1 * lx2
                            -4.0 * lx1 * lx3 + 8.0 * lx1 * lx4))
                        );
            complex<double> c = -2.0 / 243.0 / s_hat / denom * (
                    4.0 * s_hat * (-8.0 + 17.0 * s_hat) * (li_2 + ln * log(1.0 - s_hat))
                    + 3.0 * (2.0 + s_hat) * (3.0 - s_hat) * pow(lx2 - lx1, 2.0)
                    + 12.0 * M_PI * (-6.0 - s_hat + s_hat2) * (M_PI / 2.0 - atan(sqrt1z)));
            complex<double> d = 2.0 / (2187.0 * s_hat * pow(denom, 2.0)) * complex<double>(
                    -18.0 * s_hat * (120.0 - 211.0 * s_hat + 73.0 * s_hat2) * ln
                    -288.0 - 8.0 * s_hat + 934.0 * s_hat2 - 692.0 * s_hat * s_hat2,
                    18.0 * M_PI * s_hat * (82.0 - 173.0 * s_hat + 73.0 * s_hat2));
            complex<double> e = -4.0 / (243.0 * s_hat * pow(denom, 3.0)) * std::complex<double>(
                    -2.0 * sqrt1z * (4.0 - 3.0 * s_hat - 18.0 * s_hat2 + 16.0 * s_hat * s_hat2 - 5.0 * s_hat2 * s_hat2) * (M_PI / 2 - atan(sqrt1z))
                    -9.0 * s_hat * s_hat2 * ln2,
                    2.0 * M_PI * s_hat * (8.0 - 33.0 * s_hat + 51.0 * s_hat2 - 17.0 * s_hat * s_hat2) * ln);
            complex<double> f = 2.0 / (729.0 * s_hat * pow(denom, 4.0)) * (72.0 * (3.0 - 8.0 * s_hat + 2.0 * s_hat2)
                    * pow(M_PI / 2.0 - atan(sqrt1z), 2.0)
                    - M_PI * M_PI * (54.0 - 53.0 * s_hat - 286.0 * s_hat2 + 612.0 * s_hat * s_hat2 - 446 * s_hat2 * s_hat2 + 113.0 * s_hat2 * s_hat2 * s_hat));

            // cf. [S2004], Eq. (29), p. 8
            return a + b + c + d + e + f;
    }

    complex<double>
    CharmLoops::C(const double & mu, const double & s, const double & m_b)
    {
        static const double zeta3 = 1.20206;

        return complex<double>(16.0 / 81.0 * log(mu * mu / s) + 428.0 / 243.0 - 64.0 / 27.0 * zeta3,
                16.0 / 81.0 * M_PI);
    }

    /* Two-Loop functions for massless quarks from[S2004], suitable for up-quark loops */
    complex<double>
    CharmLoops::F17_massless(const double & mu, const double & s, const double & m_b)
    {
        // cf. [S2004], Eq. (22), p. 7 and consider a global sign (compare [ABGW2003], Eq. (7), p. 8 with [S2004], Eq. (16), p. 6)
        return -A(mu, s, m_b);
    }

    complex<double>
    CharmLoops::F19_massless(const double & mu, const double & s, const double & m_b)
    {
        // cf. [S2004], Eq. (24), p. 7 and consider a global sign (compare [ABGW2003], Eq. (7), p. 8 with [S2004], Eq. (16), p. 6)
        return -B(mu, s, m_b) - 4.0 * C(mu, s, m_b);
    }

    complex<double>
    CharmLoops::F27_massless(const double & mu, const double & s, const double & m_b)
    {
        // cf. [S2004], Eq. (23), p. 7 and consider a global sign (compare [ABGW2003], Eq. (7), p. 8 with [S2004], Eq. (16), p. 6)
        return 6.0 * A(mu, s, m_b);
    }

    complex<double>
    CharmLoops::F29_massless(const double & mu, const double & s, const double & m_b)
    {
        // cf. [S2004], Eq. (25), p. 7 and consider a global sign (compare [ABGW2003], Eq. (7), p. 8 with [S2004], Eq. (16), p. 6)
        return 6.0 * B(mu, s, m_b) - 3.0 * C(mu, s, m_b);
    }

    /* Two-Loop functions for charm-quark loops */
    // cf. [AAGW2001], Eq. (56), p. 20
    complex<double>
    CharmLoops::F17_massive(const double & mu, const double & s, const double & m_b, const double & m_c)
    {
        // cf. [ABGW2001], Appendix B, pp. 34-38
        static long double kap1700[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-1.14266, -0.517135}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-2.20356, 1.59186}, {-5.21743, 1.86168}, {0.592593, 3.72337}, {0.395062, 0}, {0, 0}},
            {{1.86366, -3.06235}, {-4.66347, 0}, {0, 3.72337}, {0.395062, 0}, {0, 0}},
            {{-1.21131, 2.89595}, {2.99588, -2.48225}, {-4.14815, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1710[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-2.07503, 1.39626}, {-0.444444, 0.930842}, {0, 0}, {0, 0}, {0, 0}},
            {{-25.9259, 5.78065}, {-3.40101, 13.0318}, {-4.4917, 3.72337}, {0.395062, 0}, {-0.395062, 0}},
            {{11.4229, -15.2375}, {-34.0806, 11.1701}, {10.3704, 18.6168}, {2.37037, 0}, {0, 0}},
            {{11.7509, 15.6984}, {18.9564, -24.8225}, {-14.6173, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1711[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0164609, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{1.03704, 0.930842}, {0.592593, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-4.66347, 0}, {0, 7.44674}, {2.37037, 0}, {0, 0}, {0, 0}},
            {{6.73754, 1.86168}, {1.18519, -7.44674}, {-2.37037, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1720[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.00555556, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-19.4691, 1.59019}, {-11.6779, 0.930842}, {-2.96296, 0}, {-0.395062, 0}, {0, 0}},
            {{-90.4953, 14.7788}, {14.9329, 22.3402}, {-24.438, 3.72337}, {1.18519, 0}, {-1.18519, 0}},
            {{23.8816, -32.8021}, {-82.7915, 39.0954}, {32.2963, 44.6804}, {5.92593, 0}, {0, 0}},
            {{38.1415, 34.8683}, {38.6436, -80.673}, {-41.5802, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1721[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0164609, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{2.37037, 1.86168}, {1.18519, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-13.9904, 3.72337}, {2.37037, 22.3402}, {7.11111, 0}, {0, 0}, {0, 0}},
            {{27.5428, 3.72337}, {2.37037, -29.787}, {-9.48148, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1730[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.00010778, 0.00258567}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.946811, -0.0258567}, {0.488889, 0}, {0.0987654, 0}, {0, 0}, {0, 0}},
            {{-41.9952, 1.63673}, {-30.2091, 0.930842}, {-6.22222, 0}, {-1.18519, 0}, {0, 0}},
            {{-189.354, 25.8196}, {42.6566, 31.0281}, {-57.765, 3.72337}, {2.76543, 0}, {-2.37037, 0}},
            {{45.1784, -52.4207}, {-145.181, 88.7403}, {70.9136, 81.9141}, {11.0617, 0}, {0, 0}},
            {{77.3602, 54.2499}, {58.4491, -184.927}, {-96.0988, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1731[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0164609, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{3.85185, 2.79253}, {1.77778, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-27.3882, 13.0318}, {8.2963, 44.6804}, {14.2222, 0}, {0, 0}, {0, 0}},
            {{69.4495, 1.86168}, {1.18519, -74.4674}, {-23.7037, 0}, {0, 0}, {0, 0}}
        };

        double m_c_hat = m_c / m_b, z = pow(m_c_hat, 2);
        double s_hat = s / pow(m_b, 2);

        const double rho17[4] = {
            1.94955 * pow(m_c_hat, 3), 11.6973 * m_c_hat, 70.1839 * m_c_hat, -3.8991 / m_c_hat + 159.863 * m_c_hat
        };

        // real part
        double r = -208.0 / 243.0 * log(mu / m_b);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 4 ; m++)
                r = r + kap1700[l][m][0] * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap1710[l][m][0] * s_hat * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1711[l][m][0] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 2 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap1720[l][m][0] * s_hat * s_hat * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1721[l][m][0] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap1730[l][m][0] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1731[l][m][0] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 0 ; l < 4; l++)
            r = r + rho17[l] * pow(s_hat, l);

        // imaginary part
        double i = 0.0;

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1700[l][m][1] * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1710[l][m][1] * s_hat * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1711[l][m][1] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1720[l][m][1] * s_hat * s_hat * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1721[l][m][1] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1730[l][m][1] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_c_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1731[l][m][1] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_c_hat), m);

        return std::complex<double>(r, i);
    }

    namespace impl
    {
        complex<double> f27_0(const double & mu, const double & m_b, const double & m_q)
        {
            static long double kap2700[7][5][2] = {
                {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
                {{6.85597, 3.10281}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
                {{13.2214, -9.55118}, {31.3046, -11.1701}, {-3.55556, -22.3402}, {-2.37037, 0}, {0, 0}},
                {{-11.182, 18.3741}, {27.9808, 0}, {0, -22.3402}, {-2.37037, 0}, {0, 0}},
                {{7.26787, -17.3757}, {-17.9753, 14.8935}, {24.8889, 0}, {0, 0}, {0, 0}}
            };

            const double m_q_hat = m_q / m_b, z = pow(m_q_hat, 2);

            const long double rho27 = -11.6973 * pow(m_q_hat, 3);

            // real part
            double r = 416.0 / 81.0 * log(mu / m_b);

            for (int l = 3 ; l < 7 ; l++)
                for (int m = 0 ; m < 4 ; m++)
                    r = r + kap2700[l][m][0] * pow(z, l-3) * pow(log(m_q_hat), m);

            r = r + rho27 ;

            // imaginary part
            double i = 0.0;

            for (int l = 3 ; l < 7 ; l++)
                for (int m = 0 ; m < 3 ; m++)
                    i = i + kap2700[l][m][1] * pow(z, l-3) * pow(log(m_q_hat), m);

            return complex<double>(r, i);
        }
    }

    // cf. [AAGW2001], Eq. (56), p. 20
    complex<double>
    CharmLoops::F27_massive(const double & mu, const double & s, const double & m_b, const double & m_q)
    {
        // cf. [ABGW2001], Appendix B, pp. 34-38
        static long double kap2700[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{6.85597, 3.10281}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{13.2214, -9.55118}, {31.3046, -11.1701}, {-3.55556, -22.3402}, {-2.37037, 0}, {0, 0}},
            {{-11.182, 18.3741}, {27.9808, 0}, {0, -22.3402}, {-2.37037, 0}, {0, 0}},
            {{7.26787, -17.3757}, {-17.9753, 14.8935}, {24.8889, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2710[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{12.4502, -8.37758}, {2.66667, -5.58505}, {0, 0}, {0, 0}, {0, 0}},
            {{155.555, -34.6839}, {20.4061, -78.1908}, {26.9502, -22.3402}, {-2.37037, 0}, {2.37037, 0}},
            {{-68.5374, 91.4251}, {204.484, -67.0206}, {-62.2222, -111.701}, {-14.2222, 0}, {0, 0}},
            {{-70.5057, -94.1903}, {-113.738, 148.935}, {87.7037, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2711[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.0987654, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-6.22222, -5.58505}, {-3.55556, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{27.9808, 0}, {0, -44.6804}, {-14.2222, 0}, {0, 0}, {0, 0}},
            {{-40.4253, -11.1701}, {-7.11111, 44.6804}, {14.2222, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2720[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0333333, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{116.815, -9.54113}, {70.0677, -5.58505}, {17.7778, 0}, {2.37037, 0}, {0, 0}},
            {{542.972, -88.6728}, {-89.5971, -134.041}, {146.628, -22.3402}, {-7.11111, 0}, {7.11111, 0}},
            {{-143.29, 196.813}, {496.749, -234.572}, {-193.778, -268.083}, {-35.5556, 0}, {0, 0}},
            {{-228.849, -209.21}, {-231.862, 484.038}, {249.481, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2721[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.0987654, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-14.2222, -11.1701}, {-7.11111, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{83.9424, -22.3402}, {-14.2222, -134.041}, {-42.6667, 0}, {0, 0}, {0, 0}},
            {{-165.257, -22.3402}, {-14.2222, 178.722}, {56.8889, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2730[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.000646678, -0.015514}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-5.68087, 0.15514}, {-2.93333, 0}, {-0.592593, 0}, {0, 0}, {0, 0}},
            {{251.971, -9.82039}, {181.255, -5.58505}, {37.3333, 0}, {7.11111, 0}, {0, 0}},
            {{1136.13, -154.918}, {-255.94, -186.168}, {346.59, -22.3402}, {-16.5926, 0}, {14.2222, 0}},
            {{-271.07, 314.524}, {871.089, -532.442}, {-425.481, -491.485}, {-66.3704, 0}, {0, 0}},
            {{-464.161, -325.499}, {-350.695, 1109.56}, {576.593, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2731[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.0987654, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-23.1111, -16.7552}, {-10.6667, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{164.329, -78.1908}, {-49.7778, -268.083}, {-85.3333, 0}, {0, 0}, {0, 0}},
            {{-416.697, -11.1701}, {-7.11111, 446.804}, {142.222, 0}, {0, 0}, {0, 0}}
        };

        double m_q_hat = m_q / m_b, z = pow(m_q_hat, 2);
        double s_hat = s / m_b / m_b;

        const long double rho27[4] = {
            -11.6973 * pow(m_q_hat, 3), -70.1839 * m_q_hat, -421.103 * m_q_hat, 23.3946 / m_q_hat - 959.179 * m_q_hat
        };

        if (s_hat == 0)
        {
            return impl::f27_0(mu, m_b, m_q);
        }

        // real part
        double r = 416.0 / 81.0 * log(mu / m_b);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 4 ; m++)
                r = r + kap2700[l][m][0] * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap2710[l][m][0] * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2711[l][m][0] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 2 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap2720[l][m][0] * s_hat * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2721[l][m][0] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap2730[l][m][0] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2731[l][m][0] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0 ; l < 4; l++)
            r = r + rho27[l] * pow(s_hat, l);

        // imaginary part
        double i = 0.0;

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2700[l][m][1] * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2710[l][m][1] * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2711[l][m][1] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2720[l][m][1] * s_hat * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2721[l][m][1] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2730[l][m][1] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2731[l][m][1] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        return complex<double>(r, i);
    }

    // cf. [AAGW2001], Eq. (54), p. 19
    complex<double>
    CharmLoops::F19_massive(const double & mu, const double & s, const double & m_b, const double & m_q)
    {
        // cf. [ABGW2001], Appendix B, pp. 34-38
        static long double kap1900[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-4.61812, 3.67166}, {5.62963, 1.86168}, {0, 0}, {0, 0}, {0, 0}},
            {{14.4621, -16.2155}, {9.59321, -11.1701}, {-1.18519, -7.44674}, {-0.790123, 0}, {0, 0}},
            {{-16.0864, 26.7517}, {54.2439, -14.8935}, {-15.4074, -29.787}, {-3.95062, 0}, {0, 0}},
            {{-14.73, -23.6892}, {-28.5761, 34.7514}, {20.1481, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1901[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0493827, -0.103427}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.592593, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{4.95977, -1.86168}, {-1.18519, -7.44674}, {-2.37037, 0}, {0, 0}, {0, 0}},
            {{-9.20287, -1.65483}, {-1.0535, 9.92898}, {3.16049, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1910[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-2.48507, -0.186168}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{4.47441, -0.310281}, {1.48148, -1.86168}, {0, 0}, {0, 0}, {0, 0}},
            {{71.3855, -30.7987}, {8.47677, -33.5103}, {12.5389, -7.44674}, {-0.790123, 0}, {0.790123, 0}},
            {{-18.1301, 66.1439}, {149.596, -67.0206}, {-49.1852, -81.9141}, {-11.0617, 0}, {0, 0}},
            {{-72.89, -63.7828}, {-68.135, 134.041}, {63.6049, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1911[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-2.66667, -1.86168}, {-1.18519, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{18.6539, -7.44674}, {-4.74074, -29.787}, {-9.48148, 0}, {0, 0}, {0, 0}},
            {{-41.6104, -3.72337}, {-2.37037, 44.6804}, {14.2222, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1920[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.403158, -0.0199466}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0613169, 0.0620562}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{37.1282, -1.36524}, {22.0621, -1.86168}, {5.33333, 0}, {0.790123, 0}, {0, 0}},
            {{212.74, -52.2081}, {-21.9215, -52.1272}, {57.1724, -7.44674}, {-2.37037, 0}, {2.37037, 0}},
            {{-44.6829, 108.713}, {272.015, -163.828}, {-119.111, -156.382}, {-21.3333, 0}, {0, 0}},
            {{-137.203, -106.832}, {-99.437, 330.139}, {168.889, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1921[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.0164609, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-5.33333, -3.72337}, {-2.37037, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{40.786, -22.3402}, {-14.2222, -67.0206}, {-21.3333, 0}, {0, 0}, {0, 0}},
            {{-111.356, 0}, {0, 119.148}, {37.9259, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1930[7][5][2] = {
            {{-0.0759415, -0.00295505}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.00480894, 0.00369382}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-1.81002, 0.0871741}, {-0.919459, 0}, {-0.197531, 0}, {0, 0}, {0, 0}},
            {{79.7475, -1.72206}, {57.3171, -1.86168}, {11.2593, 0}, {2.37037, 0}, {0, 0}},
            {{425.579, -76.6479}, {-68.8016, -69.5029}, {129.357, -7.44674}, {-5.53086, 0}, {4.74074, 0}},
            {{-87.8946, 148.481}, {417.612, -311.522}, {-227.16, -253.189}, {-34.7654, 0}, {0, 0}},
            {{-279.268, -135.118}, {-146.853, 652.831}, {331.259, 0}, {0, 0}, {0, 0}}
        };

        static long double kap1931[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.0219479, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-8.2963, -5.58505}, {-3.55556, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{70.2698, -49.6449}, {-31.6049, -119.148}, {-37.9259, 0}, {0, 0}, {0, 0}},
            {{-231.893, 18.6168}, {11.8519, 248.225}, {79.0123, 0}, {0, 0}, {0, 0}}
        };

        double m_q_hat = m_q / m_b, z = pow(m_q_hat, 2);
        double s_hat = s / m_b / m_b;

        const long double rho19[4] = {
            3.8991 * pow(m_q_hat, 3), -23.3946 * m_q_hat, -140.368 * m_q_hat, 7.79821 / m_q_hat - 319.726 * m_q_hat
        };

        // real part
        double r = (-1424.0 / 729.0 + 64.0 / 27.0 * log(m_q_hat)) * log(mu/m_b)
            - 16.0 / 243.0 * log(mu/m_b) * log(s_hat)
            + (16.0 / 1215.0 - 32.0 / 135.0 /pow(m_q_hat, 2)) * log(mu/m_b) * s_hat
            + (4.0 / 2835.0 - 8.0 / 315.0 /pow(m_q_hat, 4)) * log(mu/m_b) * s_hat * s_hat
            + (16.0 / 76545.0 - 32.0 /8505.0 / pow(m_q_hat, 6)) * log(mu/m_b) * pow(s_hat, 3)
            - 256.0 / 243.0 * pow(log(mu/m_b), 2);

        for (int l = 3  ; l < 7 ; l++)
            for (int m = 0  ; m < 4  ; m++)
                r = r + kap1900[l][m][0] * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3  ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1901[l][m][0] * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 2  ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap1910[l][m][0] * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4  ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1911[l][m][0] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 1  ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap1920[l][m][0] * s_hat * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3  ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1921[l][m][0] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0  ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap1930[l][m][0] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3  ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap1931[l][m][0] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0  ; l < 4; l++)
            r = r + rho19[l] * pow(s_hat, l);

        // imaginary part
        double i = 16.0 / 243.0 * M_PI * log(mu/m_b);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1900[l][m][1] * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1901[l][m][1] * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 2 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1910[l][m][1] * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1911[l][m][1] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1920[l][m][1] * s_hat * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1921[l][m][1] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap1930[l][m][1] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap1931[l][m][1] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        return complex<double>(r, i);
    }

    // cf. [AAGW2001], Eq. (54), p. 19
    complex<double>
    CharmLoops::F29_massive(const double & mu, const double & s, const double & m_b, const double & m_q)
    {
        // cf. [ABGW2001], Appendix B, pp. 34-38
        static long double kap2900[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-24.2913, -22.0299}, {-23.1111, -11.1701}, {0, 0}, {0, 0}, {0, 0}},
            {{-86.7723, 97.2931}, {-57.5593, 67.0206}, {7.11111, 44.6804}, {4.74074, 0}, {0, 0}},
            {{96.5187, -160.51}, {-325.463, 89.3609}, {92.4444, 178.722}, {23.7037, 0}, {0, 0}},
            {{88.3801, 142.135}, {171.457, -208.509}, {-120.889, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2901[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.296296, 0.620562}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{3.55556, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-29.7586, 11.1701}, {7.11111, 44.6804}, {14.2222, 0}, {0, 0}, {0, 0}},
            {{55.2172, 9.92898}, {6.32099, -59.5739}, {-18.963, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2910[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.8462, 1.11701}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-26.8464, 1.86168}, {-8.88889, 11.1701}, {0, 0}, {0, 0}, {0, 0}},
            {{-428.313, 184.792}, {-50.8606, 201.062}, {-75.2337, 44.6804}, {4.74074, 0}, {-4.74074, 0}},
            {{108.781, -396.864}, {-897.575, 402.124}, {295.111, 491.485}, {66.3704, 0}, {0, 0}},
            {{437.34, 382.697}, {408.81, -804.248}, {-381.63, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2911[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{16., 11.1701}, {7.11111, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-111.923, 44.6804}, {28.4444, 178.722}, {56.8889, 0}, {0, 0}, {0, 0}},
            {{249.663, 22.3402}, {14.2222, -268.083}, {-85.3333, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2920[7][5][2] = {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0132191, 0.11968}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.367901, -0.372337}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-222.769, 8.19141}, {-132.372, 11.1701}, {-32., 0}, {-4.74074, 0}, {0, 0}},
            {{-1276.44, 313.249}, {131.529, 312.763}, {-343.034, 44.6804}, {14.2222, 0}, {-14.2222, 0}},
            {{268.098, -652.279}, {-1632.09, 982.969}, {714.667, 938.289}, {128., 0}, {0, 0}},
            {{823.218, 640.989}, {596.622, -1980.83}, {-1013.33, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2921[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.0987654, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{32., 22.3402}, {14.2222, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-244.716, 134.041}, {85.3333, 402.124}, {128., 0}, {0, 0}, {0, 0}},
            {{668.137, 0}, {0, -714.887}, {-227.556, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2930[7][5][2] = {
            {{-0.0142243, 0.0177303}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0.0288536, -0.0221629}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{10.8601, -0.523045}, {5.51675, 0}, {1.18519, 0}, {0, 0}, {0, 0}},
            {{-478.485, 10.3323}, {-343.902, 11.1701}, {-67.5556, 0}, {-14.2222, 0}, {0, 0}},
            {{-2553.47, 459.887}, {412.809, 417.017}, {-776.143, 44.6804}, {33.1852, 0}, {-28.4444, 0}},
            {{527.368, -890.889}, {-2505.67, 1869.13}, {1362.96, 1519.13}, {208.593, 0}, {0, 0}},
            {{1675.61, 810.709}, {881.117, -3916.98}, {-1987.56, 0}, {0, 0}, {0, 0}}
        };

        static long double kap2931[7][5][2] = {
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-0.131687, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{49.7778, 33.5103}, {21.3333, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{-421.619, 297.87}, {189.63, 714.887}, {227.556, 0}, {0, 0}, {0, 0}},
            {{1391.36, -111.701}, {-71.1111, -1489.35}, {-474.074, 0}, {0, 0}, {0, 0}}
        };

        double m_q_hat = m_q / m_b, z = pow(m_q_hat, 2);
        double s_hat = s / m_b / m_b;

        const long double rho29[4] = {
            -23.3946 * pow(m_q_hat, 3), 140.368 * m_q_hat, 842.206 * m_q_hat, -46.7892 / m_q_hat + 1918.36 * m_q_hat
        };

        // real part
        double r = (256.0 / 243.0 - 128.0 / 9.0 * log(m_q_hat)) * log(mu / m_b)
            + 32.0 / 81.0 * log(mu / m_b) * log(s_hat)
            + (-32.0 / 405.0 + 64.0 / 45 / pow(m_q_hat, 2)) * log(mu / m_b) * s_hat
            + (-8.0 / 945.0 + 16.0 / 105 / pow(m_q_hat, 4)) * log(mu / m_b) * s_hat * s_hat
            + (-32.0 / 25515.0 + 64.0 / 2835 / pow(m_q_hat, 6)) * log(mu / m_b) * pow(s_hat, 3)
            + 512.0 / 81.0 * pow(log(mu / m_b), 2);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 4 ; m++)
                r = r + kap2900[l][m][0] * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2901[l][m][0] * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 2 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap2910[l][m][0] * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2911[l][m][0] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap2920[l][m][0] * s_hat * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2921[l][m][0] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0 ; l < 7 ; l++)
            for (int m = 0 ; m < 5 ; m++)
                r = r + kap2930[l][m][0] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                r = r + kap2931[l][m][0] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0 ; l < 4; l++)
            r = r + rho29[l] * pow(s_hat, l);

        // imaginary part
        double i = - 32.0 / 81.0 * M_PI * log(mu/m_b);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2900[l][m][1] * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 3 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2901[l][m][1] * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 2 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2910[l][m][1] * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2911[l][m][1] * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 1 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2920[l][m][1] * s_hat * s_hat * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2921[l][m][1] * s_hat * s_hat * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 0 ; l < 7 ; l++)
            for (int m = 0 ; m < 3 ; m++)
                i = i + kap2930[l][m][1] * pow(s_hat, 3) * pow(z, l-3) * pow(log(m_q_hat), m);

        for (int l = 4 ; l < 7 ; l++)
            for (int m = 0 ; m < 2 ; m++)
                i = i + kap2931[l][m][1] * pow(s_hat, 3) * log(s_hat) * pow(z, l-3) * pow(log(m_q_hat), m);

        return complex<double>(r, i);
    }

    // cf. [BFS2001], Eq. (82), p. 30
    complex<double>
    CharmLoops::F87_massless(const double & mu, const double & s, const double & m_q)
    {
        // Loop-Functions are calculated for the pole mass!
        double s_hat = s / (m_q * m_q);
        double s_hat2 = s_hat * s_hat;
        double denom = (1.0 - s_hat);
        double denom2 = denom * denom;

        complex<double> a = complex<double>(-32.0 * std::log(mu / m_q) - 8.0 * s_hat / denom * std::log(s_hat)
                - 4.0 * (11.0 - 16.0 * s_hat + 8.0 * s_hat2) / denom2,
                -8.0 * M_PI);
        complex<double> b = (4.0 / denom / denom2)
            * ((9.0 * s_hat - 5.0 * s_hat2 + 2.0 * s_hat * s_hat2) * B0(s, m_q) - (4.0 + 2.0 * s_hat) * C0(s, m_q));

        return (1.0 / 9.0) * (a + b);
    }

    // cf. [BFS2001], Eq. (83), p. 30
    complex<double>
    CharmLoops::F89_massless(const double & mu, const double & s, const double & m_q)
    {
        // Loop-Functions are calculated for the pole mass!
        double s_hat = s / (m_q * m_q);
        double denom = (1.0 - s_hat);
        double denom2 = denom * denom;

        double a = 16.0 * std::log(s_hat) / denom + 8.0 * (5.0 - 2.0 * s_hat) / denom2;
        complex<double> b = (-8.0 * (4.0 - s_hat) / denom / denom2) * ((1.0 + s_hat) * B0(s, m_q) - 2.0 * C0(s, m_q));

        return (1.0 / 9.0) * (a + b);
    }

    complex<double>
    CharmLoops::B0(const double & s, const double & m_q)
    {
        if ((0.0 == m_q) && (0.0 == s))
        {
            throw InternalError("Implementation<BToKstarDilepton<LargeRecoil>>::B0: m_q == 0 & s == 0");
        }

        if (0.0 == s)
            return complex<double>(-2.0, 0.0);

        double z = 4.0 * m_q * m_q / s;
        complex<double> result;

        if (z > 1.0)
        {
            result = complex<double>(-2.0 * std::sqrt(z - 1.0) * std::atan(1.0 / std::sqrt(z - 1.0)), 0.0);
        }
        else
        {
            result = complex<double>(std::sqrt(1.0 - z) * std::log((1.0 - std::sqrt(1 - z)) / (1.0 + std::sqrt(1.0 - z))),
                    std::sqrt(1.0 - z) * M_PI);
        }

        return result;
    }

    complex<double>
    CharmLoops::C0(const double & s, const double & m_q)
    {
        if (0.0 == s)
            return complex<double>(-M_PI * M_PI / 6.0, 0.0);

        double s_hat = s/(m_q * m_q);
        double A = sqrt(s_hat * (4.0 - s_hat));
        double at1 = atan(A / (2.0 - s_hat));
        double at2 = atan(A / s_hat);
        double log1 = log(2.0 - s_hat);

        complex<double> arg;
        gsl_sf_result res_re, res_im;

        arg = 0.5 * complex<double>(2.0 - s_hat, -A);
        int status = gsl_sf_complex_dilog_e(abs(arg), atan2(imag(arg), real(arg)), &res_re, &res_im);
        complex<double> Li_1(res_re.val, res_im.val);

        arg = 0.5 * complex<double>(2.0 - s_hat, +A);
        status = gsl_sf_complex_dilog_e(abs(arg), atan2(imag(arg), real(arg)), &res_re, &res_im);
        complex<double> Li_2(res_re.val, res_im.val);

        arg = 0.5 * complex<double>(1.0, -A / (2.0 - s_hat));
        status = gsl_sf_complex_dilog_e(abs(arg), atan2(imag(arg), real(arg)), &res_re, &res_im);
        complex<double> Li_3(res_re.val, res_im.val);

        arg = 0.5 * complex<double>(1.0, +A / (2.0 - s_hat));
        status = gsl_sf_complex_dilog_e(abs(arg), atan2(imag(arg), real(arg)), &res_re, &res_im);
        complex<double> Li_4(res_re.val, res_im.val);

        return 1.0 / (1.0 - s_hat) * (2.0 * at1 * (at1 - at2) + log1 * log1 - Li_1 - Li_2 + Li_3 + Li_4);
    }
}