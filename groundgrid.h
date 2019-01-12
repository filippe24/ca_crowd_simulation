#ifndef GROUNDGRID_H
#define GROUNDGRID_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <queue>


class groundgrid
{

//typedef struct {int a; int b;} cell;
typedef std::pair<int,int> cellT;

public:
    groundgrid();
    groundgrid(float cell_dimension);
    void createGrid(int col_dim, int row_dim);

    //getter
    int getColumnDimension();
    int getRowDimension();
    float getCellDim();

    int getCellType(int c, int r);
    glm::vec3 getCellPosition(int c, int r);
    cellT getCurrentCell(glm::vec3 currentPos);
    cellT getCurrentCell(float pos_x, float pos_y, float pos_z);
    std::vector<cellT> getObstaclesPositions();

    //A* ALGORITHM
    std::vector<cellT> getNeighbours(cellT cell);

    glm::vec3 getGoalPos();
    //SETTER
    void setGoal(int c, int r);





private:
    std::priority_queue<cellT> pQueue;

    std::vector<std::vector<int>> ground;
    int col_dimension = 20;
    int row_dimension = 20;
    float cell_dimension = 1.0f;

    int number_of_obstacles = 0;
    std::vector<cellT> obstaclesPositions;


    cellT goal;


};

#endif // GROUNDGRID_H
