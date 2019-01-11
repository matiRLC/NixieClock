package com.example.matias.nixieclockapp;

import android.app.Activity;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

/*******************************************************
 * CREATED ON    : 2018-12-29
 * BY            : Matias (matias.quintana.r@gmail.com)
 * DESCRIPTION   : Activity where specific hour and
 *                 minutes are selected and set to
 *                 Nixie Clock
 *******************************************************/

public class SetTimeActivity extends AppCompatActivity {
    private Button sendButton;
    private EditText mHour;
    private EditText mMinutes;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothSocket btSocket = null;
    private Set<BluetoothDevice> pairedDevices;
    private BluetoothDevice hc05 = null;
    private final UUID portUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); //Serial Port Service ID
    private boolean connectSuccess = true;
    private OutputStream outputStream;


    /**
     * OnCreate method.
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settime);

        sendButton= (Button) findViewById(R.id.sendTimeButton);
        sendButton.setOnClickListener(SetButtonListener);
        mHour = (EditText) findViewById(R.id.setHour);
        mHour.setOnClickListener(EditTextListenerHour);
        mMinutes = (EditText) findViewById(R.id.setMin);
        mMinutes.setOnClickListener(EditTextListenerMinutes);

        /* Hide soft keyboard */
        hideSoftKeyboard(findViewById(android.R.id.content));

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (mBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(), "Bluetooth device not available", Toast.LENGTH_SHORT).show();
            finish();
        } else if (!mBluetoothAdapter.isEnabled()) {
            Intent turnBTon = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnBTon, 1);
        }

        // Get the list of the already paired devices
        pairedDevicesList();

        connectSuccess = true;
        try {
            btSocket = hc05.createInsecureRfcommSocketToServiceRecord(portUUID);
            btSocket.connect();
        } catch (IOException e) {
            e.printStackTrace();
            connectSuccess = false;

        }

        if(connectSuccess) {
            try {
                outputStream = btSocket.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void pairedDevicesList () {
        pairedDevices = mBluetoothAdapter.getBondedDevices();

        if (pairedDevices.size() > 0) {
            for (BluetoothDevice bt : pairedDevices) {
                // look for the bluetooth circuit being used
                if(bt.getName().toString().equals("HC-06")) { // change the name according to the device
                    Toast.makeText(getApplicationContext(), "Device " + bt.getName().toString() + " found and paired!", Toast.LENGTH_SHORT).show();
                    hc05 = bt;
                }
            }
        } else {
            Toast.makeText(getApplicationContext(), "No Paired Bluetooth Devices Found.", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * sendSetTime function
     * Sends the selected hour and minutes through bluetooth
     */
    private void sendSetTime() {
        // get values and remove whitespaces
        final String hour = mHour.getText().toString().trim();
        final String minutes = mMinutes.getText().toString().trim();

        if (hour.length() == 0 || hour.equals("") || hour.toLowerCase().equals("Set Hour") ||
            minutes.length() == 0 || minutes.equals("") || minutes.toLowerCase().equals("Set Minutes")) {
            // nothing was typed
            Toast.makeText(getApplicationContext(), "No time was set", Toast.LENGTH_SHORT).show();
        } else if(hour.matches(".*[a-zA-Z]+.*") || minutes.matches(".*[a-zA-Z]+.*")) {
            // letters were introduced
            Toast.makeText(getApplicationContext(), "Please type only numbers", Toast.LENGTH_SHORT).show();
        } else if(Integer.parseInt(hour) > 23 || Integer.parseInt(minutes) > 59) {
            // out of range numbers for hour or minutes
            Toast.makeText(getApplicationContext(), "Please type a valid time", Toast.LENGTH_SHORT).show();
        } else {
            try {
                String hourMin = hour + "," + minutes; // coma delimiters help to distinguish numbers and end of frame
                outputStream.write(hourMin.getBytes());
                Toast.makeText(getApplicationContext(), "Data Sent!", Toast.LENGTH_SHORT).show();
            } catch (IOException e){
                e.printStackTrace();
                Toast.makeText(getApplicationContext(), "Couldn't connect to device", Toast.LENGTH_SHORT).show();
            }
        }
    }

    /*************************************  onClick Event Listeners  *************************************/

    /**
     * SubmitButtonListener.
     */
    private View.OnClickListener SetButtonListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            sendSetTime();
        }
    };

    /**
     * EditTextListener.
     */
    private View.OnClickListener EditTextListenerHour = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            mHour.setText("");
        }
    };

    /**
     * EditTextListener.
     */
    private View.OnClickListener EditTextListenerMinutes = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            mMinutes.setText("");
        }
    };

    /*********************************  OS attribute controllers  *********************************/

    /**
     * This method is executed when somewhere other than EditText Views.
     * @param view
     */
    private void hideSoftKeyboard(View view) {
        /* If user touches non editText view */
        if(view != null && !(view instanceof EditText)) {
            view.setOnTouchListener(new View.OnTouchListener() {
                public boolean onTouch(View v, MotionEvent event) {
                    hideSoftKeyboardHelper(SetTimeActivity.this);
                    return false;
                }
            });
        }
        /* If a layout container, iterate over children and see recursion */
        if (view instanceof ViewGroup) {
            for (int i = 0; i < ((ViewGroup) view).getChildCount(); i++) {
                View innerView = ((ViewGroup) view).getChildAt(i);
                hideSoftKeyboard(innerView);
            }
        }
    }

    /**
     * This helper method is executed when somewhere other than EditText Views.
     * @param activity
     */
    public void hideSoftKeyboardHelper(Activity activity) {
        InputMethodManager imm = (InputMethodManager) activity.getSystemService(Activity.INPUT_METHOD_SERVICE);
        //Find the currently focused view, so we can grab the correct window token from it.
        View view = activity.getCurrentFocus();
        //If no view currently has focus, create a new one, just so we can grab a window token from it.
        if (view == null) {
            view = new View(activity);
        }
        imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
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
        try {
            outputStream.close();
            btSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        super.onDestroy();
    }

    /**
     * This method is executed when the back button is pressed.
     */
    public void onBackPressed() {
        try {
            outputStream.close();
            btSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        startActivity(new Intent(SetTimeActivity.this, MainActivity.class));
        finish();
    }

}
