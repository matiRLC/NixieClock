package com.example.matias.nixieclockapp;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

/*******************************************************
 * CREATED ON    : 2018-06-07
 * BY            : Matias (matias.quintana.r@gmail.com)
 * DESCRIPTION   : Main activity that displays basic
 *                 information and main button
 *******************************************************/
public class MainActivity extends AppCompatActivity {
    private Button setButton;

    /**
     * OnCreate method.
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setButton = (Button) findViewById(R.id.setTimeButton);
        setButton.setOnClickListener(SetButtonListener);
    }

    /**
     *
     */
    @Override
    protected void onPause() {
        super.onPause();
    }

    /**
     *
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * doSetTime function
     * Launches next activty where user sets time
     */
    private void doSetTime() {
        startActivity(new Intent(MainActivity.this, SetTimeActivity.class));
        finish();
    }

    /*************************************  onClick Event Listeners  *************************************/

    /**
     * SubmitButtonListener.
     */
    private View.OnClickListener SetButtonListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            doSetTime();
        }
    };

    /*********************************  OS attribute controllers  *********************************/

    /**
     * This method is executed when the back button is pressed.
     */
    public void onBackPressed() {
        AlertDialog.Builder alert_confirm = new AlertDialog.Builder(MainActivity.this);
        alert_confirm.setMessage("\"No one ever says good-bye unless they want to see you again\"-TATWD \nWould you like to exit?").setCancelable(false)
                .setPositiveButton("Exit", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        ActivityCompat.finishAffinity(MainActivity.this);
                        System.exit(0);
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        return;
                    }
                });
        AlertDialog alert = alert_confirm.create();
        alert.show();
    }
}
