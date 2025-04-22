#ifndef FLOORPLAN_H
#define FLOORPLAN_H

# include <iostream>
# include <utility>
# include <vector>
# include <time.h>
using namespace std;


typedef struct Coordinate {
    int x;
    int y;
} Coordinate;

typedef struct Block {
    string block_name;
    int w;
    int h;
} Block;

typedef struct Terminal {
    string terminal_name;
    Coordinate coordinate;
} Terminal;

class FloorPlan {
public:
    // === Functions ===
    void Read_block_file(string file_path);
    void Read_net_file(string file_path);
    void Init(double alpha);
    unsigned long long Optimize_SA(int max_sec, double T0, double beta); // Simulated annealing
    void Write_result(string file_path);

private:
    // === Inputs ===
    pair<int, int> outline;
    int num_blocks;
    int num_terminals;
    int num_nets;

    vector<Block> vec_block;
    vector<Terminal> vec_terminal;
    vector<vector<pair<bool, int>>> vec_nets; // <0: block 1: terminal, index>

    double alpha;

    // === Algo ===
    // Init
    unsigned long long initial_cost;

    // Global best
    vector<Block> vec_block_G;
    vector<int> P_x_G;
    vector<int> P_y_G;
    unsigned long long hpwl_G;
    unsigned long long area_G;
    unsigned long long cost_G;
    int width_G;
    int height_G;

    // Save state
    vector<int> X;
    vector<int> Y;
    vector<int> match;

    // Successor
    vector<int> X_s;
    vector<int> Y_s;
    vector<int> match_s;
    vector<Block> vec_block_rotate;

    vector<int> L;
    vector<int> P_x;
    vector<int> P_y;

    unsigned long long hpwl;
    unsigned long long area;
    unsigned long long cost;
    int width;
    int height;

    // === Timer ===
    clock_t start_time = time(NULL);
    clock_t last_update_time = start_time;

    // Others 
    int move; // 0: M1 1: M2 2: M3
    int _eqetqpqrp;

    // === Functions ===
    unsigned long long Cost(unsigned long long area, unsigned long long wire_length);
    int LCS_W();
    int LCS_H();
    unsigned long long HPWL();
    double Out_line_error();
    double Dead_area_error();

    void Create_new_successor();
    void Discard_new_successor();
    void Save_new_successor();
};


#endif