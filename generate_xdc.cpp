#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "zynq_model.h"
#include "fine_grained.h"
#include "generate_xdc.h"

using namespace std;

string slot_names[] = {"slot_0", "slot_1", "shift", "name_4,"
                       "slot_5", "slot_6", "shift1", "name_5"};

string pblock_names[] = {"hdmi_out_i/slot_0_0", "hdmi_out_i/slot_1_0,"
                         "hdmi_out_i/slot_2_0", "hdmi_out_i/slot_3_0,"
                         "hdmi_out_i/slot_4_0", "hdmi_out_i/slot_5_0"};
#define generate_xdc(fpga_type, from_solver, to_solver,  num_slots)\
{\
    unsigned long a;\
    int status;\
    unsigned long width, num_rows, num_forbidden_slots;\
    vector<slice> slices_in_slot = vector<slice> (MAX_SLOTS);\
    vector <slice_addres> slice_clb     = vector <slice_addres>(MAX_SLOTS);\
    vector <slice_addres> slice18_bram  = vector <slice_addres>(MAX_SLOTS);\
    vector <slice_addres> slice36_bram  = vector <slice_addres>(MAX_SLOTS);\
    vector <slice_addres> slice_dsp     = vector <slice_addres>(MAX_SLOTS);\
    ofstream write_xdc;\
    sort_output(fpga_type, from_solver, to_solver, slices_in_slot, num_slots)\
    cout << "inside sort output clb " << endl;\
    for (a = 0; a < num_slots; a++){\
        cout << "x1 " << slices_in_slot[a][0].slice_x1 << " x2 " << slices_in_slot[a][0].slice_x2 <<\
                " y1 " << slices_in_slot[a][0].slice_y1 << " y2 " << slices_in_slot[a][0].slice_y2 << endl;\
    }\
    cout << "inside sort output bram 18" << endl;\
    for (a = 0; a < num_slots; a++){\
        cout << "x1 " << slices_in_slot[a][1].slice_x1 << " x2 " << slices_in_slot[a][1].slice_x2 <<\
                " y1 " << slices_in_slot[a][1].slice_y1 << " y2 " << slices_in_slot[a][1].slice_y2 << endl;\
    }\
    cout << "inside sort output bram 36" << endl;\
    for (a = 0; a < num_slots; a++){\
        cout << "x1 " << slices_in_slot[a][2].slice_x1 << " x2 " << slices_in_slot[a][2].slice_x2 <<\
                " y1 " << slices_in_slot[a][2].slice_y1 << " y2 " << slices_in_slot[a][2].slice_y2 << endl;\
    }\
    cout << "inside sort output dsp " << endl;\
    for (a = 0; a < num_slots; a++){\
        cout << "x1 " << slices_in_slot[a][3].slice_x1 << " x2 " << slices_in_slot[a][3].slice_x2 <<\
                " y1 " << slices_in_slot[a][3].slice_y1 << " y2 " << slices_in_slot[a][3].slice_y2 << endl;\
    }\
    write_xdc.open("/usr/local/src/xilinx/xilinx_16/pblocks.xdc");\
    write_xdc<< "# User Generated miscellaneous constraints" << endl <<endl <<endl;\
    for(a = 0; a < num_slots; a++){\
        write_xdc << "set_property HD.RECONFIGURABLE true [get_cells "<<pblock_names[a] <<"]" <<endl;\
        write_xdc <<"create_pblock pblock_"<<slot_names[a] <<endl;\
        write_xdc <<"add_cells_to_pblock [get_pblocks pblock_"<<slot_names[a] <<"] [get_cells -quiet [list "<<pblock_names[a] <<"]]" <<endl;\
        write_xdc << "resize_pblock [get_pblocks pblock_"<<slot_names[a]<< "] -add {SLICE_X" <<slices_in_slot[a][0].slice_x1<<"Y" <<\
                     slices_in_slot[a][0].slice_y1 <<":" <<"SLICE_X"<<slices_in_slot[a][0].slice_x2<<"Y"<<slices_in_slot[a][0].slice_y2 << "}" <<endl;\
        write_xdc << "resize_pblock [get_pblocks pblock_"<<slot_names[a]<< "] -add {RAMB18_X" <<slices_in_slot[a][1].slice_x1<<"Y" <<\
                     slices_in_slot[a][1].slice_y1 <<":" <<"RAMB18_X"<<slices_in_slot[a][1].slice_x2<<"Y"<<slices_in_slot[a][1].slice_y2 << "}" <<endl;\
        write_xdc << "resize_pblock [get_pblocks pblock_"<<slot_names[a]<< "] -add {RAMB36_X" <<slices_in_slot[a][2].slice_x1<<"Y" <<\
                    slices_in_slot[a][2].slice_y1 <<":" <<"RAMB36_X"<<slices_in_slot[a][2].slice_x2<<"Y"<<slices_in_slot[a][2].slice_y2 << "}" <<endl;\
        write_xdc << "resize_pblock [get_pblocks pblock_"<<slot_names[a]<< "] -add {DSP48_X" <<slices_in_slot[a][3].slice_x1<<"Y" <<\
                    slices_in_slot[a][3].slice_y1 <<":" <<"DSP48_X"<<slices_in_slot[a][3].slice_x2<<"Y"<<slices_in_slot[a][3].slice_y2 << "}" <<endl;\
        write_xdc << "set_property RESET_AFTER_RECONFIG true [get_pblocks pblock_"<< slot_names[a] <<"]" <<endl;\
        write_xdc << "set_property SNAPPING_MODE ON [get_pblocks pblock_"<< slot_names[a] <<"]" <<endl;\
        write_xdc <<endl <<endl;\
    }\
    write_xdc.close();\
}

/*
#define sort_output(fpga_type, param, num_slots, )
void sort_output(param_from_solver *param, int res_type, vector <slice_addres> *output_vec, int bram_type, unsigned long num_slots)
{
    int i, k, m, index;\
    int flag = 0;
    int x, y, w, h;
    int col;
    bool is_bram_18 = false;

    // sort the slices
    for(i = 0; i < num_slots; i++){
        x = (*param->x)[i];
        y = (*param->y)[i];
        w = (*param->w)[i];
        h = (*param->h)[i];

        for(m = 0, index = 0; m < 3; m++, index++) {
            k = x;
            flag = 0;

            while(flag == 0){
                if(k < x + w){
                    if((fpga_type->fg[k].type_of_res) == m) {
                        output_vec[i][index].slice_x1 = fpga_type->fg[k].slice_1;
                        flag = 1;
                    }
                    else
                        k += 1;
                }
                else
                    flag = 1;
            }

            flag = 0;
            k = x + w - 1;

            while(flag == 0){
                if (k >= x) {
                    if((fpga_type->fg[k].type_of_res) == m){
                        output_vec[i][index].slice_x2 = fpga_type->fg[k].slice_2;
                        flag = 1;
                    }
                    else
                        k -= 1;
                }
                else
                    flag = 1;
            }

            if(m == CLB)
                col = pynq_instance->clb_per_col;
            else if (m == BRAM && bram_type == 0)
                col = pynq_instance->bram_per_col * 2;
            else if(m == BRAM && bram_type == 1)
                col = pynq_instance->bram_per_col;
            else if(m == DSP)
                col = pynq_instance->dsp_per_col;

            if(m == CLB) {
                if((*param->clb_from_solver)[i] != 0) {
                    output_vec[i][index].slice_y1 = y /10 * col;
                    output_vec[i][index].slice_y2 = (((y + h)/10) * col) - 1;
                }

                else {
                    output_vec[i][index].slice_y1 = 0;
                    output_vec[i][index].slice_y2 = 0;
                }
            }
                if(m == BRAM) {
                    if((*param->bram_from_solver)[i] != 0) {
                        output_vec[i][index].slice_y1 = y /10 * col;
                        output_vec[i][index].slice_y2 = (((y + h)/10) * col) - 1;
                    }

                    else {
                        output_vec[i][index].slice_y1 = 0;
                        output_vec[i][index].slice_y2 = 0;
                    }
               }
                 if(m == DSP) {
                      if((*param->dsp_from_solver)[i] != 0) {
                          output_vec[i][index].slice_y1 = y /10 * col;
                          output_vec[i][index].slice_y2 = (((y + h)/10) * col) - 1;
                      }

                       else {
                            output_vec[i][index].slice_y1 = 0;
                            output_vec[i][index].slice_y2 = 0;
                        }
                }
                 if(m = BRAM && is_bram_18 == false) {
                     m -= 1;
                     is_bram_18 = true;
                 }
           //output_vec[i][index].slice_y1 = y /10 * col;
           //output_vec[i][index].slice_y2 = (((y + h)/10) * col) - 1;

        }
    }

}
*/
