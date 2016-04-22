package com.cv.carplate;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.cv.emmp_ard.scancapture.ScanCaptureActivity;
import com.cv.openv_carplate.R;
import com.example.carplate.CarPlateDetection;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

/**
 * More info see https://github.com/liuruoze/EasyPR
 * 如果你想进行ANN训练，请参照上面的连接，将训练文件放至sdcard跟目录下
 */
public class MainActivity extends Activity {
    private Bitmap bmp = null;
    private TextView m_text = null;
    private String path = null; // SDCARD ��Ŀ¼
    private ImageView view;
    private ListView listView;
    private MAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        view = (ImageView) findViewById(R.id.image_view);
        m_text = (TextView) findViewById(R.id.myshow);
        listView = (ListView) findViewById(android.R.id.list);
        adapter = new MAdapter();
        listView.setAdapter(adapter);
        path = Environment.getExternalStorageDirectory().getAbsolutePath();
        //bmp = BitmapFactory.decodeFile(path + "/test2.jpg");
        //view.setImageBitmap(bmp);
        // System.out.println(path);
    }

    static {
        System.loadLibrary("gnustl_shared"); // added
        //System.loadLibrary("opencv_java");
        System.loadLibrary("imageproc");
    }


    public void click(View view) {
        Intent intent = new Intent(this,
                ScanCaptureActivity.class);
        intent.putExtra("index", 2);
        startActivityForResult(intent, 0);
    }

    public void click1(View view) {
        new Thread(new Runnable() {

            @Override
            public void run() {
                CarPlateDetection.annProc(path);
            }
        }).start();
    }

    public void click2(View view) {
        new Thread(new Runnable() {

            @Override
            public void run() {
                CarPlateDetection.annProcEx(path);
            }
        }).start();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        String scanCode = data == null ? null : data.getStringExtra("code");
//        String scanCode = data.getStringExtra("code");
        if (TextUtils.isEmpty(scanCode)) {
            m_text.setText("识别失败");
        } else {
            m_text.setText(scanCode);
            bmp = BitmapFactory.decodeFile(path + "/plate.jpg");
            saveBitmap(scanCode, bmp, path);
            adapter.notifyDataSetChanged();
            view.setImageBitmap(bmp);
        }
    }

    static class Result {
        public String path;
        public String result;

        private Result() {
        }

        static Result createResult(String path, String result) {
            Result r = new Result();
            r.path = path;
            r.result = result;
            return r;
        }
    }


    static ArrayList<Result> results = new ArrayList<Result>();

    public static int count = 0;

    public static void saveBitmap(String result, Bitmap bmp, String path) {
        File f = new File(path + "/test/" + result + ".jpg");
        //System.out.println("car save to : " + f.getPath());
        if (f.exists()) {
            f.delete();
        }
        try {
            FileOutputStream out = new FileOutputStream(f);
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, out);
            out.flush();
            out.close();
            results.add(Result.createResult(f.getPath(), result));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    class MAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            return results.size();
        }

        @Override
        public Object getItem(int position) {
            return results.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            Holder holder;
            if (convertView == null) {
                holder = new Holder();
                convertView = getLayoutInflater().inflate(R.layout.item, parent, false);
                holder.result = (TextView) convertView.findViewById(R.id.result);
                holder.image = (ImageView) convertView.findViewById(R.id.image);
                convertView.setTag(holder);
            } else {
                holder = (Holder) convertView.getTag();
            }
            Result result = (Result) getItem(position);
            holder.result.setText(result.result);
            holder.image.setImageBitmap(BitmapFactory.decodeFile(result.path));
            return convertView;
        }

        class Holder {
            TextView result;
            ImageView image;
        }
    }
}
