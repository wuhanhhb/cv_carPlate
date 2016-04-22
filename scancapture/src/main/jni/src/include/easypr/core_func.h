#ifndef __CORE_FUNC_H__
#define __CORE_FUNC_H__

#include <opencv2/opencv.hpp>

using namespace cv;

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

enum Color { BLUE, YELLOW, WHITE, UNKNOWN };

enum LocateType { SOBEL, COLOR, OTHER };

//! 根据一幅图像与颜色模板获取对应的二值图
//! 输入RGB图像, 颜色模板（蓝色、黄色）
//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）
Mat colorMatch(const Mat& src, Mat& match, const Color r,
               const bool adaptive_minsv);

//! �ж�һ�����Ƶ���ɫ
//! ���복��mat����ɫģ��
//! ����true��fasle
bool plateColorJudge(const Mat& src, const Color r, const bool adaptive_minsv,
                     float& percent);

bool bFindLeftRightBound(Mat& bound_threshold, int& posLeft, int& posRight);
bool bFindLeftRightBound1(Mat& bound_threshold, int& posLeft, int& posRight);
bool bFindLeftRightBound2(Mat& bound_threshold, int& posLeft, int& posRight);

//ȥ�����Ϸ���ť��
//����ÿ��Ԫ�صĽ�Ծ�����С��X��Ϊ����������ȫ����0��Ϳ�ڣ�
// X���Ƽ�ֵΪ���ɸ��ʵ�ʵ���
bool clearLiuDing(Mat& img);
void clearLiuDingOnly(Mat& img);
void clearLiuDing(Mat mask, int& top, int& bottom);

//! ��ó�����ɫ
Color getPlateType(const Mat& src, const bool adaptive_minsv);

//! ֱ��ͼ���
Mat histeq(Mat in);
Mat features(Mat in, int sizeData);
Rect GetCenterRect(Mat& in);
Mat CutTheRect(Mat& in, Rect& rect);
int ThresholdOtsu(Mat mat);

//! ��ȡ��ֱ��ˮƽ����ֱ��ͼ
Mat ProjectedHistogram(Mat img, int t);

} /*! \namespace easypr*/

#endif
/* endif __CORE_FUNC_H__ */
