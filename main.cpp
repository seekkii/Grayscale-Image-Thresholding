#include <QApplication>
#include "interface.h"


using namespace std;



int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    program p;
    p.show();

    return app.exec();



}//main
