#include <QApplication>
#include "interface.h"


using namespace std;

//~~~~~~~segment widget~~~~~~~~
segment::segment(QWidget *parent){
    this->parent = parent;
    resize(200,120);
    otsu_btn = new QPushButton("&Otsu Segmentation ",this);
    mean_btn = new QPushButton("&Mean Adaptive Segmentation ",this);
    median_btn = new QPushButton("&Median Adaptive Segmentation ",this);
    connect(otsu_btn, SIGNAL(clicked()), this, SLOT(OtsuThresh()));
    connect(mean_btn, SIGNAL(clicked()), this, SLOT(AdaptiveMeanThresh()));
    connect(median_btn, SIGNAL(clicked()), this, SLOT(AdaptiveMedianThresh()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(otsu_btn);
    layout->addWidget(mean_btn);
    layout->addWidget(median_btn);

    w = new QWidget(parent);

};// constructor

//~~~~~~~~~main window~~~~~~~~~~
void program::load()
{
    fileName = QFileDialog::getOpenFileName();
    if (fileName == ""){
        return;
    }
    if (!image.load(fileName)){
        QTextStream(stdout) << "Cannot load image: " << fileName << Qt::endl;
        return;
    }

    if (image.load(fileName))
    {
        seg->setImg(QImage(0,0,QImage::Format_Grayscale8));
        image = image.convertToFormat(QImage::Format_Grayscale8);
        if (image.width()<this->width()||image.height()<this->height()){
            image = image.scaled(this->width(),this->height(),Qt::KeepAspectRatio);
        }
        QPalette palette;
        palette.setBrush(QPalette::Window, image);
        this->setFixedSize(image.width(),image.height());
        this->setPalette(palette);
        update();
    }
}

void program::openSegmentOption(){
    if (image.size() == QSize(0,0)){
        return;
    }
    QImage ImgSegment(abs(selected_height)+1, abs(selected_width)+1, QImage::Format_Grayscale8);

    int fromx = (selected_height >= 0) ? mousePosInitial.x() : mousePosNext.x();
    int tox = (selected_height <= 0) ? mousePosInitial.x() : mousePosNext.x();
    int fromy = (selected_width >= 0) ? mousePosInitial.y() : mousePosNext.y();
    int toy = (selected_width <= 0) ? mousePosInitial.y() : mousePosNext.y();

    for (int indx_row = fromy; indx_row <= toy; indx_row++)
    {
        quint8* ptr_row_seg = (quint8*)(ImgSegment.bits()
                + (indx_row-fromy)* ImgSegment.bytesPerLine());
        quint8* ptr_row_img = (quint8*)(image.bits()
                + indx_row * image.bytesPerLine());

        for (int indx_col = fromx; indx_col <= tox; indx_col++)
        {
            ptr_row_seg[indx_col-fromx] = ptr_row_img[indx_col];
        }
    }
    seg->setImg(ImgSegment);
    seg->setImgPos(QPoint(fromx,fromy));
    seg->show();
    update();
}

void program::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons())
    {
        if (selected_height!=0 || selected_width!=0){
            selected_height=0;
            selected_width=0;
            showInstruct = false;
            update();
        }
        mousePosInitial = event->pos();
    }
}//store mouse position on clicked

void program::mouseReleaseEvent(QMouseEvent *event)
{
    if (selected_height!=0 || selected_width!=0){
        showInstruct = true;
        update();
    }
}//store mouse position on clicked

void program::mouseMoveEvent(QMouseEvent *event)
{
    //don't catch mouse position if it is outside the image
    if (event->pos().x()<0 || event->pos().y()<0 || event->pos().x()>image.width() || event->pos().y()>image.height()){
        return;
    }
    //catch mouse positions
    if(event->buttons())
    {
        mousePosNext = event->pos();
        selected_height = (mousePosNext.x()-mousePosInitial.x());
        selected_width = (mousePosNext.y()-mousePosInitial.y());
        update();
    }
}//store cursor's position each time mouse is clicked

void program::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    if (seg->isActiveWindow()) {
       pen = QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    }
    else {
            auto back = QRect{seg->getImgPos(), seg->getImage().size()};//rectangle that has center of your char to print pixmap from
            painter.drawImage(back,seg->getImage());
            pen =  QPen(Qt::red, 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);

    }
    painter.setPen(pen);
    QRect rect = QRect(mousePosInitial, QSize(selected_height, selected_width));
    painter.drawRect(rect);
    pen = QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    if(showInstruct){
        painter.drawText(rect.center()-QPoint(30,0),"Press Cltr+T");
    }
}




int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    program p;
    p.show();




    return app.exec();



}//main
