#include "easypr/chars_recognise.h"
#include "easypr/util.h"
#include "easypr/grobal.h"
//#include <android/log.h>
//#include <string>
//#define LOG_TAG "System.out"
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

CCharsRecognise::CCharsRecognise() {
  m_charsSegment = new CCharsSegment();
  m_charsIdentify = new CCharsIdentify();
}

CCharsRecognise::~CCharsRecognise() {
  SAFE_RELEASE(m_charsSegment);
  SAFE_RELEASE(m_charsIdentify);
}

void CCharsRecognise::LoadANN(string s) {
  m_charsIdentify->LoadModel(s.c_str());
}

string CCharsRecognise::charsRecognise(Mat plate) {
  return m_charsIdentify->charsIdentify(plate);
}

void CCharsRecognise::charsRecogniseEx(Mat plate, string& name) {
      vector<Mat> matVec;
      //LOGE("load begin 4");
      int result = m_charsSegment->charsSegment(plate, matVec);
      //LOGE("load begin 5");
      if (result == 0) {
          int num = matVec.size();
          for (int j = 0; j < num; j++) {
            Mat charMat = matVec[j];
            //bool isChinses = false;
            //bool isSpeci = false;

            //默认首个字符块是中文字符
            //if (j == 0) isChinses = true;
            //if (j == 1) isSpeci = true;

            stringstream ss(stringstream::in | stringstream::out);
            ss << grobal_sdcard << "/chars/" << name << "_" << j << ".jpg";
            imwrite(ss.str(), charMat);
          }
      }
}
/*
string& trim(string &s)
{
    int begin = 0;

    begin = s.find(" ",begin);  //查找空格在str中第一次出现的位置

    while(begin != -1)  //表示字符串中存在空格
    {
        s.replace(begin, 1, "");  // 用空串替换str中从begin开始的1个字符
        begin = s.find(" ",begin);  //查找空格在替换后的str中第一次出现的位置
    }
    return s;
}*/

int CCharsRecognise::charsRecognise(Mat plate, string& plateLicense, int index) {
  //车牌字符方块集合
  vector<Mat> matVec;

  string plateIdentify = "";

  int result = m_charsSegment->charsSegment(plate, matVec, index);

  int num = matVec.size();
  if(num < 7){
      return -1;
  }
  if (result == 0) {
    for (int j = 0; j < num; j++) {
      Mat charMat = matVec[j];
      bool isChinses = false;
      bool isSpeci = false;

      //默认首个字符块是中文字符
      if (j == 0) isChinses = true;
      if (j == 1) isSpeci = true;

      string charcater =
          m_charsIdentify->charsIdentify(charMat, isChinses, isSpeci);
      //LOGE("charsRecognise charcater > %s",charcater.c_str());
      plateIdentify = plateIdentify + charcater;
    }
  }

  plateLicense = plateIdentify;

  /*if (plateLicense.size() < 7) {
    return -1;
  }else{
    plateLicense = trim(plateIdentify);
    if (num < 7) {
        LOGE("========================= charsRecognise again begin > %d",num);
        plateIdentify = "";
        bool first = false;
        for (int j = 0; j < num; j++) {
            Mat charMat = matVec[j];
            bool isChinses = false;
            bool isSpeci = false;

            //默认首个字符块是中文字符
            if (j == 0) isChinses = true;
            if (j == 1 && !first) isSpeci = true;

            LOGE("========================= charsRecognise try find > %d %d",j,charMat.cols);

            if(charMat.cols > 30){
                //LOGE("========================= charsRecognise again find > %d %d",j,charMat.cols);
                int width = charMat.cols >> 1;
                Rect r1(0, 0, width, charMat.rows);
                Rect r2(width, 0, width, charMat.rows);
                Mat ax1(charMat,r1);
                Mat ax2(charMat,r2);
                if(j == 0){
                    first = true;
                    string c1 = m_charsIdentify->charsIdentify(ax1, true, false);
                    string c2 = m_charsIdentify->charsIdentify(ax2, false, true);
                    plateIdentify = plateIdentify + c1 + c2;
                }else if(j == 1){
                    string c1 = m_charsIdentify->charsIdentify(ax1, false, true);
                    string c2 = m_charsIdentify->charsIdentify(ax2, false, false);
                    plateIdentify = plateIdentify + c1 + c2;
                }else{
                    string c1 = m_charsIdentify->charsIdentify(ax1, false, false);
                    string c2 = m_charsIdentify->charsIdentify(ax2, false, false);
                    plateIdentify = plateIdentify + c1 + c2;
                }
            }else{
                string charcater = m_charsIdentify->charsIdentify(charMat, isChinses, isSpeci);
                plateIdentify = plateIdentify + charcater;
            }
        }
        plateLicense = plateIdentify;
        LOGE("========================= charsRecognise again end > %s",plateLicense.c_str());
    }else{
        LOGE("========================= charsRecognise > %s",plateLicense.c_str());
    }*/
//  }

  return result;
}

} /*! \namespace easypr*/
