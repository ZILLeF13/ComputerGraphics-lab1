//#include <QtCore/QCoreApplication>
#include<iostream>
#include <QImage>
#include "Filter.h"
#include "Filter.cpp"
void main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    //return a.exec();
    std:: string s;
    QImage img;
    for (int i = 0; i < argc; i++)
    {
        if (!std::strcmp(argv[i], "-p") && (i + 1 < argc))
        {
            s = argv[i + 1];
        }
    }
    img.load(QString(s.c_str()));
    img.save("Images/Sourse.png");
    InvertFilter invert;
    invert.process(img).save("Images/Invert.png");
}
