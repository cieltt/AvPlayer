package com.example.ciel.AvPlayer.player;

import android.media.MediaPlayer;
import android.text.TextUtils;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;

/**
 * @author Lance
 * @date 2018/8/6
 */
public class DNPlayer implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("dnplayer");
    }

    /**
     * 播放文件路径
     */
    private String dataSource;
    private OnPrepareListener onPrepareListener;
    private SurfaceHolder surfaceHolder;
    private OnErrorListener onErrorListener;

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }


    public void setSurfaceView(SurfaceView surfaceView) {
        if (null != this.surfaceHolder) {
            this.surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        native_set_surface(surfaceHolder.getSurface());
        this.surfaceHolder.addCallback(this);
    }

    public void prepare() {
        native_prepare(dataSource);
    }


    public void onError(int errorCode) {
        if (null != onErrorListener) {
            onErrorListener.onError(errorCode);
        }
    }

    public void onPrepare() {
        if (null != onPrepareListener) {
            onPrepareListener.onPrepared();
        }
    }

    public void start() {
        native_start();
    }

    public void stop() {
        native_stop();
    }

    public void release() {
        native_release();
    }

    private native void native_start();

    private native void native_prepare(String dataSource);

    private native void native_set_surface(Surface surface);

    private native void native_stop();

    private native void native_release();


    public void setOnPrepareListener(OnPrepareListener onPrepareListener) {
        this.onPrepareListener = onPrepareListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        native_set_surface(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        System.out.println("========================================== destory");
    }


    public interface OnPrepareListener {
        void onPrepared();
    }

    public interface OnErrorListener {
        void onError(int error);
    }
}
