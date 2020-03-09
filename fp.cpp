#include "fp.h"
#include "ui_fp.h"

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <iostream>
#include <string>
#include "paint.h"
#include "generate_xdc.h"

using namespace std;

fp::fp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fp)
{
    unsigned long i;
    ui->setupUi(this);
    zynq = new zynq_7010();
    virt = new virtex();
    virt_5 = new virtex_5();
    pynq_inst = new pynq();

    init_gui();

    //scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(&scene);

    //fp::brush_background.setColor(Qt::black);
    //paint_zynq();

    //paint_virtex();                                                                                                                                                                                                                                                                  paint_virtex();
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(10);

    connect(ui->set_button, SIGNAL(released()), this, SLOT(set_pressed()));
    connect(ui->FPGA, SIGNAL(released()), this, SLOT(fpga_pressed()));
    connect(ui->enter_button, SIGNAL(released()),this, SLOT(enter_pressed()));
    connect(ui->start_fp, SIGNAL(released()), this, SLOT(start_pressed()));
    connect(ui->browse_button, SIGNAL(released()), this, SLOT(set_browse()));
    connect(ui->set_util, SIGNAL(released()), this, SLOT(set_util()));
    connect(ui->xdc, SIGNAL(released()), this, SLOT(generate_xdc()));

    for(i = 0; i < virt->num_forbidden_slots; i++)
        forbidden_region_virtex[i] = virt->forbidden_pos[i];

    for(i = 0; i < virt_5->num_forbidden_slots; i++)
        forbidden_region_virtex_5[i] = virt_5->forbidden_pos[i];

    for(i = 0; i < zynq->num_forbidden_slots; i++) {
        forbidden_region_zynq[i] = zynq->forbidden_pos[i];
        //cout<< " fbdn" << forbidden_region_zynq[i].x << endl;
    }

    for(i = 0; i < pynq_inst->num_forbidden_slots; i++) {
        forbidden_region_pynq[i] = pynq_inst->forbidden_pos[i];
    }
}

fp::~fp()
{
    delete ui;
}

void fp::init_gui()
{
    ui->comboBox->addItem("Zynq");
    ui->comboBox->addItem("Virtex");
    ui->comboBox->addItem("virtex_5");
    ui->comboBox->addItem("pynq");
}

//this function first plots the FPGA visualizer
void fp::fpga_pressed()
{
    if(ui->comboBox->currentText() == "Zynq") {
        type = ZYNQ;
        //fp::clb_height = 8;
        //fp::bram_height = 40;
        //fp::dsp_height = 20;
        scene.clear();
        paint(ZYNQ, zynq);
        qDebug() << "selected zynq" << endl;
    }
    else if(ui->comboBox->currentText() == "Virtex") {
        type = VIRTEX;
        //fp::clb_height = 6;
        //fp::bram_height = 30;
        //fp::dsp_height = 15;
        scene.clear();
        qDebug() << "selected virtex" << endl;
        paint(VIRTEX, virt);
    }

    else if(ui->comboBox->currentText() == "virtex_5") {
        type = VIRTEX_5;
        //fp::clb_height = 8;
        //fp::bram_height = 40;
        //fp::dsp_height = 20;
        scene.clear();
        qDebug() << "selected virtex 5" << endl;
        paint(VIRTEX_5, virt_5);
    }

    else if(ui->comboBox->currentText() == "pynq") {
        type = PYNQ;
        //fp::clb_height = 6;
        //fp::bram_height = 30;
        //fp::dsp_height = 15;
        scene.clear();
        paint(PYNQ, pynq_inst);
        qDebug() << "selected zynq" << endl;
    }
}

void fp::set_pressed()
{
     unsigned long int p;

     str = ui->num_slotsLineEdit_2->text();
     num_slots = (unsigned long) str.toInt();
     ui->comboBox_2->clear();

     for(p = 0; p < num_slots; p++) {
        ui->comboBox_2->addItem(QString::number(p));
     }

     if(num_slots > 0) {
         clb_vector.clear();
         bram_vector.clear();
         dsp_vector.clear();

         eng_x.clear();
         eng_y.clear();
         eng_w.clear();
         eng_h.clear();

         x_vector.clear();
         y_vector.clear();
         w_vector.clear();
         h_vector.clear();
     }

     scene.clear();
     if(type == ZYNQ) {
        paint(ZYNQ, zynq);
     }
     else if(type == VIRTEX) {
        paint(VIRTEX, virt);
    }
     else if(type == VIRTEX_5) {
         paint(VIRTEX_5, virt_5);
     }
     else if(type == PYNQ) {
         paint(PYNQ, pynq_inst);
     }
}

void fp::enter_pressed()
{
    int clb = 0, bram = 0 , dsp = 0;
    QString st, temp;
    unsigned int num;

    st = ui->comboBox_2->currentText();
    num = (unsigned int) st.toInt();

    temp = ui->cLBLineEdit->text();
    clb = temp.toInt();
    clb_vector[num] = clb;

    temp = ui->bRAMLineEdit->text();
    bram = temp.toInt();
    bram_vector[num] = bram;

    temp = ui->dSPLineEdit->text();
    dsp =  temp.toInt();
    dsp_vector[num] = dsp;

}

void fp::start_pressed()
{
    unsigned long m, scale;

    //send description of FPGA parameters and application parameters to optimizer
    param.bram = &bram_vector;
    param.clb  = &clb_vector;
    param.dsp  = &dsp_vector;
    param.num_slots = num_slots;
    param.num_connected_slots = connections;
    param.conn_vector = &connection_matrix;

    scene.clear();
    if(type == ZYNQ) {
      paint(ZYNQ, zynq);
    }
    else if(type == VIRTEX) {
       paint(VIRTEX, virt);
    }

    else if(type == VIRTEX_5){
       paint(VIRTEX_5, virt_5);
    }
    else if(type == PYNQ) {
        paint(PYNQ, pynq_inst);
    }
    if(type == VIRTEX) {
        param.forbidden_slots = virt->num_forbidden_slots;
        param.num_rows = virt->num_rows;
        param.width = virt->width;
        param.fbdn_slot = &forbidden_region_virtex;
        param.num_clk_regs = virt->num_clk_reg / 2;
        param.clb_per_tile = VIRTEX_CLB_PER_TILE;
        param.bram_per_tile = VIRTEX_BRAM_PER_TILE;
        param.dsp_per_tile = VIRTEX_DSP_PER_TILE;

        virtex_start_optimizer(&param, &from_solver);
        scale = 2;
    }

    if(type ==ZYNQ) {
        param.forbidden_slots = zynq->num_forbidden_slots;
        param.num_rows = zynq->num_rows;
        param.width = zynq->width;
        param.fbdn_slot = &forbidden_region_zynq;
        param.num_clk_regs  = zynq->num_clk_reg /2;
        param.clb_per_tile  = ZYNQ_CLB_PER_TILE;
        param.bram_per_tile = ZYNQ_BRAM_PER_TILE;
        param.dsp_per_tile  = ZYNQ_DSP_PER_TILE;

        scale = 1;
        zynq_start_optimizer(&param, &from_solver);
    }

    if(type ==VIRTEX_5) {
        param.forbidden_slots = virt_5->num_forbidden_slots;
        param.num_rows = virt_5->num_rows;
        param.width = virt_5->width;
        param.fbdn_slot = &forbidden_region_virtex_5;
        param.num_clk_regs = virt_5->num_clk_reg /2;
        param.clb_per_tile = VIRTEX_5_CLB_PER_TILE;
        param.bram_per_tile = VIRTEX_5_BRAM_PER_TILE;
        param.dsp_per_tile = VIRTEX_5_DSP_PER_TILE;

         virtex_start_optimizer_v5(&param, &from_solver);
        scale = 2;
    }

    if(type ==PYNQ) {
        param.forbidden_slots = pynq_inst->num_forbidden_slots;
        param.num_rows = pynq_inst->num_rows;
        param.width = pynq_inst->width;
        param.fbdn_slot = &forbidden_region_pynq;
        param.num_clk_regs = pynq_inst->num_clk_reg /2;
        param.clb_per_tile = PYNQ_CLB_PER_TILE;
        param.bram_per_tile = PYNQ_BRAM_PER_TILE;
        param.dsp_per_tile = PYNQ_DSP_PER_TILE;

        scale = 1;
        pynq_start_optimizer(&param, &from_solver);
    }

    //calibrate the data returned from optimizer for visualization
    plot_rects(&from_solver);

    //Plot slots
    QPen slot_pen(Qt::red);
    slot_pen.setWidth(4);

    for(m = 0; m < num_slots; m++) {
        scene.addRect(x_vector[m] / scale, y_vector[m] / scale, w_vector[m] / scale, h_vector[m] / scale, slot_pen, brush);
        qDebug() << x_vector[m] << " " << y_vector[m] << " " << w_vector[m] << " " << h_vector[m] << endl;
    }
}

void fp::plot_rects(param_from_solver *fs)
{
    unsigned long int i;

    /*The information about the total height must be integral part of the FPGA description*/
    for(i = 0; i < num_slots; i++) {
        x_vector[i] = (*fs->x)[i] * clb_width;
        y_vector[i] = total_height - (((*fs->y)[i] + (*fs->h)[i]) * bram_height);
        h_vector[i] = (*fs->h)[i] * bram_height;
        w_vector[i] = (*fs->w)[i] * clb_width;

  //      qDebug() << "total height " <<total_height <<endl;
    }
}

void fp::set_browse()
{
    unsigned long row, col;
    int i , k;
    unsigned int ptr;
    string str;

    file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("CSV (*.csv)" ));
    CSVData csv_data(file_path.toStdString());

    row = csv_data.rows();
    col = csv_data.columns();

    qDebug() << " row " <<row << "col " << col << endl;
    cout <<"in browse" << endl;

    if(row >= num_slots) {
        for(i = 0, ptr = 0, k = 0; i < num_slots; i++, ptr++) {
            str = csv_data.get_value(i, k++);
            clb_vector[ptr] = std::stoi(str);

            str = csv_data.get_value(i, k++);
            bram_vector[ptr] = std::stoi(str);

            str = csv_data.get_value(i, k++);
            dsp_vector[ptr] = std::stoi(str);
            k = 0;
            //cout << "clb " << clb_vector[ptr] << " bram " << bram_vector[ptr] << "dsp " << dsp_vector[ptr] << endl;
        }
    }

    if(row > num_slots) {
        for(i = num_slots + 1, ptr = 0; i < row; i++, ptr++){
            for(k = 0; k < 3; k++) {
                str = csv_data.get_value(i, k);
                connection_matrix[ptr][k] = std::stoi(str);
            }
        }
       connections = row - num_slots - 1;
    }
    else
        connections = 0;

    if(connections > 0) {
        for(i = num_slots + 1, ptr = 0, k = 0; i < row; i++, ptr++){
            for(k = 0; k < 3; k++) {
                cout << connection_matrix[ptr][k] << " " ;
            }
            cout << endl;
        }
    }
    cout << "connectons " << connections <<endl;
}

void fp::set_util()
{
    unsigned long i;
    unsigned long clb_tot, bram_tot, dsp_tot;
    unsigned long clb_min, bram_min, dsp_min;
    unsigned long clb_max, bram_max, dsp_max;
    unsigned long clb_mod, bram_mod, dsp_mod;

    QString str;
   // int temp_util = 0;
   // float util_temp = 0.0;

    vector <unsigned long> clb_vec (num_slots, 0);
    vector <unsigned long> bram_vec (num_slots, 0);
    vector <unsigned long> dsp_vec (num_slots, 0);

    //slot s1, s2, s3;
    //slot sl_array[num_slots];
     srand((unsigned)time(0));

     str = ui->utilLineEdit->text();
     utilization = str.toFloat();

     //qDebug() << endl << "util is" << utilization << endl;

     if(type == ZYNQ) {
         clb_tot  = ZYNQ_CLB_TOT;
         bram_tot = ZYNQ_BRAM_TOT;
         dsp_tot  = ZYNQ_DSP_TOT;
         clb_min  = ZYNQ_CLB_MIN;
         clb_max  = 400;
         bram_min = ZYNQ_BRAM_MIN;
         bram_max = 12;
         dsp_min  = ZYNQ_DSP_MIN;
         dsp_max  = 20;
         clb_mod = clb_tot * utilization;
         bram_mod = bram_tot * 0.1;
         dsp_mod = dsp_tot * 0.1;
     }

     else if(type == VIRTEX) {
         clb_tot = VIRTEX_CLB_TOT;
         bram_tot = VIRTEX_BRAM_TOT;
         dsp_tot = VIRTEX_DSP_TOT;
         clb_min = VIRTEX_CLB_MIN;
         clb_max = 10000;
         bram_min = VIRTEX_BRAM_MIN;
         bram_max = 50;
         dsp_min = VIRTEX_DSP_MIN;
         dsp_max = 100;
         clb_mod = clb_tot * utilization;
         bram_mod = bram_tot * 0.1;
         dsp_mod = dsp_tot * 0.1;
     }
/*
     int mod_clb = (clb_tot * 2 * utilization) / num_slots;
     int mod_bram = (bram_tot * 1 * utilization) / num_slots;
     int mod_dsp = (dsp_tot * 1 * utilization) / num_slots;

     do{
         for(i = 0; i < num_slots; i++) {
             sl_array[i].clb = rand()%mod_clb;
         }
     }while(is_compatible(sl_array, num_slots, clb_tot, clb_min, CLB));
/*
     do{
         for(i = 0; i < num_slots; i++)
             sl_array[i].bram = rand()%mod_bram;//bram_tot;
     }while(is_compatible(sl_array, num_slots, bram_tot, bram_min, BRAM));

     do{
         for(i = 0; i < num_slots; i++)
             sl_array[i].dsp = rand()%mod_dsp; //dsp_tot;
     }while(is_compatible(sl_array, num_slots, dsp_tot, dsp_min, DSP));

     for(i = 0; i < num_slots; i++) {
         qDebug() << "in slot " << i << " " << sl_array[i].clb <<" " << sl_array[i].bram <<" "<< sl_array[i].dsp << endl;
         temp_util += sl_array[i].clb;
     }

    util_temp = (float) temp_util / clb_tot;
    qDebug() << "utilization " << util_temp << endl;
    */

    qDebug() << "clbs " <<clb_mod << " bram " << bram_mod << " dsp " << dsp_mod << endl;
    clb_vec = fp::get_units_per_task(num_slots, clb_mod, clb_min, clb_max);
    //bram_vec = fp::get_units_per_task(num_slots, bram_mod , bram_min, bram_max);
    //dsp_vec = fp::get_units_per_task(num_slots, dsp_mod, dsp_min, dsp_max);

 //   for(i = 0; i < num_slots; i++) {
          //cout<< "clb" << i << " " <<clb_vec[i] << " bram" << i << " " << bram_vec[i] << " dsp" << i << " " <<dsp_vec[i] <<endl;
//          cout<<clb_vec[i] << "," << bram_vec[i] << ", " <<dsp_vec[i] <<endl;
 //   }
    if(num_slots != 0) {
        for(i = 0; i < num_slots; i++) {
            clb_vector[i] = clb_vec[i]; //sl_array[i].clb;
            bram_vector[i] = bram_vec[i]; //sl_array[i].bram;
            dsp_vector[i] =  dsp_vec[i]; //sl_array[i].dsp;
        }
    }
}

vector<unsigned long> fp::get_units_per_task(unsigned long n, unsigned long n_units, unsigned long n_min, unsigned long n_max)
{
    vector<unsigned long> ret;
    double rand_dbl;

    uint n_units_sum = n_units, n_units_next=0;

    for(uint i=0; i < n-1; i++)
    {
        srand(time(0));
        rand_dbl = pow(MY_RAND(),(1.0 / (double)(n - i - 1)));
        n_units_next = floor((double)n_units_sum * rand_dbl);
        //cout << n_units_next << " " << rand_dbl << endl;
        // --------- LIMIT Task Utilization --------------
        if(n_units_next > (n_units_sum - n_min))
            n_units_next = n_units_sum - n_min;

        if(n_units_next < ((n - i - 1) * n_min))
            n_units_next = (n - i - 1) * n_min;

        if((n_units_sum - n_units_next) > n_max)
            n_units_next = n_units_sum - n_max;
        // ------------------------------------------------

        ret.push_back(n_units_sum - n_units_next);
        n_units_sum = n_units_next;
    }

    ret.push_back(n_units_sum);

    return ret;
}

void fp::generate_xdc()
{
    param_from_solver *from_sol_ptr = &from_solver;

    if(type == ZYNQ) {
        zynq_fine_grained *fg_zynq_instance = new zynq_fine_grained();
        generate_xdc_file(fg_zynq_instance, from_sol_ptr, param, num_slots);
    }

    else if(type ==PYNQ) {
        pynq_fine_grained *fg_pynq_instance = new pynq_fine_grained();
        generate_xdc_file(fg_pynq_instance, from_sol_ptr, param, num_slots);
    }



}
