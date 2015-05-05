#ifndef YAGOL_H_INCLUDED
#define YAGOL_H_INCLUDED

#include <vector>

class Cell
{
public:
    Cell();
    Cell(int gridX, int gridY);
    virtual ~Cell();
    void setAlive();
    void setDead();
    void checkState();
    void updateState();
    void draw();
    int getX(){return x;};
    int getY(){return y;};
    int getGridX(){return gridX;};
    int getGridY(){return gridY;};
    bool isAlive(){return alive;};
private:
    int x, y, gridX, gridY;
    bool alive, aliveNext, changingState;
};

class Grid
{
public:
    Grid();
    virtual ~Grid();
    Cell *getCell(int x, int y);
    std::vector<Cell*> getCells(){return cells;};
    int nbAliveCells;
    int nbChangedCells;
    bool newCellsAdded;
private:
    std::vector<Cell*> cells;
};

struct Point{
    int x;
    int y;
};
typedef Point *Points;
Points neighbors[8];

Grid *grid;
const int gridWidth = 20;
const int gridHeight = 20;
const int cellSize = 10;

void drawPixel(u8* fb, int x,int y, u32 color);

#endif // YAGOL_H_INCLUDED
