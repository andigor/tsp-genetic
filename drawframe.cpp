#include "drawframe.h"





#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

#include <cassert>

#include <QDebug>

DrawFrame::DrawFrame(QWidget *parent) : QFrame(parent)
{

}

DrawFrame::~DrawFrame()
{

}

void DrawFrame::draw(const Points & points, const Path & paths)
{
    points_ = points;
    paths_  = paths;
    update();
}


void DrawFrame::paintEvent(QPaintEvent * e)
{
    QFrame::paintEvent(e);


    QPainter painter;
    painter.begin(this);
    painter.setPen( Qt::red );    
    for ( const auto & p : points_) {
        painter.drawEllipse( p, 2, 2 );
    }

    QPainterPath ppath;
    bool first_time = true;
    walk_graph( points_, paths_, [&]( const auto & p ){
        if ( first_time ) {
            first_time = false;
            ppath.moveTo( p );
        }
        else {
            ppath.lineTo( p );
        }
    } );


    painter.setPen( Qt::green );
    painter.drawPath( ppath );


    painter.end();

}
