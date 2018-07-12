package eu.d8s.Tanca;

//import android.support.annotation.Keep;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
//import android.widget.TextView;

import android.webkit.WebView;
import android.webkit.WebChromeClient;
import android.webkit.WebViewClient;

public class MainActivity extends AppCompatActivity {


    private WebView mWebView;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initialize();

        mWebView = (WebView) findViewById(R.id.activity_main_webview);
        mWebView.clearCache(true);
        mWebView.setWebChromeClient(new WebChromeClient());
        mWebView.setWebViewClient(new WebViewClient());
        mWebView.getSettings().setJavaScriptEnabled(true);
        mWebView.loadUrl("http://127.0.0.1:8000");
    }

/*
    int hour = 0;
    int minute = 0;
    int second = 0;
    TextView tickView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tickView = (TextView) findViewById(R.id.tickView);
        initialize();
    }
    @Override
    public void onResume() {
        super.onResume();
        hour = minute = second = 0;
        ((TextView)findViewById(R.id.hellojniMsg)).setText(stringFromJNI());
        startTicks();
    }

    @Override
    public void onPause () {
        super.onPause();
        StopTicks();
    }


    // A function calling from JNI to update current timer
    @Keep
    private void updateTimer() {
        ++second;
        if(second >= 60) {
            ++minute;
            second -= 60;
            if(minute >= 60) {
                ++hour;
                minute -= 60;
            }
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String ticks = "" + MainActivity.this.hour + ":" +
                        MainActivity.this.minute + ":" +
                        MainActivity.this.second;
                MainActivity.this.tickView.setText(ticks);
            }
        });
    }
    */

    static {
        System.loadLibrary("tanca_server");
        System.loadLibrary("icl");
    }



    public native  String stringFromJNI();
    public native void startTicks();
    public native void StopTicks();
    public native void initialize();
}
