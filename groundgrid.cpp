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
    int id = 0;
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

            cellStruct new_cell;
            new_cell.createCellStruct(id, cellT(col_i,row_i), new_val);
            groundCells.push_back(new_cell);
            id++;

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
        int ret_val = groundCells[getIdFromPos(c,r)].type;
        return ret_val;
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
    if(currentPos.x > 0.0f )
        returnedCell.first = c_diff + center_c;
    else
        returnedCell.first = c_diff + center_c - 1;
    if(currentPos.z > 0.0f )
        returnedCell.second = r_diff + center_r;
    else
        returnedCell.second = r_diff + center_r - 1;


    return returnedCell;
}

std::pair<int,int> groundgrid::getCurrentCell(float pos_x, float pos_y, float pos_z)
{
    glm::vec3 currentPos = glm::vec3(pos_x, pos_y, pos_z);
    return getCurrentCell(currentPos);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ALGORITHM A*
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//function for selecting the walkable neighbours with 2 diferent inputs
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
        else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        //c-1, r-1
        if(r > 0)
        {
            new_cell.first = c-1;
            new_cell.second = r-1;
            if(getCellType(c-1,r-1) == 0)
                returned_list.push_back(new_cell);
            else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        }
        //c-1, r+1
        if(r < row_dimension)
        {
            new_cell.first = c-1;
            new_cell.second = r+1;
            if(getCellType(c-1,r+1) == 0)
                returned_list.push_back(new_cell);
            else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        }
    }

    //c, r-1
    if(r > 0)
    {
        new_cell.first = c;
        new_cell.second = r-1;
        if(getCellType(c,r-1) == 0)
            returned_list.push_back(new_cell);
        else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

    }
    //c, r-1
    if(r < row_dimension)
    {
        new_cell.first = c;
        new_cell.second = r+1;
        if(getCellType(c,r+1) == 0)
            returned_list.push_back(new_cell);
        else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

    }

    if(c < col_dimension)
    {
        //c+1, r
        new_cell.first = c+1;
        new_cell.second = r;
        if(getCellType(c+1,r) == 0)
            returned_list.push_back(new_cell);
        else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        //c+1, r-1
        if(r > 0)
        {
            new_cell.first = c+1;
            new_cell.second = r-1;
            if(getCellType(c+1,r-1) == 0)
                returned_list.push_back(new_cell);
            else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        }
        //c+1, r+1
        if(r < row_dimension)
        {
            new_cell.first = c+1;
            new_cell.second = r+1;
            if(getCellType(c+1,r+1) == 0)
                returned_list.push_back(new_cell);
            else std::cout << " ***********************************OBSTACLEEEEEEEEEE~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        }
    }

    return returned_list;
}


std::vector<uint> groundgrid::getNeighboursID(uint id)
{
    std::vector<uint> returned_vec;
    cellT pos = groundCells[id].cellPos;
    std::vector<cellT> neighPos = getNeighbours(pos);
    for(uint n = 0; n < neighPos.size(); n++)
    {
        uint curr_id = getIdFromPos(neighPos[n]);
        returned_vec.push_back(curr_id);
    }
    return returned_vec;
}



//implemented like slides
void groundgrid::computeAstar()
{
    uint start_id = getIdFromPos(start);
    uint goal_id = getIdFromPos(goal);
    std::cout << "start pos : " << start.first << ", " << start.second << "  with id = " << start_id << std::endl;
    std::cout << "goal pos : " << goal.first << ", " << goal.second << "  with id = " << goal_id << std::endl;


    //compute H for all the cells
    for(uint c = 0; c < groundCells.size(); c++)
    {
        groundCells[c].h = H(groundCells[c].cellPos);
    }





    if(openQueue.empty())
            std::cout << "open is empty" << std::endl;
    else {
        std::priority_queue<cellStruct> emptyList;
        openQueue = emptyList;
    }
    openQueue.push(groundCells[start_id]);
    groundCells[start_id].open = true;

    if(closedQueue.empty())
        std::cout << "it is empty" << std::endl;
    else {
        std::priority_queue<cellStruct> emptyList;
        closedQueue = emptyList;
    }
    int iteration =0;

    while(openQueue.top().id != goal_id)
    {
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout<< "iteration number : " << iteration << std::endl;
        iteration ++;

        cellStruct current = openQueue.top();
        uint current_id = current.id;
        std::cout << " we got current id : " << current.id << " in postion " << current.cellPos.first << "  and  " << current.cellPos.second  << std::endl;
        std::cout << "OPEN size beofre :" << openQueue.size();

        openQueue.pop();
        groundCells[current_id].open = false;

        std::cout << "   OPEN size after get current :" << openQueue.size() << std::endl;
        std::cout << "CLOSED size  :" << closedQueue.size() << std::endl;

        std::cout << std::endl;

        closedQueue.push(current);
        groundCells[current_id].close = true;

        std::vector<uint> neighbours = getNeighboursID(current_id);

        for(uint n = 0; n< neighbours.size(); n++)
        {
            uint neb = neighbours[n];
            std::cout << " the " << n << "  neighbours has pos : " << groundCells[neb].cellPos.first << ", " << groundCells[neb].cellPos.second << "  with id = " << groundCells[neb].id << " and position =" << neb << "  and h= " << groundCells[neb].h << std::endl;

            int cost = current.g + movementCost(current_id, groundCells[neb].id);
            std::cout << " cost = " << cost << "  composed of: g(current) = " << current.g << "  and movementCost = " << movementCost(current.id, groundCells[neb].id) << std::endl;

            if(groundCells[neb].open  && cost < groundCells[neb].g)
            {
                std::cout << "IN open and cost : " << cost << " AND is LESS then " << groundCells[neb].g << std::endl;
                groundCells[neb].open = false;
            }
            if(groundCells[neb].close  && cost < groundCells[neb].g)
            {
                std::cout << "IN close and cost : " << cost << " AND is LESS then " << groundCells[neb].g << std::endl;

                groundCells[neb].close = false;
            }
            if(!groundCells[neb].open  && !groundCells[neb].close)
            {

                std::cout << "NOT IN open and cost : " << cost << " AND NOT IN close " << std::endl;

                groundCells[neb].computeF();
                groundCells[neb].g = cost;

                openQueue.push(groundCells[neb]);
                groundCells[neb].open = true;

                if(groundCells[neb].father != nullptr)
                    std::cout << " the current neighbour " << neb << "  had father " <<  groundCells[neb].father->id << std::endl;

                groundCells[neb].father = &groundCells[current_id];
                std::cout << " the current neighbour " << neb << "  will have father " << current_id << std::endl;
            }

        }

    }

    reconstructPath();

}


void groundgrid::reconstructPath()
{
    path.clear();
    pathPositions.clear();
    uint goal_id = getIdFromPos(goal);
    uint start_id = getIdFromPos(start);

    if(groundCells[start_id].father == nullptr)
        std::cout << " starting cell has no father :)" << std::endl;
    else groundCells[start_id].father = nullptr;

    path.push_back(goal_id);
    pathPositions.push_back(groundCells[goal_id].cellPos);


    uint father_id = groundCells[goal_id].father->id;
    while(father_id != start_id)
    {
        path.push_back(father_id);
        pathPositions.push_back(groundCells[father_id].cellPos);
        std::cout << "father id = " << father_id << " in position : c: " << groundCells[father_id].cellPos.first << "  r: " << groundCells[father_id].cellPos.second << std::endl;
        father_id = groundCells[father_id].father->id;
    }

    path.push_back(start_id);
    pathPositions.push_back(groundCells[start_id].cellPos);




}


//H COMPUTATION with 2 different inputs (using MANHATTAN DISTANCE)
int groundgrid::H(cellT cell)
{
     return H(getIdFromPos(cell));
}
int groundgrid::H(uint cell_id)
{
   cellT pos = groundCells[cell_id].cellPos;
   return manhattanDist(pos, goal);
}
//manhattan distance
int groundgrid::manhattanDist(cellT start, cellT end)
{
    int start_c = start.first;
    int start_r = start.second;

    int end_c = end.first;
    int end_r = end.second;

    int distance = abs(end_c - start_c) + abs(end_r -start_r);
    return distance;
}

//G COMPUTATION with 2 different inputs
int groundgrid::G(cellT cell)
{
     return G(getIdFromPos(cell));
}
int groundgrid::G(uint cell_id)
{
   cellT pos = groundCells[cell_id].cellPos;
   return manhattanDist(pos, goal);
}

//Movement Cost to move to the close cells:
int groundgrid::movementCost(cellT A, cellT B)
{
    return movementCost(getIdFromPos(A), getIdFromPos(B));
}
int groundgrid::movementCost(uint A_id, uint B_id)
{
    cellT pos_A = groundCells[A_id].cellPos;
    cellT pos_B = groundCells[B_id].cellPos;
    int dist = manhattanDist(pos_A, pos_B);
    if(dist == 1)
        return 10;
    else if (dist == 2) return 15;
    else return 10*dist;
}









//SETTER AND GETTER
std::vector<groundgrid::cellT> groundgrid::getObstaclesPositions()
{
    return obstaclesPositions;
}
void groundgrid::setGoal(int c, int r)
{
    goal.first = c;
    goal.second = r;
}
void groundgrid::setStart(int c, int r)
{
    start.first = c;
    start.second = r;
}
glm::vec3 groundgrid::getGoalPos()
{
    return getCellPosition(goal.first,goal.second);
}
glm::vec3 groundgrid::getStartPos()
{
    return getCellPosition(start.first,start.second);
}

std::vector<groundgrid::cellT> groundgrid::getPathPositions()
{
    return pathPositions;
}
