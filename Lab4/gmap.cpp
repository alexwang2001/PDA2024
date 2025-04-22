# include "gmap.h"
using namespace std;


void Gmap::Create_Bump_List() {
    // Match pair
    for (int i = 0; i < bump_info[0].size(); i++) {
        for (int j = 0; j < bump_info[1].size(); j++) {
            if (bump_info[0][i].idx == bump_info[1][j].idx) {
                BumpPair new_pair;
                new_pair.index = bump_info[0][i].idx;
                new_pair.source = bump_info[0][i].c_bl + chip_info[0].c_bl;
                new_pair.sink = bump_info[1][j].c_bl + chip_info[1].c_bl;
                new_pair.source.x /= grid_size.x;
                new_pair.source.y /= grid_size.y;
                new_pair.sink.x /= grid_size.x;
                new_pair.sink.y /= grid_size.y;
                //new_pair.source.Print();
                //new_pair.sink.Print();
                bump_pair_list.push_back(new_pair);
                break;
            }
        }
    }
    // Calculate distance
    for (vector<BumpPair>::iterator iter = bump_pair_list.begin(); iter != bump_pair_list.end(); iter++) {
        iter->distance = abs(iter->sink.x - iter->source.x) + abs(iter->sink.y - iter->source.y);
    }
    // Sort by distance
    sort(bump_pair_list.begin(), bump_pair_list.end(), [](BumpPair a, BumpPair b) {return a.distance < b.distance;});
}

void Gmap::Route() {

    // Bump pair loop
    for (vector<BumpPair>::iterator bump_pair_ptr = bump_pair_list.begin(); bump_pair_ptr != bump_pair_list.end(); bump_pair_ptr++) {

        //cout << "-------------------------------------" << endl;
        //cout << "[gmap] Start routing net " << bump_pair_ptr->index << endl;

        vector<vector<GraphNode>> grid_map;
        grid_map.resize(n_w);
        for (int i = 0; i < n_w; i++) {
            grid_map[i].resize(n_h);
        }
        for (int x = 0; x < n_w; x++) {
            for (int y = 0; y < n_h; y++) {
                grid_map[x][y].pos.x = x;
                grid_map[x][y].pos.y = y;
            }
        }

        Coordinate cur_pos, next_pos, start_pos, tar_pos;
        cur_pos = bump_pair_ptr->source;
        start_pos = bump_pair_ptr->source;
        tar_pos = bump_pair_ptr->sink;

        // Add start node to open list
        vector<reference_wrapper<GraphNode>> open_list;
        open_list.push_back(grid_map[cur_pos.x][cur_pos.y]);
        grid_map[cur_pos.x][cur_pos.y].direction = SOURCE;
        grid_map[cur_pos.x][cur_pos.y].g = 0;
        grid_map[cur_pos.x][cur_pos.y].h = Heuristic_cost(cur_pos, tar_pos, NONE);

        auto Heap_cmp = [](reference_wrapper<GraphNode> a, reference_wrapper<GraphNode> b) {return (a.get().g + a.get().h) > (b.get().g + b.get().h);};
        make_heap(open_list.begin(), open_list.end(), Heap_cmp);


        while (!open_list.empty()) {

            DIRECTION cur_dir = open_list.front().get().direction;
            double cur_g = open_list.front().get().g;
            cur_pos = open_list.front().get().pos;


            //cout << "[gmap] Current position  ";
            //cur_pos.Print();
            //cout << "[gmap] g: " << open_list.front().get().g << " h: " << open_list.front().get().h << endl;

            pop_heap(open_list.begin(), open_list.end(), Heap_cmp);
            open_list.pop_back();

            // Reach sink point
            if (cur_pos == tar_pos) {

                // Back Trace
                //cout << "[gmap] Back Tracing" << endl;

                double cost = 0;
                DIRECTION last_dir = grid_map[cur_pos.x][cur_pos.y].direction;
                stack<Coordinate> result;

                // The first grid
                if (last_dir == LEFT || last_dir == RIGHT) {
                    //cost += Additional_cost(1, 0, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1);
                    result.push(Coordinate(-1, -1));
                    result.push(cur_pos);
                }
                else {
                    //cost += Additional_cost(1, 0, m1_grid_cost[cur_pos.x][cur_pos.y], 0);
                    result.push(cur_pos);
                }
                //cout << "[gmap] ";
                //grid_map[cur_pos.x][cur_pos.y].pos.Print();

                while (cur_pos != start_pos) {

                    Update_Position(cur_pos, grid_map[cur_pos.x][cur_pos.y].direction);

                    while (last_dir == grid_map[cur_pos.x][cur_pos.y].direction && cur_pos != start_pos) {
                        last_dir = grid_map[cur_pos.x][cur_pos.y].direction;
                        Update_Position(cur_pos, grid_map[cur_pos.x][cur_pos.y].direction);
                    }

                    if (cur_pos == start_pos) {
                        result.push(cur_pos);
                        break;
                    }

                    last_dir = grid_map[cur_pos.x][cur_pos.y].direction;

                    //cout << "[gmap] ";
                    //grid_map[cur_pos.x][cur_pos.y].pos.Print();
                    result.push(cur_pos);
                    result.push(Coordinate(-1, -1));
                    result.push(cur_pos);
                }

                if (last_dir == LEFT || last_dir == RIGHT) result.push(Coordinate(-1, -1));

                result_vec.push_back(make_pair(bump_pair_ptr->index, result));

                break;
            }

            double g;
            double h;

            // top
            if (cur_pos.y + 1 < n_h) {
                next_pos = Coordinate(cur_pos.x, cur_pos.y + 1);
                // Calculate g and h
                if (cur_dir == SOURCE || cur_dir == DOWN) {
                    if (gcell[next_pos.x][next_pos.y].y <= 0) {
                        g = Additional_cost(grid_size.y, 1, m1_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.y, 0, m1_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, DOWN);
                }
                else if (cur_dir == LEFT || cur_dir == RIGHT) {
                    if (gcell[next_pos.x][next_pos.y].y <= 0) {
                        g = Additional_cost(grid_size.y, 1, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.y, 0, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, DOWN);
                }
                // Update
                if (cur_dir != TOP && g + h < grid_map[next_pos.x][next_pos.y].g + grid_map[next_pos.x][next_pos.y].h) {
                    grid_map[next_pos.x][next_pos.y].g = g;
                    grid_map[next_pos.x][next_pos.y].h = h;
                    grid_map[next_pos.x][next_pos.y].direction = DOWN;
                    open_list.push_back(grid_map[next_pos.x][next_pos.y]);
                    push_heap(open_list.begin(), open_list.end(), Heap_cmp);
                }
            }

            // bottom 
            if (cur_pos.y - 1 >= 0) {
                next_pos = Coordinate(cur_pos.x, cur_pos.y - 1);
                // Calculate g and h
                if (cur_dir == SOURCE || cur_dir == TOP) {
                    if (gcell[cur_pos.x][cur_pos.y].y <= 0) {
                        g = Additional_cost(grid_size.y, 1, m1_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.y, 0, m1_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, TOP);
                }
                else if (cur_dir == LEFT || cur_dir == RIGHT) {
                    if (gcell[cur_pos.x][cur_pos.y].y <= 0) {
                        g = Additional_cost(grid_size.y, 1, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.y, 0, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, TOP);
                }
                // Update
                if (cur_dir != DOWN && g + h < grid_map[next_pos.x][next_pos.y].g + grid_map[next_pos.x][next_pos.y].h) {
                    grid_map[next_pos.x][next_pos.y].g = g;
                    grid_map[next_pos.x][next_pos.y].h = h;
                    grid_map[next_pos.x][next_pos.y].direction = TOP;
                    open_list.push_back(grid_map[next_pos.x][next_pos.y]);
                    push_heap(open_list.begin(), open_list.end(), Heap_cmp);
                }
            }

            // right 
            if (cur_pos.x + 1 < n_w) {
                next_pos = Coordinate(cur_pos.x + 1, cur_pos.y);
                // Calculate g and h
                if (cur_dir == LEFT) {
                    if (gcell[next_pos.x][next_pos.y].x <= 0) {
                        g = Additional_cost(grid_size.x, 1, m2_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.x, 0, m2_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, LEFT);
                }
                else if (cur_dir == SOURCE || cur_dir == TOP || cur_dir == DOWN) {
                    if (gcell[next_pos.x][next_pos.y].x <= 0) {
                        g = Additional_cost(grid_size.x, 1, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.x, 0, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, LEFT);
                }

                // Update
                if (cur_dir != RIGHT && g + h < grid_map[next_pos.x][next_pos.y].g + grid_map[next_pos.x][next_pos.y].h) {
                    grid_map[next_pos.x][next_pos.y].g = g;
                    grid_map[next_pos.x][next_pos.y].h = h;
                    grid_map[next_pos.x][next_pos.y].direction = LEFT;
                    open_list.push_back(grid_map[next_pos.x][next_pos.y]);
                    push_heap(open_list.begin(), open_list.end(), Heap_cmp);
                }
            }

            // left
            if (cur_pos.x - 1 >= 0) {
                next_pos = Coordinate(cur_pos.x - 1, cur_pos.y);
                // Calculate g and h
                if (cur_dir == RIGHT) {
                    if (gcell[cur_pos.x][cur_pos.y].x <= 0) {
                        g = Additional_cost(grid_size.x, 1, m2_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.x, 0, m2_grid_cost[cur_pos.x][cur_pos.y], 0) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, RIGHT);
                }
                else if (cur_dir == SOURCE || cur_dir == TOP || cur_dir == DOWN) {
                    if (gcell[cur_pos.x][cur_pos.y].x <= 0) {
                        g = Additional_cost(grid_size.x, 1, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    else {
                        g = Additional_cost(grid_size.x, 0, (m1_grid_cost[cur_pos.x][cur_pos.y] + m2_grid_cost[cur_pos.x][cur_pos.y]) / 2, 1) + cur_g;
                    }
                    h = Heuristic_cost(next_pos, tar_pos, RIGHT);
                }
                // Update
                if (cur_dir != LEFT && g + h < grid_map[next_pos.x][next_pos.y].g + grid_map[next_pos.x][next_pos.y].h) {
                    grid_map[next_pos.x][next_pos.y].g = g;
                    grid_map[next_pos.x][next_pos.y].h = h;
                    grid_map[next_pos.x][next_pos.y].direction = RIGHT;
                    open_list.push_back(grid_map[next_pos.x][next_pos.y]);
                    push_heap(open_list.begin(), open_list.end(), Heap_cmp);
                }
            }
        }
    }

}

double Gmap::Additional_cost(double a, double b, double g, double d) {
    return alpha * a + beta * 0.5 * largest_cell_cost * b + gamma * g + delta * d * via_cost;
}

double Gmap::Heuristic_cost(Coordinate start, Coordinate end, DIRECTION d) {

    double wire_len = fabs(end.y - start.y) * grid_size.y + fabs(end.x - start.x) * grid_size.x;

    /*
    double cell_cost_a = 0;
    double cell_cost_b = 0;

    Coordinate cur_pos = start;
    while (cur_pos != end) {
        if (cur_pos.x < end.x) {
            cell_cost_a += m2_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.x++;
        }
        else if (cur_pos.x > end.x) {
            cell_cost_a += m2_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.x--;
        }
        else if (cur_pos.y < end.y) {
            cell_cost_a += m1_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.y++;
        }
        else if (cur_pos.y > end.y) {
            cell_cost_a += m1_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.y--;
        }
    }

    cur_pos = start;
    while (cur_pos != end) {
        if (cur_pos.y < end.y) {
            cell_cost_b += m1_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.y++;
        }
        else if (cur_pos.y > end.y) {
            cell_cost_b += m1_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.y--;
        }
        else if (cur_pos.x < end.x) {
            cell_cost_b += m2_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.x++;
        }
        else if (cur_pos.x > end.x) {
            cell_cost_b += m2_grid_cost[cur_pos.x][cur_pos.y];
            cur_pos.x--;
        }
    }

    double grid_cost = min(cell_cost_a, cell_cost_b);
    */

    return Additional_cost(wire_len, 0, 0, 2);
}

void Gmap::Update_Position(Coordinate& pos, DIRECTION d) {
    if (d == TOP) {
        gcell[pos.x][pos.y + 1].y -= 1;
        pos.y += 1;
    }
    else if (d == DOWN) {
        gcell[pos.x][pos.y].y -= 1;
        pos.y -= 1;
    }
    else if (d == RIGHT) {
        gcell[pos.x + 1][pos.y].x -= 1;
        pos.x += 1;
    }
    else if (d == LEFT) {
        gcell[pos.x][pos.y].x -= 1;
        pos.x -= 1;
    }
}

void Gmap::Output_result(string file_path) {
    ofstream out_file(file_path);

    for (int i = 0; i < result_vec.size(); i++) {
        out_file << "n" << result_vec[i].first << endl;

        while (!result_vec[i].second.empty()) {
            if (result_vec[i].second.top().x == -1) {
                out_file << "via" << endl;
                //result_vec[i].second.top().Print();
                result_vec[i].second.pop();
                continue;
            }
            else {
                Coordinate c1, c2;
                c1 = result_vec[i].second.top();
                //result_vec[i].second.top().Print();
                result_vec[i].second.pop();
                c2 = result_vec[i].second.top();
                //result_vec[i].second.top().Print();
                result_vec[i].second.pop();

                if (c1.x == c2.x) { // M1
                    out_file << "M1 ";
                    out_file << routing_area.c_bl.x + grid_size.x * c1.x << " ";
                    out_file << routing_area.c_bl.y + grid_size.y * c1.y << " ";
                    out_file << routing_area.c_bl.x + grid_size.x * c2.x << " ";
                    out_file << routing_area.c_bl.y + grid_size.y * c2.y << " ";
                    out_file << endl;
                }
                else { // M2
                    out_file << "M2 ";
                    out_file << routing_area.c_bl.x + grid_size.x * c1.x << " ";
                    out_file << routing_area.c_bl.y + grid_size.y * c1.y << " ";
                    out_file << routing_area.c_bl.x + grid_size.x * c2.x << " ";
                    out_file << routing_area.c_bl.y + grid_size.y * c2.y << " ";
                    out_file << endl;
                }
            }
        }

        out_file << ".end" << endl;
    }

    out_file.close();

}