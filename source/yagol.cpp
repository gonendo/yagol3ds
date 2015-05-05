#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "yagol.h"

Cell::Cell(){
}

Cell::~Cell(){
}

Cell::Cell(int pX, int pY){
    alive = false;
    gridX = pX;
    gridY = pY;
    x = 50+(gridX*(cellSize+1));
    y = 10+(gridY*(cellSize+1));
}

void Cell::setAlive(){
    alive = true;
}

void Cell::setDead(){
    alive = false;
}

void Cell::checkState(){
    int i, aliveNeighbors;
    Cell *neighbor;
    aliveNeighbors = 0;

    for(i=0; i < 8; i++){
        neighbor = grid->getCell(gridX+neighbors[i]->x, gridY+neighbors[i]->y);
        if(neighbor && neighbor->isAlive()){
            aliveNeighbors++;
        }
    }

    changingState=false;

    if(aliveNeighbors==0)
        return;

    if(aliveNeighbors==3){
        if(!alive){
            aliveNext=true;
            changingState=true;
        }

    }
    else if(aliveNeighbors<2 || aliveNeighbors>3){
        if(alive){
            aliveNext=false;
            changingState=true;
        }

    }

    if(changingState)
        grid->nbChangedCells++;
}

void Cell::updateState(){
    if(changingState && aliveNext){
            setAlive();
    }
    else if(changingState && !aliveNext){
            setDead();
    }
}

void Cell::draw(){
    int i,j;
    u8* bottomfb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    for(i=0; i < cellSize; i++){
        for(j=0; j < cellSize; j++){
            drawPixel(bottomfb, x+i, y+j, !alive ? 0x959595 : 0xf4a00e);
        }
    }
}


Grid::Grid(){
    int i,j;
    for(i=0; i < gridHeight; i++)
        for(j=0; j < gridWidth; j++)
            cells.push_back(new Cell(j,i));
    nbChangedCells = 0;
}

Grid::~Grid(){
}

Cell* Grid::getCell(int x , int y){
    int i;
    Cell *c;
    int nbCells =  cells.size();
    for(i=0; i < nbCells; i++){
        c = cells[i];
        if(c->getGridX()==x && c->getGridY()==y){
            return c;
        }
    }
    return NULL;
}

extern "C" void yagol_init(){
    grid = new Grid();

    int i;
    for(i=0; i < 8; i++)
        neighbors[i] = (Point*) malloc(sizeof(Point));

    neighbors[0]->x = -1; neighbors[0]->y = -1;
    neighbors[1]->x = 0; neighbors[1]->y = -1;
    neighbors[2]->x = 1; neighbors[2]->y = -1;
    neighbors[3]->x = -1; neighbors[3]->y = 0;
    neighbors[4]->x = 1; neighbors[4]->y = 0;
    neighbors[5]->x = -1; neighbors[5]->y = 1;
    neighbors[6]->x = 0; neighbors[6]->y = 1;
    neighbors[7]->x = 1; neighbors[7]->y = 1;
}

extern "C" void yagol_render(){
    int i;
    Cell *c;
    int nbCells =  grid->getCells().size();
    for(i=0; i < nbCells; i++){
        c = grid->getCells()[i];
        c->draw();
    }
}

extern "C" void yagol_input(touchPosition *pos){
    int i;
    Cell *c;
    int nbCells =  grid->getCells().size();
    for(i=0; i < nbCells; i++){
        c = grid->getCells()[i];
        if(pos->px >= c->getX() &&
           pos->px <= c->getX()+cellSize &&
           pos->py >= c->getY() &&
           pos->py <= c->getY()+cellSize)
        {
            c->setAlive();
        }
    }
}

extern "C" void yagol_clear(){
    int i;
    Cell *c;
    int nbCells =  grid->getCells().size();
    for(i=0; i < nbCells; i++){
        c = grid->getCells()[i];
        c->setDead();
    }
}

extern "C" void yagol_generate(){
    grid->nbChangedCells=0;
    grid->newCellsAdded=false;

    int nbCells =  grid->getCells().size();
    Cell *c;
    int i;

    for(i=0; i < nbCells; i++){
        c = grid->getCells()[i];
        c->checkState();
    }
    for(i=0; i < nbCells; i++){
        c = grid->getCells()[i];
        c->updateState();
    }
}

extern "C" u8* playWav(const char* wavFile){
    Handle fileHandle;

    //Open wav file
    FS_archive sdmcArchive = (FS_archive){ ARCH_SDMC, (FS_path){ PATH_EMPTY, 1, (u8*)"" } };
    FS_path filePath = FS_makePath(PATH_CHAR, wavFile);
    Result ret = FSUSER_OpenFileDirectly(NULL, &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
    if (ret){
        printf("\x1b[7;1HCan't open file");
        return 0;
    }

    u32 samplerate, bytesRead;
    u64 size;
    FSFILE_GetSize(fileHandle, &size);
    u8* audiobuf = (u8*)linearAlloc(size - 44);
    FSFILE_Read(fileHandle, &bytesRead, 24, &samplerate, 4);
    FSFILE_Read(fileHandle, &bytesRead, 44, audiobuf, size - 44);

    GSPGPU_FlushDataCache(NULL, audiobuf, size);
    Result res = csndPlaySound(0x08, SOUND_FORMAT_16BIT | SOUND_ONE_SHOT, samplerate, 1, 0, (u32*)audiobuf, NULL, size);
    if (res != 0){
        printf("\x1b[7;1HCan't play file");
    }

    u8 playing = 0;
    csndIsPlaying(0x08, &playing);
    if (playing == 0){
        CSND_SetPlayState(0x08, 1);
    }
    csndExecCmds(true);
    return audiobuf;
}

void drawPixel(u8* fb, int x,int y, u32 color){
	int idx = ((x)*240) + (239-(y));
	fb[idx*3+0] = (color);
	fb[idx*3+1] = (color) >> 8;
	fb[idx*3+2] = (color) >> 16;
}
