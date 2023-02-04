#include "Thresholding.h"
#include "qdebug.h"
#include "qlabel.h"

//~~~~~~~~~~Otsu Method~~~~~~~~~~~~~~
Otsu::Otsu(QImage img){
    image = img;
    QPair<QPoint,QPoint> p ;
    p.first = QPoint(0,img.width());
    p.second = QPoint(0,img.height());
    last.push_back(p);
}

void Otsu::calcHisto(QImage img, double histo[])
{
    for (int i = 0; i <= INTENS_MAX; i++)
    {
        histo[i] = 0;
    }
    for (int indx_row = 0; indx_row < img.height(); indx_row++)
    {
        quint8* ptr_row = (quint8*)(img.bits()
                + indx_row * img.bytesPerLine());
        for (int indx_col = 0; indx_col < img.width(); indx_col++)
        {
            histo[ptr_row[indx_col]]++;
        }
    }
    int numb_pix = img.height() * img.width();
    for (int i = 0; i <= INTENS_MAX; i++)
    {
        histo[i] /= numb_pix;
    }
}//calcHisto

void Otsu::printHisto(double histo[])
{
    for (int i = 0; i <= INTENS_MAX; i++)
    {
        QTextStream(stdout) << i << ": " << histo[i] << Qt::endl;
    }
}//printHisto

void Otsu::thresh(QImage &img,int thr)
{
    for (int indx_row = 0; indx_row < img.height(); indx_row++)
    {
        quint8* ptr_row = (quint8*)(img.bits()
                + indx_row * img.bytesPerLine());
        for (int indx_col = 0; indx_col < img.width(); indx_col++)
        {
            ptr_row[indx_col] =
                (ptr_row[indx_col] < thr) ? INTENS_MIN : INTENS_MAX;
        }
    }
}//thresh

int Otsu::otsu(const double histo[])
{
    // compute cumulative sums
    double p_1[INTENS_MAX + 1] = {0};
    p_1[0] = histo[0];
    for (int i = 1; i <= INTENS_MAX; i++)
    {
        p_1[i] = p_1[i - 1] + histo[i];
    }

    // cumulative mean
    double m[INTENS_MAX + 1] = {0};
    for (int i = 1; i <= INTENS_MAX; i++)
    {
        m[i] = m[i - 1] + i * histo[i];
    }

    // global mean
    double m_g = m[INTENS_MAX];

    // between-class
    double b_c[INTENS_MAX + 1] = {0};
    for (int i = 1; i <= INTENS_MAX; i++)
    {
        double div = (p_1[i] * (1 - p_1[i]));
        b_c[i] =
                (div < EPS) ? 0 :
            ((m_g * p_1[i] - m[i]) * (m_g * p_1[i] - m[i])) / div;

        //QTextStream(stdout) << "Max: " << b_c[i] << Qt::endl;
    }

    // find max
    double max = 0;
    int max_i = 0;
    for (int i = 0; i <= INTENS_MAX; i++)
    {
        if (b_c[i] > max)
        {
            max = b_c[i];
            max_i = i;
        }
    }

    //QTextStream(stdout) << "Th: " << max_i << Qt::endl;

    return max_i;
}//otsu

QImage Otsu::NtilesOtsu(QImage img, int N){
    QVector<QImage> subImg;
    subImg.resize(N);
    double histo[N][INTENS_MAX + 1];
    QVector<int> thr;
    thr.resize(N);


    for (int tile = 0; tile < N;tile++){
        for (int i = 0; i <= INTENS_MAX; i++)
        {
            histo[tile][i] = 0;
        }
    }


    for (int sub = 0; sub < N; sub++){
        if (img.width()>img.height()){
            subImg[sub] = QImage(img.width()/2,img.height(),QImage::Format_Grayscale8);
        }
        else{
            subImg[sub] = QImage(img.width(),img.height()/2,QImage::Format_Grayscale8);
        }
    }

    for (int tile = 1; tile <= N; tile++ ){
        for (int indx_row = img.height()/N*tile - img.height()/N ; indx_row < img.height()/N*tile; indx_row++)
        {
            quint8* ptr_row_out = (quint8*)(subImg[tile-1].bits()
                    + (indx_row - img.height()/N*(tile - 1)) * subImg[tile-1].bytesPerLine());
            quint8* ptr_row_in = (quint8*)(img.bits()
                    + indx_row * img.bytesPerLine());
            for (int indx_col = 0; indx_col < img.width(); indx_col++)
            {
                ptr_row_out[indx_col] = ptr_row_in[indx_col];
                histo[tile-1][ptr_row_out[indx_col]]++;
            }
        }
    }

    int numb_pix = img.height() * img.width()/N;
    for (int tile = 0; tile < N;tile++){
        for (int i = 0; i <= INTENS_MAX; i++)
        {
            histo[tile][i] /= numb_pix;
        }
        thr[tile] = otsu(histo[tile]);
    }
    qDebug()<<thr[0]<<thr[1];
    if (thr[0]==thr[1]){
        return image;
    }
    if (thr[0]<thr[1]){
        if (img.width() > img.height()){
            QPair<QPoint,QPoint> p = last.back();
            p.first = last.back().first/2;
        }
        else{
                QPair<QPoint,QPoint> p = last.back();
                p.second = last.back().second/2;
            }

            thresh(subImg[0],thr[0]);
            return NtilesOtsu(subImg[1],2);

    }
    else{
        if (img.width() > img.height()){
            QPair<QPoint,QPoint> p = last.back();

        }
        else{
                QPair<QPoint,QPoint> p = last.back();
                p.second = last.back().second/2;
            }
        thresh(subImg[1],thr[1]);

        return NtilesOtsu(subImg[0],2);
    }


}

QImage Otsu::imageSegment(){
    double histo[INTENS_MAX + 1];
    calcHisto(image,histo);
    int th = otsu(histo);
    thresh(image,th);
    return image;
}



//~~~~~~~~~~~~~~~Adaptive Mean~~~~~~~~~~~~~~~~~~~~~~~
AdaptiveThresh::AdaptiveThresh(QImage img){
    image = img;
}

int AdaptiveThresh::median(std::vector<int> &intens)
{
    sort(intens.begin(), intens.end());
    intens.erase(unique(intens.begin(), intens.end()), intens.end());

    return intens[intens.size() / 2];
}//median

int AdaptiveThresh::mean(std::vector<int> &intens)
{
    int sum = std::accumulate(intens.begin(), intens.end(), 0);
    return sum/intens.size();
}//median

QImage AdaptiveThresh::medianFlt(int kern_dim)
{
    QImage out_image(image.width(), image.height(), QImage::Format_Grayscale8);
    const int DK = kern_dim / 2;
    for (int indx_row = 0; indx_row < image.height(); indx_row++)
    {
        quint8* ptr_out_row = (quint8*)(out_image.bits()
                + indx_row * out_image.bytesPerLine());
        for (int indx_col = 0; indx_col < image.width(); indx_col++)
        {
            std::vector<int> intens;
            for (int indx_ker_row = 0; indx_ker_row < kern_dim; indx_ker_row++)
            {
                int x = indx_row - DK + indx_ker_row;
                if (x >= 0 && x < image.height())
                {
                    quint8* ptr_in_row = (quint8*)(image.bits()
                        + x * image.bytesPerLine());
                    for (int indx_ker_col = 0; indx_ker_col < kern_dim; indx_ker_col++)
                    {
                        int y = indx_col - DK + indx_ker_col;
                        if (y >= 0 && y < image.width())
                        {
                            intens.push_back(ptr_in_row[y]);
                        }
                    }
                }
            }
            ptr_out_row[indx_col] = median(intens);
        }
    }

    return out_image;
}//medianFlt

QImage AdaptiveThresh::meanFlt(int kern_dim)
{
    QImage out_image(image.width(), image.height(), QImage::Format_Grayscale8);
    const int DK = kern_dim / 2;
    for (int indx_row = 0; indx_row < image.height(); indx_row++)
    {
        quint8* ptr_out_row = (quint8*)(out_image.bits()
                + indx_row * out_image.bytesPerLine());
        for (int indx_col = 0; indx_col < image.width(); indx_col++)
        {
            std::vector<int> intens;
            for (int indx_ker_row = 0; indx_ker_row < kern_dim; indx_ker_row++)
            {
                int x = indx_row - DK + indx_ker_row;
                if (x >= 0 && x < image.height())
                {
                    quint8* ptr_in_row = (quint8*)(image.bits()
                        + x * image.bytesPerLine());
                    for (int indx_ker_col = 0; indx_ker_col < kern_dim; indx_ker_col++)
                    {
                        int y = indx_col - DK + indx_ker_col;
                        if (y >= 0 && y < image.width())
                        {
                            intens.push_back(ptr_in_row[y]);
                        }
                    }
                }
            }
            ptr_out_row[indx_col] = mean(intens);
        }
    }

    return out_image;
}//medianFlt

QImage AdaptiveThresh::imageSegment(int kern_dim, int C, QString stat)
{

    QImage out;
    if (stat == "median"){
        out = medianFlt(kern_dim);
    }else if (stat == "mean")
    {
        out = meanFlt(kern_dim);
    }

    for (int indx_row = 0; indx_row < image.height(); indx_row++)
    {
        quint8* ptr_in_row = (quint8*)(image.bits()
                + indx_row * image.bytesPerLine());
        quint8* ptr_out_row = (quint8*)(out.bits()
                + indx_row * out.bytesPerLine());
        for (int indx_col = 0; indx_col < image.width(); indx_col++)
        {
            ptr_in_row[indx_col] = ((ptr_out_row[indx_col]-ptr_in_row[indx_col])> C) ? INTENS_MIN : INTENS_MAX;
        }
    }

    return image;
}//medianFlt

