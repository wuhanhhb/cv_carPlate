/*
 * Copyright (C) 2008 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.cv.emmp_ard.scan.view;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;

import com.cv.emmp_ard.scancapture.ScanCaptureActivity;
import com.cv.scancapture.R;
import com.google.zxing.ResultPoint;
import com.cv.emmp_ard.scan.camera.CameraManager;

import java.util.Collection;
import java.util.HashSet;

/**
 * This view is overlaid on top of the camera preview. It adds the viewfinder
 * rectangle and partial transparency outside it, as well as the laser scanner
 * animation and result points.
 *
 * @author dswitkin@google.com (Daniel Switkin)
 */
public final class ViewfinderView extends View {

    private static final int[] SCANNER_ALPHA = {0, 64, 128, 192, 255, 192,
            128, 64};
    private static final long ANIMATION_DELAY = 100L;
    private static final int OPAQUE = 0xFF;

    private final Paint paint;
    private Bitmap resultBitmap;
    private final int maskColor;
    private final int resultColor;
    private final int frameColor;
    private final int laserColor;
    private final int resultPointColor;
    private int scannerAlpha;
    private Collection<ResultPoint> possibleResultPoints;
    private Collection<ResultPoint> lastPossibleResultPoints;
    private int width, height;
    private final boolean NEED_FRAME = false;
    private static int OFFSET = 0;
    private Context context;
    float density;

    // This constructor is used when the class is built from an XML resource.
    public ViewfinderView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // Initialize these once for performance rather than calling them every
        // time in onDraw().
        this.context = context;
        paint = new Paint();
        paint.setAntiAlias(true);
        Resources resources = getResources();
        maskColor = resources.getColor(R.color.viewfinder_mask);
        resultColor = resources.getColor(R.color.result_view);
        frameColor = resources.getColor(R.color.viewfinder_frame);
        laserColor = resources.getColor(R.color.viewfinder_laser);
        resultPointColor = resources.getColor(R.color.possible_result_points);
        scannerAlpha = 0;
        possibleResultPoints = new HashSet<ResultPoint>(5);
        width = height = 0;
        DisplayMetrics dm = new DisplayMetrics();
        dm = getResources().getDisplayMetrics();

        density = dm.density;
    }

    @Override
    public void onDraw(Canvas canvas) {
        if (CameraManager.get() == null) {
            return;
        }
        Log.d("onDraw", "onDraw");
        Rect frame = CameraManager.get().getFramingRect();
        if (frame == null) {
            return;
        }
        final int step = ((int) (density + 0.5) * frame.height()) >> 4;
        if (OFFSET == 0 || OFFSET > frame.height() + frame.top - 3) {
            OFFSET = frame.top + 1;
        } else {
            OFFSET += step;
        }
        // int width = canvas.getWidth();
        // int height = canvas.getHeight();
        // int height = canvas.getWidth();
        // int width = canvas.getHeight();

        // Draw the exterior (i.e. outside the framing rect) darkened
        // paint.setColor(resultBitmap != null ? resultColor : maskColor);
        // canvas.drawRect(0, 0, width, frame.top, paint);
        // canvas.drawRect(0, frame.top, frame.left, frame.bottom + 1, paint);
        // canvas.drawRect(frame.right + 1, frame.top, width, frame.bottom + 1,
        // paint);
        // canvas.drawRect(0, frame.bottom + 1, width, height, paint);
        paint.setColor(maskColor);


        if (width == 0) {
            width = canvas.getWidth();
            height = canvas.getHeight();
        }
        if (width > 0 && height > 0) {
            // top & bottom
            canvas.drawRect(0, 0, width, frame.top, paint);
            canvas.drawRect(0, frame.bottom + 1, width, height, paint);
            // left & right
            canvas.drawRect(0, frame.top, frame.left, frame.bottom + 1, paint);
            canvas.drawRect(frame.right + 1, frame.top, width,
                    frame.bottom + 1, paint);
        }
        paint.setColor(Color.GREEN);

        canvas.drawRect(frame.left, frame.top, frame.left + 15, frame.top + 5,
                paint);

        canvas.drawRect(frame.left, frame.top, frame.left + 5, frame.top + 15,
                paint);


        canvas.drawRect(frame.right - 15, frame.top, frame.right,
                frame.top + 5, paint);

        canvas.drawRect(frame.right - 5, frame.top, frame.right,
                frame.top + 15, paint);


        canvas.drawRect(frame.left, frame.bottom - 5, frame.left + 15,
                frame.bottom, paint);

        canvas.drawRect(frame.left, frame.bottom - 15, frame.left + 5,
                frame.bottom, paint);


        canvas.drawRect(frame.right - 15, frame.bottom - 5, frame.right,
                frame.bottom, paint);

        canvas.drawRect(frame.right - 5, frame.bottom - 15, frame.right,
                frame.bottom, paint);

        if (resultBitmap != null) {
            // Draw the opaque result bitmap over the scanning rectangle
            paint.setAlpha(OPAQUE);
            canvas.drawBitmap(resultBitmap, frame.left, frame.top, paint);
        } else {

            // Draw a two pixel solid black border inside the framing rect
            if (NEED_FRAME) {
                paint.setColor(frameColor);
                canvas.drawRect(frame.left, frame.top, frame.right + 1,
                        frame.top + 2, paint);
                canvas.drawRect(frame.left, frame.top + 2, frame.left + 2,
                        frame.bottom - 1, paint);
                canvas.drawRect(frame.right - 1, frame.top, frame.right + 1,
                        frame.bottom - 1, paint);
                canvas.drawRect(frame.left, frame.bottom - 1, frame.right + 1,
                        frame.bottom + 1, paint);
            }
            // Draw a red "laser scanner" line through the middle to show
            // decoding is active
            paint.setColor(laserColor);
//            paint.setAlpha(SCANNER_ALPHA[scannerAlpha]);
//            scannerAlpha = (scannerAlpha + 1) % SCANNER_ALPHA.length;
            //int middle = frame.height() / 2 + frame.top;
//            canvas.drawRect(frame.left + 1, OFFSET - 1, frame.right - 1,
//                    OFFSET + 2, paint);
            RectF rect = new RectF(frame.left + 1, OFFSET - 1, frame.right - 1, OFFSET + 1);
            canvas.drawOval(rect, paint);
            //draw tip
            paint.setColor(Color.WHITE);
            paint.setTextSize(18 * density);
            int left = (int) ((frame.left + frame.right - paint.measureText(ScanCaptureActivity.des)) / 2);
            canvas.drawText(ScanCaptureActivity.des, left, frame.bottom + 80, paint);

            Collection<ResultPoint> currentPossible = possibleResultPoints;
            Collection<ResultPoint> currentLast = lastPossibleResultPoints;
            if (currentPossible.isEmpty()) {
                lastPossibleResultPoints = null;
            } else {
                possibleResultPoints = new HashSet<ResultPoint>(5);
                lastPossibleResultPoints = currentPossible;
                paint.setAlpha(OPAQUE);
                paint.setColor(resultPointColor);
                for (ResultPoint point : currentPossible) {
                    canvas.drawCircle(frame.left + point.getX(), frame.top
                            + point.getY(), 6.0f, paint);
                }
            }
            if (currentLast != null) {
                paint.setAlpha(OPAQUE / 2);
                paint.setColor(resultPointColor);
                for (ResultPoint point : currentLast) {
                    canvas.drawCircle(frame.left + point.getX(), frame.top
                            + point.getY(), 3.0f, paint);
                }
            }
            // Request another update at the animation interval, but only
            // repaint the laser line,
            // not the entire viewfinder mask.
            postInvalidateDelayed(ANIMATION_DELAY, frame.left, frame.top,
                    frame.right, frame.bottom);
        }
    }

    public void drawViewfinder() {
        resultBitmap = null;
        invalidate();
    }

    /**
     * Draw a bitmap with the result points highlighted instead of the live
     * scanning display.
     *
     * @param barcode An image of the decoded barcode.
     */
    public void drawResultBitmap(Bitmap barcode) {
        resultBitmap = barcode;
        invalidate();
    }

    public void addPossibleResultPoint(ResultPoint point) {
        possibleResultPoints.add(point);
    }

}
