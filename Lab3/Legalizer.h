#ifndef LEGALIZER_H
#define LEGALIZER_H

#include <vector>
#include <map>
#include <queue>
#include "block.h"


class Placement_Row {
public:
    int start_x, start_y;
    int width, height;
    int num_of_sites;

    Placement_Row(int start_x, int start_y, int width, int height, int num_of_sites) {
        this->start_x = start_x;
        this->start_y = start_y;
        this->width = width;
        this->height = height;
        this->num_of_sites = num_of_sites;
    }

    void operator=(Placement_Row pr) {
        start_x = pr.start_x;
        start_y = pr.start_y;
        width = pr.width;
        height = pr.height;
        num_of_sites = pr.num_of_sites;
    }
};

class Legalizer {
public:

    double alpha;
    double beta;
    int die_x_0, die_x_1;
    int die_y_0, die_y_1;

    Coordinate placement_row_bl;
    Coordinate placement_row_tr;

    int placement_row_height;
    int placement_row_site_width;

    vector<vector<Block>> placement;
    map<string, Block> block_map;

    Legalizer();

    void set_alpha(double alpha);
    void set_beta(double beta);
    void set_die_coordinate(int x0, int y0, int x1, int y1);
    void add_placement_row(Placement_Row placement_row);

    void Initailize();

    void Block_Creating(string block_name, Coordinate bl, Coordinate tr);
    void Block_Creating_End();

    void Block_Inserting(string block_name, Coordinate bl, Coordinate tr);
    Coordinate Insert_cell(string block_name, Coordinate bl, Coordinate tr, vector<Block>& move_record);
    void Delete_cell(string block_name);

    vector<Block>::iterator Find_X(int row, int x);
    void Find_overlap(vector<Block>& overlap_blocks, Coordinate bl, Coordinate tr);

    void Gen_layout_txt(char* layout_file_name);
    bool Block_in_vec(vector<Block>& vec, Block& block);
    bool Has_block(string block_name);


};

#endif