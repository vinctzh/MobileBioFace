package com.beyare.ndkopencv2;


import android.os.Bundle;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.drawable.BitmapDrawable;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

public class MainActivity extends Activity {

	ImageView imgView;  
    Button btnNDK, btnRestore;  
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        this.setTitle("使用NDK编译C++版Opencv");
        btnRestore = (Button) this.findViewById(R.id.btnRestore);  
        btnRestore.setOnClickListener(new ClickEvent());  
        btnNDK = (Button) this.findViewById(R.id.btnNDK);  
        btnNDK.setOnClickListener(new ClickEvent());  
        imgView = (ImageView) this.findViewById(R.id.ImageView01);  
    }

    class ClickEvent implements View.OnClickListener {  
        public void onClick(View v) {  
            if (v == btnNDK) {  
                long current = System.currentTimeMillis();  
                Bitmap img1 = ((BitmapDrawable) getResources().getDrawable(  
                        R.drawable.lena)).getBitmap();  
                int w = img1.getWidth(), h = img1.getHeight();  
                int[] pix = new int[w * h];
                Log.i("Test", "size"+h+" "+w);
                img1.getPixels(pix, 0, w, 0, 0, w, h);  
                int[] resultInt = new int[w * h] ;
                Log.e("Test", "begin jnicv");
                libJNICV.JNICV(pix,resultInt, w, h);
                Log.e("Test", "end jnicv");
                
                Bitmap resultImg = Bitmap.createBitmap(w, h, Config.RGB_565);  
                resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);  
                long performance = System.currentTimeMillis() - current;  
                imgView.setImageBitmap(resultImg);  
                MainActivity.this.setTitle("w:" + String.valueOf(img1.getWidth())  
                        + ",h:" + String.valueOf(img1.getHeight()) + "NDK耗时"  
                        + String.valueOf(performance) + " 毫秒");  
            } else if (v == btnRestore) {  
                Bitmap img2 = ((BitmapDrawable) getResources().getDrawable(  
                        R.drawable.lena)).getBitmap();  
                imgView.setImageBitmap(img2);  
                MainActivity.this.setTitle("使用OpenCV进行图像处理");  
            }  
        }  
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
}
