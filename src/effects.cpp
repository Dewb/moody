#include "effects.h"
#include "color.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstring>
#include <iostream>
#include <sys/time.h>

#define SIMULATION_HEIGHT 100
#define SIMULATION_THRESH 150.0
#define SIMULATION_WIDTH 306
#define NUM_PARTICLES 400
#define MIN_PARTICLES 50

#define CREATING 0
#define DECAYING 1

int bufferWidth = 0;
int bufferHeight = 0;

int mode = CREATING;

float frand(float max = 1.0)
{
    return max * (float)rand() / (float)RAND_MAX;
}

int fudgemax(int a, int b)
{
    return (a > b) ? a + (b >> 4) : b + (a >> 4);
}

void paintHSV(uint8_t* data, int x, int y, HsvColor& hsv)
{
    RgbColor rgb = HsvToRgb(hsv);
    int index = (x + bufferWidth * y) * 3;
    
    data[index+0] = fudgemax(data[index+0], rgb.r);
    data[index+1] = fudgemax(data[index+1], rgb.g);
    data[index+2] = fudgemax(data[index+2], rgb.b);
}

class Particle
{
public:
    int slot;
    float height;
    float velocity;
    HsvColor color;

protected:
    bool active;
    
public:
    static int activeCount;

    Particle()
    {
        active = false;
    }
    
    bool isActive() const { return active; }
    
    void setActive(bool bActive)
    {
        if (active != bActive)
        {
            if (bActive)
                activeCount++;
            else
                activeCount--;
        }
        active = bActive;
    }

    void launch()
    {
        static float hue = 0;
        
        if (frand() < 0.012)
        {
            height = 0;
            velocity = -(frand(0.17) + 0.12);
            color.h = (int)(hue + 40) % 255;
        } else {
            height = SIMULATION_HEIGHT;
            velocity = frand(0.20) + 0.04;
            color.h = hue;
        }
        
        slot = rand() % SIMULATION_WIDTH;
        color.s = 240;
        color.v = 255;
        
        setActive(true);
        
        hue += 0.018;
        if (hue > 255) hue = 0;
    }
    
    void update()
    {
        if (height > -SIMULATION_THRESH &&
            height < SIMULATION_THRESH)
        {
            height -= velocity;
        }
        else
        {
            setActive(false);
        }
    }
    
    void render(uint8_t* data)
    {
        float h1 = height / (SIMULATION_HEIGHT * 1.0);
        for (int jj = 0; jj < bufferHeight; jj++)
        {
            float h2 = jj / (bufferHeight * 1.0);
            float d = fabs(h1 - h2);
            
            HsvColor c = color;
            if (d > 0.4)
                continue;
            else if (d > 0.05)
                c.v = c.v * (1 - 2 * d);
            
            paintHSV(data, slot, jj, c);
        }
     
    }
    
};

int Particle::activeCount = 0;

std::vector<float> slotSeeds;
std::vector<Particle> particles;


void initEffects(int width, int height)
{
    bufferWidth = width;
    bufferHeight = height;
    
    struct timeval now;
    gettimeofday(&now, NULL);
    
    unsigned int seed = (unsigned int)((now.tv_sec + 1) * (now.tv_usec + 1));
    srand(seed);
    
    for (int ii = 0; ii < width; ii++)
    {
        slotSeeds.push_back(frand());
    }
    
    particles.reserve(NUM_PARTICLES);
    
}


void runWaterfallEffect(uint8_t* data)
{
    static int spawndelay = 0;
        
    if (Particle::activeCount == NUM_PARTICLES)
    {
        mode = DECAYING;
    }
    else if (Particle::activeCount < MIN_PARTICLES)
    {
        mode = CREATING;
    }
    
    int targetParticles = Particle::activeCount;
    
    if (spawndelay == 0)
    {
        if (mode == CREATING)
        {
            if (particles.size() < NUM_PARTICLES)
            {
                Particle p;
                p.launch();
                particles.push_back(p);
            }
            targetParticles += 1;
        }
        else
        {
            targetParticles -= 5;
        }
        spawndelay = 80;
    } else {
        spawndelay--;
    }
        
    std::vector<Particle>::iterator p = particles.begin();
    
    while (p != particles.end())
    {
        p->update();
        if (!(p->isActive()))
        {
            if (Particle::activeCount < targetParticles) {
                p->launch();
            }
        }
        
        if (p->isActive())
        {
            p->render(data);
        }
        p++;
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
    
    float spreadtime = 16.0;
    float totaltime = 64.0;
    int cycle = lfo_ramp(totaltime) * totaltime;
    
    int wavefront = bufferWidth * lfo_ramp(spreadtime);
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
            
            if (ii > wavefront + 3)
            {
                if (jj == 0)
                {
                    color.v /= 2;
                }
                else
                {
                    continue;
                }
            }
            else if (ii > wavefront + 1)
            {
                if (jj < 3)
                {
                    color.v *= lfo_ramp(spreadtime/50, jj);
                }
                else
                {
                    continue;
                }
            }
            else if (ii > wavefront)
            {
                color.v *= lfo_ramp(0.3, jj);
            }
            
            paintHSV(data, ii, jj, color);
            
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
