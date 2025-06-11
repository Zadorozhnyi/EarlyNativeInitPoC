package com.example.poc;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i("EarlyFunctionExecution", "MainActivity()");
        super.onCreate(savedInstanceState);
    }
}