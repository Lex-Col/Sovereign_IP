package com.sipv;

import android.app.*;
import android.content.*;
import android.content.pm.ServiceInfo;
import android.os.*;
import android.util.Log;

public class OracleWrapper extends Service {
    private static final String TAG = "SIPV_Oracle";
    private static final String CHANNEL_ID = "oracle_heartbeat";
    private static final int NOTIF_ID = 1337;

    static {
        System.loadLibrary("oracle_core");
    }

    private native void nativeStartEngine();
    private native void nativeStopEngine();

    @Override
    public void onCreate() {
        super.onCreate();
        createNotificationChannel();
        
        Notification notification = new Notification.Builder(this, CHANNEL_ID)
                .setContentTitle("SIPV: ONLINE")
                .setContentText("Oracle Engine Resident")
                .setSmallIcon(android.R.drawable.ic_lock_idle_lock)
                .build();

        startForeground(NOTIF_ID, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_REMOTE_MESSAGING);
        Log.i(TAG, "Ghost-Oracle initialized. OOM_ADJ should be dropping.");
        new Thread(this::nativeStartEngine).start();
    }

    private void createNotificationChannel() {
        NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID, "SIPV Oracle", NotificationManager.IMPORTANCE_MIN);
        getSystemService(NotificationManager.class).createNotificationChannel(channel);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) { return null; }

    @Override
    public void onDestroy() {
        nativeStopEngine();
        super.onDestroy();
    }
}
