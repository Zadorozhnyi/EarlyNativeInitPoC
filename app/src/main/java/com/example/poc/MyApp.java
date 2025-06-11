package com.example.poc;

import android.app.Application;
import android.util.Log;

public class MyApp extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        Log.i("EarlyFunctionExecution", "MyApp Application.onCreate()");
    }
}