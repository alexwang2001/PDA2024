#include "Legalizer.h"
#include <algorithm>
#include <fstream>
#include <queue>
#include <stack>
#include <map>
#include <utility>
#include <random>
using namespace std;

Legalizer::Legalizer() {
    placement_row_bl.y = INT32_MAX;
    placement_row_tr.y = 0;
}

void Legalizer::set_alpha(double alpha) {
    this->alpha = alpha;
}

void Legalizer::set_beta(double beta) {
    this->beta = beta;
}

void Legalizer::set_die_coordinate(int x0, int y0, int x1, int y1) {
    die_x_0 = x0;
    die_y_0 = y0;
    die_x_1 = x1;
    die_y_1 = y1;
}

void Legalizer::add_placement_row(Placement_Row placement_row) {
    placement_row_bl.x = placement_row.start_x;
    placement_row_tr.x = placement_row.start_x + placement_row.width * placement_row.num_of_sites;
    placement_row_bl.y = std::min(placement_row_bl.y, placement_row.start_y);
    placement_row_tr.y = std::max(placement_row_tr.y, placement_row.start_y + placement_row.height);
    placement_row_height = placement_row.height;
    placement_row_site_width = placement_row.width;

}

void Legalizer::Initailize() {
    placement.resize((placement_row_tr.y - placement_row_bl.y) / placement_row_height);
}

void Legalizer::Block_Creating(string block_name, Coordinate bl, Coordinate tr) {
    // placement
    // stating y
    int s_y = (bl.y - placement_row_bl.y) / placement_row_height;
    int e_y = (tr.y - placement_row_bl.y) / placement_row_height - 1;
    for (int row = s_y; row <= e_y; row++) {
        placement[row].push_back(Block(block_name, tr, bl));
    }
    // map
    block_map.insert(pair<string, Block>(block_name, Block(block_name, tr, bl)));
}

void Legalizer::Block_Creating_End() {
    for (vector<vector<Block>>::iterator it = placement.begin(); it != placement.end(); it++) {
        sort((*it).begin(), (*it).end(), [](Block a, Block b) {return a.c_bl.x < b.c_bl.x;});
    }
}





void Legalizer::Delete_cell(string block_name) {
    map<string, Block>::iterator it = block_map.find(block_name);
    Coordinate bl = (*it).second.c_bl;
    Coordinate tr = (*it).second.c_tr;
    int s_y = (bl.y - placement_row_bl.y) / placement_row_height;
    int e_y = (tr.y - placement_row_bl.y) / placement_row_height - 1;

    // delete from vec
    for (int row = s_y; row <= e_y; row++) {
        vector<Block>& place_row = placement[row];
        for (vector<Block>::iterator bptr = place_row.begin(); bptr != place_row.end(); bptr++) {
            if ((*bptr).c_bl.x == bl.x) {
                place_row.erase(bptr);
                break;
            }
        }
    }

    // delete from map
    block_map.erase(block_name);
}

void Legalizer::Gen_layout_txt(char* layout_file_name) {
    ofstream out_file(layout_file_name, ios::out);
    out_file << block_map.size() << endl;
    out_file << die_x_1 << " " << die_y_1 << endl;
    for (map<string, Block>::iterator it = block_map.begin(); it != block_map.end(); it++) {
        Block block = (*it).second;
        if (block.block_name[0] == 'C') {
            out_file << -1 << " ";
        }
        else {
            out_file << 1 << " ";
        }
        out_file << block.c_bl.x << " " << block.c_bl.y << " ";
        out_file << block.c_tr.x - block.c_bl.x << " " << block.c_tr.y - block.c_bl.y << endl;
    }
    out_file.close();
}

vector<Block>::iterator Legalizer::Find_X(int row, int x) {
    vector<Block>::iterator it;
    for (it = placement[row].begin(); it != placement[row].end(); it++) {
        if (it->c_bl.x >= x) return it;
    }
    return it;
}

void Legalizer::Find_overlap(vector<Block>& overlap_blocks, Coordinate bl, Coordinate tr) {

    overlap_blocks.clear();
    int s_y = (bl.y - placement_row_bl.y) / placement_row_height;
    int e_y = (tr.y - placement_row_bl.y) / placement_row_height - 1;
    for (int row = s_y; row <= e_y; row++) {
        for (vector<Block>::iterator bptr = placement[row].begin(); bptr != placement[row].end(); bptr++) {
            if (!(bptr->c_tr.x <= bl.x || bptr->c_bl.x >= tr.x)) {
                if (!Block_in_vec(overlap_blocks, (*bptr))) {
                    overlap_blocks.push_back((*bptr));
                }

            }
            else if (bptr->c_bl.x >= tr.x) {
                break;
            }

        }
    }
}

void Legalizer::Block_Inserting(string block_name, Coordinate bl, Coordinate tr) {
    int s_y = (bl.y - placement_row_bl.y) / placement_row_height;
    int e_y = (tr.y - placement_row_bl.y) / placement_row_height - 1;
    // vec
    for (int row = s_y; row <= e_y; row++) {
        vector<Block>::iterator it = Find_X(row, bl.x); // Find the position to insert 
        placement[row].insert(it, Block(block_name, tr, bl));
    }
    // map
    block_map.insert(pair<string, Block>(block_name, Block(block_name, tr, bl)));
}

bool Legalizer::Block_in_vec(vector<Block>& vec, Block& block) {
    // return if the block is in the vector
    vector<Block>::iterator it;
    for (it = vec.begin(); it != vec.end(); it++) {
        if (it->c_bl.x == block.c_bl.x) {
            if (it->c_bl.y == block.c_bl.y) {
                if (it->block_name == block.block_name) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool Legalizer::Has_block(string block_name) {
    map<string, Block>::iterator it = block_map.find(block_name);
    if (it == block_map.end()) {
        return 0;
    }
    else {
        return 1;
    }
}


Coordinate Legalizer::Insert_cell(string block_name, Coordinate bl, Coordinate tr, vector<Block>& move_record) {
    srand(123456);
    //cout << "Inserting block " << block_name << endl;
    cout << " bl : " << bl.x << " " << bl.y << endl;
    cout << " tr : " << tr.x << " " << tr.y << endl;

    Coordinate new_bl = bl;
    Coordinate new_tr = tr;
    int cell_w = tr.x - bl.x;
    int cell_h = tr.y - bl.y;

    bool retry = 1;
    int iter_idx = 0;
    while (retry) {
        retry = 0;

        cout << "iteration " << iter_idx << endl;
        iter_idx++;

        // Allocate new location to prevent out of border location
        //cout << "Allocating new location\n";
        if (new_bl.x < placement_row_bl.x) {
            new_bl.x = placement_row_bl.x;
            new_tr.x = new_bl.x + cell_w;
        }
        else if (new_tr.x > placement_row_tr.x) {
            new_tr.x = placement_row_tr.x;
            new_bl.x = new_tr.x - cell_w;
        }
        if (new_bl.y < placement_row_bl.y) {
            new_bl.y = placement_row_bl.y;
            new_tr.y = new_bl.y + cell_h;
        }
        else if (new_tr.y > placement_row_tr.y) {
            new_tr.y = placement_row_tr.y;
            new_bl.y = new_tr.y - cell_h;
        }

        // Move to onsite location
        //cout << "Move to onsite location\n";
        int displacement;
        displacement = (new_bl.x - placement_row_bl.x) % placement_row_site_width;
        new_bl.x -= displacement;
        new_tr.x -= displacement;
        displacement = (new_bl.y - placement_row_bl.y) % placement_row_height;
        new_bl.y -= displacement;
        new_tr.y -= displacement;

        //cout << "Location decided" << endl;
        //cout << new_bl.x << " " << new_bl.y << endl;
        //cout << new_tr.x << " " << new_tr.y << endl;

        // Find overlapping cells
        vector<Block> overlap_cells;
        Find_overlap(overlap_cells, new_bl, new_tr);

        // Allocate new location to prevent overlapping with FIX blocks
        bool current_strategy = 0; // 1: ud 0: lr
        double cog_block_x = (new_bl.x + new_tr.x) / 2; // center of gravity
        double cog_block_y = (new_bl.y + new_tr.y) / 2;
        double cog_insert_x, cog_insert_y;

        int has_overlap_fix = 1;
        int has_overlap_fix_idx = 0;
        while (has_overlap_fix) {
            has_overlap_fix_idx++;
            // Allocate new location
            for (vector<Block>::iterator block = overlap_cells.begin(); block != overlap_cells.end(); block++) {

                if (block->block_name[0] == 'C') { // fixed cells
                    if (current_strategy == 1) {
                        cog_block_y = ((block->c_bl.y + block->c_tr.y) / 2 + cog_block_y) / 2;
                        cog_insert_y = (new_bl.y + new_tr.y) / 2;

                        if (cog_insert_y > cog_block_y) {
                            new_tr.y = new_tr.y + (block->c_tr.y - new_bl.y);
                            new_bl.y = block->c_tr.y;
                        }
                        else {
                            new_bl.y = new_bl.y - (new_tr.y - block->c_bl.y);
                            new_tr.y = block->c_bl.y;
                        }
                    }
                    else {
                        cog_block_x = ((block->c_bl.x + block->c_tr.x) / 2 + cog_block_x) / 2;
                        cog_insert_x = (new_bl.x + new_tr.x) / 2;

                        if (cog_insert_x < cog_block_x) {
                            new_bl.x = new_bl.x - (new_tr.x - block->c_bl.x);
                            new_tr.x = block->c_bl.x;
                        }
                        else {
                            new_tr.x = new_tr.x + (block->c_tr.x - new_bl.x);
                            new_bl.x = block->c_tr.x;
                        }
                    }
                    current_strategy = !current_strategy; // change strategy mode
                    //cout << "new location " << new_bl.x << " " << new_bl.y << endl;
                }
            }

            // Add random factor
            int rand_fac_1 = rand() % 2 == 0 ? -1 * (rand() % has_overlap_fix_idx) : rand() % has_overlap_fix_idx;
            int rand_fac_2 = rand() % 2 == 0 ? -1 * (rand() % has_overlap_fix_idx) : rand() % has_overlap_fix_idx;
            new_bl.x += rand_fac_1 * placement_row_site_width;
            new_tr.x = new_bl.x + cell_w;
            new_bl.y += rand_fac_2 * placement_row_height;
            new_tr.y = new_bl.y + cell_h;

            // Allocate new location to prevent out of border location
            //cout << "Allocating new location\n";
            if (new_bl.x < placement_row_bl.x) {
                new_bl.x = placement_row_bl.x;
                new_tr.x = new_bl.x + cell_w;
            }
            else if (new_tr.x > placement_row_tr.x) {
                new_tr.x = placement_row_tr.x;
                new_bl.x = new_tr.x - cell_w;
            }
            if (new_bl.y < placement_row_bl.y) {
                new_bl.y = placement_row_bl.y;
                new_tr.y = new_bl.y + cell_h;
            }
            else if (new_tr.y > placement_row_tr.y) {
                new_tr.y = placement_row_tr.y;
                new_bl.y = new_tr.y - cell_h;
            }

            // Move to onsite location
            //cout << "Move to onsite location\n";
            int displacement;
            displacement = (new_bl.x - placement_row_bl.x) % placement_row_site_width;
            new_bl.x -= displacement;
            new_tr.x -= displacement;
            displacement = (new_bl.y - placement_row_bl.y) % placement_row_height;
            new_bl.y -= displacement;
            new_tr.y -= displacement;


            // Check if there is still any fixed blocks in the region
            Find_overlap(overlap_cells, new_bl, new_tr);
            has_overlap_fix = 0;
            for (vector<Block>::iterator block = overlap_cells.begin(); block != overlap_cells.end(); block++) {
                if (block->block_name[0] == 'C') {
                    has_overlap_fix = 1;
                    break;
                }
            }


        }

        // Print the overlapping cells (and check space avalibility)
        //cout << "*** Overlap Cells ***\n";
        //for (vector<Block>::iterator block = overlap_cells.begin(); block != overlap_cells.end(); block++) {
        //    block->Print_info();
        //}
        //cout << "*********************\n";

        int met_fix = 0;
        queue<Block> original_record;

        // Iterate thought every overlap cell
        for (vector<Block>::iterator block = overlap_cells.begin(); block != overlap_cells.end(); block++) {

            cog_block_x = (block->c_bl.x + block->c_tr.x) / 2;
            cog_insert_x = (new_bl.x + new_tr.x) / 2;

            // Move the block left or right
            if (cog_block_x < cog_insert_x) { // move the block left
                //block->Print_info();
                //cout << "move left\n";

                queue<Block> move_queue;
                vector<Block> overlap;
                move_queue.push(Block(*block));
                original_record.push(Block(*block));

                Delete_cell(block->block_name);

                while (!move_queue.empty()) {
                    Block current_block = move_queue.front();
                    //cout << "prop" << endl;
                    //current_block.Print_info();

                    Find_overlap(overlap, current_block.c_bl, current_block.c_tr);
                    int wall = new_bl.x; // initial wall

                    for (vector<Block>::iterator cell_itr = overlap.begin(); cell_itr != overlap.end(); cell_itr++) {
                        // Update to the most left
                        if (cell_itr->c_bl.x < wall) {
                            wall = cell_itr->c_bl.x;
                        }
                    }
                    // New insertion coordinate
                    Coordinate m_bl, m_tr;
                    m_tr.y = current_block.c_tr.y;
                    m_bl.y = current_block.c_bl.y;
                    m_tr.x = wall;
                    m_bl.x = wall - (current_block.c_tr.x - current_block.c_bl.x);

                    if (m_bl.x < placement_row_bl.x) {
                        //cout << "[Legalizer] out of border solver" << endl;
                        //cout << placement_row_tr.x << endl;
                        //cout << m_bl.x << endl;
                        met_fix++;
                        break;
                    }

                    Find_overlap(overlap, m_bl, m_tr);
                    if (overlap.size() == 0) {
                        Block_Inserting(current_block.block_name, m_bl, m_tr);
                        for (vector<Block>::iterator iter = move_record.begin(); iter != move_record.end(); iter++) {
                            if (iter->block_name == current_block.block_name) {
                                move_record.erase(iter);
                                break;
                            }
                        }
                        move_record.push_back(Block(current_block.block_name, m_tr, m_bl));
                    }
                    else {
                        for (vector<Block>::reverse_iterator cell_itr = overlap.rbegin(); cell_itr != overlap.rend(); cell_itr++) {
                            if (cell_itr->block_name[0] == 'C') {
                                //cout << "[Legalizer] met fixed block" << endl;
                                met_fix++;
                                break;
                            }
                            else {
                                move_queue.push(Block(*cell_itr));
                                original_record.push(Block(*cell_itr));
                                Delete_cell(cell_itr->block_name);
                            }
                        }

                        if (met_fix > 0) break;

                        Block_Inserting(current_block.block_name, m_bl, m_tr);
                        for (vector<Block>::iterator iter = move_record.begin(); iter != move_record.end(); iter++) {
                            if (iter->block_name == current_block.block_name) {
                                move_record.erase(iter);
                                break;
                            }
                        }
                        move_record.push_back(Block(current_block.block_name, m_tr, m_bl));

                    }

                    move_queue.pop();
                }

            }
            else { // move the block right
                //block->Print_info();
                //cout << "move right\n";
                queue<Block> move_queue;
                vector<Block> overlap;
                move_queue.push(Block(*block));
                original_record.push(Block(*block));
                Delete_cell(block->block_name);
                while (!move_queue.empty()) {
                    Block current_block = move_queue.front();
                    Find_overlap(overlap, current_block.c_bl, current_block.c_tr);
                    int wall = new_tr.x; // initial wall
                    for (vector<Block>::iterator cell_itr = overlap.begin(); cell_itr != overlap.end(); cell_itr++) {
                        //cout << "***" << endl;
                        //cell_itr->Print_info();
                        if (cell_itr->c_tr.x > wall) {
                            wall = cell_itr->c_tr.x;
                        }
                    }
                    //current_block.Print_info();
                    //cout << "wall " << wall << endl;
                    // New insertion coordinate
                    Coordinate m_bl, m_tr;
                    m_tr.y = current_block.c_tr.y;
                    m_bl.y = current_block.c_bl.y;
                    m_tr.x = wall + (current_block.c_tr.x - current_block.c_bl.x);
                    m_bl.x = wall;

                    if (m_tr.x > placement_row_tr.x) {
                        //cout << "[Legalizer] out of border solver" << endl;
                        //cout << placement_row_tr.x << endl;
                        //cout << m_tr.x << endl;
                        met_fix++;
                        break;
                    }

                    Find_overlap(overlap, m_bl, m_tr);
                    if (overlap_cells.size() == 0) {
                        Block_Inserting(current_block.block_name, m_bl, m_tr);
                        for (vector<Block>::iterator iter = move_record.begin(); iter != move_record.end(); iter++) {
                            if (iter->block_name == current_block.block_name) {
                                move_record.erase(iter);
                                break;
                            }
                        }
                        move_record.push_back(Block(current_block.block_name, m_tr, m_bl));
                    }
                    else {
                        for (vector<Block>::iterator cell_itr = overlap.begin(); cell_itr != overlap.end(); cell_itr++) {
                            if (cell_itr->block_name[0] == 'C') {
                                //cout << "[Legalizer] met fixed block" << endl;
                                met_fix++;
                                break;
                            }
                            else {
                                move_queue.push(Block(*cell_itr));
                                original_record.push(Block(*cell_itr));
                                Delete_cell(cell_itr->block_name);
                            }
                        }
                        if (met_fix > 0) break;
                        Block_Inserting(current_block.block_name, m_bl, m_tr);
                        for (vector<Block>::iterator iter = move_record.begin(); iter != move_record.end(); iter++) {
                            if (iter->block_name == current_block.block_name) {
                                move_record.erase(iter);
                                break;
                            }
                        }
                        move_record.push_back(Block(current_block.block_name, m_tr, m_bl));
                    }
                    move_queue.pop();
                }
            }

            if (met_fix > 0) {
                // Recover all the steps
                queue<Block> tmp_q = original_record;
                while (!tmp_q.empty()) {
                    Delete_cell(tmp_q.front().block_name);
                    tmp_q.pop();
                }
                while (!original_record.empty()) {
                    if (!Has_block(original_record.front().block_name)) {
                        Block_Inserting(original_record.front().block_name, original_record.front().c_bl, original_record.front().c_tr);
                    }
                    original_record.pop();
                }


                retry = 1;
                new_bl.x = (rand() % 2 == 0) ? new_bl.x + (rand() % iter_idx) * placement_row_site_width : new_bl.x - (rand() % iter_idx) * placement_row_site_width;
                new_bl.y = (rand() % 2 == 0) ? new_bl.y + (rand() % iter_idx) * placement_row_height : new_bl.y - (rand() % iter_idx) * placement_row_height;
                new_tr.x = new_bl.x + cell_w;
                new_tr.y = new_bl.y + cell_h;
                break;
            }
        }

        if (retry) {
            cout << "Retry" << endl;
            move_record.clear();
            continue;
        }

        Block_Inserting(block_name, new_bl, new_tr);
    }
    return Coordinate(new_bl);
}