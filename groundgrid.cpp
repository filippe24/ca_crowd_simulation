#include "groundgrid.h"

groundgrid::groundgrid()
{

}



void groundgrid::createGrid(int col_dim, int row_dim)
{
    ground.clear();
    col_dimension = col_dim;
    row_dimension = row_dim;
    for( int col_i = 0; col_i < col_dim; col_i ++)
    {
        std::vector<int> row;
        for(int row_i = 0; row_i < row_dim; row_i ++)
        {
            float r = ((double) rand() / (RAND_MAX));
            int new_val = 0;
            if(r > 0.8)
                new_val = 1;
            row.push_back(new_val);
        }
        ground.push_back(row);
    }
}

int groundgrid::getCellType(int c, int r)
{
    if(c > col_dimension)
        std::cerr << "selected cell " << c << " out of bound, max column :" << col_dimension << std::endl;
     else if(r > row_dimension)
        std::cerr << "selected cell " << c << " out of bound, max column :" << col_dimension << std::endl;
    else
    {
        int returned_value = ground[c][r];
        std::cout << "returned value :"<< returned_value << std::endl;
        return returned_value;
    }
}

