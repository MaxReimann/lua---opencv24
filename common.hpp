#ifndef __COMMON_H__
#define __COMMON_H__

#include<cstdio>
#include<cmath>
#include<cassert>
#include<iostream>
#include<vector>
#include<string>
using namespace std;

#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

#include "THpp.hpp"

#if CV_MAJOR_VERSION != 2
#error OpenCV version must be 2.x.x
#endif
#if CV_MINOR_VERSION < 4
#error OpenCV version must be >= 2.4.0
#endif

typedef Mat_<float> matf;
typedef Mat_<double> matd;
typedef matf matr;
typedef Mat_<unsigned char> matb;
typedef Mat_<Vec3b> mat3b;
typedef unsigned char ubyte;

template<typename T, typename T2> inline bool epsEqual(T a, T2 b, double eps = 0.01) {
  return (a-eps < b) && (b < a+eps);
}

void display(const Mat & im);

template<typename Treal>
mat3b TensorToMat3b(const TH::Tensor<Treal> & im) {
  if (im.size(0) == 3) {
    long h = im.size(1);
    long w = im.size(2);
    const long* is = im.stride();
    const Treal* im_p = im.data();
    mat3b ret(h, w);
    for (int i = 0; i < h; ++i)
      for (int j = 0; j < w; ++j)
	ret(i,j)=Vec3b(min<Treal>(255., max<Treal>(0.0, im_p[is[0]*2+is[1]*i+is[2]*j]*255.)),
		       min<Treal>(255., max<Treal>(0.0, im_p[is[0]  +is[1]*i+is[2]*j]*255.)),
		       min<Treal>(255., max<Treal>(0.0, im_p[is[0]*0+is[1]*i+is[2]*j]*255.)));
    return ret;
  } else if (im.size(2) == 3) {
    long h = im.size(0);
    long w = im.size(1);
    const long* is = im.stride();
    const Treal* im_p = im.data();
    mat3b ret(h, w);
    for (int i = 0; i < h; ++i)
      for (int j = 0; j < w; ++j)
	ret(i,j)=Vec3b(min<Treal>(255., max<Treal>(0.0, im_p[is[0]*i+is[1]*j+is[2]*2]*255.)),
		       min<Treal>(255., max<Treal>(0.0, im_p[is[0]*i+is[1]*j+is[2]  ]*255.)),
		       min<Treal>(255., max<Treal>(0.0, im_p[is[0]*i+is[1]*j+is[2]*0]*255.)));
    return ret;
  } else {
    THerror("TensorToMat3b: tensor must be 3xHxW or HxWx3");
  }
  return mat3b(0,0); //remove warning
}

// byte case : we may not have to copy
template<>
mat3b TensorToMat3b<ubyte>(const TH::Tensor<ubyte> & im);

template<typename Treal>
Mat TensorToMat(TH::Tensor<Treal> & T) {
  const int n = T.nDimension();
  switch(n) {
  case 0:
    return Mat(0, 0, DataType<Treal>::type);
  case 1:
    return Mat(T.size(0), 1, DataType<Treal>::type, (void*)T.data(),
	       T.stride(0)*sizeof(Treal));
  case 2:
    if (T.stride(1) != 1)
      T = T.newContiguous();
    return Mat(T.size(0), T.size(1), DataType<Treal>::type, (void*)T.data(),
	       T.stride(0)*sizeof(Treal));
  case 3:
    if (T.stride(1) != T.size(2))
      T = T.newContiguous();
    if (T.size(2) == 2) {
      return Mat(T.size(0), T.size(1), DataType<Vec<Treal, 2> >::type, (void*)T.data());
    } else if (T.size(2) == 3) {
      return Mat(T.size(0), T.size(1), DataType<Vec<Treal, 3> >::type, (void*)T.data());
    }
  default:
    {
      T = T.newContiguous();
      int* sizes = new int[n];
      size_t* steps = new size_t[n-1];
      for (int i = 0; i < n; ++i)
	sizes[i] = T.size(i);
      for (int i = 0; i < n-1; ++i)
	steps[i] = T.stride(i)*sizeof(Treal);
      Mat ret(T.nDimension(), sizes, DataType<Treal>::type, (void*)T.data(), steps);
      delete[] steps;
      delete[] sizes;
      return ret;
    }
  }
}

template<typename Treal>
inline Mat_<Treal> TensorToMat2d(TH::Tensor<Treal> & T) {
  return (Mat_<Treal>)TensorToMat(T);
}

template<typename Treal>
inline Mat_<Vec<Treal, 3> > TensorToMatImage(TH::Tensor<Treal> & T) {
  return (Mat_<Vec<Treal, 3> >)TensorToMat(T);
}

#endif
