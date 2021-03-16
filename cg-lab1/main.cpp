//#include <QtCore/QCoreApplication>
#include<iostream>
#include <QImage>
#include "Filter.h"
void main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    //return a.exec();
    std:: string s;
    QImage image;
    for (int i = 0; i < argc; i++)
    {
        if (!std::strcmp(argv[i], "-p") && (i + 1 < argc))
        {
            s = argv[i + 1];
        }
    }
    image.load(QString(s.c_str()));
    image.save("Sourse.png");
    InvertFilter invert;
    invert.process(image).save("Invert.png");
}
