#include "kinet.h"
#include "effects.h"
#include <vector>
#include <cstdlib>
#include <iostream>


std::vector<FixtureTile*> strips;
std::vector<PowerSupply*> supplies;

#define BEGIN_POWER_NODE(ip, stripLength, startAddress, direction) { \
PowerSupply* pSupply = new PowerSupply(ip); \
supplies.push_back(pSupply); \
int start = startAddress; \
bool dir = direction; \
int length = stripLength; \
std::cout << "Created power supply " << ip << "\n";

#define CHANNEL(channel, ycoord) { \
FixtureTile* pStrip = new FixtureTile(channel, length, 1); \
pStrip->setVideoRect(start, ycoord, length, 1); \
pStrip->setSourceData(data, dataWidth, dataHeight, 3); \
pStrip->setFlippedX(dir); \
strips.push_back(pStrip); \
pSupply->addFixture(pStrip); \
std::cout << "Added channel " << channel << " length: " << length << " X: " << start << " Y: " << ycoord << " flip: " << dir << "\n"; \
}

#define END_POWER_NODE }

#define DIRECTION_RIGHT false
#define DIRECTION_LEFT true


void createNetwork(const uint8_t* data, int dataWidth, int dataHeight)
{
    // Left outer
    BEGIN_POWER_NODE("10.32.0.05", 51, 50, DIRECTION_LEFT)
    CHANNEL(1, 0)
    CHANNEL(2, 2)
    END_POWER_NODE
    
    BEGIN_POWER_NODE("10.32.0.06", 51, 50, DIRECTION_LEFT)
    CHANNEL(1, 1)
    CHANNEL(2, 3)
    END_POWER_NODE
    
    // Left middle outer
    BEGIN_POWER_NODE("10.32.0.44", 51, 101, DIRECTION_LEFT)
    CHANNEL(1, 0)
    CHANNEL(2, 1)
    END_POWER_NODE
    
    BEGIN_POWER_NODE("10.32.0.17", 51, 101, DIRECTION_LEFT)
    CHANNEL(1, 2)
    CHANNEL(2, 3)
    END_POWER_NODE
    
    // Left middle inner
    BEGIN_POWER_NODE("10.32.0.13", 51, 153, DIRECTION_RIGHT)
    CHANNEL(1, 0)
    CHANNEL(2, 1)
    END_POWER_NODE
    
    BEGIN_POWER_NODE("10.32.0.12", 51, 153, DIRECTION_RIGHT)
    CHANNEL(1, 2)
    CHANNEL(2, 3)
    END_POWER_NODE
    
    // Right middle inner
    BEGIN_POWER_NODE("10.32.0.70", 51, 203, DIRECTION_LEFT)
    CHANNEL(1, 1)
    CHANNEL(2, 0)
    END_POWER_NODE
    
    BEGIN_POWER_NODE("10.32.0.16", 51, 203, DIRECTION_LEFT)
    CHANNEL(1, 2)
    CHANNEL(2, 3)
    END_POWER_NODE
    
    // Right middle outer
    BEGIN_POWER_NODE("10.32.0.15", 51, 255, DIRECTION_RIGHT)
    CHANNEL(1, 1)
    CHANNEL(2, 0)
    END_POWER_NODE
    
    BEGIN_POWER_NODE("10.32.0.14", 51, 255, DIRECTION_RIGHT)
    CHANNEL(1, 2)
    CHANNEL(2, 3)
    END_POWER_NODE
    
    // Right outer
    BEGIN_POWER_NODE("10.32.0.19", 51, 306, DIRECTION_RIGHT)
    CHANNEL(1, 2)
    CHANNEL(2, 3)
    END_POWER_NODE
    
    BEGIN_POWER_NODE("10.32.0.20", 51, 306, DIRECTION_RIGHT)
    CHANNEL(1, 0)
    CHANNEL(2, 1)
    END_POWER_NODE
}

void applyFixups(uint8_t* buffer)
{
    // swap any crossed middle pixels here
}

int main()
{
    int width = 51 * 6; // lengths of strips
    int height = 4;
    uint8_t* buffer = (uint8_t*)malloc(width * height * 3);
    
    createNetwork(buffer, width, height);
    initEffects(width, height);
    
    while (1)
    {
        clearEffect(buffer);
        
        //runTestEffect(buffer);
        runFlameEffect(buffer);
        //runWaterfallEffect(buffer);

        applyFixups(buffer);
        
        for (int ii = 0; ii < supplies.size(); ii++)
        {
            supplies[ii]->go();
        }
        usleep(70);
    }
    
    free(buffer);
    return 0;
}
