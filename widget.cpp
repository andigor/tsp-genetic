#include "widget.h"
#include "ui_widget.h"
#include "drawframe.h"
#include "points.h"
#include "paths.h"
//#include "tools.h"

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

}

void Widget::onRegen()
{
    const size_t points_num = 10;
    const size_t paths_num  = 100;
    const size_t fights_num = points_num * 0.2;

    Points points = generate_points<Points>( points_num, 300, 300 );

    Paths offspring = first_age_offspring<Paths>( points, paths_num, points_num );

    print_paths_with_best( points, offspring );

    ui->frame->draw( points, shortest_path(points, offspring) );

    for ( auto i = 0; i<20; ++i) {
        auto need_size = offspring.size();

        offspring = fitness(points, offspring, fights_num); //some could be dead after


        offspring = crossover(points, offspring, need_size);
    }

    print_paths_with_best( points, offspring );

    ui->frameResult->draw( points, shortest_path(points, offspring) );
}
