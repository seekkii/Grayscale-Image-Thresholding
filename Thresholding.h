#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include "qlabel.h"
#endif // THRESHOLDING_H
#include <QImage>
#include <QTextStream>
#include <QString>

const int INTENS_MIN = 0;
const int INTENS_MAX = 255;
const double EPS = 1.0e-14;

class Otsu
{
    QImage image;
    public:
        Otsu(QImage img);
        void calcHisto(QImage img,double histo[]);
        void printHisto(double histo[]);
        int otsu(const double histo[]);
        void thresh(QImage &img,int thr);
        QImage NtilesOtsu(QImage img, int N);
        QImage imageSegment();

        QVector<QPair<QPoint,QPoint>> last;

};

class AdaptiveThresh
{
    QImage image;

    public:
        AdaptiveThresh(QImage img);
        int median(std::vector<int> &intens);
        int mean(std::vector<int> &intens);
        QImage medianFlt(int kern_dim);
        QImage meanFlt(int kern_dim);

        QImage imageSegment(int kern_dim, int C, QString stat);


};
