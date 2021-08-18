package com.sc.scplayer.player;

import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/17/21 10:35 AM
 * @last-modified-date: 8/17/21 10:35 AM
 * @description:
 */
public interface ScSeek {

    View seekCreater(@NonNull FrameLayout parent);

    void seek(int progress);



}
