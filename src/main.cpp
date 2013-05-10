#include "kinet.h"
#include "effects.h"
#include <vector>
#include <cstdlib>

std::vector<FixtureTile*> strips;
std::vector<PowerSupply*> supplies;


void createNetwork(const uint8_t* data, int dataWidth, int dataHeight)
{
    strips.push_back(new FixtureTile(1, 50, 1));
    strips.push_back(new FixtureTile(2, 50, 1));
    strips.push_back(new FixtureTile(1, 50, 1));
    strips.push_back(new FixtureTile(2, 50, 1));
    
    supplies.push_back(new PowerSupply("10.32.0.12"));
    supplies.push_back(new PowerSupply("10.32.0.13"));
    
    supplies[0]->addFixture(strips[0]);
    supplies[0]->addFixture(strips[1]);
    supplies[1]->addFixture(strips[2]);
    supplies[1]->addFixture(strips[3]);
    
    int mapping[] = { 0, 1, 3, 2 };
    
    for (int ii = 0; ii < strips.size(); ii++)
    {
        strips[ii]->setVideoRect(0, mapping[ii], 50, 1);
        strips[ii]->setSourceData(data, dataWidth, dataHeight, 3);
    }
    
    for (int ii = 0; ii < supplies.size(); ii++)
    {
        supplies[ii]->go();
    }
}

void applyFixups(uint8_t* buffer)
{
    // swap any crossed middle pixels here
}

int main()
{
    int width = 50; //51 * 3 + 32 + 34 + 50; // lengths of strips
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
