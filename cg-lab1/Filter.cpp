#include<iostream>
#include "Filter.h"
#include<QImage>
#include <fstream>
#include <string.h>
#include <string>
#include <algorithm>
#include <stdio.h>

template <class T>
T Clamp(T value, T max, T min)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

QImage Filter::process(const QImage& img)const
{
	QImage res(img);
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y);
			res.setPixelColor(x, y, color);
		}
	return res;
}

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y)const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}

QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y)const
{
	float returnR = 0, returnG = 0, returnB = 0;
	int size = mKernel.getSize(), radius = mKernel.getRadius();
	for (int i=-radius;i<=radius;i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	return QColor(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));

}

QColor GrayScaleFilter::calcNewPixelColor(const QImage& img, int x, int y)const
{
	QColor color = img.pixelColor(x, y);
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
	color.setRgb(intensity, intensity, intensity);
	return color;
}

QColor SepiaFilter::calcNewPixelColor(const QImage& img, int x, int y)const
{
	QColor color = img.pixelColor(x, y);
	float k =50;
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
	float returnG = intensity + 0.5 * k;
	return QColor(Clamp(intensity + 2 * k, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(intensity - 1 * k, 255.f, 0.f));
}

QColor BrightnessFilter::calcNewPixelColor(const QImage& img, int x, int y)const
{
	QColor color = img.pixelColor(x, y);
	float k = 50;
	return QColor(Clamp(color.red() + k, 255.f, 0.f), Clamp(color.green() + k, 255.f, 0.f), Clamp(color.blue() + k, 255.f, 0.f));
}

QImage _GrayWorldFilter::process(const QImage& img)const
{
	float R = 0,G=0,B=0; 
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = img.pixelColor(x, y);
			R += Clamp<float>(color.red(), 255.f, 0.f);
			G += Clamp<float>(color.green(), 255.f, 0.f);
			B += Clamp<float>(color.blue(), 255.f, 0.f);
		}
	unsigned long long pixelsCount = img.width() * img.height();
	float returnR = R / pixelsCount;
	float returnG = G / pixelsCount;
	float returnB = B / pixelsCount;
	float Avg = (returnR + returnG + returnB) / 3.f;

	QImage res(img);
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y,returnR,returnG,returnB,Avg);
			res.setPixelColor(x, y, color);
		}
	return res;
}

QColor GrayWorldFilter::calcNewPixelColor(const QImage& img, int x, int y,float returnR, float returnG, float returnB, float Avg )const
{
	QColor color = img.pixelColor(x, y);
	return QColor(Clamp((color.red() * Avg) / returnR, 255.f, 0.f), Clamp((color.green() * Avg )/ returnG, 255.f, 0.f), Clamp((color.blue() * Avg) / returnB, 255.f, 0.f));
}

QImage _Histogram::process(const QImage& img)const
{
	float R = 0, G = 0, B = 0, minR = 0, minG = 0, minB = 0, maxR = 0, maxG = 0, maxB = 0;
	QColor color = img.pixelColor(0, 0);
	maxR=minR = Clamp<float>(color.red(), 255.f, 0.f);
	maxG=minG = Clamp<float>(color.green(), 255.f, 0.f);
	maxB=minB = Clamp<float>(color.blue(), 255.f, 0.f);
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{

			color = img.pixelColor(x, y);
			R = Clamp<float>(color.red(), 255.f, 0.f);
			G = Clamp<float>(color.green(), 255.f, 0.f);
			B = Clamp<float>(color.blue(), 255.f, 0.f);
			if (R < minR)
				minR = R;
			if (R > maxR)
				maxR = R;
			if (G < minG)
				minG = G;
			if (G > maxG)
				maxG = G;
			if (B < minB)
				minB = B;
			if (B > maxB)
				maxB = B;
		}

	QImage res(img);
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y, minR = 0, minG , minB , maxR , maxG , maxB);
			res.setPixelColor(x, y, color);
		}
	return res;
}

QColor HistogramFilter::calcNewPixelColor(const QImage& img, int x, int y, float minR, float minG, float minB, float maxR,float maxG,float maxB)const
{
	QColor color = img.pixelColor(x, y);
	return QColor(Clamp((color.red() - minR) *255.f/(maxR-minR), 255.f, 0.f), Clamp((color.green() - minG) * 255.f / (maxG- minG), 255.f, 0.f), Clamp((color.blue() - minB) * 255.f / (maxB - minB), 255.f, 0.f));
}

QImage _Transfer::process(const QImage& img)const   
{   
	QImage res(img);
	for (int x = 0; x < img.width()-50; x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = img.pixelColor(x+50, y);
			res.setPixelColor(x, y, color);
		}
	for (int x = img.width()-50; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y);
			res.setPixelColor(x, y, color);
		}
	return res;
}

QColor TransferFilter::calcNewPixelColor(const QImage& img, int x, int y)const
{
	QColor color = img.pixelColor(x, y);
	return QColor(Clamp(color.red()-255.f, 255.f, 0.f), Clamp(color.green() -255.f, 255.f, 0.f), Clamp(color.blue() - 255.f, 255.f, 0.f));
}

QColor Dilation::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0, R = 0;
	float returnG = 0, G = 0;
	float returnB = 0, B = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int n = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			R = color.red() * mKernel[n];
			G = color.green() * mKernel[n];
			B = color.blue() * mKernel[n];
			if (R > returnR)
				returnR = R;
			if (G > returnG)
				returnG = G;
			if (B > returnB)
				returnB = B;
		}
	return QColor(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));
}

QColor Erosion::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 255, R = 0;
	float returnG = 255, G = 0;
	float returnB = 255, B = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int n = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			R = color.red() * mKernel[n];
			G = color.green() * mKernel[n];
			B = color.blue() * mKernel[n];
			if (R < returnR)
				returnR = R;
			if (G < returnG)
				returnG = G;
			if (B < returnB)
				returnB = B;
		}
	return QColor(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));
}

QImage Opening::process(const QImage& img) const
{
	QImage returnQ;
	int radius = mKernel.getRadius();
	Dilation dilation(radius);
	returnQ = dilation.process(img);
	Erosion erosion(radius);
	returnQ = erosion.process(returnQ);
	return returnQ;
}

QColor Opening::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor QC;
	return QC;
}

QImage Closing::process(const QImage& img) const
{
	QImage returnQ;
	int radius = mKernel.getRadius();
	Erosion erosion(radius);
	returnQ = erosion.process(img);
	Dilation dilation(radius);
	returnQ = dilation.process(returnQ);
	return returnQ;
}


QColor Closing::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor QC;
	return QC;
}

QImage Grad::process(const QImage& img) const
{
	QImage tmp1, tmp2, returnQ(img);
	int radius = mKernel.getRadius();
	Dilation dilation(radius);
	tmp1 = dilation.process(img);
	Erosion erosion(radius);
	tmp2 = erosion.process(img);

	for (int x = 0; x < tmp1.width(); x++)
		for (int y = 0; y < tmp2.height(); y++)
		{
			QColor color1 = tmp1.pixelColor(x, y);
			QColor color2 = tmp2.pixelColor(x, y);
			QColor res;
			res.setRgb(Clamp(color1.red() - color2.red(), 255, 0), Clamp(color1.green() - color2.green(), 255, 0), Clamp(color1.blue() - color2.blue(), 255, 0));
			returnQ.setPixelColor(x, y, res);
		}

	return returnQ;
}



QColor Grad::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor QC;
	return QC;
}


QColor Median::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	float* R, * G, * B;
	R = new float[size * size];
	G = new float[size * size];
	B = new float[size * size];
	for (int i = -radius; i <= radius; i++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			int n = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			R[n] = color.red();
			G[n] = color.green();
			B[n] = color.blue();
		}
	}
	std::sort(R, R + size * size);
	std::sort(G, G + size * size);
	std::sort(B, B + size * size);
	QColor QC;
	QC.setRgb(Clamp(R[size * size / 2], 255.f, 0.f), Clamp(G[size * size / 2], 255.f, 0.f), Clamp(B[size * size / 2], 255.f, 0.f));
	return QC;
}