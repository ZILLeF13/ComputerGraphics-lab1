#pragma once
#include <QImage>
#include <string.h> 
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage &img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;

};

class InvertFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y)const override;
};

class Kernel
{
protected:
	//данные ядра
	std::unique_ptr<float[]> data;
	//радиус ядра
	std::size_t radius;
	//размер памяти ядра
	std::size_t getLen() const { return getSize() * getSize(); }
public:
	//конструктор пустого ядра
	Kernel(std::size_t radius) : radius(radius)
	{
		//выделение памяти под массив
		data = std::make_unique<float[]>(getLen());
	}
	//конструктор копирования 
	Kernel(const Kernel& other) : Kernel(other.radius)
	{
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}
	std::size_t getRadius() const { return radius; }
	std::size_t getSize() const { return 2 * radius + 1; }
	float operator[](std::size_t id) const { return data[id]; }
	float& operator[](std::size_t id) { return data[id]; }
	void SetKernel(float* dataK, int rad)
	{
		radius = rad;
		std::size_t len = getLen();
		if (data)
		{
			data.reset();
		}
		data = std::make_unique<float[]>(len);
		std::copy(dataK, dataK + len, data.get());
	}
};

class MatrixFilter : public Filter
{
protected:
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y)const override;
public:
	MatrixFilter(const Kernel& kernel) :mKernel(kernel) {};
	virtual ~MatrixFilter() = default; 
};

class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	BlurKernel(std::size_t radius = 2) : Kernel(radius)
	{
		for (std::size_t i = 0; i < getLen(); i++)
		{
			data[i] = 1.0f / getLen();
		}
	}
};

class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(std::size_t radius = 1) :MatrixFilter(BlurKernel(radius)) {};
};

class GaussianKernel : public Kernel
{
public:
	//наследование класов 
	using Kernel::Kernel;
	GaussianKernel(std::size_t radius = 2, float sigma = 3.f) : Kernel(radius)
	{
		//коэф нормировки ядра
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		//рассчитываем ядро линейного фильтра
		for(int x=-signed_radius;x<=signed_radius;x++)
			for (int y = -signed_radius; y <= signed_radius; y++)
			{
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
		}
		//нормируем ядро
		for (std::size_t i = 0; i < getLen(); i++)
		{
			data[i] /= norm;
		}
	}
};

class GaussianFilter : public MatrixFilter
{
public:
	GaussianFilter(std::size_t radius = 1) :MatrixFilter(GaussianKernel(radius)) {};
};

class GrayScaleFilter :public Filter
{
public:
	QColor calcNewPixelColor(const QImage& img, int x, int y)const override;
};

class SepiaFilter :public Filter
{
public:
	QColor calcNewPixelColor(const QImage& img, int x, int y)const override;
};

class BrightnessFilter :public Filter
{
public:
	QColor calcNewPixelColor(const QImage& img, int x, int y)const override;
};

class SobelXKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SobelXKernel() :Kernel(1)
	{
		data[0] = -1.f;
		data[1] = 0.f;
		data[2] = 1.f;
		data[3] = -2.f;
		data[4] = 0.f;
		data[5] = 2.f;
		data[6] = -1.f;
		data[7] = 0.f;
		data[8] = 1.f;
	}
};

class SobelXFilter : public MatrixFilter
{
public:
	SobelXFilter() : MatrixFilter(SobelXKernel()) {}
};

class SobelYKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SobelYKernel() :Kernel(1)
	{
		data[0] = -1.f;
		data[1] = -2.f;
		data[2] = -1.f;
		data[3] = 0.f;
		data[4] = 0.f;
		data[5] = 0.f;
		data[6] = 1.f;
		data[7] = 2.f;
		data[8] = 1.f;
	}
};

class SobelYFilter : public MatrixFilter
{
public:
	SobelYFilter() : MatrixFilter(SobelYKernel()) {}
};

class SharpnessKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SharpnessKernel() :Kernel(1)
	{
		data[0] = 0.f;
		data[1] = -1.f;
		data[2] = 0.f;
		data[3] = -1.f;
		data[4] = 5.f;
		data[5] = -1.f;
		data[6] = 0.f;
		data[7] = -1.f;
		data[8] = 0.f;
	}
};

class SharpnessFilter : public MatrixFilter
{
public:
	SharpnessFilter() : MatrixFilter(SharpnessKernel()) {}
};

class _GrayWorldFilter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y, float returnR, float returnG, float returnB, float Avg) const = 0;
public:
	virtual ~_GrayWorldFilter() = default;
	virtual QImage process(const QImage& img) const;

};

class GrayWorldFilter: public _GrayWorldFilter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y, float returnR, float returnG, float returnB, float Avg)const override;
};

class _Histogram
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y, float minR, float minG, float minB, float maxR, float maxG, float maxB) const = 0;
public:
	virtual ~_Histogram() = default;
	virtual QImage process(const QImage& img) const;

};

class HistogramFilter : public _Histogram
{
	QColor calcNewPixelColor(const QImage& img, int x, int y, float minR, float minG, float minB, float maxR, float maxG, float maxB)const override;
};

class _Transfer
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~_Transfer() = default;
	virtual QImage process(const QImage& img) const;

};

class TransferFilter : public _Transfer
{
	QColor calcNewPixelColor(const QImage& img, int x, int y)const override;
};

class ShcharraYKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ShcharraYKernel() :Kernel(1)
	{
		data[0] = 3.f;
		data[1] = 10.f;
		data[2] = 3.f;
		data[3] = 0.f;
		data[4] = 0.f;
		data[5] = 0.f;
		data[6] = -3.f;
		data[7] = -10.f;
		data[8] = -3.f;
	}
};

class ShcharraYFilter : public MatrixFilter
{
public:
	ShcharraYFilter() : MatrixFilter(ShcharraYKernel()) {}
};

class ShcharraXKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ShcharraXKernel() :Kernel(1)
	{
		data[0] = 3.f;
		data[1] = 0.f;
		data[2] = -3.f;
		data[3] = 10.f;
		data[4] = 0.f;
		data[5] = -10.f;
		data[6] = 3.f;
		data[7] = 0.f;
		data[8] = -3.f;
	}
};

class ShcharraXFilter : public MatrixFilter
{
public:
	ShcharraXFilter() : MatrixFilter(ShcharraXKernel()) {}
};

//Dilation

class Dilation_K : public Kernel
{
public:
	using Kernel::Kernel;
	Dilation_K(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	}
};

class Dilation : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Dilation(std::size_t radius = 1) :MatrixFilter(Dilation_K(radius)) {}
	Dilation(Kernel& k) :MatrixFilter(k) {}
};

//Erosion

class Erosion_K : public Kernel
{
public:
	using Kernel::Kernel;
	Erosion_K(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	}
};

class Erosion : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Erosion(std::size_t radius = 1) : MatrixFilter(Erosion_K(radius)) {}
	Erosion(Kernel& k) :MatrixFilter(k) {}
};

//Opening

class Opening_K : public Kernel
{
public:
	using Kernel::Kernel;
	Opening_K(std::size_t radius = 1) : Kernel(radius)
	{

		for (int i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	}
};

class Opening : public MatrixFilter
{
protected:
    QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Opening(std::size_t radius = 1) : MatrixFilter(Opening_K(radius)) {}
	Opening(Kernel& k) :MatrixFilter(k) {}
	QImage process(const QImage& img) const;
};

//Closing

class Closing_K : public Kernel
{
public:
	using Kernel::Kernel;
	Closing_K(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	}
};

class Closing : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Closing(std::size_t radius = 1) : MatrixFilter(Closing_K(radius)) {}
	Closing(Kernel& k) :MatrixFilter(k) {}
	QImage process(const QImage& img) const;
};

//Grad

class Grad_K : public Kernel
{
public:
	using Kernel::Kernel;
	Grad_K(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	}
};


class Grad : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Grad(std::size_t radius = 1) : MatrixFilter(Grad_K(radius)) {}
	Grad(Kernel& k) :MatrixFilter(k) {}
	QImage process(const QImage& img) const;
};

//Median

class Median_K : public Kernel
{
public:
	using Kernel::Kernel;
	Median_K(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	}
};

class Median : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Median(std::size_t radius = 1) : MatrixFilter(Median_K(radius)) {}
};
