package com.example.poc;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.util.Log;

public class EarlyInitProvider extends ContentProvider {
    static {
        System.loadLibrary("native-lib");
        Log.i("EarlyFunctionExecution", "native-lib loaded via ContentProvider");
    }

    @Override
    public boolean onCreate() {
        Log.i("EarlyFunctionExecution", "onCreate()");
        return true;
    }

    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder) { return null; }
    public String getType(Uri uri) { return null; }
    public Uri insert(Uri uri, ContentValues values) { return null; }
    public int delete(Uri uri, String selection, String[] selectionArgs) { return 0; }
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) { return 0; }
}