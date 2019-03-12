/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.matias.nixieclockapp;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ExpandableListView;
import android.widget.SimpleExpandableListAdapter;
import android.widget.Toast;
import java.util.UUID;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/*******************************************************************************
 * For a given BLE device, this Activity provides the user interface to connect, display data,
 * and display GATT services and characteristics supported by the device.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 * CREATED ON    : 2019-03-10
 * BY            : Matias (matias.quintana.r@gmail.com)
 * DESCRIPTION   : Activity where specific hour and minutes are selected. It's a merge between the
 *                 original DeviceControlActivity and SetTimeActivity so that BLE 4.0 devices
 *                 such as the HM-10 can be used.
 *                 Many sections from the original DeviceControlActivity have been discarded since
 *                 they are not being used.
 * REFERENCES: http://mydroidbegin.blogspot.com/2015/12/android-send-data-to-bluetooth-le.html
 *             http://android-er.blogspot.com/2015/12/connect-hm-10-ble-module-to-android.html
 *             http://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/
 ******************************************************************************/
public class DeviceControlActivity extends Activity {
    private final static String TAG = DeviceControlActivity.class.getSimpleName();

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    private Button sendButton;
    private EditText mHour;
    private EditText mMinutes;

    private String mDeviceAddress;

    private BluetoothLeService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;

    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";

    private BluetoothGattCharacteristic bluetoothGattCharacteristicHM_10;

    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            mBluetoothLeService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    /* Handles various events fired by the Service.
     * ACTION_GATT_CONNECTED: connected to a GATT server.
     * ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
     * ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
     * ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
     *                       or notification operations.
     */
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                Log.d(TAG, "Connected");

                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                Log.d(TAG, "Disconnected");
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                displayGattServices(mBluetoothLeService.getSupportedGattServices());
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                // Here is where received data could be analyzed.
                // e.g. Echo back received data, with something inserted
//                final byte[] rxBytes = bluetoothGattCharacteristicHM_10.getValue();
//                final byte[] insertSomething = {(byte)'\n'};
//                byte[] txBytes = new byte[insertSomething.length + rxBytes.length];
//                System.arraycopy(insertSomething, 0, txBytes, 0, insertSomething.length);
//                System.arraycopy(rxBytes, 0, txBytes, insertSomething.length, rxBytes.length);

            }
        }
    };

    /*
     * If a given GATT characteristic is selected, check for supported features.  This sample
     * demonstrates 'Read' and 'Notify' features.  See
     * http://d.android.com/reference/android/bluetooth/BluetoothGatt.html for the complete
     * list of supported characteristic features.
     */
    private final ExpandableListView.OnChildClickListener servicesListClickListner =
            new ExpandableListView.OnChildClickListener() {
                @Override
                public boolean onChildClick(ExpandableListView parent, View v, int groupPosition,
                                            int childPosition, long id) {
                    if (mGattCharacteristics != null) {
                        final BluetoothGattCharacteristic characteristic =
                                mGattCharacteristics.get(groupPosition).get(childPosition);
                        final int charaProp = characteristic.getProperties();
                        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                            // If there is an active notification on a characteristic, clear
                            // it first so it doesn't update the data field on the user interface.
                            if (mNotifyCharacteristic != null) {
                                mBluetoothLeService.setCharacteristicNotification(
                                        mNotifyCharacteristic, false);
                                mNotifyCharacteristic = null;
                            }
                            mBluetoothLeService.readCharacteristic(characteristic);
                        }
                        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                            mNotifyCharacteristic = characteristic;
                            mBluetoothLeService.setCharacteristicNotification(
                                    characteristic, true);
                        }
                        return true;
                    }
                    return false;
                }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
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

        final Intent intent = getIntent();
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            Log.d(TAG, "Connect request result=" + result);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.gatt_services, menu);
        if (mConnected) {
            menu.findItem(R.id.menu_connect).setVisible(false);
            menu.findItem(R.id.menu_disconnect).setVisible(true);
        } else {
            menu.findItem(R.id.menu_connect).setVisible(true);
            menu.findItem(R.id.menu_disconnect).setVisible(false);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId()) {
            case R.id.menu_connect:
                mBluetoothLeService.connect(mDeviceAddress);
                return true;
            case R.id.menu_disconnect:
                mBluetoothLeService.disconnect();
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /*
     * Demonstrates how to iterate through the supported GATT Services/Characteristics.
     * In this sample, we populate the data structure that is bound to the ExpandableListView
     * on the UI.
     */
    private void displayGattServices(List<BluetoothGattService> gattServices) {
        UUID UUID_HM_10 = UUID.fromString(SampleGattAttributes.HM_10); // HM-10

        if (gattServices == null) return;
        String uuid = null;
        String unknownServiceString = getResources().getString(R.string.unknown_service);
        String unknownCharaString = getResources().getString(R.string.unknown_characteristic);
        ArrayList<HashMap<String, String>> gattServiceData = new ArrayList<HashMap<String, String>>();
        ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData
                = new ArrayList<ArrayList<HashMap<String, String>>>();
        mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();

        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {
            HashMap<String, String> currentServiceData = new HashMap<String, String>();
            uuid = gattService.getUuid().toString();
            currentServiceData.put(
                    LIST_NAME, SampleGattAttributes.lookup(uuid, unknownServiceString));
            currentServiceData.put(LIST_UUID, uuid);
            gattServiceData.add(currentServiceData);

            ArrayList<HashMap<String, String>> gattCharacteristicGroupData =
                    new ArrayList<HashMap<String, String>>();
            List<BluetoothGattCharacteristic> gattCharacteristics =
                    gattService.getCharacteristics();
            ArrayList<BluetoothGattCharacteristic> charas =
                    new ArrayList<BluetoothGattCharacteristic>();

            // Loops through available Characteristics.
            for (BluetoothGattCharacteristic gattCharacteristic : gattCharacteristics) {
                charas.add(gattCharacteristic);
                HashMap<String, String> currentCharaData = new HashMap<String, String>();
                uuid = gattCharacteristic.getUuid().toString();
                currentCharaData.put(
                        LIST_NAME, SampleGattAttributes.lookup(uuid, unknownCharaString));
                currentCharaData.put(LIST_UUID, uuid);
                gattCharacteristicGroupData.add(currentCharaData);

                //Check if it is "HM_10"
                if(uuid.equals(SampleGattAttributes.HM_10)){
                    bluetoothGattCharacteristicHM_10 = gattService.getCharacteristic(UUID_HM_10);

                }

            }
            mGattCharacteristics.add(charas);
            gattCharacteristicData.add(gattCharacteristicGroupData);
        }

        SimpleExpandableListAdapter gattServiceAdapter = new SimpleExpandableListAdapter(
                this,
                gattServiceData,
                android.R.layout.simple_expandable_list_item_2,
                new String[] {LIST_NAME, LIST_UUID},
                new int[] { android.R.id.text1, android.R.id.text2 },
                gattCharacteristicData,
                android.R.layout.simple_expandable_list_item_2,
                new String[] {LIST_NAME, LIST_UUID},
                new int[] { android.R.id.text1, android.R.id.text2 }
        );
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    /**
     * sendSetTime function
     * Sends the selected hour and minutes through bluetooth
     */
    private void sendSetTime() {
        // TODO: actually send the data using the write characteristic functionality

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
            // append both values in one variable
            String hourMin = hour + "," + minutes; // coma delimiters help to distinguish numbers and end of frame

            if(bluetoothGattCharacteristicHM_10 != null){
                bluetoothGattCharacteristicHM_10.setValue(hourMin.getBytes());
                mBluetoothLeService.writeCharacteristic(bluetoothGattCharacteristicHM_10);
                mBluetoothLeService.setCharacteristicNotification(bluetoothGattCharacteristicHM_10,true);
                Log.e(TAG, "Data Sent: " + hourMin);
            }
            Toast.makeText(getApplicationContext(), "Data Sent!", Toast.LENGTH_SHORT).show();
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
                    hideSoftKeyboardHelper(DeviceControlActivity.this);
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
     * This method is executed when the back button is pressed.
     */
    public void onBackPressed() {
        startActivity(new Intent(DeviceControlActivity.this, MainActivity.class));
        finish();
    }
}
