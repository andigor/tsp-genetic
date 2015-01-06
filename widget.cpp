#include "widget.h"
#include "ui_widget.h"
#include "drawframe.h"
#include "points.h"
#include "paths.h"
//#include "tools.h"

const size_t points_num = 20;
const size_t paths_num  = 200;
const size_t fights_num = paths_num * 0.4;
const int mutation_perc = 90;

Points points;

Paths offspring, begin;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onRecalc()
{
    for ( auto i = 0; i<100; ++i ) {
    auto need_size = offspring.size();

    offspring = fitness(points, offspring, fights_num); //some could be dead after

    offspring = crossover(points, offspring, need_size, mutation_perc);

    ui->frameResult->draw( points, shortest_path(points, offspring) );

    print_paths_with_best( points, begin );
    print_paths_with_best( points, offspring );
}

}

void Widget::onRegen()
{
    points = generate_points<Points>( points_num, 300, 300 );

    begin = offspring = first_age_offspring<Paths>( points, paths_num, points_num );

    ui->frame->draw( points, shortest_path(points, offspring) );
}
