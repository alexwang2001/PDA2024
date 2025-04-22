#include <iostream>
#include <fstream>
#include <queue>
#include "block.h"
#include "graph.h"
using namespace std;

int main(int argc, char* argv[]) {

    /*
    if (argc != 3) {
        cout << "Format : ./Lab1 [input] [output]" << endl;
        return 1;
    }
    */

    Graph graph;

    ifstream input_file(argv[1], ios::in);

    input_file >> graph.range.x;
    input_file >> graph.range.y;
    input_file.ignore(1, '\n');

    //cout << "width: " << graph.range.x << endl;
    //cout << "height: " << graph.range.y << endl;
    //cout << "=====================================\n";

    graph.Create_start_block();

    while (input_file.peek() != EOF) {

        // Point Finding
        if (input_file.peek() == 'P') {
            int tx, ty;
            char op;
            Block* block_found;
            input_file >> op >> tx >> ty;
            //cout << "Point finding task (" << tx << "," << ty << ")" << endl;
            block_found = graph.Point_Finding(tx, ty);
            //cout << "=====================================\n";
        }
        // Block Creating
        else {
            int create_index;
            int bl_x, bl_y, width, height;
            input_file >> create_index >> bl_x >> bl_y >> width >> height;
            //cout << "Block Creating task " << bl_x << " " << bl_y << " " << width << " " << height << endl;
            graph.Block_Creating(create_index, Coordinate(bl_x, bl_y), Coordinate(bl_x + width, bl_y + height));
            //char file_path[100] = "layout.txt";
            //graph.Gen_layout_txt(file_path);
            //cout << "=====================================\n";
        }

        while (input_file.peek() == '\n') {
            input_file.ignore(1, '\n');
        }
    }

    input_file.close();

    graph.Gen_output_txt(argv[2]);

    return 0;
}