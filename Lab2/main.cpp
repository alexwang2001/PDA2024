# include <iostream>
# include <string>
# include <future>
# include "floorplan.h"
using namespace std;


int main(int argc, char* argv[]) {

    if (argc < 4) {
        cerr << "invalid input" << endl;
        return 1;
    }

    srand(5201314);
    string alpha_string(argv[1]);
    string block_file(argv[2]);
    string net_file(argv[3]);
    string out_file(argv[4]);
    double alpha = stod(alpha_string);


    FloorPlan floorplan_1, floorplan_2, floorplan_3, floorplan_4, floorplan_5, floorplan_6, floorplan_7, floorplan_8;
    floorplan_1.Read_block_file(block_file);
    floorplan_1.Read_net_file(net_file);
    floorplan_1.Init(alpha);
    floorplan_2.Read_block_file(block_file);
    floorplan_2.Read_net_file(net_file);
    floorplan_2.Init(alpha);
    floorplan_3.Read_block_file(block_file);
    floorplan_3.Read_net_file(net_file);
    floorplan_3.Init(alpha);
    floorplan_4.Read_block_file(block_file);
    floorplan_4.Read_net_file(net_file);
    floorplan_4.Init(alpha);

    floorplan_5.Read_block_file(block_file);
    floorplan_5.Read_net_file(net_file);
    floorplan_5.Init(alpha);
    floorplan_6.Read_block_file(block_file);
    floorplan_6.Read_net_file(net_file);
    floorplan_6.Init(alpha);
    floorplan_7.Read_block_file(block_file);
    floorplan_7.Read_net_file(net_file);
    floorplan_7.Init(alpha);
    floorplan_8.Read_block_file(block_file);
    floorplan_8.Read_net_file(net_file);
    floorplan_8.Init(alpha);

    unsigned long long cost_min = UINT64_MAX;
    unsigned long long cost_cur;
    int idx = 0;

    //floorplan.Optimize_SA(295, 1e7, 0.999);
    future<unsigned long long> f_1 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_1, 298, 1e7, 0.999);
    future<unsigned long long> f_2 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_2, 298, 1e7, 0.999);
    future<unsigned long long> f_3 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_3, 298, 1e7, 0.999);
    future<unsigned long long> f_4 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_4, 298, 1e7, 0.999);
    future<unsigned long long> f_5 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_5, 298, 1e7, 0.999);
    future<unsigned long long> f_6 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_6, 298, 1e7, 0.999);
    future<unsigned long long> f_7 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_7, 298, 1e7, 0.999);
    future<unsigned long long> f_8 = async(launch::async, &FloorPlan::Optimize_SA, &floorplan_8, 298, 1e7, 0.999);

    //cout << "async start\n";

    cost_cur = f_1.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 1;
    }
    cost_cur = f_2.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 2;
    }
    cost_cur = f_3.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 3;
    }
    cost_cur = f_4.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 4;
    }
    cost_cur = f_5.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 5;
    }
    cost_cur = f_6.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 6;
    }
    cost_cur = f_7.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 7;
    }
    cost_cur = f_8.get();
    if (cost_cur < cost_min) {
        cost_min = cost_cur;
        idx = 8;
    }

    //floorplan.Write_result(out_file);
    if (idx == 1) {
        floorplan_1.Write_result(out_file);
    }
    else if (idx == 2) {
        floorplan_2.Write_result(out_file);
    }
    else if (idx == 3) {
        floorplan_3.Write_result(out_file);
    }
    else if (idx == 4) {
        floorplan_4.Write_result(out_file);
    }
    else if (idx == 5) {
        floorplan_5.Write_result(out_file);
    }
    else if (idx == 6) {
        floorplan_6.Write_result(out_file);
    }
    else if (idx == 7) {
        floorplan_7.Write_result(out_file);
    }
    else if (idx == 8) {
        floorplan_8.Write_result(out_file);
    }

    return 0;

}