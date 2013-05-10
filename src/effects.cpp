#include "effects.h"
#include "color.h"

#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>
#include <ctime>

#define SIMULATION_HEIGHT 100
#define SIMULATION_WIDTH 50
#define NUM_PARTICLES 180

int bufferWidth = 0;
int bufferHeight = 0;

class Particle
{
public:
    int slot;
    float height;
    float velocity;
    HsvColor color;
    int randomOffset;

    Particle()
    {
    }

    void launch()
    {
        slot = rand() % SIMULATION_WIDTH;
        height = SIMULATION_HEIGHT;
        velocity = 0.3;
        color.h = 255;
        color.s = 200;
        color.v = 180;
        randomOffset = rand() % 10000;
    }
    
    void update()
    {
        height += velocity;
        if (height <= 0 || height >= SIMULATION_HEIGHT)
            launch();
    }
    
    void render(uint8_t* data)
    {
        
    }
    
};

std::deque<int> slotQueue;
std::vector<float> slotSeeds;
std::vector<Particle> particles;


void initEffects(int width, int height)
{
    bufferWidth = width;

    bufferHeight = height;
    
    for (int ii = 0; ii < width; ii++)
    {
        slotQueue.push_back(ii);
        slotSeeds.push_back((float)rand() / (float)RAND_MAX);
    }
    std::random_shuffle(slotQueue.begin(), slotQueue.end());
}

void paintHSV(uint8_t* data, int x, int y, HsvColor& hsv)
{
    RgbColor rgb = HsvToRgb(hsv);
    int index = (x + bufferWidth * y) * 3;

    data[index+0] = data[index+0] + rgb.r;
    data[index+1] = data[index+1] + rgb.g;
    data[index+2] = data[index+2] + rgb.b;
}

void runWaterfallEffect(uint8_t* data)
{
    int radius = 10;
    if (particles.size() < NUM_PARTICLES)
    {
        particles.push_back(Particle());
        particles.rbegin()->launch();
    }
    
    for (int pp = 0; pp < particles.size(); pp++)
    {
        particles[pp].update();
        particles[pp].render(data);
        
        for (int jj = 0; jj < bufferHeight; jj++)
        {
            float d = SIMULATION_HEIGHT / (bufferHeight * 1.0) - particles[jj].height;
            float dsqr = pow(d, 2);
            float rsqr = pow(radius, 2);
            float v = 1.0 / (1.0 + (2.0/radius) * d + dsqr/(rsqr*1.0));
            particles[jj].color.v = v;
            paintHSV(data, particles[pp].slot, jj, particles[pp].color);
        }
    }
}

float lfo_ramp(float period, float offset = 0)
{
    float t = ((float)clock())/CLOCKS_PER_SEC;
    return fmod(t + offset, period) / period;
}

void runFlameEffect(uint8_t* data)
{
    float t = ((float)clock())/CLOCKS_PER_SEC;
    //uint8_t v = (sin(t/100000.0)+1) * 0.5 * 170 + 85;
    HsvColor color;
    
    float spreadtime = 4.0;
    float totaltime = 16.0;
    int cycle = lfo_ramp(totaltime) * totaltime;
    
    int wavefront = bufferWidth * lfo_ramp(spreadtime * bufferWidth/50);
    float attenuate = 1;
    if (cycle >= spreadtime)
    {
        wavefront = bufferWidth;
        attenuate = (1 - lfo_ramp(totaltime)); 
    }
    
    for (int ii = 0; ii < bufferWidth; ii++)
    {
        float vp = (sin(t/10 + 10*slotSeeds[ii]) + 1) * 0.5;
        float v = floor(vp*100) + rand() % 55 + 100;
        //v *= attenuate;
        
        for (int jj = 0; jj < bufferHeight; jj++)
        {
            color.h = 23 + rand() % 4 * slotSeeds[ii];
            color.s = rand() % 15 + 240;
            color.v = v;
            
            v -= 20 + rand() % 40;
         
            if (cycle > totaltime - spreadtime || ii > wavefront + 2)
            {
                if (jj == 0)
                {
                    color.v /= 2;
                    paintHSV(data, ii, jj, color);
                }
            }
            else if (ii > wavefront + 1)
            {
                if (jj < 3)
                {
                    color.v *= lfo_ramp(spreadtime/50, jj);
                    paintHSV(data, ii, jj, color);
                }
            }
            else if (ii > wavefront)
            {
                color.v *= lfo_ramp(0.3, jj);
                paintHSV(data, ii, jj, color);
            }
            else
            {
                paintHSV(data, ii, jj, color);
            }
        }
    }
}

void clearEffect(uint8_t* data)
{
    memset(data, 0, bufferWidth * bufferHeight * 3);
}

void runTestEffect(uint8_t* data)
{
    clock_t t = clock();
    uint8_t v = (sin(t/1000000.0)+1) * 0.5 * 170 + 85;
    
    HsvColor colors[4];
    colors[0].h = 255;
    colors[0].s = 255;
    colors[0].v = v;
    colors[1].h = 200;
    colors[1].s = 255;
    colors[1].v = v;
    colors[2].h = 170;
    colors[2].s = 255;
    colors[2].v = v;
    colors[3].h = 120;
    colors[3].s = 255;
    colors[3].v = v;
    
    for (int ii = 0; ii < bufferWidth; ii++)
    {
        for (int jj = 0; jj < bufferHeight; jj++)
        {
            paintHSV(data, ii, jj, colors[jj]);
        }
    }
}