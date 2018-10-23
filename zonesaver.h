#ifndef ZONESAVER_H
#define ZONESAVER_H
#include <vector>
#include <fstream>
#include <iostream>
#include <windows.h>
namespace ZoneSaver{


class ZoneBlock{
public:
    unsigned int x, y, z;
    char r, g, b, type;
    ZoneBlock(unsigned int _x, unsigned int _y, unsigned int _z, char _r, char _g, char _b, char _type){
        this->x = _x;
        this->y = _y;
        this->z = _z;
        this->r = _r;
        this->g = _g;
        this->b = _b;
        this->type = _type;
    }

};


class ZoneContainer{
public:
    std::vector<ZoneBlock*> blocks;
    unsigned int zone_x, zone_y;

    ZoneContainer(unsigned int _zonex, unsigned int _zoney){
        this->zone_x = _zonex;
        this->zone_y = _zoney;
    }

    //Block coordinates should be absolute, not relative to zone
    void SetZoneBlock(unsigned int x, unsigned int y, unsigned int z, char r, char g, char b, char type){
        //Update existing block if it exists.
        for (ZoneBlock* block : this->blocks){
            if (block->x == x && block->y == y && block->z == z){
                block->r = r;
                block->g = g;
                block->b = b;
                block->type = type;
                return;
            }
        }

        //Add a new block if does not exist
        ZoneBlock* block = new ZoneBlock(x, y, z, r, g, b, type);
        this->blocks.push_back(block);
    }

    void OutputFile(char worldName[]){
        char fileName[256] = {0};
        char folderName[256] = { 0 };

        //Folder for all saves
        sprintf(folderName, "%s", "build_saves");
        CreateDirectory(folderName, NULL);

        //Folder for specific world
        sprintf(folderName, "%s\\%s", "build_saves", worldName);
        CreateDirectory(folderName, NULL);

        //File for this Zone
        sprintf(fileName, "%s\\%u-%u.cwb", folderName, this->zone_x, this->zone_y);
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::binary);

        //Write each block to file
        for (ZoneBlock* block : this->blocks){
            file.write((char*)block, sizeof(*block));
        }
        file.close();
    }
};

class WorldContainer{
public:
    std::vector<ZoneContainer*> zones;
    void SetBlock(unsigned int x, unsigned int y, unsigned int z, char r, char g, char b, char type){
        unsigned int zonex = x / 0x100;
        unsigned int zoney = y / 0x100;

        //check to see if there is already a zone for this block
        for (ZoneContainer* zc : zones){
            if ( (zc->zone_x == zonex) && (zc->zone_y == zoney) ){
                zc->SetZoneBlock(x, y, z, r, g, b, type);
                return;
            }
        }

        //No zone created yet
        ZoneContainer* zc = new ZoneContainer(zonex, zoney);
        zc->SetZoneBlock(x, y, z, r, g, b, type);
        this->zones.push_back(zc);
    }

    void OutputFiles(char worldName[]){
        for (ZoneContainer* zc : zones){
            zc->OutputFile(worldName);
        }
    }
};

}
#endif // ZONESAVER_H