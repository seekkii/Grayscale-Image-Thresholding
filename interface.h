#ifndef INTERFACE_H
#define INTERFACE_H

#include <QImage>
#include <QLabel>
#include <QString>
#include <vector>
#include <QMouseEvent>
#include <QtWidgets>
#include <QPainter>
#include <QFile>
#include <QSaveFile>
#include <QMainWindow>

#include "Thresholding.h"


class segment: public QWidget{
    Q_OBJECT
 public:
    explicit segment(QWidget *parent);

    void setImg(QImage img){
        this->img = img;
    }
    QImage getImage(){ return img;}
    void setImgPos(QPoint pos){
        this->pos = pos;
    }
    QPoint getImgPos(){
        return pos;
    }
    void showImg(){
        w->resize(img.width(),img.height());
        QPalette palette;
        palette.setBrush(QPalette::Window, img);
        w->setPalette(palette);
        w->move(0,0);
        w->show();
        this->hide();
    }



 public slots:
    void OtsuThresh(){
        Otsu otsu_method(img);
        img = otsu_method.imageSegment();
        showImg();
    };

    void AdaptiveMeanThresh(){
        AdaptiveThresh adaptTh(img);
        img = adaptTh.imageSegment(5,3,"mean");
        showImg();
    };

    void AdaptiveMedianThresh(){
        AdaptiveThresh adaptTh(img);
        img = adaptTh.imageSegment(5,3,"median");
        showImg();
    };




   private:
       QWidget *w;
       QWidget *parent;


       QPushButton *median_btn;
       QPushButton *otsu_btn;
       QPushButton *mean_btn;
       QImage img;
       QPoint pos;
       QPoint starting_point;
       QString fileName;


};



class program : public QWidget
{
    Q_OBJECT

 public:
    segment *seg;
    QImage image;
    QPoint mousePosInitial;// qpoint that contains mouse positions
    QPoint mousePosNext;// qpoint that contains mouse positions
    explicit program(){
        seg = new segment(this);
        resize(300,300);
        showInstruct = false;

        button = new QPushButton("&load image ",this);
        ctrlT = new QShortcut(this);
        ctrlT->setKey(Qt::CTRL | Qt::Key_T); // Set the key code

        connect(button, SIGNAL(clicked()), this, SLOT(load()));
        connect(ctrlT, SIGNAL(activated()), this, SLOT(openSegmentOption()));
    };// constructor

    void paintEvent(QPaintEvent *) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event)override;




   public slots:
       void load();
       void openSegmentOption();



   private:
       int selected_height;
       int selected_width;
       bool showInstruct;
       QPushButton *button;
       QShortcut *ctrlT;
       QString fileName;


};



#endif // INTERFACE_H
