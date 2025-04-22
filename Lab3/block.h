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

    void operator=(Coordinate c) {
        this->x = c.x;
        this->y = c.y;
    }
};

class Block {

public:
    string block_name;

    Coordinate c_tr; // Coordinate of top rights
    Coordinate c_bl; // Coordinate of bottom left

    int Height() {
        return c_tr.y - c_bl.y;
    }

    int Width() {
        return c_tr.x - c_bl.x;
    }

    void Print_info() {
        Print_name();
        Print_coordinate();
    }

    void Print_name() {
        cout << "block " << block_name << endl;
    }

    void Print_coordinate() {
        cout << "bot left  x: " << c_bl.x << endl;
        cout << "bot left  y: " << c_bl.y << endl;
        cout << "top right x: " << c_tr.x << endl;
        cout << "top right y: " << c_tr.y << endl;
    }

    Block(string name) {
        this->block_name = name;
    }

    Block(string name, int tr_x, int tr_y, int bl_x, int bl_y) {
        this->block_name = name;
        this->c_tr.x = tr_x;
        this->c_tr.y = tr_y;
        this->c_bl.x = bl_x;
        this->c_bl.y = bl_y;
    }

    Block(string name, Coordinate tr, Coordinate bl) {
        this->block_name = name;
        this->c_tr = tr;
        this->c_bl = bl;
    }

    Block(Block* bptr) {
        this->block_name = bptr->block_name;
        this->c_tr = bptr->c_tr;
        this->c_bl = bptr->c_bl;
    }

    void operator=(Block& b) {
        this->block_name = b.block_name;
        this->c_tr = b.c_tr;
        this->c_bl = b.c_bl;
    }

    void operator=(Block b) {
        this->block_name = b.block_name;
        this->c_tr = b.c_tr;
        this->c_bl = b.c_bl;
    }

};


#endif