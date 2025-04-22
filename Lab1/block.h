#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
using namespace std;

struct Coordinate {
    int x;
    int y;

    Coordinate() {}
    Coordinate(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

class Block {

public:
    int block_index;
    Block* b_top;
    Block* b_right;
    Block* b_bottom;
    Block* b_left;

    Coordinate c_tr; // Coordinate of top rights
    Coordinate c_bl; // Coordinate of bottom left

    void Print_info() {
        Print_index();
        Print_coordinate();
    }

    void Print_index() {
        cout << "block " << block_index << endl;
    }

    void Print_coordinate() {
        cout << "top right x: " << c_tr.x << endl;
        cout << "top right y: " << c_tr.y << endl;
        cout << "bot left  x: " << c_bl.x << endl;
        cout << "bot left  y: " << c_bl.y << endl;
    }

    Block(int block_index) {
        this->block_index = block_index;
        this->b_top = NULL;
        this->b_bottom = NULL;
        this->b_right = NULL;
        this->b_left = NULL;
    }

    Block(int block_index, int tr_x, int tr_y, int bl_x, int bl_y) {
        this->block_index = block_index;
        this->b_top = NULL;
        this->b_bottom = NULL;
        this->b_right = NULL;
        this->b_left = NULL;
        this->c_tr.x = tr_x;
        this->c_tr.y = tr_y;
        this->c_bl.x = bl_x;
        this->c_bl.y = bl_y;
    }

    Block(int block_index, Coordinate tr, Coordinate bl) {
        this->block_index = block_index;
        this->b_top = NULL;
        this->b_bottom = NULL;
        this->b_right = NULL;
        this->b_left = NULL;
        this->c_tr = tr;
        this->c_bl = bl;
    }

};


#endif