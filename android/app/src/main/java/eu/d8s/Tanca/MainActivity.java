package eu.d8s.Tanca;

//import android.support.annotation.Keep;
import android.app.Activity;
import android.os.Bundle;
//import android.widget.TextView;

import android.webkit.WebView;
import android.webkit.WebSettings;
import android.webkit.WebChromeClient;
import android.webkit.WebViewClient;


public class MainActivity extends Activity {


    private WebView mWebView;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        int tcpPort = initialize();

        mWebView = (WebView) findViewById(R.id.activity_main_webview);
        mWebView.clearCache(true);
        mWebView.setWebChromeClient(new WebChromeClient());
        mWebView.setWebViewClient(new WebViewClient());

        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);

        mWebView.loadUrl("http://127.0.0.1:" + String.valueOf(tcpPort)); // FIXME: use the TCP port discovered just before
    }

    static {
        System.loadLibrary("tanca_server");
        System.loadLibrary("icl");
    }

    public native int initialize();
}
