#include "easypr/plate_recognize.h"
#include <android/log.h>
#define LOG_TAG "System.out"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
using namespace std;

namespace easypr {

CPlateRecognize::CPlateRecognize() {
	// cout << "CPlateRecognize" << endl;
	// m_plateDetect= new CPlateDetect();
	// m_charsRecognise = new CCharsRecognise();
}
void CPlateRecognize::plateAnn(Mat plate, string& name){
	//charsRecognise(plate, name, 0);
	charsRecogniseEx(plate,name);
}
// !车牌识别模块
int CPlateRecognize::plateRecognize(const char* sdcard, Mat src,
		std::vector<string> &licenseVec, int index) {
	// 车牌方块集合
	vector<CPlate> plateVec;

	// 进行深度定位，使用颜色信息与二次Sobel
	int resultPD = plateDetect(src, plateVec, getPDDebug(), index);

	if (resultPD == 0) {
		int num = plateVec.size();
		int i = 0;
		LOGI("plateDetect 1 : %d", num);
		//依次识别每个车牌内的符号
		for (int j = 0; j < num; j++) {
			CPlate item = plateVec[j];
			Mat plate = item.getPlateMat();
//			stringstream ss(stringstream::in | stringstream::out);
//			ss << "/storage/emulated/0/plateRecognize_" << j << ".jpg";
//			Mat grayResult;
//			cvtColor(plate, grayResult, CV_BGR2GRAY);
//			threshold(grayResult, grayResult, 125, 255, CV_THRESH_OTSU);
//			imwrite(ss.str(), grayResult);
			//获取车牌颜色
			string plateType = getPlateColor(plate);

			//获取车牌号
			string plateIdentify = "";
			int resultCR = charsRecognise(plate, plateIdentify, index);
			if (resultCR == 0) {
				stringstream ss(stringstream::in | stringstream::out);
				ss << sdcard << "/plate.jpg";
				imwrite(ss.str(), plate);
				string license = plateType + ":" + plateIdentify;
				licenseVec.push_back(license);
			}
		}
		//完整识别过程到此结束
	}

	return resultPD;
}

} /*! \namespace easypr*/
