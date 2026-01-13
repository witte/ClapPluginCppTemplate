module;
export module Utils;
import std;


namespace utils
{

export inline constexpr auto minusInfinitydB = -96.0;
export inline constexpr auto twelvedBGain = 3.9810717874921617;  // decibelsToGain(12.0)

export double decibelsToGain(double decibels)
{
    return decibels > minusInfinitydB ? std::pow(10.0f, decibels * 0.05f)
                                      : 0.0;
}

export double gainToDecibels(double gain)
{
    return gain > double() ? std::max(minusInfinitydB, std::log10f(gain) * 20.0)
                           : minusInfinitydB;
}

export double toExponentialCurve(double linearValue)
{
    return std::pow(linearValue, 3) * twelvedBGain;
}

export double toLinearCurve(double exponentialValue)
{
    return std::cbrt(exponentialValue / twelvedBGain);
}

}
