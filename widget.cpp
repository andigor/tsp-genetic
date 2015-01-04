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
    Points points = generate_points<Points>( 10, 300, 300 );

    Paths offspring = first_age_offspring<Paths>( 200, 10 );

    ui->frame->draw( points, shortest_path(points, offspring) );

    for ( auto i = 0; i<2000; ++i) {
        offspring = fitness(points, offspring);

        offspring = crossover(offspring);
    }

    ui->frameResult->draw( points, shortest_path(points, offspring) );
}
