package com.example.carplate;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import com.cv.emmp_ard.scan.camera.CameraManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;


public class CarPlateDetection {

    public static boolean TEST = false;
    public static String sdpath = Environment.getExternalStorageDirectory().getAbsolutePath();
    public static ArrayList<String> results = new ArrayList<String>();
    public static ArrayList<IntCharPair> cps = new ArrayList<IntCharPair>();
    public final static int PRO_TIME = 10;
    public final static int CAL_TIME = 5;
    public static int counter = -1;
    public final static int RES_LENGTH = 10;
    public static int c_weight[][] = new int[PRO_TIME + 2][RES_LENGTH];
    public static int s_weight[] = new int[PRO_TIME + 2];
    //    private static final int[] indexs = new int[]{1, 2, 4, 5, 6, 7, 8};
    public static int errors = 0;


    public static String ImageProc(String path) {
        String svmpath = sdpath + "/svm.xml";
        String annpath = sdpath + "/ann.xml";
        String imgpath = path;// + "/test" + 2 + ".jpg";
        byte[] resultByte = CarPlateDetection.ImageProc(sdpath, imgpath,
                svmpath, annpath);
        String result = null;
        try {
            result = new String(resultByte, "utf-8");
            System.out.println("ImageProc:" + result);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        if (result != null && result.length() >= RES_LENGTH) {
//            return result;
            return ProByhx(result);
//        } else if (result != null) {
//            errors++;
//            if (errors > 15) {
//                errors = 0;
//            }
//            return null;
        } else {
            return null;
        }
    }

    private static String ProByhx(String result) {
        // TODO Auto-generated method stub


//        if (counter < CAL_TIME) {
        if (counter == -1) {
            for (int i = 0; i < PRO_TIME + 2; i++) {
                for (int j = 0; j < RES_LENGTH; j++)
                    c_weight[i][j] = 0;
            }
        }

        results.add(result);
        counter++;

        for (int i = 0; i < counter; i++) {
            String temp = results.get(i);

            for (int j = 0; j < cps.size(); j++) {
                IntCharPair temPair = cps.get(j);
                if (temPair != null && temPair.m_char == temp.charAt(temPair.m_int)) {
                    c_weight[counter][temPair.m_int] = c_weight[counter][temPair.m_int] + 2;
                }
            }
            for (int j = 0; j < RES_LENGTH; j++) {
                if (result.charAt(j) == temp.charAt(j)) {
                    c_weight[i][j]++;
                    c_weight[counter][j]++;
                }
            }
        }


        if (counter < CAL_TIME - 1) {
            return null;
        }

        for (int i = 0; i < CAL_TIME; i++) {
            for (int j = 0; j < RES_LENGTH; j++)
                if (c_weight[i][j] > c_weight[PRO_TIME][j]) {
                    c_weight[PRO_TIME][j] = c_weight[i][j];
                    c_weight[PRO_TIME + 1][j] = i;
                }
        }


        boolean flag = false;
        cps.clear();
        for (int i = 0; i < RES_LENGTH; i++) {
            if (c_weight[PRO_TIME][i] < 2) {
                flag = true;
            } else {
                cps.add(new IntCharPair(i, results.get(c_weight[PRO_TIME + 1][i]).charAt(i)));
            }
        }
        if (flag) {
            counter = -1;
            results.clear();
            CameraManager.get().toast("failed scan again!");
            return null;
        }
        char c_result[] = new char[RES_LENGTH];

        for (int i = 0; i < RES_LENGTH; i++) {
            c_result[i] = results.get(c_weight[PRO_TIME + 1][i]).charAt(i);


        }

        counter = -1;
        results.clear();

        String ret = String.valueOf(c_result);
        return ret;

    }

//    public static String ImageProc(int[] datas, int[] sizes) {
//        String svmpath = sdpath + "/svm.xml";
//        String annpath = sdpath + "/Fann.xml";
//        byte[] resultByte = CarPlateDetection.ImageProc(datas, sizes, sdpath,
//                svmpath, annpath);
//        String result = null;
//        try {
//            result = new String(resultByte, "utf-8");
//            System.out.println(result);
//        } catch (UnsupportedEncodingException e) {
//            e.printStackTrace();
//        }
//        if (result != null && result.length() > 5) {
//            return result;
//        } else {
//            return null;
//        }
//    }

    static {
        System.loadLibrary("gnustl_shared"); // added
//		System.loadLibrary("opencv_java");
        System.loadLibrary("imageproc");
    }

    public static native byte[] ImageProc(String sdpath, String imgpath,
                                          String svmpath, String annpath);

    public static native byte[] ImageProc(int[] datas, int[] sizes, String sdpath,
                                          String svmpath, String annpath);

    public static native int annProc(String path);

    public static native int annProcEx(String path);


    public static void copyXml(Context context) {
        copyFilesFassets(context, "svm.xml", sdpath + "/svm.xml");
        copyFilesFassets(context, "ann.xml", sdpath + "/ann.xml");
    }

    /**
     *
     */
    private static void copyFilesFassets(Context context, String oldPath, String newPath) {
        if (new File(newPath).exists()) return;
        Log.w("cv", "copyFilesFassets: from " + oldPath + " to " + newPath);
        try {
            InputStream is = context.getAssets().open(oldPath);
            FileOutputStream fos = new FileOutputStream(new File(newPath));
            byte[] buffer = new byte[1024];
            int byteCount = 0;
            while ((byteCount = is.read(buffer)) != -1) {
                fos.write(buffer, 0, byteCount);
            }
            fos.flush();
            is.close();
            fos.close();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

}
