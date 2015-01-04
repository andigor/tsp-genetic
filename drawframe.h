#ifndef DRAWFRAME_H
#define DRAWFRAME_H

#include <QFrame>

#include "tools.h"

#include "points.h"
#include "paths.h"

class DrawFrame : public QFrame
{
    Q_OBJECT
public:
    explicit DrawFrame(QWidget *parent = 0);
    ~DrawFrame();
    void draw( const Points&, const Path&  );
protected:
    virtual void paintEvent(QPaintEvent *);
private:
    Points points_;
    Path  paths_;
};

#endif // DRAWFRAME_H
