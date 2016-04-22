#include<com_example_carplate_CarPlateDetection.h>
#include "easypr/plate_locate.h"
#include "easypr/plate_judge.h"
#include "easypr/chars_segment.h"
#include "easypr/chars_identify.h"
#include "easypr/plate_detect.h"
#include "easypr/chars_recognise.h"
#include "easypr/plate_recognize.h"
#include "../train/ann_train.cpp"
using namespace easypr;
#include <android/log.h>
#include <string>
#include <easypr/grobal.h>
#define LOG_TAG "System.out"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

char* jstring2str(JNIEnv* env, jstring jstr) {
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("GB2312");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0) {
		rtn = (char*) malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
}

JNIEXPORT jint JNICALL Java_com_example_carplate_CarPlateDetection_annProcEx(
		JNIEnv *env, jclass obj, jstring path) {
	char* sdcard = jstring2str(env, path);
	grobal_sdcard = sdcard;
	LOGE("load grobal_sdcard %s ", grobal_sdcard);
	CPlateRecognize pr;
	pr.setDebug(false);
    	pr.setPDLifemode(false);
    	pr.setVerifyMax(200);
    	pr.setLiuDingSize(7);
    	pr.setColorThreshold(150);

    	//LOGE("load begin 1");

    	stringstream ss(stringstream::in | stringstream::out);
        	ss << sdcard << "/test";
        	vector<string> files = searchdir(ss.str().c_str()); //Utils::getFiles(ss.str());
        	LOGI("plateAnn: %s %d", ss.str().c_str(), files.size());
            int size = files.size();
            for (size_t j = 0; j < size; j++) {
            	cout << files[j].c_str() << endl;
            	Mat plate = imread(files[j].c_str());
            	//Mat mat_blur;
                //mat_blur = plate.clone();
                //GaussianBlur(plate, mat_blur, Size(3, 3), 0, 0, BORDER_DEFAULT);
            	//Mat mat_gray;
            	//cvtColor(mat_blur, mat_gray, CV_BGR2GRAY);

            	//LOGE("load begin 3");

            	stringstream ss(stringstream::in | stringstream::out);
                ss << "plateAnn_" << j << "_";
                string name(ss.str());
                pr.plateAnn(plate,name);
            }
            LOGI("plateAnn over");
	//plateAnn(sdcard,pr);
	return 0;
}

JNIEXPORT jint JNICALL Java_com_example_carplate_CarPlateDetection_annProc(
		JNIEnv *env, jclass obj, jstring path) {
	char* sdcard = jstring2str(env, path);
	annMain(sdcard);
	return 0;
}
JNIEXPORT jbyteArray JNICALL Java_com_example_carplate_CarPlateDetection_ImageProc(
		JNIEnv *env, jclass obj, jstring path, jstring imgpath, jstring svmpath,
		jstring annpath) {
	CPlateRecognize pr;
//	const string *img = (*env)->GetStringUTFChars(env, imgpath, 0);
//	const string *svm = (*env)->GetStringUTFChars(env, svmpath, 0);
//	const string *ann = (*env)->GetStringUTFChars(env, annpath, 0);
	char* img = jstring2str(env, imgpath);
	char* svm = jstring2str(env, svmpath);
	char* ann = jstring2str(env, annpath);
	char* sdcard = jstring2str(env, path);

	grobal_sdcard = sdcard;
	LOGE("load grobal_sdcard %s ", grobal_sdcard);

	Mat src = imread(img);
	pr.LoadSVM(svm);
	pr.LoadANN(ann);

	pr.setDebug(false);
//	pr.setGaussianBlurSize(5);
//	pr.setMorphSizeWidth(17);
//	pr.setMorphSizeHeight(3);
	pr.setPDLifemode(false);
	//pr.setVerifyMin(3);
	pr.setVerifyMax(200);
	pr.setLiuDingSize(7);
	pr.setColorThreshold(150);
	vector<string> plateVec;

	int count = pr.plateRecognize(sdcard, src, plateVec);
	string str = "0";
	if (count == 0 && plateVec.size() > 0) {
		str = plateVec[0];
	}
	char *result = new char[str.length() + 1];
	strcpy(result, str.c_str());
	jbyte *by = (jbyte*) result;
	jbyteArray jarray = env->NewByteArray(strlen(result));
	env->SetByteArrayRegion(jarray, 0, strlen(result), by);
	return jarray;
}
JNIEXPORT jbyteArray JNICALL Java_com_example_carplate_CarPlateDetection_ImageProcEx(
		JNIEnv *env, jclass obj, jintArray datas,jintArray sizes,jstring path, jstring svmpath,
		jstring annpath) {
	CPlateRecognize pr;
//	const string *img = (*env)->GetStringUTFChars(env, imgpath, 0);
//	const string *svm = (*env)->GetStringUTFChars(env, svmpath, 0);
//	const string *ann = (*env)->GetStringUTFChars(env, annpath, 0);
	//char* img = jstring2str(env, imgpath);
	char* svm = jstring2str(env, svmpath);
	char* ann = jstring2str(env, annpath);
	char* sdcard = jstring2str(env, path);

	grobal_sdcard = sdcard;
LOGE("load grobal_sdcard 1");
	jint *cbuf;
	jint *csize;
    cbuf = env->GetIntArrayElements(datas, false);
    csize = env->GetIntArrayElements(sizes, false);

	if(cbuf == NULL || csize == NULL)
	{
		return NULL;
	}

LOGE("load grobal_sdcard 2");

	Mat mat(csize[4], csize[5], CV_8UC4, (unsigned char*)cbuf);
	Rect_<float> bound_rect(csize[0], csize[1], csize[2], csize[3]);
	Mat src = mat(bound_rect);

	LOGE("load grobal_sdcard %s %d %d %d %d %d %d", grobal_sdcard,csize[0],csize[1],csize[2],csize[3],csize[4],csize[5]);

	pr.LoadSVM(svm);
	pr.LoadANN(ann);

	pr.setDebug(false);
//	pr.setGaussianBlurSize(5);
//	pr.setMorphSizeWidth(17);
//	pr.setMorphSizeHeight(3);
	pr.setPDLifemode(false);
	//pr.setVerifyMin(3);
	pr.setVerifyMax(200);
	pr.setLiuDingSize(7);
	pr.setColorThreshold(150);
	vector<string> plateVec;

	int count = pr.plateRecognize(sdcard, src, plateVec);
	string str = "0";
	if (count == 0 && plateVec.size() > 0) {
		str = plateVec[0];
	}
	char *result = new char[str.length() + 1];
	strcpy(result, str.c_str());
	jbyte *by = (jbyte*) result;
	jbyteArray jarray = env->NewByteArray(strlen(result));
	env->SetByteArrayRegion(jarray, 0, strlen(result), by);
	//need
	env->ReleaseIntArrayElements(datas, cbuf, 0);
	env->ReleaseIntArrayElements(sizes, csize, 0);

	return jarray;
}
