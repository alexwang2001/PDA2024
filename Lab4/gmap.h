#ifndef GMAP_H
#define GMAP_H

# include <iostream>
# include <vector>
# include <stack>
# include <utility>
# include <algorithm>
# include <map>
# include <float.h>
# include <fstream>
# include <functional>
# include <bits/stdc++.h>
using namespace std;

class Coordinate {
public:
    int x;
    int y;

    Coordinate() {}
    Coordinate(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void Print() {
        cout << "x: " << x << " y: " << y << endl;
    }

    Coordinate operator+(Coordinate c) {
        return Coordinate(this->x + c.x, this->y + c.y);
    }

    void operator=(Coordinate c) {
        this->x = c.x;
        this->y = c.y;
    }

    bool operator==(Coordinate& c) {
        if (this->x == c.x && this->y == c.y) {
            return 1;
        }
        return 0;
    }

    bool operator!=(Coordinate& c) {
        if (this->x != c.x || this->y != c.y) {
            return 1;
        }
        return 0;
    }
};

class Block {
public:
    int idx;
    Coordinate c_wh; // Width and Height
    Coordinate c_bl; // Coordinate of bottom left

    int Height() {
        return c_wh.y;
    }

    int Width() {
        return c_wh.x;
    }

    void Print_info() {
        cout << "---------------------------\n";
        Print_index();
        Print_coordinate();
        cout << "---------------------------\n";
    }

    void Print_index() {
        cout << "block " << idx << endl;
    }

    void Print_coordinate() {
        cout << "bot left  x: " << c_bl.x << endl;
        cout << "bot left  y: " << c_bl.y << endl;
        cout << "width : " << c_wh.x << endl;
        cout << "height: " << c_wh.y << endl;
    }

    Block() {}

    Block(int index, int bl_x, int bl_y, int width, int height) {
        this->idx = index;
        this->c_wh.x = width;
        this->c_wh.y = height;
        this->c_bl.x = bl_x;
        this->c_bl.y = bl_y;
    }

    Block(int index, Coordinate tr, Coordinate bl) {
        this->idx = index;
        this->c_wh = Coordinate(tr.x - bl.x, tr.y - bl.y);
        this->c_bl = bl;
    }

    Block(Block* bptr) {
        this->idx = bptr->idx;
        this->c_wh = bptr->c_wh;
        this->c_bl = bptr->c_bl;
    }

    void operator=(Block& b) {
        this->idx = b.idx;
        this->c_wh = b.c_wh;
        this->c_bl = b.c_bl;
    }

    void operator=(Block b) {
        this->idx = b.idx;
        this->c_wh = b.c_wh;
        this->c_bl = b.c_bl;
    }

};

class BumpPair {
public:
    int index;
    Coordinate source;
    Coordinate sink;
    double distance;
};

enum DIRECTION { TOP, DOWN, RIGHT, LEFT, NONE, SOURCE };

class GraphNode {
public:
    double g;
    double h;
    Coordinate pos;
    DIRECTION direction;
    GraphNode() { g = DBL_MAX; h = DBL_MAX; direction = NONE; }

};

class Gmap {
public:
    // routing area 
    Block routing_area;
    Coordinate grid_size;
    int n_w, n_h;

    // chip info
    vector<Block> chip_info;
    vector<vector<Block>> bump_info;

    // gcell info
    vector<vector<Coordinate>> gcell;

    // Costs
    double alpha, beta, gamma, delta;
    double via_cost;
    double largest_cell_cost;
    vector<vector<double>> m1_grid_cost, m2_grid_cost;

    // Calculations
    vector<BumpPair> bump_pair_list;

    // Result save
    vector<pair<int, stack<Coordinate>>> result_vec;

    Gmap() {}

    // Functions
    void Create_Bump_List();
    void Route();

    double Additional_cost(double a, double b, double g, double d);
    double Heuristic_cost(Coordinate start, Coordinate end, DIRECTION d);
    void Update_Position(Coordinate& pos, DIRECTION d);
    void Output_result(string file_path);

};



#endif