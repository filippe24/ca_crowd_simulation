#include "groundgrid.h"

typedef std::pair<int,int> cellT;


groundgrid::groundgrid()
{

}


groundgrid::groundgrid(float dim)
{
    cell_dimension = dim;
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
            float r = ( float(rand()) / (RAND_MAX));
            int new_val = 0;
            if(r > 0.9f )
            {
                new_val = 1;
                number_of_obstacles++;
                cellT obst_cell = cellT(col_i, row_i);
                obstaclesPositions.push_back(obst_cell);
            }
            row.push_back(new_val);
        }
        ground.push_back(row);
    }
}

int groundgrid::getColumnDimension()
{ return col_dimension;}
int groundgrid::getRowDimension()
{ return row_dimension;}
float groundgrid::getCellDim()
{ return cell_dimension;}



//1 = occluded       0 = free
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
    return -1;
}


glm::vec3 groundgrid::getCellPosition(int c, int r)
{
    glm::vec3 returned_vector = glm::vec3(0.0,0.0,0.0);
    if(c > col_dimension)
    {
        std::cerr << "selected cell " << c << " out of bound, max column :" << col_dimension << std::endl;
        return returned_vector;
    }
    if(r > row_dimension)
    {
        std::cerr << "selected cell " << c << " out of bound, max column :" << col_dimension << std::endl;
        return returned_vector;
    }

    int center_c = col_dimension/2;
    int center_r = row_dimension/2;

    int c_diff = c - center_c;
    int r_diff = r - center_r;

    returned_vector.x = c_diff*cell_dimension;
    returned_vector.z = r_diff*cell_dimension;
    return returned_vector;
}

std::pair<int,int> groundgrid::getCurrentCell(glm::vec3 currentPos)
{
    std::pair<int,int> returnedCell;
    int center_c = col_dimension/2;
    int center_r = row_dimension/2;
    int c_diff = int(currentPos.x/cell_dimension);
    int r_diff = int(currentPos.z/cell_dimension);

    returnedCell.first = c_diff + center_c;
    returnedCell.second = r_diff + center_r;

    return returnedCell;
}

std::pair<int,int> groundgrid::getCurrentCell(float pos_x, float pos_y, float pos_z)
{
    std::pair<int,int> returnedCell;
    int center_c = col_dimension/2;
    int center_r = row_dimension/2;
    int c_diff = int(pos_x/cell_dimension);
    int r_diff = int(pos_z/cell_dimension);

    returnedCell.first = c_diff + center_c;
    returnedCell.second = r_diff + center_r;

    return returnedCell;
}



//ALGORITHM A*
std::vector<cellT> groundgrid::getNeighbours(cellT cell)
{
    std::vector<cellT> returned_list;
    int c = cell.first;
    int r = cell.second;

    //check all the neighbours of the cell
    cellT new_cell;
    if(c > 0)
    {
        //c-1, r
        new_cell.first = c-1;
        new_cell.second = r;
        if(getCellType(c-1,r) == 0)
            returned_list.push_back(new_cell);

        //c-1, r-1
        if(r > 0)
        {
            new_cell.first = c-1;
            new_cell.second = r-1;
            if(getCellType(c-1,r-1) == 0)
                returned_list.push_back(new_cell);
        }
        //c-1, r+1
        if(r < row_dimension)
        {
            new_cell.first = c-1;
            new_cell.second = r+1;
            if(getCellType(c-1,r+1) == 0)
                returned_list.push_back(new_cell);
        }
    }

    //c, r-1
    if(r > 0)
    {
        new_cell.first = c;
        new_cell.second = r-1;
        if(getCellType(c,r-1) == 0)
            returned_list.push_back(new_cell);
    }
    //c, r-1
    if(r < row_dimension)
    {
        new_cell.first = c;
        new_cell.second = r+1;
        if(getCellType(c,r+1) == 0)
            returned_list.push_back(new_cell);
    }

    if(c < cell_dimension)
    {
        //c+1, r
        new_cell.first = c+1;
        new_cell.second = r;
        if(getCellType(c+1,r) == 0)
            returned_list.push_back(new_cell);
        //c+1, r-1
        if(r > 0)
        {
            new_cell.first = c+1;
            new_cell.second = r-1;
            if(getCellType(c+1,r-1) == 0)
                returned_list.push_back(new_cell);
        }
        //c+1, r+1
        if(r < row_dimension)
        {
            new_cell.first = c+1;
            new_cell.second = r+1;
            if(getCellType(c+1,r+1) == 0)
                returned_list.push_back(new_cell);
        }
    }

    return returned_list;
}













std::vector<groundgrid::cellT> groundgrid::getObstaclesPositions()
{
    return obstaclesPositions;
}
void groundgrid::setGoal(int c, int r)
{
    goal.first = c;
    goal.second = r;
}
glm::vec3 groundgrid::getGoalPos()
{
    return getCellPosition(goal.first,goal.second);
}





