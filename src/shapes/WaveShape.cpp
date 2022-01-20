#include "WaveShape.h"
#include <iostream>
#include <random>
#include <iterator>
#include "WaveUtils.h"

WaveShape::WaveShape()
{
}



WaveShape::WaveShape(int param1, int param2) :
    m_param1(param1),
    m_param2(param2)
{
    init_params();

    use_FFT = false;
    CreateVertices();
    Tesselate_gerstener(m_time);
    initializeOpenGLShapeProperties();
}



WaveShape::~WaveShape()
{
}

void WaveShape::CreateVertices()
{
    m_vertices.push_back(glm::vec3(-0.5f, 0.f, -0.5f));
    m_vertices.push_back(glm::vec3(-0.5f, 0.f,  0.5f));
    m_vertices.push_back(glm::vec3( 0.5f, 0.f, -0.5f));
    m_vertices.push_back(glm::vec3( 0.5f, 0.f,  0.5f));
}

void WaveShape::init_params()
{
    m_waves = {glm::vec2(4,5), glm::vec2(1,-6), glm::vec2(-5,-1) };
    m_amplitudes = {0.002, 0.008, 0.01};
    m_wavelengths = {0.10, 0.06, 0.30};
    m_phases = {0.2*M_PI, 0.8*M_PI, 0.4*M_PI};

    for (int w=0;w<m_wave_num;w++)
    {
        float wl = m_wavelengths[w];
        float k = 2*M_PI/wl;
        m_ks.push_back(k);
        m_waves[w] = glm::normalize(m_waves[w])*k;

        float omega = std::sqrt(9.8*k*(1+k*k*0.1*0.1));
        omega = 3*int(omega/3);
        m_frequencies.push_back(omega);
    }
}


WaveShape::WaveShape(int param1, int param2, float surface_size, float wind_speed,
                     float amplitude, glm::vec2 wind) :
    m_param1(param1), m_param2(param2), m_wind(glm::normalize(wind)),
    m_size(surface_size), wind_speed(wind_speed), amplitude(amplitude)
{
    init_params();

    use_FFT = true;
    CreateVertices();

    glm::vec3 p1 = m_vertices[0];
    glm::vec3 p2 = m_vertices[1];
    glm::vec3 p3 = m_vertices[2];
    glm::vec3 p4 = m_vertices[3];


    int offset1 = m_param1/2, offset2 = m_param2/2;
    float ratio = (2*M_PI)/m_size;
    float rise = wind_speed*wind_speed/9.8;

    const float mean = 0.0;
    const float stddev = 1.0;
    std::default_random_engine generator;
    std::normal_distribution<float> dist(mean, stddev);

    amplitude_field.reserve((m_param1+1)*(m_param2+1));
    amplitude_field2.reserve((m_param1+1)*(m_param2+1));

    for(int i=0;i<m_param1+1;i++)
    {
        for(int j=0;j<m_param2+1;j++)
        {
            glm::vec2 k(j-offset2, i-offset1);
            k = k*ratio;
            float specturm = phillips_spectrum(k, m_wind, amplitude, rise);
            float specturm2 = phillips_spectrum(-k, m_wind, amplitude, rise);
            std::complex<float> gaussian(dist(generator), dist(generator));

            std::complex<float> amplitude = wave_fourier_amplitude(gaussian, specturm);
            std::complex<float> amplitude_2 = wave_fourier_amplitude(gaussian, specturm2);
            amplitude_2 = std::conj(amplitude_2);

            amplitude_field.push_back(amplitude);
            amplitude_field2.push_back(amplitude_2);
        }
    }
    Tesselate(m_time);
    initializeOpenGLShapeProperties();
}

void WaveShape::inverse_fourier(glm::vec2 x, std::vector<std::complex<float>>& wave_field,
                                float& height, float& norm_x, float& norm_y)
{
    int offset1 = m_param1/2, offset2 = m_param2/2;
    float ratio = (2*M_PI)/m_size;
    for(int i=0;i<m_param1+1;i++)
    {
        for(int j=0;j<m_param2+1;j++)
        {
            if(i%3!=0 || j%3!=0) continue;

            glm::vec2 k(j-offset2, i-offset1);
            k = k*ratio;

            std::complex<float> h_component = wave_field[i*(m_param2+1)+j];
            std::complex<float> exp_term(0, glm::dot(x, k));
            exp_term = std::exp(exp_term);
            std::complex<float> H = h_component*exp_term;
            height += H.real();
            norm_x += -k.x*H.imag();
            norm_y += -k.y*H.imag();
        }
    }
    height *= 9;
    norm_x *= 9;
    norm_y *= 9;
}

void WaveShape::Tesselate(float time)
{
    glm::vec3 p1 = m_vertices[0];
    glm::vec3 p2 = m_vertices[1];
    glm::vec3 p3 = m_vertices[2];
    glm::vec3 p4 = m_vertices[3];

//    std::vector<glm::vec3> mesh_vetices;
    m_mesh_vetices.clear();
    m_mesh_vetices.reserve((m_param2+1)*(m_param2+1));
    m_normals.clear();
    m_normals.reserve((m_param2+1)*(m_param2+1));
//    std::vector<glm::vec3> mesh_normals;
    std::vector<glm::vec2> texture_uvs;

    std::vector<std::complex<float>> wave_field;
    int offset1 = m_param1/2, offset2 = m_param2/2;
    float ratio = (2*M_PI)/m_size;
    for(int i=0;i<m_param1+1;i++)
    {
        for(int j=0;j<m_param2+1;j++)
        {
//            if(i%3!=0 || j%3!=0) continue;

            glm::vec2 k(j-offset2, i-offset1);
            k = k*ratio;
            std::complex<float> h_component = wave_height_field(
                                                    amplitude_field[i*(m_param2+1)+j],
                                                    amplitude_field2[i*(m_param2+1)+j],
                                                    time, glm::length(k));
            wave_field.push_back(h_component);
        }
    }

    ratio = m_size;

    glm::vec3 normal, ref;
    glm::vec2 tmp;
    glm::vec3 point;
    float dis;
    for(int i=0;i<m_param1+1;i++)
    {
        float step1 = (float)i/m_param1;
        glm::vec3 start = p1*(1.f - step1 ) + p2*step1;
        glm::vec3 end = p3*(1.f - step1 ) + p4*step1;
        for(int j=0;j<m_param2+1;j++)
        {
            float step2 = (float)j/m_param2;
            point = start*(1.f - step2 ) + end*step2;

            float height=0, norm_x=0, norm_y=0;
            glm::vec2 x(point.x, point.z);
            x = (x-0.5f)*ratio;
            inverse_fourier(x, wave_field, height, norm_x, norm_y);

            m_mesh_vetices.push_back(glm::vec3(point.x, height, point.z));
            normal = glm::normalize(glm::vec3(-norm_x/std::sqrt((1+norm_x*norm_x)),
                                              0.707,
                                              -norm_y/std::sqrt((1+norm_y*norm_y)) ));
            m_normals.push_back(normal);
            texture_uvs.push_back(glm::vec2(step2, step1));
        }
    }
//    CalcNormals(&mesh_vetices, &mesh_normals, m_param1, m_param2);
//    std::cout << "recalculate \n";
    LoadVBOInput(&m_vertexData, &m_normals, &m_mesh_vetices, &texture_uvs, false, m_param1, m_param2);
}

std::vector<glm::vec3>* WaveShape::get_vertex()
{
    return &m_mesh_vetices;
}

std::vector<glm::vec3>* WaveShape::get_normal()
{
    return &m_normals;
}

void WaveShape::Tesselate_gerstener(float time)
{
    glm::vec3 p1 = m_vertices[0];
    glm::vec3 p2 = m_vertices[1];
    glm::vec3 p3 = m_vertices[2];
    glm::vec3 p4 = m_vertices[3];

    std::vector<glm::vec3> mesh_vetices;
    std::vector<glm::vec3> mesh_normals;
    std::vector<glm::vec2> texture_uvs;


    for(int i=0;i<m_param1+1;i++)
    {
        float step1 = (float)i/m_param1;
        glm::vec3 start = p1*(1.f - step1 ) + p2*step1;
        glm::vec3 end = p3*(1.f - step1 ) + p4*step1;
        for(int j=0;j<m_param2+1;j++)
        {
            float step2 = (float)j/m_param2;
            glm::vec3 point = start*(1.f - step2 ) + end*step2;

            float y = 0;
            glm::vec2 wave, org = {point.x, point.z}, cur = {point.x, point.z};
            float amplitude, length, k, fre, phase;
            for (int w=0;w<m_wave_num;w++)
            {
                wave = m_waves[w];
                amplitude = m_amplitudes[w];
                length = m_wavelengths[w];
                k = m_ks[w];
                fre = m_frequencies[w];
                phase = m_phases[w];

                cur = cur - wave/k*amplitude*std::sin(glm::dot(wave, org) - fre*time + phase);
                y += amplitude*std::cos(glm::dot(wave, org) - fre*time + phase);
            }
            mesh_vetices.push_back(glm::vec3(cur.x, y, cur.y));
            texture_uvs.push_back(glm::vec2(step2, step1));
        }
    }

    CalcNormals(&mesh_vetices, &mesh_normals, m_param1, m_param2);

//    std::cout << "recalculate \n";
    LoadVBOInput(&m_vertexData, &mesh_normals, &mesh_vetices, &texture_uvs, false, m_param1, m_param2);
}

void WaveShape::addTime(float t)
{
    m_time += t;
}

void WaveShape::WaveUpdate()
{
    if(m_time-m_prev_time>0.03)
    {
        m_prev_time = m_time;

        m_vertexData.clear();

        if(use_FFT) Tesselate(m_time*0.2);
        else Tesselate_gerstener(m_time*0.2);

        m_VAO.release() ;
        initializeOpenGLShapeProperties();
    }

}
