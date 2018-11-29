#ifndef GROUNDGRID_H
#define GROUNDGRID_H

#include <iostream>
#include <vector>

class groundgrid
{
public:
    groundgrid();
    void createGrid(int col_dim, int row_dim);
    int getCellType(int c, int r);


private:

    std::vector<std::vector<int>> ground;
    int col_dimension = 20;
    int row_dimension = 20;
    float cell_dimension = 1.0f;


};

#endif // GROUNDGRID_H
