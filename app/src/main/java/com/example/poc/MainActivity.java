package com.example.poc;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

    // JNI method declaration
    public native boolean isDeviceRooted();

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i("EarlyFunctionExecution", "MainActivity()");
        super.onCreate(savedInstanceState);

        boolean detected = isDeviceRooted();
        Log.i("EarlyFunctionExecution", "Root check (JNI): " + (detected ? "DETECTED" : "NOT detected"));

        if (detected) {
            new AlertDialog.Builder(this)
                .setTitle("Root Warning")
                .setMessage("playIntegrityFix was detected on this device.")
                .setCancelable(false)
                .setPositiveButton("OK", null)
                .show();
        }
    }
}