package com.sc.scplayer.player;

import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.SeekBar;

import androidx.annotation.NonNull;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/17/21 10:35 AM
 * @last-modified-date: 8/17/21 10:35 AM
 * @description:
 */
public class ScSeekImpl implements ScSeek, SeekBar.OnSeekBarChangeListener {

    protected SeekBar seekBar;

    @Override
    public View seekCreater(@NonNull FrameLayout parent) {
        seekBar = new SeekBar(parent.getContext());
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 20);
        lp.gravity = Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
        parent.addView(seekBar, lp);
        seekBar.setOnSeekBarChangeListener(this);
        return seekBar;
    }

    @Override
    public void seek(int progress) {
        native_seek(progress);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        native_seek(seekBar.getProgress());
    }


    protected native void native_seek(int progress);
}
