package com.sc.scplayer.player;

import android.content.Context;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;

import java.util.Map;

/**
 * @author: SuperChen
 * @last-modifier: SuperChen
 * @version: 1.0
 * @create-date: 7/29/21 3:23 PM
 * @last-modified-date: 7/29/21 3:23 PM
 * @description:
 */
public class ScPlayer extends FrameLayout implements ScLifecycler, ScController, ScController.Callback,ScSeek {

    private ScControllerImpl controller;


    public ScPlayer(Context context) {
        this(context, null);
    }

    public ScPlayer(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public ScPlayer(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(attrs, defStyleAttr);
    }

    private void init(AttributeSet attrs, int defStyleAttr) {
        SurfaceView surfaceView = new SurfaceView(getContext());
        addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, (int) dp2Px(200)));
        controller = new ScControllerImpl(surfaceView);
        controller.addCallback(this);
        seekCreater(this);
    }

    public void setUrl(String urlOrPath) {
        controller.setUrl(urlOrPath);
    }

    @Override
    public void statusHasChanged(int newStatus) {
        controller.statusHasChanged(newStatus);
    }

    @Override
    public int getStatus() {
        return controller.getStatus();
    }

    @Override
    public void startPlay() {
        controller.startPlay();
    }

    @Override
    public void stopPlay() {
        controller.stopPlay();
    }

    @Override
    public void resumePlay() {
        controller.resumePlay();
    }

    @Override
    public void pausePlay() {
        controller.pausePlay();
    }

    @Override
    public void resumePlayByUser() {
        controller.resumePlayByUser();
    }

    @Override
    public void pausePlayByUser() {
        controller.pausePlayByUser();
    }

    @Override
    public void release() {
        controller.release();
    }

    @Override
    public void forward() {
        controller.forward();
    }

    @Override
    public void rewind() {
        controller.rewind();
    }

    @Override
    public void doubleSpeed(float doubleB) {
        controller.doubleSpeed(doubleB);
    }

    @Override
    public void mute(boolean mute) {
        controller.mute(mute);
    }

    @Override
    public void showSubTitle(boolean show) {
        controller.showSubTitle(show);
    }

    @Override
    public <S extends ScSeek> void setSeekController(S seekController) {
        controller.setSeekController(seekController);
    }

    @Override
    public <S extends ScSeek> S getSeekController() {
        return controller.getSeekController();
    }

    @Override
    public void addCallback(Callback callback) {
        controller.addCallback(callback);
    }

    @Override
    public void removeCallback(Callback callback) {
        controller.removeCallback(callback);
    }

    @Override
    public View seekCreater(@NonNull FrameLayout parent) {
        return controller.seekCreater(parent);
    }

    @Override
    public void seek(int progress) {
        controller.seek(progress);
    }

    @Override
    public void videoInfoGetSuccess(Map<String, Object> videoInfo) {

    }

    @Override
    public void progressChanged(int progress, boolean seekDrag) {

    }

    @Override
    public void progressChangedBySeek(int progress) {

    }

    @Override
    public void exception(int errorCode, String errorDescription) {

    }


    private float dp2Px(int dp) {
        return getResources().getDisplayMetrics().density * 1.5f;
    }
}
