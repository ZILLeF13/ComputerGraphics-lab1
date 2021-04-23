#include<iostream>
#include <QImage>
#include <fstream>
#include <string.h>
#include <string>
#include <algorithm>
#include <stdio.h>
#include "Filter.h"
void main(int argc, char *argv[])
{
    std:: string s;
    QImage image;
    for (int i = 0; i < argc; i++)
    {
        if (!std::strcmp(argv[i], "-p") && (i + 1 < argc))
        {
            s = argv[i + 1];
        }
    }

    char size[80];
    std::ifstream ifs("Kernel.txt");
    ifs.getline(size, 3, '\n');
    int sizei = std::atoi(size);
    Kernel MatKernel(sizei);
    std::unique_ptr<float[]> tmp = std::make_unique<float[]>(sizei * sizei);

    for (int i = 0; i < sizei * sizei; i++)
    {
        ifs.getline(size, 2, '\n');
        tmp[i] = std::atoi(size);
    }

    MatKernel.SetKernel(tmp.get(), sizei / 2);

    image.load(QString(s.c_str()));
    
    image.save("Picture/Sourse.jpg");
    
    InvertFilter invert;
    invert.process(image).save("Picture/Invert.jpg");

    BlurFilter blur;
    blur.process(image).save("Picture/Blur.jpg");
    
    GaussianFilter gaussian;
    gaussian.process(image).save("Picture/Gaussian.jpg");
    
    GrayScaleFilter GrayScale;
    GrayScale.process(image).save("Picture/GrayScale.jpg");
    
    SepiaFilter Sepia;
    Sepia.process(image).save("Picture/Sepia.jpg");
    
    BrightnessFilter Brightness;
    Brightness.process(image).save("Picture/Brightness.jpg");
    
    SobelXFilter SobelX;
    SobelX.process(image).save("Picture/SobelX.jpg");

    SobelYFilter SobelY;
    SobelY.process(image).save("Picture/SobelY.jpg");
    
    SharpnessFilter Sharpness;
    Sharpness.process(image).save("Picture/Sharpness.jpg");
    
    GrayWorldFilter GrayWorld;
    GrayWorld.process(image).save("Picture/GrayWorld.jpg");
    
    HistogramFilter Histogram;
    Histogram.process(image).save("Picture/Histogram.jpg");
    
    TransferFilter Transfer;
    Transfer.process(image).save("Picture/Transfer.jpg");
    
    ShcharraYFilter ShcharraY;
    ShcharraY.process(image).save("Picture/ShcharraY.jpg");

    ShcharraXFilter ShcharraX;
    ShcharraX.process(image).save("Picture/ShcharraX.jpg");
    
    Dilation dilation(MatKernel);
    dilation.process(image).save("Picture/Dilation.jpg");
    
    Erosion erosion(MatKernel);
    erosion.process(image).save("Picture/Erosion.jpg");

    Opening opening(MatKernel);
    opening.process(image).save("Picture/Opening.jpg");
    
    Closing closing(MatKernel);
    closing.process(image).save("Picture/Closing.jpg");
    
    Grad grad(MatKernel);
    grad.process(image).save("Picture/Grad.jpg");

    Median median;
    median.process(image).save("Picture/Median.jpg");
    
}

