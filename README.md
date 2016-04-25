# info
    
    a chinese car plate recognise(base on https://github.com/liuruoze/EasyPR Version 1.3)

# how to Use it
    
    compile 'com.cv:carplate:1.0.1'
    
code in activity:
    
    startActivityForResult(new Intent(this, ScanCaptureActivity.class), 0)
    
onActivityResult:

    String scanCode = data == null ? null : data.getStringExtra("code");//Get result
    Bitmap bmp = BitmapFactory.decodeFile( Environment.getExternalStorageDirectory().getAbsolutePath() + "/plate.jpg");//get bitmap
    
    
more info see Simple Demo
    
    
# Ndk build

if you want to build it , you need:

    1   install:OpenCV-2.4.10-android-sdk,and modified the opencv's path on Android.mk
    2   modified the scancapture's build.gradle(A simple way is replace it with build_ndk.gradle)
    3   modified the ndk path on gradle.properties & local.properties
    4   run it~
    