// ann_train.cpp : ann???????????????????OCR??

#include <opencv2/opencv.hpp>
#include "easypr/plate_recognize.h"
#include "easypr/util.h"

#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include <vector>
#include <iostream>
#include <cstdlib>

#ifdef OS_WINDOWS
#include <io.h>
#endif

#include <dirent.h>
#include <time.h>

using namespace easypr;
using namespace cv;
using namespace std;

#define HORIZONTAL 1
#define VERTICAL 0

CvANN_MLP ann;

//?й???
const char strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', /* ???I */
		'J', 'K', 'L', 'M', 'N', /* ???O */'P', 'Q', 'R', 'S', 'T', 'U', 'V',
		'W', 'X', 'Y', 'Z' };
const int numCharacter = 34;
/* ???I??O,10????????24??????????? */

//???????????????????????????????????棬??Щ??????????????????????
//??Щ?????????2?????????????????????????Σ???????????洢
const std::string strChinese[] = { "zh_cuan" /* ?? */, "zh_e" /* ?? */,
		"zh_gan" /* ??*/, "zh_gan1" /*??*/, "zh_gui" /* ?? */,
		"zh_gui1" /* ?? */, "zh_hei" /* ?? */, "zh_hu" /* ?? */,
		"zh_ji" /* ?? */, "zh_jin" /* ?? */, "zh_jing" /* ?? */,
		"zh_jl" /* ?? */, "zh_liao" /* ?? */, "zh_lu" /* ? */,
		"zh_meng" /* ?? */, "zh_min" /* ?? */, "zh_ning" /* ?? */,
		"zh_qing" /* ?? */, "zh_qiong" /* ?? */, "zh_shan" /* ?? */,
		"zh_su" /* ?? */, "zh_sx" /* ?? */, "zh_wan" /* ?? */,
		"zh_xiang" /* ?? */, "zh_xin" /* ?? */, "zh_yu" /* ? */,
		"zh_yu1" /* ?? */, "zh_yue" /* ?? */, "zh_yun" /* ?? */,
		"zh_zang" /* ?? */, "zh_zhe" /* ?? */};

const int numChinese = 31;
const int numAll = 65;

void annTrain(Mat TrainData, Mat classes, int nNeruns) {
	ann.clear();
	Mat layers(1, 3, CV_32SC1);
	layers.at<int>(0) = TrainData.cols;
	layers.at<int>(1) = nNeruns;
	layers.at<int>(2) = numAll;
	ann.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

	// Prepare trainClases
	// Create a mat with n trained data by m classes
	Mat trainClasses;
	trainClasses.create(TrainData.rows, numAll, CV_32FC1);
	for (int i = 0; i < trainClasses.rows; i++) {
		for (int k = 0; k < trainClasses.cols; k++) {
			// If class of data i is same than a k class
			if (k == classes.at<int>(i))
				trainClasses.at<float>(i, k) = 1;
			else
				trainClasses.at<float>(i, k) = 0;
		}
	}
	Mat weights(1, TrainData.rows, CV_32FC1, Scalar::all(1));

	// Learn classifier
	// ann.train( TrainData, trainClasses, weights );

	// Setup the BPNetwork

	// Set up BPNetwork's parameters
	CvANN_MLP_TrainParams params;
	params.term_crit= TermCriteria( TermCriteria::MAX_ITER + TermCriteria::EPS, 5000, 0.01 );
	//cvTermCriteria(TermCriteria::CV_TerMCrIT_ITER+TermCriteria::CV_TERMCRIT_EPS,5000,0.01);
	params.train_method = CvANN_MLP_TrainParams::BACKPROP;
	params.bp_dw_scale = 0.1;
	params.bp_moment_scale = 0.1;

	// params.train_method=CvANN_MLP_TrainParams::RPROP;
	// params.rp_dw0 = 0.1;
	// params.rp_dw_plus = 1.2;
	// params.rp_dw_minus = 0.5;
	// params.rp_dw_min = FLT_EPSILON;
	// params.rp_dw_max = 50.;

	ann.train(TrainData, trainClasses, Mat(), Mat(), params);
}

vector<string> searchdir(const char* path) {
	vector<string> ret;
	DIR *dp;
	struct dirent *dmsg;
	int i = 0;
	char *tmpstr;
	if ((dp = opendir(path)) != NULL) {

		while ((dmsg = readdir(dp)) != NULL) {

			if (!strcmp(dmsg->d_name, ".") || !strcmp(dmsg->d_name, ".."))
				continue;
			if (dmsg->d_type == DT_DIR) {
				char *temp;
				temp = dmsg->d_name;
				if (strchr(dmsg->d_name, '.')) {
					if ((strcmp(strchr(dmsg->d_name, '.'), dmsg->d_name) == 0)) {
						continue;
					}
				}
//				LOGI("........directname:%s", dmsg->d_name);
//				ret.push_back(dmsg->d_name);
			} else {
				stringstream ss(stringstream::in | stringstream::out);
				ss << path << "/" << dmsg->d_name;
				LOGI("directname:%s", ss.str().c_str());
				ret.push_back(ss.str());
			}
		}
	} else {
		LOGE("Open Failed. %s", path);
	}
	closedir(dp);
	return ret;
}

int saveTrainData(const char* sdcard) {
	LOGI("Begin saveTrainData in %s", sdcard);
	Mat classes;
	Mat trainingDataf5;
	Mat trainingDataf10;
	Mat trainingDataf15;
	Mat trainingDataf20;

	vector<int> trainingLabels;

	for (int i = 0; i < numCharacter; i++) {
		stringstream ss(stringstream::in | stringstream::out);
		ss << sdcard << "/chars_recognise_ann/chars2/" << strCharacters[i];

		vector<string> files = searchdir(ss.str().c_str()); //Utils::getFiles(ss.str());
		LOGI(
				"Character: %c %s %d", strCharacters[i], ss.str().c_str(), files.size());
		int size = files.size();
		for (size_t j = 0; j < size; j++) {
			cout << files[j].c_str() << endl;
			Mat img = imread(files[j].c_str(), 0);
			Mat f5 = features(img, 5);
			Mat f10 = features(img, 10);
			Mat f15 = features(img, 15);
			Mat f20 = features(img, 20);

			trainingDataf5.push_back(f5);
			trainingDataf10.push_back(f10);
			trainingDataf15.push_back(f15);
			trainingDataf20.push_back(f20);
			trainingLabels.push_back(i); //??????????????????????????±?
		}
	}

	for (int i = 0; i < numChinese; i++) {
//		LOGI("Character: %s",strChinese[i]);
		stringstream ss(stringstream::in | stringstream::out);
		ss << sdcard << "/chars_recognise_ann/charsChinese/" << strChinese[i];

		vector<string> files = searchdir(ss.str().c_str()); //Utils::getFiles(ss.str());
		LOGI(
				"Character: %s %s %d", strChinese[i].c_str(), ss.str().c_str(), files.size());
		int size = files.size();
		for (int j = 0; j < size; j++) {
			cout << files[j].c_str() << endl;
			Mat img = imread(files[j].c_str(), 0);
			Mat f5 = features(img, 5);
			Mat f10 = features(img, 10);
			Mat f15 = features(img, 15);
			Mat f20 = features(img, 20);

			trainingDataf5.push_back(f5);
			trainingDataf10.push_back(f10);
			trainingDataf15.push_back(f15);
			trainingDataf20.push_back(f20);
			trainingLabels.push_back(i + numCharacter);
		}
	}

	trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
	trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
	trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
	trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);

	stringstream ssfs(stringstream::in | stringstream::out);
	ssfs << sdcard << "/chars_recognise_ann/ann_data.xml";
	FileStorage fs(ssfs.str(), FileStorage::WRITE);
	fs << "TrainingDataF5" << trainingDataf5;
	fs << "TrainingDataF10" << trainingDataf10;
	fs << "TrainingDataF15" << trainingDataf15;
	fs << "TrainingDataF20" << trainingDataf20;
	fs << "classes" << classes;
	fs.release();

	LOGI("End saveTrainData in %s", sdcard);

	return 0;
}

long getTimestampEx() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1e3 + ts.tv_nsec / 1e6);
}

void saveModel(const char* sdcard, int _predictsize, int _neurons) {
	FileStorage fs;
	stringstream ssfs(stringstream::in | stringstream::out);
	ssfs << sdcard << "/chars_recognise_ann/ann_data.xml";
	fs.open(ssfs.str(), FileStorage::READ);

	Mat TrainingData;
	Mat Classes;

	string training;
	if (1) {
		stringstream ss(stringstream::in | stringstream::out);
		ss << "TrainingDataF" << _predictsize;
		training = ss.str();
	}

	fs[training] >> TrainingData;
	fs["classes"] >> Classes;

	// train the Ann
	LOGI("Begin to saveModel : %d %d", _predictsize, _neurons);

	long start = getTimestampEx(); //Utils::getTimestampEx();
	annTrain(TrainingData, Classes, _neurons);

	long end = getTimestampEx(); //Utils::getTimestampEx();
	LOGI("Elapse: %L", (end - start) / 1000);

	LOGI("End the saveModelChar");

	stringstream ss(stringstream::in | stringstream::out);
	ss << sdcard << "/chars_recognise_ann/Fann.xml";
//	string model_name = ss.str();
	// if(1)
	//{
	//	stringstream ss(stringstream::in | stringstream::out);
	//	ss << "ann_prd" << _predictsize << "_neu"<< _neurons << ".xml";
	//	model_name = ss.str();
	//}

	FileStorage fsTo(ss.str(), cv::FileStorage::WRITE);
	ann.write(*fsTo, "ann");
}

int annMain(const char* sdcard) {
	LOGI("To be begin.");

	saveTrainData(sdcard);

	//???????????????predictSize????neurons??ANN???
	// for (int i = 2; i <= 2; i ++)
	//{
	//	int size = i * 5;
	//	for (int j = 5; j <= 10; j++)
	//	{
	//		int neurons = j * 10;
	//		saveModel(size, neurons);
	//	}
	//}

	//???????????model????????ann.xml????????????predictSize=10,neurons=40??ANN????
	//???????????????????????????10?????????????????????ɡ?
	saveModel(sdcard, 10, 40);

	LOGI("To be end.");
	return 0;
}
