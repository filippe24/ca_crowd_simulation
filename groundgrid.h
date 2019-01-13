#ifndef GROUNDGRID_H
#define GROUNDGRID_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <queue>


class groundgrid
{

//typedef struct {int a; int b;} cell;
public:
    typedef std::pair<int,int> cellT;


    struct cellStruct
    {
        uint id;
        cellT cellPos;
        //0 walkable, 1 blocked
        int type = 0;
        //A*
        int h = 0;
        int g = 0;
        int f = 0;
        //list
        bool open = false;
        bool close = false;
        cellStruct* father = nullptr;

        void createCellStruct(uint id_in, cellT position_in, int type_in)
        {
            id = id_in;
            cellPos = position_in;
            type = type_in;
        }
        //comparison in the opposite way that is necessary for the priority queue to get the last value on top
        bool operator<(const cellStruct& other) const
        {
            return f > other.f;
        }
        void computeF()
        {
            f = g + h;
        }


    };

public:
    groundgrid();
    groundgrid(float cell_dimension);
    void createGrid(int col_dim, int row_dim);

    //groundcell struct
    uint getIdFromPos(int c, int r)
    {
        return uint(c*col_dimension + r);
    }
    uint getIdFromPos(cellT cell)
    {
        return uint(cell.first*col_dimension + cell.second);
    }



    //getter
    int getColumnDimension();
    int getRowDimension();
    float getCellDim();
//    float getCellDim(){return cell_dimension};

    int getCellType(int c, int r);
    glm::vec3 getCellPosition(int c, int r);
    cellT getCurrentCell(glm::vec3 currentPos);
    cellT getCurrentCell(float pos_x, float pos_y, float pos_z);
    std::vector<cellT> getObstaclesPositions();
    std::vector<groundgrid::cellT> getPathPositions();

    //A* ALGORITHM
    std::vector<cellT> getNeighbours(cellT cell);
    std::vector<uint> getNeighboursID(uint id);

    glm::vec3 getGoalPos();
    glm::vec3 getStartPos();

    //SETTER
    void setGoal(int c, int r);
    void setStart(int c, int r);



    //A*
    void computeAstar();
    void reconstructPath();
    //h
    int H(cellT cell);
    int H(uint cell_id);
    int manhattanDist(cellT start, cellT end);
    //g
    int G(cellT cell);
    int G(uint cell_id);
    //movment
    int movementCost(cellT A, cellT B);
    int movementCost(uint A_id, uint B_id);





private:
    //basic data structure an array of array, with the value 1 if it is an obstacle, 0 if it is a walkable path
    std::vector<std::vector<int>> ground;
    //vector of structure groundCell, with more complex structure, that will replace the basic one
    std::vector<cellStruct> groundCells;
    int col_dimension = 20;
    int row_dimension = 20;
    float cell_dimension = 1.0f;
    float offset;

    int number_of_obstacles = 0;
    std::vector<cellT> obstaclesPositions;


    //A*
    std::priority_queue<cellStruct> openQueue;
    std::priority_queue<cellStruct> closedQueue;


    cellT goal;
    cellT start;

    std::vector<uint> path;
    std::vector<groundgrid::cellT> pathPositions;



};

#endif // GROUNDGRID_H
