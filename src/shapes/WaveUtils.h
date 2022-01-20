#ifndef WAVEUTILS_H
#define WAVEUTILS_H

#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>
#include <math.h>
#include <complex.h>
#include <random>


// useful functions
inline float phillips_spectrum(glm::vec2 wave, glm::vec2 wind, float amplitude, float L)
{
    float cos_factor = std::abs(glm::dot(glm::normalize(wave), glm::normalize(wind)));
    cos_factor = cos_factor*cos_factor;
    float k = glm::length(wave);
    if (k==0) return 0;
    float fix_factor = std::exp(k*k*0.00001f);
    return amplitude*std::exp(-1/std::pow(k*L, 2))/std::pow(k, 4)*cos_factor*fix_factor;
}

inline std::complex<float> wave_fourier_amplitude(std::complex<float> gaussian, float spectrum)
{
    return 0.70710678118f*gaussian*std::sqrt(spectrum);
}


inline std::complex<float> wave_height_field(std::complex<float> amplitude1, std::complex<float> amplitude2, float t, float frequency)
{
    std::complex<float> x(0, std::sqrt(9.8*frequency)*t);
    return amplitude1*std::exp(x) + amplitude2*std::exp(std::conj(x));
}

#endif // WAVEUTILS_H
