#ifndef FPM_MATH_H
#define FPM_MATH_H

#include "fixed.h"
#include <cmath>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace fpm
{

//
// Helper functions
//
namespace detail
{

// Returns the index of the most-signifcant set bit
inline long find_highest_bit(unsigned long value) noexcept
{
    assert(value != 0);
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse(&index, value);
    return index;
#elif defined(__GNUC__) || defined(__clang__)
    return sizeof(value) * 8 - 1 - __builtin_clzl(value);
#else
#   error "your platform does not support find_highest_bit()"
#endif
}

}

//
// Classification methods
//

template <typename B, typename I, unsigned int F>
constexpr inline int fpclassify(fixed<B, I, F> x) noexcept
{
    return (x.raw_value() == 0) ? FP_ZERO : FP_NORMAL;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isfinite(fixed<B, I, F>) noexcept
{
    return true;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isinf(fixed<B, I, F>) noexcept
{
    return false;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isnan(fixed<B, I, F>) noexcept
{
    return false;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isnormal(fixed<B, I, F>) noexcept
{
    return true;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool signbit(fixed<B, I, F> x) noexcept
{
    return x.raw_value() < 0;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isgreater(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return x > y;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isgreaterequal(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return x >= y;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isless(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return x < y;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool islessequal(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return x <= y;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool islessgreater(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return x != y;
}

template <typename B, typename I, unsigned int F>
constexpr inline bool isunordered(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return false;
}

//
// Nearest integer operations
//
template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> ceil(fixed<B, I, F> x) noexcept
{
    constexpr auto FRAC = B(1) << F;
    auto value = x.raw_value();
    if (value > 0) value += FRAC - 1;
    return fixed<B, I, F>::from_raw_value(value / FRAC * FRAC);
}

template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> floor(fixed<B, I, F> x) noexcept
{
    constexpr auto FRAC = B(1) << F;
    auto value = x.raw_value();
    if (value < 0) value -= FRAC - 1;
    return fixed<B, I, F>::from_raw_value(value / FRAC * FRAC);
}

template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> trunc(fixed<B, I, F> x) noexcept
{
    constexpr auto FRAC = B(1) << F;
    return fixed<B, I, F>::from_raw_value(x.raw_value() / FRAC * FRAC);
}

template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> round(fixed<B, I, F> x) noexcept
{
    constexpr auto FRAC = B(1) << F;
    auto value = x.raw_value() / (FRAC / 2);
    return fixed<B, I, F>::from_raw_value(((value / 2) + (value % 2)) * FRAC);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> nearbyint(fixed<B, I, F> x) noexcept
{
    // Rounding mode is assumed to be FE_TONEAREST
    constexpr auto FRAC = B(1) << F;
    auto value = x.raw_value();
    const bool is_half = std::abs(value % FRAC) == FRAC / 2;
    value /= FRAC / 2;
    value = (value / 2) + (value % 2);
    value -= (value % 2) * is_half;
    return fixed<B, I, F>::from_raw_value(value * FRAC);
}

template <typename B, typename I, unsigned int F>
constexpr inline fixed<B, I, F> rint(fixed<B, I, F> x) noexcept
{
    // Rounding mode is assumed to be FE_TONEAREST
    return nearbyint(x);
}

//
// Mathematical functions
//
template <typename B, typename I, unsigned int F>
constexpr inline fixed<B, I, F> abs(fixed<B, I, F> x) noexcept
{
    return (x >= fixed<B, I, F>{0}) ? x : -x;
}

template <typename B, typename I, unsigned int F, typename C, typename J, unsigned int G>
constexpr inline fixed<B, I, F> copysign(fixed<B, I, F> x, fixed<C, J, G> y) noexcept
{
    return
        x = abs(x),
        (y >= fixed<C, J, G>{0}) ? x : -x;
}

template <typename B, typename I, unsigned int F>
constexpr inline fixed<B, I, F> fmod(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return
        assert(y.raw_value() != 0),
        fixed<B, I, F>::from_raw_value(x.raw_value() % y.raw_value());
}

template <typename B, typename I, unsigned int F>
constexpr inline fixed<B, I, F> remainder(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    return
        assert(y.raw_value() != 0),
        x - nearbyint(x / y) * y;
}

template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> remquo(fixed<B, I, F> x, fixed<B, I, F> y, int* quo) noexcept
{
    assert(y.raw_value() != 0);
    assert(quo != nullptr);
    *quo = x.raw_value() / y.raw_value();
    return fixed<B, I, F>::from_raw_value(x.raw_value() % y.raw_value());
}

//
// Power functions
//

template <typename B, typename I, unsigned int F, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
fixed<B, I, F> pow(fixed<B, I, F> base, T exp) noexcept
{
    using Fixed = fixed<B, I, F>;
    constexpr auto FRAC = B(1) << F;

    if (base == Fixed(0)) {
        assert(exp > 0);
        return Fixed(0);
    }

    Fixed result {1};
    if (exp < 0)
    {
        for (Fixed intermediate = base; exp != 0; exp /= 2, intermediate *= intermediate)
        {
            if ((exp % 2) != 0)
            {
                result /= intermediate;
            }
        }
    }
    else
    {
        for (Fixed intermediate = base; exp != 0; exp /= 2, intermediate *= intermediate)
        {
            if ((exp % 2) != 0)
            {
                result *= intermediate;
            }
        }
    }
    return result;
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> pow(fixed<B, I, F> base, fixed<B, I, F> exp) noexcept
{
    using Fixed = fixed<B, I, F>;

    if (base == Fixed(0)) {
        assert(exp > Fixed(0));
        return Fixed(0);
    }

    if (exp < Fixed(0))
    {
        return 1 / pow(base, -exp);
    }

    constexpr auto FRAC = B(1) << F;
    if (exp.raw_value() % FRAC == 0)
    {
        // Non-fractional exponents are easier to calculate
        return pow(base, exp.raw_value() / FRAC);
    }

    // For negative bases we do not support fractional exponents.
    // Technically fractions with odd denominators could work,
    // but that's too much work to figure out.
    assert(base > Fixed(0));
    return exp2(log2(base) * exp);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> exp(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    if (x < Fixed(0)) {
        return 1 / exp(-x);
    }
    constexpr auto FRAC = B(1) << F;
    const B x_int = x.raw_value() / FRAC;
    x -= x_int;
    assert(x >= Fixed(0) && x < Fixed(1));

    constexpr Fixed fA { 1.3903728105644451e-2 };
    constexpr Fixed fB { 3.4800571158543038e-2 };
    constexpr Fixed fC { 1.7040197373796334e-1 };
    constexpr Fixed fD { 4.9909609871464493e-1 };
    constexpr Fixed fE { 1.0000794567422495 };
    constexpr Fixed fF { 9.9999887043019773e-1 };
    return pow(Fixed::E, x_int) * (((((fA * x + fB) * x + fC) * x + fD) * x + fE) * x + fF);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> exp2(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    if (x < Fixed(0)) {
        return 1 / exp2(-x);
    }
    constexpr auto FRAC = B(1) << F;
    const B x_int = x.raw_value() / FRAC;
    x -= x_int;
    assert(x >= Fixed(0) && x < Fixed(1));

    constexpr Fixed fA { 1.8964611454333148e-3 };
    constexpr Fixed fB { 8.9428289841091295e-3 };
    constexpr Fixed fC { 5.5866246304520701e-2 };
    constexpr Fixed fD { 2.4013971109076949e-1 };
    constexpr Fixed fE { 6.9315475247516736e-1 };
    constexpr Fixed fF { 9.9999989311082668e-1 };
    return Fixed(1 << x_int) * (((((fA * x + fB) * x + fC) * x + fD) * x + fE) * x + fF);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> expm1(fixed<B, I, F> x) noexcept
{
    return exp(x) - 1;
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> log2(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    assert(x > Fixed(0));

    // Normalize input to the [1:2] domain
    B value = x.raw_value();
    const long highest = detail::find_highest_bit(value);
    if (highest >= F) {
        value >>= (highest - F);
    } else {
        value <<= (F - highest);
    }
    x = Fixed::from_raw_value(value);
    assert(x >= Fixed(1) && x < Fixed(2));

    constexpr Fixed fA {  4.4873610194131727e-2 };
    constexpr Fixed fB { -4.1656368651734915e-1 };
    constexpr Fixed fC {  1.6311487636297217 };
    constexpr Fixed fD { -3.5507929249026341 };
    constexpr Fixed fE {  5.0917108110420042 };
    constexpr Fixed fF { -2.8003640347009253 };
    return Fixed(highest - F) + (((((fA * x + fB) * x + fC) * x + fD) * x + fE) * x + fF);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> log(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    return log2(x) / log2(Fixed::E);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> log10(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    return log2(x) / log2(Fixed(10));
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> log1p(fixed<B, I, F> x) noexcept
{
    return log(1 + x);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> cbrt(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;

    if (x == Fixed(0))
    {
        return x;
    }
    if (x < Fixed(0))
    {
        return -cbrt(-x);
    }
    assert(x >= Fixed(0));

    // Finding the cube root of an integer, taken from Hacker's Delight,
    // based on the square root algorithm.

    // We start at the greatest power of eight that's less than the argument.
    int ofs = ((detail::find_highest_bit(x.raw_value()) + 2*F) / 3 * 3);
    I num = I{x.raw_value()};
    I res = 0;

    const auto do_round = [&]
    {
        for (; ofs >= 0; ofs -= 3)
        {
            res += res;
            const I val = (3*res*(res + 1) + 1) << ofs;
            if (num >= val)
            {
                num -= val;
                res++;
            }
        }
    };

    // We should shift by 2*F (since there are two multiplications), but that
    // could overflow even the intermediate type, so we have to split the
    // algorithm up in two rounds of F bits each. Each round will deplete
    // 'num' digit by digit, so after a round we can shift it again.
    num <<= F;
    ofs -= F;
    do_round();

    num <<= F;
    ofs += F;
    do_round();

    return Fixed::from_raw_value(res);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> sqrt(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;

    assert(x >= Fixed(0));
    if (x == Fixed(0))
    {
        return x;
    }

    // Finding the square root of an integer in base-2, from:
    // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29

    // Shift by F first because it's fixed-point.
    I num = I{x.raw_value()} << F;
    I res = 0;

    // "bit" starts at the greatest power of four that's less than the argument.
    for (I bit = I{1} << ((detail::find_highest_bit(x.raw_value()) + F) / 2 * 2); bit != 0; bit >>= 2)
    {
        const I val = res + bit;
        res >>= 1;
        if (num >= val)
        {
            num -= val;
            res += bit;
        }
    }

    // Round the last digit up if necessary
    if (num > res)
    {
        res++;
    }

    return Fixed::from_raw_value(res);
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> hypot(fixed<B, I, F> x, fixed<B, I, F> y) noexcept
{
    assert(x != 0 || y != 0);
    return sqrt(x*x + y*y);
}

//
// Trigonometry functions
//

template <typename B, typename I, unsigned int F>
fixed<B, I, F> sin(fixed<B, I, F> x) noexcept
{
    // This sine uses a fifth-order curve-fitting approximation originally
    // described by Jasper Vijn on coranac.com which has a worst-case
    // relative error of 0.07% (over [-pi:pi]).
    using Fixed = fixed<B, I, F>;

    // Turn x from [0..2*PI] domain into [0..4] domain
    x = fmod(x, Fixed::TWO_PI);
    x = x / Fixed::HALF_PI;

    // Take x modulo one rotation, so [-4..+4].
    if (x < Fixed(0)) {
        x += Fixed(4);
    }

    int sign = +1;
    if (x > Fixed(2)) {
        // Reduce domain to [0..2].
        sign = -1;
        x -= Fixed(2);
    }

    if (x > Fixed(1)) {
        // Reduce domain to [0..1].
        x = Fixed(2) - x;
    }

    const Fixed x2 = x*x;
    return sign * x * (Fixed::PI - x2*(Fixed::TWO_PI - 5 - x2*(Fixed::PI - 3)))/2;
}

template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> cos(fixed<B, I, F> x) noexcept
{
    return sin(fixed<B, I, F>::HALF_PI + x);
}

template <typename B, typename I, unsigned int F>
inline fixed<B, I, F> tan(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;

    auto cx = cos(x);

    // Tangent goes to infinity at 90 and -90 degrees.
    // We can't represent that with fixed-point maths.
    assert(abs(cx).raw_value() > 1);

    return sin(x) / cx;
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> atan(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    if (x < Fixed(0))
    {
        return -atan(-x);
    }

    if (x > Fixed(1))
    {
        return Fixed::HALF_PI - atan(Fixed(1) / x);
    }

    const Fixed fA(0.0776509570923569);
    const Fixed fB(-0.287434475393028);
    const Fixed fC(0.995181681698119);   // PI/4 - A - B

    const auto xx = x * x;
    return ((fA*xx + fB)*xx + fC)*x;
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> asin(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    assert(x >= Fixed(-1) && x <= Fixed(+1));

    const auto yy = Fixed(1) - x * x;
    if (yy == Fixed(0))
    {
        return copysign(Fixed::HALF_PI, x);
    }
    return atan(x / sqrt(yy));
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> acos(fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    assert(x >= Fixed(-1) && x <= Fixed(+1));

    if (x == Fixed(-1))
    {
        return Fixed::PI;
    }
    const auto yy = Fixed(1) - x * x;
    return Fixed(2)*atan(sqrt(yy) / (Fixed(1) + x));
}

template <typename B, typename I, unsigned int F>
fixed<B, I, F> atan2(fixed<B, I, F> y, fixed<B, I, F> x) noexcept
{
    using Fixed = fixed<B, I, F>;
    if (x == Fixed(0))
    {
        assert(y != Fixed(0));
        return (y > Fixed(0)) ? Fixed::HALF_PI : -Fixed::HALF_PI;
    }
    auto ret = atan(y / x);
    if (x < Fixed(0))
    {
        return (y >= Fixed(0)) ? ret + Fixed::PI : ret - Fixed::PI;
    }
    return ret;
}

}

#endif