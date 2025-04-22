# include <iostream>
# include <fstream>
# include <sstream>
# include "gmap.h"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 4) {
        cout << "Input format: ./D2DGRter *.gmp *.gcl *.cst *.lg" << endl;
    }

    cout << "[main] gmp     : " << argv[1] << endl;
    cout << "[main] gcl     : " << argv[2] << endl;
    cout << "[main] cst     : " << argv[3] << endl;
    cout << "[main] lg      : " << argv[4] << endl;

    string gmp_path(argv[1]);
    string gcl_path(argv[2]);
    string cst_path(argv[3]);
    string lg_path(argv[4]);

    Gmap gmap;

    string line;

    // --- Read gmp file ---
    cout << "[main] Reading gmp file" << endl;
    ifstream gmp_file(gmp_path);

    int cell_idx = 0;

    while (getline(gmp_file, line)) {
        int idx, x, y, w, h;
        string token;
        stringstream ss(line);
        ss >> token;
        //cout << token << endl;

        if (token == ".ra") {
            getline(gmp_file, line);
            ss.clear();
            ss.str(line);
            ss >> x >> y >> w >> h;
            //cout << x << " " << y << " " << w << " " << h << endl;
            gmap.routing_area = Block(0, x, y, w, h);
        }
        else if (token == ".g") {
            getline(gmp_file, line);
            ss.clear();
            ss.str(line);
            ss >> gmap.grid_size.x >> gmap.grid_size.y;
            //cout << gmap.grid_size.x << " " << gmap.grid_size.y << endl;
        }
        else if (token == ".c") {
            getline(gmp_file, line);
            ss.clear();
            ss.str(line);
            ss >> x >> y >> w >> h;
            //cout << x << " " << y << " " << w << " " << h << endl;
            gmap.chip_info.push_back(Block(cell_idx, x, y, w, h));
        }
        else if (token == ".b") {
            vector<Block> bumps;
            while (getline(gmp_file, line) && !line.empty()) {
                ss.clear();
                ss.str(line);
                ss >> idx >> x >> y;
                //cout << idx << " " << x << " " << y << endl;
                bumps.push_back(Block(idx, x, y, 0, 0));
            }
            gmap.bump_info.push_back(bumps);
            cell_idx++;
        }
        else {
            cout << "no such token" << endl;
            exit(1);
        }
    }

    gmp_file.close();

    // --- Read gcl file ---
    cout << "[main] Reading gcl file" << endl;
    ifstream gcl_file(gcl_path);

    while (getline(gcl_file, line)) {
        int cap_x, cap_y;
        string token;
        stringstream ss(line);
        ss >> token;
        if (token == ".ec") {
            gmap.n_w = gmap.routing_area.Width() / gmap.grid_size.x;
            gmap.n_h = gmap.routing_area.Height() / gmap.grid_size.y;
            gmap.gcell.resize(gmap.n_w);
            for (int i = 0; i < gmap.n_w; i++) {
                gmap.gcell[i].resize(gmap.n_h);
            }
            for (int j = 0; j < gmap.n_h; j++) {
                for (int i = 0; i < gmap.n_w; i++) {
                    getline(gcl_file, line);
                    ss.clear();
                    ss.str(line);
                    ss >> cap_x >> cap_y;
                    gmap.gcell[i][j] = Coordinate(cap_x, cap_y);
                }
            }
        }
        else {
            cout << "no such token" << endl;
            exit(1);
        }
    }

    gcl_file.close();

    // --- Read cst file ---
    cout << "[main] Reading cst file" << endl;
    ifstream cst_file(cst_path);

    int grid_idx = 0;
    gmap.largest_cell_cost = 0;

    while (getline(cst_file, line)) {
        double value;
        string token;
        stringstream ss(line);
        ss >> token;
        if (token == ".alpha") {
            ss >> value;
            gmap.alpha = value;
        }
        else if (token == ".beta") {
            ss >> value;
            gmap.beta = value;
        }
        else if (token == ".gamma") {
            ss >> value;
            gmap.gamma = value;
        }
        else if (token == ".delta") {
            ss >> value;
            gmap.delta = value;
        }
        else if (token == ".v") {
            getline(cst_file, line);
            ss.clear();
            ss.str(line);
            ss >> gmap.via_cost;
        }
        else if (token == ".l") {
            if (grid_idx == 0) {

                gmap.m1_grid_cost.resize(gmap.n_w);
                for (int i = 0; i < gmap.n_w; i++) {
                    gmap.m1_grid_cost[i].resize(gmap.n_h);
                }
                int i = 0;
                while (i < gmap.n_w * gmap.n_h) {
                    getline(cst_file, line);
                    ss.clear();
                    ss.str(line);
                    while (ss >> value) {
                        if (value > gmap.largest_cell_cost) gmap.largest_cell_cost = value;
                        gmap.m1_grid_cost[i % gmap.n_w][i / gmap.n_w] = value;
                        i++;
                    }
                }
                grid_idx = 1;
            }
            else {
                gmap.m2_grid_cost.resize(gmap.n_w);
                for (int i = 0; i < gmap.n_w; i++) {
                    gmap.m2_grid_cost[i].resize(gmap.n_h);
                }
                int i = 0;
                while (i < gmap.n_w * gmap.n_h) {
                    getline(cst_file, line);
                    ss.clear();
                    ss.str(line);
                    while (ss >> value) {
                        if (value > gmap.largest_cell_cost) gmap.largest_cell_cost = value;
                        gmap.m2_grid_cost[i % gmap.n_w][i / gmap.n_w] = value;
                        i++;
                    }
                }
            }
        }
        else {
            cout << "no such token" << endl;
            exit(1);
        }
    }

    cst_file.close();

    // --- Decide routing order ---
    gmap.Create_Bump_List();

    // --- Run A* Algorithm ---
    gmap.Route();

    // --- Output ---
    gmap.Output_result(lg_path);


    return 0;
} 