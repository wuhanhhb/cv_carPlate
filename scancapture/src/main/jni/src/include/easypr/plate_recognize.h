//////////////////////////////////////////////////////////////////////////
// Name:	    plate_recognize Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CPlateRecognize
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_RECOGNIZE_H__
#define __PLATE_RECOGNIZE_H__

#include "easypr/plate_detect.h"
#include "easypr/chars_recognise.h"

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {

class CPlateRecognize: public CPlateDetect, public CCharsRecognise {
public:
	CPlateRecognize();

	void plateAnn(Mat plate, string& name);
	//! ���Ƽ�����ַ�ʶ��
	int plateRecognize(const char* sdcard, cv::Mat src,
			std::vector<std::string>& licenseVec, int index = 0);

	//! ���ģʽ�빤ҵģʽ�л�
	inline void setLifemode(bool param) {
		CPlateDetect::setPDLifemode(param);
	}

	//! �Ƿ�������ģʽ
	inline void setDebug(bool param) {
		CPlateDetect::setPDDebug(param);
		CCharsRecognise::setCRDebug(param);
	}

};

} /* \namespace easypr  */

#endif /* endif __PLATE_RECOGNITION_H__ */
