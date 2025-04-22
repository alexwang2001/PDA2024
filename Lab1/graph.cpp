#include "graph.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include "block.h"
using namespace std;


void Graph::Create_start_block() {
    start_block = new Block(0, range.x, range.y, 0, 0);
    blocks.push_back(start_block);
};

Block* Graph::Point_Finding(int tx, int ty) {

    if (tx < 0 || tx < 0 || tx >= range.x || ty >= range.y)
        return NULL;

    Block* current_block = start_block;
    while (!(current_block->c_tr.x > tx && current_block->c_tr.y > ty && current_block->c_bl.x <= tx && current_block->c_bl.y <= ty)) {
        if (current_block->c_tr.y <= ty) {
            current_block = current_block->b_top;
        }
        else if (current_block->c_bl.y > ty) {
            current_block = current_block->b_bottom;
        }
        else if (current_block->c_bl.x > tx) {
            current_block = current_block->b_left;
        }
        else if (current_block->c_tr.x <= tx) {
            current_block = current_block->b_right;
        }

        if (current_block == NULL) {
            return NULL;
        }
    }

    if (current_block != NULL) {
        points_found.push(current_block->c_bl);
        //current_block->Print_coordinate();
    }

    return current_block;
}

Block* Graph::Point_Finding(Block* start_block, int tx, int ty) {

    if (tx < 0 || tx < 0 || tx >= range.x || ty >= range.y)
        return NULL;

    Block* current_block = start_block;
    while (!(current_block->c_tr.x > tx && current_block->c_tr.y > ty && current_block->c_bl.x <= tx && current_block->c_bl.y <= ty)) {
        if (current_block->c_tr.y <= ty) {
            current_block = current_block->b_top;
        }
        else if (current_block->c_bl.y > ty) {
            current_block = current_block->b_bottom;
        }
        else if (current_block->c_bl.x > tx) {
            current_block = current_block->b_left;
        }
        else if (current_block->c_tr.x <= tx) {
            current_block = current_block->b_right;
        }

        if (current_block == NULL) {
            return NULL;
        }
    }

    return current_block;
}


Block* Graph::Horizontal_split(Block* target_block, int y, bool rtn_up/*1:up 0:down*/) {
    //std::cout << "[Horizontal split] start @ y = " << y << std::endl;
    //target_block->Print_info();


    if (y <= target_block->c_bl.y || y >= target_block->c_tr.y)
        return NULL; // No need to split

    // Create two new blocks
    Block* new_block_up;
    Block* new_block_down;

    new_block_up = new Block(0, target_block->c_tr, Coordinate(target_block->c_bl.x, y));
    new_block_down = new Block(0, Coordinate(target_block->c_tr.x, y), target_block->c_bl);
    blocks.push_back(new_block_up);
    blocks.push_back(new_block_down);

    new_block_up->b_top = target_block->b_top;
    new_block_up->b_bottom = new_block_down;
    new_block_up->b_left = Point_Finding(target_block, new_block_up->c_bl.x - 1, new_block_up->c_bl.y);
    new_block_up->b_right = target_block->b_right;

    new_block_down->b_top = new_block_up;
    new_block_down->b_bottom = target_block->b_bottom;
    new_block_down->b_left = target_block->b_left;
    new_block_down->b_right = Point_Finding(target_block, new_block_down->c_tr.x, new_block_down->c_tr.y - 1);

    Block* current_block;
    // Update left side block pointer (from the bottom)
    current_block = target_block->b_left;
    if (current_block != NULL) {
        while (current_block->c_tr.y <= target_block->c_tr.y) {
            if (current_block->c_tr.y <= new_block_down->c_tr.y) {
                current_block->b_right = new_block_down;
            }
            else {
                current_block->b_right = new_block_up;
            }
            current_block = current_block->b_top;
            if (current_block == NULL)
                break;
        }
    }
    // Update right side block pointer (from the top)
    current_block = target_block->b_right;
    if (current_block != NULL) {
        while (current_block->c_bl.y >= target_block->c_bl.y) {
            if (current_block->c_bl.y >= new_block_up->c_bl.y) {
                current_block->b_left = new_block_up;
            }
            else {
                current_block->b_left = new_block_down;
            }
            current_block = current_block->b_bottom;
            if (current_block == NULL)
                break;
        }
    }
    // Update top side block pointer (from right)
    current_block = target_block->b_top;
    if (current_block != NULL) {
        while (current_block->c_bl.x >= target_block->c_bl.x) {
            current_block->b_bottom = new_block_up;
            current_block = current_block->b_left;
            if (current_block == NULL)
                break;
        }
    }
    // Update bottom side block pointer (from left)
    current_block = target_block->b_bottom;
    if (current_block != NULL) {
        while (current_block->c_tr.x <= target_block->c_tr.x) {
            current_block->b_top = new_block_down;
            current_block = current_block->b_right;
            if (current_block == NULL)
                break;
        }
    }
    // remove old block
    if (target_block == start_block) {
        start_block = new_block_down;
    }
    blocks.erase(std::remove(blocks.begin(), blocks.end(), target_block), blocks.end());
    delete target_block;

    //std::cout << "[Horizontal split] end" << std::endl;
    //std::cout << "--------------------------------" << std::endl;

    if (rtn_up) {
        return new_block_up;
    }
    else {
        return new_block_down;
    }

}

Block* Graph::Vertical_split(Block* target_block, int x, bool rtn_left/*1:left 0:right*/) {
    //std::cout << "[Vertical split] start @ x = " << x << std::endl;
    //target_block->Print_info();

    // No need to split
    if (x <= target_block->c_bl.x || x >= target_block->c_tr.x) {
        return NULL;
    }

    Block* new_block_left;
    Block* new_block_right;

    new_block_left = new Block(0, Coordinate(x, target_block->c_tr.y), target_block->c_bl);
    new_block_right = new Block(0, target_block->c_tr, Coordinate(x, target_block->c_bl.y));
    blocks.push_back(new_block_left);
    blocks.push_back(new_block_right);

    new_block_left->b_top = Point_Finding(target_block, new_block_left->c_tr.x - 1, new_block_left->c_tr.y);
    new_block_left->b_bottom = target_block->b_bottom;
    new_block_left->b_left = target_block->b_left;
    new_block_left->b_right = new_block_right;

    new_block_right->b_top = target_block->b_top;
    new_block_right->b_bottom = Point_Finding(target_block, new_block_right->c_bl.x, new_block_right->c_bl.y - 1);
    new_block_right->b_left = new_block_left;
    new_block_right->b_right = target_block->b_right;

    Block* current_block;
    // Update left side block pointer (from the bottom)
    current_block = target_block->b_left;
    if (current_block != NULL) {
        while (current_block->c_tr.y <= target_block->c_tr.y) {
            current_block->b_right = new_block_left;
            current_block = current_block->b_top;
            if (current_block == NULL)
                break;
        }
    }
    // Update right side block pointer (from the top)
    current_block = target_block->b_right;
    if (current_block != NULL) {
        while (current_block->c_bl.y >= target_block->c_bl.y) {
            current_block->b_left = new_block_right;
            current_block = current_block->b_bottom;
            if (current_block == NULL)
                break;
        }
    }
    // Update top side block pointer (from right)
    current_block = target_block->b_top;
    if (current_block != NULL) {
        while (current_block->c_bl.x >= target_block->c_bl.x) {
            if (current_block->c_bl.x >= new_block_right->c_bl.x) {
                current_block->b_bottom = new_block_right;
            }
            else {
                current_block->b_bottom = new_block_left;
            }
            current_block = current_block->b_left;
            if (current_block == NULL)
                break;
        }
    }
    // Update bottom side block pointer (from left)
    current_block = target_block->b_bottom;
    if (current_block != NULL) {
        while (current_block->c_tr.x <= target_block->c_tr.x) {
            if (current_block->c_tr.x <= new_block_left->c_tr.x) {
                current_block->b_top = new_block_left;
            }
            else {
                current_block->b_top = new_block_right;
            }
            current_block = current_block->b_right;
            if (current_block == NULL)
                break;
        }
    }

    // remove old block
    if (target_block == start_block) {
        start_block = new_block_left;
    }

    blocks.erase(std::remove(blocks.begin(), blocks.end(), target_block), blocks.end());
    delete target_block;

    //cout << "[Vertical split] end" << std::endl;
    //cout << "--------------------------------" << std::endl;

    if (rtn_left) {
        return new_block_left;
    }
    else {
        return new_block_right;
    }
}

Block* Graph::Horizontal_merge(Block* target_block_up, Block* target_block_down) {
    // Merge Check
    if (target_block_up->c_tr.x != target_block_down->c_tr.x || target_block_up->c_bl.x != target_block_down->c_bl.x) {
        std::cout << "[Horizontal_merge error] Not aligned" << std::endl;
    }
    if (target_block_up->c_bl.y != target_block_down->c_tr.y) {
        std::cout << "[Horizontal_merge error] Gap existed" << std::endl;
    }

    // Start merging
    target_block_up->c_bl = target_block_down->c_bl;
    target_block_up->b_left = target_block_down->b_left;
    target_block_up->b_bottom = target_block_down->b_bottom;

    Block* current_block;
    // Update left block pointers
    current_block = target_block_up->b_left;
    if (current_block != NULL) {
        while (current_block->b_right == target_block_down) {
            current_block->b_right = target_block_up;
            current_block = current_block->b_top;
            if (current_block == NULL) {
                break;
            }
        }
    }
    // Update right block pointers
    current_block = target_block_down->b_right;
    if (current_block != NULL) {
        while (current_block->b_left == target_block_down) {
            current_block->b_left = target_block_up;
            current_block = current_block->b_bottom;
            if (current_block == NULL) {
                break;
            }
        }
    }
    // Update bottom block pointers
    current_block = target_block_down->b_bottom;
    if (current_block != NULL) {
        while (current_block->b_top == target_block_down) {
            current_block->b_top = target_block_up;
            current_block = current_block->b_right;
            if (current_block == NULL) {
                break;
            }
        }
    }

    if (start_block == target_block_down) {
        start_block = target_block_up;
    }

    blocks.erase(std::remove(blocks.begin(), blocks.end(), target_block_down), blocks.end());
    delete target_block_down;

    return target_block_up;
}

void Graph::Merge_check(Block* block) {
    if (block->b_top != NULL) {
        if (block->b_top->c_bl.x == block->c_bl.x && \
            block->b_top->c_tr.x == block->c_tr.x && \
            block->b_top->c_bl.y == block->c_tr.y && \
            block->b_top->block_index == 0 && \
            block->block_index == 0) {
            block = Horizontal_merge(block->b_top, block);
        }
    }
    if (block->b_bottom != NULL) {
        if (block->b_bottom->c_bl.x == block->c_bl.x && \
            block->b_bottom->c_tr.x == block->c_tr.x && \
            block->b_bottom->c_tr.y == block->c_bl.y && \
            block->b_bottom->block_index == 0 && \
            block->block_index == 0) {
            block = Horizontal_merge(block, block->b_bottom);
        }
    }
}

void Graph::Neighbor_count(Block* target_block, int& num_block, int& num_space) {

    num_block = 0;
    num_space = 0;

    Block* current_block;
    // left
    current_block = target_block->b_left;
    if (current_block != NULL) {
        while (current_block->c_bl.y < target_block->c_tr.y) {
            current_block->block_index == 0 ? num_space++ : num_block++;
            current_block = current_block->b_top;
            if (current_block == NULL) {
                break;
            }
        }
    }
    // right
    current_block = target_block->b_right;
    if (current_block != NULL) {
        while (current_block->c_tr.y > target_block->c_bl.y) {
            current_block->block_index == 0 ? num_space++ : num_block++;
            current_block = current_block->b_bottom;
            if (current_block == NULL) {
                break;
            }
        }
    }
    // top
    current_block = target_block->b_top;
    if (current_block != NULL) {
        while (current_block->c_tr.x > target_block->c_bl.x) {
            current_block->block_index == 0 ? num_space++ : num_block++;
            current_block = current_block->b_left;
            if (current_block == NULL) {
                break;
            }
        }
    }
    // bottom
    current_block = target_block->b_bottom;
    if (current_block != NULL) {
        while (current_block->c_bl.x < target_block->c_tr.x) {
            current_block->block_index == 0 ? num_space++ : num_block++;
            current_block = current_block->b_right;
            if (current_block == NULL) {
                break;
            }
        }
    }
    return;
}

void Graph::Block_Creating(int block_index, Coordinate bl, Coordinate tr) {

    // Deal with top edge
    Block* top_block = Point_Finding(start_block, tr.x - 1, tr.y);
    if (top_block != NULL) {
        if (top_block->c_bl.y != tr.y) { // in block : Create two new empty blocks
            top_block = Horizontal_split(top_block, tr.y, 1);
        }
    }

    // Deal with bottom edge
    Block* bottom_block = Point_Finding(start_block, bl.x, bl.y - 1);
    if (bottom_block != NULL) {
        if (bottom_block->c_tr.y != bl.y) { // in block : Create two new empty blocks
            bottom_block = Horizontal_split(bottom_block, bl.y, 0);
        }
    }

    // Deal with left and right edge
    Block* current_block;
    if (top_block != NULL) {
        current_block = Point_Finding(top_block, bl.x, tr.y - 1);
    }
    else {
        current_block = Point_Finding(start_block, bl.x, tr.y - 1);
    }
    Block* left_block = NULL;
    Block* middle_block = NULL;
    Block* right_block = NULL;
    Block* last_middle_block = NULL;
    left_block = Vertical_split(current_block, bl.x, 1);

    if (left_block == NULL) {
        middle_block = current_block;
    }
    else {
        middle_block = left_block->b_right;
        Merge_check(left_block);
    }

    right_block = Vertical_split(middle_block, tr.x, 0);
    if (right_block != NULL) {
        middle_block = right_block->b_left;
        Merge_check(right_block);
    }

    last_middle_block = middle_block;
    last_middle_block->block_index = block_index;
    current_block = middle_block->b_bottom;

    if (current_block != NULL) {
        while (current_block->c_bl.y >= bl.y) {
            left_block = Vertical_split(current_block, bl.x, 1);
            if (left_block == NULL) {
                middle_block = current_block;
            }
            else {
                middle_block = left_block->b_right;
                Merge_check(left_block);
            }

            right_block = Vertical_split(middle_block, tr.x, 0);
            if (right_block != NULL) {
                middle_block = right_block->b_left;
                Merge_check(right_block);
            }

            middle_block = Horizontal_merge(last_middle_block, middle_block);

            last_middle_block = middle_block;
            last_middle_block->block_index = block_index;
            current_block = middle_block->b_bottom;

            if (current_block == NULL) {
                break;
            }
        }
    }

    non_empty_blocks.push_back(last_middle_block);

    return;

}


void Graph::Gen_layout_txt(char* output_path) {
    ofstream out_file(output_path, ios::out);
    out_file << blocks.size() << endl;
    out_file << range.x << " " << range.y << endl;
    for (size_t i = 0; i < blocks.size(); i++) {
        Block* block = blocks[i];
        if (block->block_index == 0) {
            out_file << "-1 ";
        }
        else {
            out_file << block->block_index << " ";
        }
        out_file << block->c_bl.x << " " << block->c_bl.y << " ";
        out_file << block->c_tr.x - block->c_bl.x << " " << block->c_tr.y - block->c_bl.y << endl;
    }
    out_file.close();
}

void Graph::Gen_output_txt(char* output_path) {
    sort(non_empty_blocks.begin(), non_empty_blocks.end(), [](Block* a, Block* b) {return a->block_index < b->block_index;});
    int num_block, num_space;

    ofstream out_file(output_path, ios::out);
    out_file << blocks.size() << endl;
    for (size_t i = 0; i < non_empty_blocks.size(); i++) {
        Block* block = non_empty_blocks[i];
        Neighbor_count(block, num_block, num_space);
        out_file << block->block_index << " " << num_block << " " << num_space << endl;
    }

    while (!points_found.empty()) {
        out_file << points_found.front().x << " " << points_found.front().y << endl;
        points_found.pop();
    }

    out_file.close();
}