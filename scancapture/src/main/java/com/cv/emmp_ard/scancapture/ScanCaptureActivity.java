package com.cv.emmp_ard.scancapture;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.os.Bundle;
import android.os.Handler;
import android.os.Vibrator;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.cv.emmp_ard.scan.decoding.CaptureActivityHandler;
import com.cv.scancapture.R;
import com.example.carplate.CarPlateDetection;
import com.google.zxing.BarcodeFormat;
import com.google.zxing.Result;
import com.cv.emmp_ard.scan.camera.CameraManager;
import com.cv.emmp_ard.scan.camera.FlashlightManager;
import com.cv.emmp_ard.scan.decoding.InactivityTimer;
import com.cv.emmp_ard.scan.view.ViewfinderView;

import java.io.IOException;
import java.util.Vector;

public class ScanCaptureActivity extends Activity implements Callback, SensorEventListener {

    private String Tag = "ScanCaptureActivity";
    private CaptureActivityHandler handler;
    private ViewfinderView viewfinderView;
    private boolean hasSurface;
    private Vector<BarcodeFormat> decodeFormats;
    private String characterSet;
    private TextView txtResult, tilte, erweima, tiaoxingma;
    ImageView back;
    private InactivityTimer inactivityTimer;
    private MediaPlayer mediaPlayer;
    private boolean playBeep;
    private static final float BEEP_VOLUME = 0.10f;
    private boolean vibrate;
    // ----@phj-------
    private Button openLigth;
    private Boolean LigthOpen = false;

    public static String des;

    private SensorManager mSensorManager;
    private Sensor mLightSensor;
    // ------------------

    public String getDes() {
        return des;
    }

    public void setDes(String des) {
        this.des = des;
    }

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.scan_main);
        CameraManager.init(getApplication());

        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mLightSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT);// TYPE_GRAVITY
        final int type;
        if (getIntent().hasExtra("index")) {
            type = getIntent().getIntExtra("index", 3);
        } else {
            type = 4;
        }
        CarPlateDetection.TEST = type == 2;
        CarPlateDetection.copyXml(this);
//        CarPlateDetection.sdpath = getIntent().getStringExtra("path");
        des = "scan car plate";
        CameraManager.get().setScanMode(CameraManager.SCAN_MODEL.CARPLATE);

        viewfinderView = (ViewfinderView) findViewById(R.id.viewfinder_view);
        tilte = (TextView) findViewById(R.id.mainTitle);
        back = (ImageView) findViewById(R.id.back);
        back.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                ScanCaptureActivity.this.finish();
            }
        });
        tilte.setText("scan");
        txtResult = (TextView) findViewById(R.id.txtResult);
        erweima = (TextView) findViewById(R.id.erweima);
        tiaoxingma = (TextView) findViewById(R.id.tiaoxingma);
//		tishiText = (TextView) findViewById(R.id.tishi_text);
        hasSurface = false;
        inactivityTimer = new InactivityTimer(this);

        openLigth = (Button) findViewById(R.id.button_ligth);
        openLigth.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (LigthOpen) {
                    FlashlightManager.disableFlashlight();
                    LigthOpen = false;
                    openLigth.setText("open flash light");
                } else {
                    FlashlightManager.enableFlashlight();
                    LigthOpen = true;
                    openLigth.setText("close flash light");
                }

            }
        });
        // -------------

        erweima.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                des = "s";
                Drawable foucs, normal;
                Resources res = getResources();
                foucs = res.getDrawable(R.drawable.erweima_img_foucs);
                foucs.setBounds(0, 0, foucs.getMinimumWidth(),
                        foucs.getMinimumHeight());
                normal = res.getDrawable(R.drawable.tiaoxingma_img_normal);
                normal.setBounds(0, 0, normal.getMinimumWidth(),
                        normal.getMinimumHeight());
                erweima.setCompoundDrawables(null, foucs, null, null);
                tiaoxingma.setCompoundDrawables(null, normal, null, null);
                CameraManager.get()
                        .setScanMode(CameraManager.SCAN_MODEL.QRCODE);
            }
        });
        tiaoxingma.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                des = "s";
                Drawable foucs, normal;
                Resources res = getResources();
                foucs = res.getDrawable(R.drawable.tiaoxingma_img_foucs);
                foucs.setBounds(0, 0, foucs.getMinimumWidth(),
                        foucs.getMinimumHeight());
                normal = res.getDrawable(R.drawable.erweima_img_normal);
                normal.setBounds(0, 0, normal.getMinimumWidth(),
                        normal.getMinimumHeight());
                erweima.setCompoundDrawables(null, normal, null, null);
                tiaoxingma.setCompoundDrawables(null, foucs, null, null);
                CameraManager.get().setScanMode(
                        CameraManager.SCAN_MODEL.BARCODE);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mSensorManager != null) {
            mSensorManager.registerListener(this,
                    mLightSensor,
                    //mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                    SensorManager.SENSOR_DELAY_NORMAL);
        }
        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.preview_view);
        SurfaceHolder surfaceHolder = surfaceView.getHolder();
        if (hasSurface) {
            initCamera(surfaceHolder);
        } else {
            surfaceHolder.addCallback(this);
            surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        }
        decodeFormats = null;
        characterSet = null;
        playBeep = true;
        AudioManager audioService = (AudioManager) getSystemService(AUDIO_SERVICE);
        if (audioService.getRingerMode() != AudioManager.RINGER_MODE_NORMAL) {
            playBeep = false;
        }
        // initBeepSound();
        vibrate = true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mSensorManager != null) {
            mSensorManager.unregisterListener(this);
        }
        if (handler != null) {
            handler.quitSynchronously();
            handler = null;
        }
        CameraManager.get().closeDriver();
    }

    @Override
    protected void onDestroy() {
        inactivityTimer.shutdown();
        super.onDestroy();
    }

    private void initCamera(SurfaceHolder surfaceHolder) {
        try {
            CameraManager.get().openDriver(surfaceHolder);
        } catch (IOException ioe) {
            return;
        } catch (RuntimeException e) {
            return;
        }
        if (handler == null) {
            handler = new CaptureActivityHandler(this, decodeFormats,
                    characterSet);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height) {
        Log.i("hx", "Calculated surfaceChanged;" + width + "," + height);
        CameraManager.get().setSurfaceSize(width, height);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (!hasSurface) {
            hasSurface = true;
            initCamera(holder);
        }

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        hasSurface = false;

    }

    public ViewfinderView getViewfinderView() {
        return viewfinderView;
    }

    public Handler getHandler() {
        return handler;
    }

    public void drawViewfinder() {
        viewfinderView.drawViewfinder();

    }

    public void handleDecode(Result obj, Bitmap barcode) {
        inactivityTimer.onActivity();
        viewfinderView.drawResultBitmap(barcode);
        playBeepSoundAndVibrate();
        txtResult.setText(obj.getBarcodeFormat().toString() + ":"
                + obj.getText());
        Log.i(Tag, "--->" + obj.getText() + "type:" + obj.getBarcodeFormat());
        Intent intent = new Intent();
        intent.putExtra("code", obj.getText());
        intent.putExtra("type", obj.getBarcodeFormat().toString());
        setResult(RESULT_OK, intent);
        finish();
    }

    public void handleDecode(String obj, Bitmap barcode) {
        inactivityTimer.onActivity();
        viewfinderView.drawResultBitmap(barcode);
        playBeepSoundAndVibrate();
        txtResult.setText(obj + ":" + obj);

        Log.i(Tag, "result --->" + obj);
        Intent intent = new Intent();
        intent.putExtra("code", obj);
        intent.putExtra("type", obj);
        setResult(RESULT_OK, intent);
        finish();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
            inactivityTimer.onActivity();
            Intent intent = new Intent();
            intent.putExtra("code", "");
            setResult(RESULT_OK, intent);
            CameraManager.get().closeDriver();
            finish();
            onDestroy();
            return false;
        } else {
            return super.onKeyDown(keyCode, event);
        }
    }


    private static final long VIBRATE_DURATION = 200L;

    private void playBeepSoundAndVibrate() {
        if (playBeep && mediaPlayer != null) {
            mediaPlayer.start();
        }
        if (vibrate) {
            Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
            vibrator.vibrate(VIBRATE_DURATION);
        }
    }

    /**
     * When the beep has finished playing, rewind to queue up another one.
     */
    private final OnCompletionListener beepListener = new OnCompletionListener() {
        public void onCompletion(MediaPlayer mediaPlayer) {
            mediaPlayer.seekTo(0);
        }
    };

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {

        } else if (event.sensor.getType() == Sensor.TYPE_LIGHT) {
            float light = event.values[0];
            System.out.println("onSensorChanged(TYPE_LIGHT):" + light);
            if (light < (SensorManager.LIGHT_CLOUDY / 4)) {
                FlashlightManager.enableFlashlight();
                LigthOpen = true;
                CameraManager.get().toast("open flash light");
            } else {
                FlashlightManager.disableFlashlight();
                LigthOpen = false;
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}