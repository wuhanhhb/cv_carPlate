//////////////////////////////////////////////////////////////////////////
// Name:	    chars_segment Header
// Version:		1.0
// Date:	    2014-09-19
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsSegment
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_SEGMENT_H__
#define __CHARS_SEGMENT_H__

#include "core_func.h"

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {

class CCharsSegment {
public:
	CCharsSegment();

	//! �ַ�ָ�
	int charsSegment(Mat, vector<Mat>&, int index = 0);

	//! �ַ�ߴ���֤
	bool verifyCharSizes(Mat r);

	//! �ַ�Ԥ����
	Mat preprocessChar(Mat in);

	//! ������⳵��������²������ַ��λ�úʹ�С
	Rect GetChineseRect(const Rect rectSpe);

	//! �ҳ�ָʾ���е��ַ��Rect��������A7003X������A��λ��
	int GetSpecificRect(const vector<Rect>& vecRect);

	//! �����������������
	//  1.�������ַ�Rect��ߵ�ȫ��Rectȥ�����������ؽ������ַ��λ�á�
	//  2.�������ַ�Rect��ʼ������ѡ��6��Rect���������ȥ��
	int RebuildRect(const vector<Rect>& vecRect, vector<Rect>& outRect,
			int specIndex);

	//! ��Rect��λ�ô����ҽ�������
	int SortRect(const vector<Rect>& vecRect, vector<Rect>& out);

	//! ���ñ���
	inline void setLiuDingSize(int param) {
		m_LiuDingSize = param;
	}
	inline void setColorThreshold(int param) {
		m_ColorThreshold = param;
	}

	inline void setBluePercent(float param) {
		m_BluePercent = param;
	}
	inline float getBluePercent() const {
		return m_BluePercent;
	}
	inline void setWhitePercent(float param) {
		m_WhitePercent = param;
	}
	inline float getWhitePercent() const {
		return m_WhitePercent;
	}

	//! �Ƿ�������ģʽ������Ĭ��0���ر�
	static const int DEFAULT_DEBUG = 0;

	//! preprocessChar���ó���
	static const int CHAR_SIZE = 20;
	static const int HORIZONTAL = 1;
	static const int VERTICAL = 0;

	//! preprocessChar���ó���
	static const int DEFAULT_LIUDING_SIZE = 7;
	static const int DEFAULT_MAT_WIDTH = 136;
	static const int DEFAULT_COLORTHRESHOLD = 150;

	//! �Ƿ�������ģʽ
	inline void setDebug(int param) {
		m_debug = param;
	}

	//! ��ȡ����ģʽ״̬
	inline int getDebug() {
		return m_debug;
	}

private:
	//�����жϲ���
	int m_LiuDingSize;

	//�����ƴ�С����
	int m_theMatWidth;

	//��������ɫ�жϲ���
	int m_ColorThreshold;
	float m_BluePercent;
	float m_WhitePercent;

	//! �Ƿ�������ģʽ��0�رգ���0����
	int m_debug;
};

} /* \namespace easypr  */

#endif /* endif __CHARS_SEGMENT_H__ */
