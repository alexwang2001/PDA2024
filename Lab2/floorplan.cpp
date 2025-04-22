# include <iostream>
# include <fstream>
# include <algorithm>
# include <cmath>
# include "floorplan.h"
using namespace std;


void FloorPlan::Read_block_file(string file_path) {
    ifstream in_file(file_path, ios::in);
    string token;

    // Read header
    in_file >> token >> outline.first >> outline.second;
    //cout << "outline " << outline.first << " " << outline.second << endl;
    in_file >> token >> num_blocks;
    //cout << "NumBlocks " << num_blocks << endl;
    in_file >> token >> num_terminals;
    //cout << "NumTerminals " << num_terminals << endl;

    while (in_file.peek() == 13 || in_file.peek() == 10) {
        in_file.get();
    }

    //cout << "-- Read blocks --\n";
    // Read blocks
    for (int i = 0; i < num_blocks; i++) {
        Block new_block;
        in_file >> new_block.block_name >> new_block.w >> new_block.h;
        vec_block.push_back(new_block);
    }

    while (in_file.peek() == 13 || in_file.peek() == 10) {
        in_file.get();
    }

    //cout << "-- Read terminals --\n";
    // Read terminals
    for (int i = 0; i < num_terminals; i++) {
        Terminal new_terminal;
        in_file >> new_terminal.terminal_name >> token >> new_terminal.coordinate.x >> new_terminal.coordinate.y;
        vec_terminal.push_back(new_terminal);
    }

    in_file.close();
}


void FloorPlan::Read_net_file(string file_path) {
    ifstream in_file(file_path, ios::in);
    string token;

    in_file >> token >> num_nets;
    for (int i = 0; i < num_nets; i++) {
        int net_degree;
        vector<pair<bool, int>> net;
        in_file >> token >> net_degree;
        for (int j = 0; j < net_degree; j++) {
            string net_name;
            in_file >> net_name;
            for (int k = 0; k < num_blocks; k++) {
                if (net_name == vec_block[k].block_name) {
                    net.push_back(pair<bool, int>(0, k));
                    break;
                }
            }
            for (int k = 0; k < num_terminals; k++) {
                if (net_name == vec_terminal[k].terminal_name) {
                    net.push_back(pair<bool, int>(1, k));
                    break;
                }
            }
        }
        vec_nets.push_back(net);
    }

    in_file.close();
}

void FloorPlan::Write_result(string file_path) {

    ofstream out_file(file_path, ios::out);
    out_file << cost_G << "\n";
    out_file << hpwl_G << "\n";
    out_file << area_G << "\n";
    out_file << width_G << " ";
    out_file << height_G << "\n";
    out_file << time(NULL) - start_time << "\n";
    for (int i = 0; i < num_blocks; i++) {
        out_file << vec_block[i].block_name << " ";
        out_file << P_x_G[i] << " " << P_y_G[i] << " ";
        out_file << P_x_G[i] + vec_block_G[i].w << " ";
        out_file << P_y_G[i] + vec_block_G[i].h << "\n";
    }
    out_file.close();
    /*
    ofstream out_file_2("./draw/draw.txt", ios::out);
    out_file_2 << num_blocks << endl;
    out_file_2 << width_G << " ";
    out_file_2 << height_G << endl;
    for (int i = 0; i < num_blocks; i++) {
        out_file_2 << 1 << " ";
        out_file_2 << P_x_G[i] << " " << P_y_G[i] << " ";
        out_file_2 << vec_block_G[i].w << " ";
        out_file_2 << vec_block_G[i].h << endl;
    }
    out_file_2.close();
    */
}

void FloorPlan::Init(double alpha) {
    // Set alpha
    this->alpha = alpha;

    // Init X, Y
    for (int i = 0; i < num_blocks; i++) {
        X.push_back(i);
        Y.push_back(i);
    }
    random_shuffle(X.begin(), X.end());
    random_shuffle(Y.begin(), Y.end());

    X_s.assign(X.begin(), X.end());
    Y_s.assign(Y.begin(), Y.end());

    L.resize(num_blocks);
    P_x.resize(num_blocks);
    P_y.resize(num_blocks);

    match.resize(num_blocks);

    vec_block_rotate.assign(vec_block.begin(), vec_block.end());

    // Init match
    for (int i = 0; i < num_blocks; i++) {
        for (int j = 0; j < num_blocks; j++) {
            if (Y[j] == X[i]) {
                match[X[i]] = j;
                break;
            }
        }
    }

    match_s.assign(match.begin(), match.end());
    /*
    cout << "match: ";
    for (int i = 0; i < num_blocks; i++) {
        cout << match[i] << " ";
    }
    cout << endl;
    */
}

unsigned long long FloorPlan::Optimize_SA(int max_sec, double T0, double beta) {
    double T = T0;

    width = LCS_W();
    height = LCS_H();
    area = width * height;
    hpwl = HPWL();
    cost = Cost(area, hpwl);
    initial_cost = cost;
    unsigned long long lowest_cost = cost;
    double err = double(cost) / initial_cost + Out_line_error();
    double err_best = err;

    while (time(NULL) - start_time < max_sec) {
        Create_new_successor();
        width = LCS_W();
        height = LCS_H();
        area = width * height;
        hpwl = HPWL();
        cost = Cost(area, hpwl);
        err = double(cost) / initial_cost + Out_line_error();
        if (err_best - err > 0) {
            Save_new_successor();
            err_best = err;
            last_update_time = time(NULL);
        }
        else {
            if (double(rand() & 0xFF) / 0xFF < exp((err_best - err) / T)) {
                Save_new_successor();
                err_best = err;
            }
            else {
                Discard_new_successor();
            }
        }
        // Save the best
        if (cost < lowest_cost) {
            if (width <= outline.first && height <= outline.second) {
                lowest_cost = cost;
                vec_block_G.assign(vec_block.begin(), vec_block.end());
                P_x_G.assign(P_x.begin(), P_x.end());
                P_y_G.assign(P_y.begin(), P_y.end());
                hpwl_G = hpwl;
                area_G = area;
                cost_G = cost;
                width_G = width;
                height_G = height;
                /*
                cout << " Width: " << width << " Height: " << height;
                cout << " Area: " << area << " Length: " << hpwl;
                cout << " Cost: " << cost << " Error: " << err;
                cout << " Temp: " << T << endl;
                */
            }
        }
        // Random restart
        if (T < 1e-100 && rand() % 100000 == 0 && time(NULL) - last_update_time > 5) {
            T = 1;
        }
        T *= beta;
    }

    return cost_G;
}

int FloorPlan::LCS_W() {
    vector<int>::iterator iter;
    // Clear L
    for (iter = L.begin(); iter != L.end(); iter++) {
        *iter = 0;
    }
    // Find LCS
    for (iter = X_s.begin(); iter != X_s.end(); iter++) {
        int x = *iter;
        int p = match_s[x];
        P_x[x] = L[p];
        int t = P_x[x] + vec_block_rotate[x].w;
        for (int j = p; j < num_blocks; j++) {
            if (t > L[j]) {
                L[j] = t;
            }
            else {
                break;
            }
        }
    }
    /*
    cout << "L: ";
    for (int i = 0; i < num_blocks; i++) {
        cout << L[i] << " ";
    }
    cout << endl;
    cout << "P_x: ";
    for (int i = 0; i < num_blocks; i++) {
        cout << P_x[i] << " ";
    }
    cout << endl;
    */
    return L.back();
}

int FloorPlan::LCS_H() {
    vector<int>::iterator iter;
    // Clear L
    for (iter = L.begin(); iter != L.end(); iter++) {
        *iter = 0;
    }
    vector<int>::reverse_iterator riter;
    // Find LCS
    for (riter = X_s.rbegin(); riter != X_s.rend(); riter++) {
        int x = *riter;
        int p = match_s[x];
        P_y[x] = L[p];
        int t = P_y[x] + vec_block_rotate[x].h;
        for (int j = p; j < num_blocks; j++) {
            if (t > L[j]) {
                L[j] = t;
            }
            else {
                break;
            }
        }
    }

    /*
    cout << "L: ";
    for (int i = 0; i < num_blocks; i++) {
        cout << L[i] << " ";
    }
    cout << endl;
    cout << "P_y: ";
    for (int i = 0; i < num_blocks; i++) {
        cout << P_y[i] << " ";
    }
    cout << endl;
    */

    return L.back();
}

unsigned long long FloorPlan::HPWL() {
    unsigned long long length = 0;
    for (auto net : vec_nets) {
        int max_x = 0;
        int max_y = 0;
        int min_x = INT32_MAX;
        int min_y = INT32_MAX;
        for (auto pin : net) {
            int x, y;
            if (pin.first) { // terminal
                x = vec_terminal[pin.second].coordinate.x;
                y = vec_terminal[pin.second].coordinate.y;
            }
            else { // block
                x = P_x[pin.second] + int(vec_block_rotate[pin.second].w / 2);
                y = P_y[pin.second] + int(vec_block_rotate[pin.second].h / 2);
            }
            if (x > max_x) {
                max_x = x;
            }
            if (x < min_x) {
                min_x = x;
            }
            if (y > max_y) {
                max_y = y;
            }
            if (y < min_y) {
                min_y = y;
            }
            //cout << max_x << " " << min_x << " " << max_y << " " << min_y << endl;
        }
        length += (max_x - min_x) + (max_y - min_y);
    }
    return length;
}

unsigned long long FloorPlan::Cost(unsigned long long area, unsigned long long wire_length) {
    return alpha * area + (1 - alpha) * wire_length;
}

double FloorPlan::Dead_area_error() {
    double full_area = outline.first * outline.second;
    double dead_area = full_area;
    for (int i = 0; i < num_blocks; i++) {
        if (P_x[i] < outline.first && P_y[i] < outline.second) {
            int w_in_outline = P_x[i] + vec_block_rotate[i].w < outline.first ? vec_block_rotate[i].w : outline.first - P_x[i];
            int h_in_outline = P_y[i] + vec_block_rotate[i].h < outline.second ? vec_block_rotate[i].h : outline.second - P_y[i];
            dead_area -= w_in_outline * h_in_outline;
        }
    }
    return dead_area / full_area;
}

double FloorPlan::Out_line_error() {
    double out_of_w = 0, out_of_h = 0;

    if (width > outline.first) {
        out_of_w = width - outline.first;
    }
    if (height > outline.second) {
        out_of_h = height - outline.second;
    }

    return out_of_w * out_of_w + out_of_h * out_of_h;
}

void FloorPlan::Create_new_successor() {
    // Select two random blocks
    int n_1, n_2;
    n_1 = rand() % num_blocks;
    n_2 = rand() % num_blocks;
    if (n_1 == n_2) n_2 = (n_2 == num_blocks - 1) ? 0 : n_2 + 1;

    int tmp;
    int r = rand();
    // M1 -> swap only one side
    if (r % 3 == 2) {
        move = 0;
        tmp = X_s[n_1];
        X_s[n_1] = X_s[n_2];
        X_s[n_2] = tmp;
    }
    // M2 -> swap both sides
    else if (r % 3 == 1) {
        move = 1;
        tmp = X_s[n_1];
        X_s[n_1] = X_s[n_2];
        X_s[n_2] = tmp;
        tmp = match_s[X_s[n_1]];
        match_s[X_s[n_1]] = match_s[X_s[n_2]];
        match_s[X_s[n_2]] = tmp;
        tmp = Y[match_s[X_s[n_1]]];
        Y[match_s[X_s[n_1]]] = Y[match_s[X_s[n_2]]];
        Y[match_s[X_s[n_2]]] = tmp;
    }
    // M3 -> Rotate block
    else {
        move = 2;
        tmp = vec_block_rotate[n_1].h;
        vec_block_rotate[n_1].h = vec_block_rotate[n_1].w;
        vec_block_rotate[n_1].w = tmp;
    }
}

void FloorPlan::Discard_new_successor() {
    if (move < 2) {
        X_s.assign(X.begin(), X.end());
        Y_s.assign(Y.begin(), Y.end());
        match_s.assign(match.begin(), match.end());
    }
    else {
        vec_block_rotate.assign(vec_block.begin(), vec_block.end());
    }
}

void FloorPlan::Save_new_successor() {
    if (move < 2) {
        X.assign(X_s.begin(), X_s.end());
        Y.assign(Y_s.begin(), Y_s.end());
        match.assign(match_s.begin(), match_s.end());
    }
    else {
        vec_block.assign(vec_block_rotate.begin(), vec_block_rotate.end());
    }
}
