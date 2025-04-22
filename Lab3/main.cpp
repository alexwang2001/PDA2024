#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include "Legalizer.h"
using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Input format: ./Legalizer *.lg *.opt *_post.lg" << endl;
    }
    
    cout << "[main] lg      : " << argv[1] << endl; 
    cout << "[main] opt     : " << argv[2] << endl; 
    cout << "[main] post lg : " << argv[3] << endl; 

    Legalizer legalizer;

    // Bulid LG placement
    ifstream infile_lg;
    infile_lg.open(argv[1], ios::in);
    string tag;

    vector<Block> all_blocks;

    while(!infile_lg.eof()){
        infile_lg >> tag;

        if (tag == "Alpha"){
            double alpha;
            infile_lg >> alpha;
            legalizer.set_alpha(alpha);
            cout << "[main] alpha = " << alpha << endl;
        }
        else if (tag == "Beta"){
            double beta;
            infile_lg >> beta;
            legalizer.set_beta(beta);
            cout << "[main] beta = " << beta << endl;
        }
        else if (tag == "DieSize"){
            int x0, y0, x1, y1;
            infile_lg >> x0 >> y0 >> x1 >> y1;
            legalizer.set_die_coordinate(x0, y0, x1, y1);
            cout << "[main] die_size = " << x0 << " " << y0 << " " <<  x1 << " " << y1 << endl;
        }
        else if (tag == "PlacementRows"){
            //cout << "[main] Add Placement Row" << endl; 
            int start_x;
            int start_y;
            int width;
            int height;
            int num_of_sites;
            infile_lg >> start_x >> start_y >> width >> height >> num_of_sites;
            legalizer.add_placement_row(Placement_Row(start_x, start_y, width, height, num_of_sites));
        }
        else{
            //cout << "[main] Add Block" << endl; 
            string block_name = tag;
            int width, height;
            Coordinate c_tr; // Coordinate of top rights
            Coordinate c_bl; // Coordinate of bottom left
            infile_lg >> c_bl.x >> c_bl.y >> width >> height >> tag;
            c_tr.x = c_bl.x + width;
            c_tr.y = c_bl.y + height;
            all_blocks.push_back(Block(block_name, c_tr, c_bl));
        }
    }

    infile_lg.close();
    cout << "[main] end of lg file read" << endl;

    legalizer.Initailize();
    for (vector<Block>::iterator iter = all_blocks.begin(); iter != all_blocks.end(); iter++){
        legalizer.Block_Creating((*iter).block_name, (*iter).c_bl, (*iter).c_tr);
    }
    legalizer.Block_Creating_End();

    // Banking
    ifstream infile_opt;
    infile_opt.open(argv[2], ios::in);
    ofstream out_file;
    out_file.open(argv[3], ios::out);
    vector<string> banking_ff_names;
    vector<int> banking_ff_indexes;
    string new_block_name;
    int start_x;
    int start_y;
    int width;
    int height;
    
    while(!infile_opt.eof()){
        
        infile_opt >> tag >> tag;
        while (tag != "-->"){
            banking_ff_names.push_back(tag);
            infile_opt >> tag;
        }

        for (vector<string>::iterator iter = banking_ff_names.begin(); iter != banking_ff_names.end(); iter++){
            cout << "[main] Delete Banked Cell " << *iter << endl;
            legalizer.Delete_cell((*iter));
        }

        infile_opt >> new_block_name >> start_x >> start_y >> width >> height; 

        cout << "[main] Insert block " << new_block_name << endl;
        vector<Block> move;
        Coordinate new_co;

        new_co = legalizer.Insert_cell(new_block_name, Coordinate(start_x, start_y) , Coordinate(start_x+width, start_y+height), move);

        out_file << new_co.x << " " << new_co.y << "\n";
        out_file << move.size() << "\n";
        while(!move.empty()){
            out_file << move.front().block_name << " " << move.front().c_bl.x << " " << move.front().c_bl.y << "\n";
            move.erase(move.begin());
        }
        
        
        banking_ff_names.clear();
        banking_ff_indexes.clear();
        
        while(infile_opt.peek() == '\n'){
            infile_opt.ignore(1, '\n');
        }
        
    }

    infile_opt.close();
    out_file.close();

    //vector<Block> ovlp;
    //legalizer.Find_overlap(ovlp, Coordinate(244800, 974400), Coordinate(254490,976500));
    //for(int i=0; i<ovlp.size(); i++){
    //    ovlp[i].Print_info();
    //}
    //exit(0);

    
    // Generate layout file
    //char layout_file_name[] = "layout.txt";
    //legalizer.Gen_layout_txt(layout_file_name);
    //cout << "[main] layout file generated" << endl;


}  