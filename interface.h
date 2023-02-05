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
 public slots:
    void OtsuThresh(){
        Otsu otsu_method(img);
        img = otsu_method.imageSegment();
        this->hide();
    };

    void AdaptiveMeanThresh(){
        AdaptiveThresh adaptTh(img);
        img = adaptTh.imageSegment(5,3,"mean");
        this->hide();
    };

    void AdaptiveMedianThresh(){
        AdaptiveThresh adaptTh(img);
        img = adaptTh.imageSegment(5,3,"median");
        this->hide();
    };

    void AdaptiveMidpointThresh(){
        AdaptiveThresh adaptTh(img);
        img = adaptTh.imageSegment(5,3,"midpoint");
        this->hide();
    };
   private:
       QWidget *parent;
       QPushButton *median_btn;
       QPushButton *otsu_btn;
       QPushButton *mean_btn;
       QPushButton *mid_btn;
       QImage img;
       QPoint pos;
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
    explicit program();

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
