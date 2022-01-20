#ifndef WaveShape_H
#define WaveShape_H

#include "BaseShape.h"
#include <complex>

// hand-written cube points and normals.. if only there were a way to do this procedurally
// data format {p1-1, n1, p1-2, n1, p1-3, n3.....}


class WaveShape : public BaseShape
{
public:
    WaveShape();
    WaveShape(int param1, int param2, float surface_size, float wind_speed, float amplitude, glm::vec2 wind);
    WaveShape(int param1, int param2);
    ~WaveShape();
    void WaveUpdate();
    void addTime(float t);
    std::vector<glm::vec3>* get_vertex();
    std::vector<glm::vec3>* get_normal();
    int m_param1;
    int m_param2;

private:
    virtual void CreateVertices();
    virtual void Tesselate(float t);
    virtual void Tesselate_gerstener(float t);
    void init_params();

    int m_wave_num = 3;
    glm::vec2 m_wind;
    std::vector<glm::vec2> m_waves;
    std::vector<float> m_amplitudes;
    std::vector<float> m_wavelengths;
    std::vector<float> m_ks;
    std::vector<float> m_frequencies;
    std::vector<float> m_phases;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_mesh_vetices;
    std::vector<glm::vec3> m_normals;
    int m_face[4];

    bool use_FFT = true;
    std::vector<std::complex<float>> amplitude_field;
    std::vector<std::complex<float>> amplitude_field2;
    float m_size;
    float wind_speed;
    float amplitude;
    void inverse_fourier(glm::vec2 x, std::vector<std::complex<float>>& wave_field,
                         float& height, float& norm_x, float& norm_y);

    float m_time = 0;
    float m_prev_time = 0;
};

#endif // WaveShape_H
