#ifndef PAINT_H
#define PAINT_H
#define paint(type, fpga_type);\
    int i, y, t;\
    int clk_reg_width, clk_reg_height;\
    int frame_height, frame_width;\
    int pos_ptr_x, pos_ptr_y, bram_pos_ptr, dsp_pos_ptr, forbdn_ptr;\
    int brams = 0, dsps = 0;\
    int bound, rec_height;\
    int height_max;\
    int mult_factor;\
    bool is_bram = false, is_dsp = false;\
    QPen clk_reg_pen, rec_pen;\
    QBrush br(Qt::black);\
    if(type == VIRTEX) {\
        fp::clb_height  = 6;\
        fp::bram_height = 30;\
        fp::dsp_height  = 15;\
        virtex_scale    = 2;\
        height_max = 60;\
        mult_factor = 5;\
    }\
    else if(type == VIRTEX_5) {\
        fp::clb_height  = 8;\
        fp::bram_height = 40;\
        fp::dsp_height  = 20;\
        virtex_scale    = 2;\
        height_max = 70;\
        mult_factor = 2;\
    }\
    else if(type == ZYNQ) {\
        fp::clb_height  = 8;\
        fp::bram_height = 40;\
        fp::dsp_height  = 20;\
        virtex_scale    = 1;\
        height_max = 10;\
        mult_factor = 5;\
    }\
    else if(type == PYNQ) {\
        fp::clb_height  = 6;\
        fp::bram_height = 30;\
        fp::dsp_height  = 15;\
        virtex_scale    = 1;\
        mult_factor = 5;\
        height_max = 20;\
    }\
    total_height = fpga_type->num_rows * bram_height * fpga_type->num_clk_reg / 2;\
    Qt::GlobalColor colors [] = {Qt::GlobalColor::red, Qt::GlobalColor::magenta,\
                             Qt::GlobalColor::blue, Qt::GlobalColor::cyan,\
                             Qt::GlobalColor::yellow};\
    qDebug() << "num clt reg is " <<fpga_type->num_clk_reg <<endl;\
    for(i = 0; i < fpga_type->num_clk_reg ; i++) {\
        clk_reg_width = fpga_type->clk_reg[i].clk_reg_pos.w * clb_width;\
        clk_reg_height = fpga_type->clk_reg[i].clb_per_column * clb_height;\
        bram_pos_ptr =  0;\
        dsp_pos_ptr = 0;\
        forbdn_ptr = 0;\
        pos_ptr_x = fpga_type->clk_reg[i].clk_reg_pos.x;\
        pos_ptr_y = fpga_type->clk_reg[i].clk_reg_pos.y;\
        brams = fpga_type->clk_reg[i].bram_num;\
        dsps = fpga_type->clk_reg[i].dsp_num;\
        if(brams > 0)\
            is_bram = true;\
        if(dsps > 0)\
            is_dsp = true;\
        clk_reg_pen.setColor(colors[2]);\
        clk_reg_pen.setWidth(2);\
        fp_rect = scene.addRect((fpga_type->clk_reg[i].clk_reg_pos.x * clb_width) / virtex_scale,\
                (fpga_type->clk_reg[i].clk_reg_pos.y * clb_height) / virtex_scale,\
                clk_reg_width / virtex_scale, clk_reg_height / virtex_scale, clk_reg_pen, brush);\
        for(y = 0; y < fpga_type->clk_reg[i].clk_reg_pos.w; y++) {\
            rec_height = clb_height;\
            bound = fpga_type->clk_reg[i].clb_per_column;\
            rec_pen.setColor(Qt::blue);\
            rec_pen.setWidth(1);\
            if(is_bram) {\
                if(pos_ptr_x + y + 1 == fpga_type->clk_reg[i].bram_pos[bram_pos_ptr]) {\
                    rec_pen.setColor(Qt::red);\
                    bound = fpga_type->clk_reg[i].bram_per_column;\
                    rec_height = bram_height;\
                if(brams - 1 !=  0)\
                    bram_pos_ptr += 1;\
                else\
                    is_bram = false;\
                }\
            }\
            if(is_dsp) {\
                if(pos_ptr_x + y + 1 == fpga_type->clk_reg[i].dsp_pos[dsp_pos_ptr]) {\
                    rec_pen.setColor(Qt::green);\
                    bound = fpga_type->clk_reg[i].dsp_per_column;\
                    rec_height = dsp_height;\
                if(dsps - 1 !=  0)\
                    dsp_pos_ptr += 1;\
                else\
                    is_dsp = false;\
                }\
            }\
        for(t = 0; t < bound; t++) {\
            fp_rect = scene.addRect((pos_ptr_x * clb_width +  y * clb_width) / virtex_scale,\
                                     (pos_ptr_y * clb_height + t * rec_height) / virtex_scale,\
                                    clb_width / virtex_scale, rec_height / virtex_scale, rec_pen, brush);\
        }\
        scene.setBackgroundBrush(fp::brush_background);\
    }\
}\
for(i = 0; i < fpga_type->num_forbidden_slots; i++) {\
    rec_pen.setColor(Qt::black);\
    fp_rect = scene.addRect((fpga_type->forbidden_pos[i].x * clb_width) / virtex_scale,\
                            (((height_max - fpga_type->forbidden_pos[i].y - fpga_type->forbidden_pos[i].h + 10) * clb_height) * mult_factor)/ virtex_scale,\
                            (fpga_type->forbidden_pos[i].w * clb_width) / virtex_scale,\
                            (fpga_type->forbidden_pos[i].h * clb_height * mult_factor)/ virtex_scale, rec_pen, br);\
}
#endif // PAINT_H
