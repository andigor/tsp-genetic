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

    Points points = generate_points<Points>( 6, 300, 300 );

    Paths paths = first_age_offspring<Paths>( 20, 6 );
    //paths.add( {0, 1, 2, 3, 4, 5} );

    Paths offspring;

    offspring = crossover(paths);

    ui->frame->draw( points, shortest_path(points, paths) );
}
