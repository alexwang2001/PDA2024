#ifndef OPERATION_H
#define OPERATION_H
#include <vector>
#include <queue>
#include "block.h"
using namespace std;

/*
*    bl : bottom left
*    tr : top right
*/

class Graph {
public:

    Block* start_block;
    Coordinate range;
    queue<Coordinate> points_found;
    vector<Block*> blocks;
    vector<Block*> non_empty_blocks;

    void Create_start_block();
    Block* Point_Finding(int tx, int ty);
    void Block_Creating(int block_index, Coordinate bl, Coordinate tr);
    void Gen_layout_txt(char* output_path);
    void Gen_output_txt(char* output_path);

private:

    Block* Point_Finding(Block* start_block, int tx, int ty);
    Block* Horizontal_split(Block* target_block, int y, bool rtn_up/*1:up 0:down*/);
    Block* Vertical_split(Block* target_block, int x, bool rtn_left/*1:left 0:right*/);
    Block* Horizontal_merge(Block* target_block_up, Block* target_block_down);
    void Neighbor_count(Block* target_block, int& num_block, int& num_space);
    void Merge_check(Block* block);


};







#endif