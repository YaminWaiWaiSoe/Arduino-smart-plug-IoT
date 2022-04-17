# Arduino-smart-plug-IoT

Control TP-Link smart plugs with an Arduino and FireBettle Esp 32. I tested with Kasa Smart Wi-Fi Plug, HS100(UK) model, it also works with their other similar smart plugs since you can control most of them with your KASA account.

## Preliminary steps

In order to control the smart plugs, you need to get your TP-Link KASA token from your account, the endpoint URL for your smart plugs, and the deviceId of them as well.

### TP-Link KASA token
The first step is to get the smart plug token by using POST request to https://wap.tplinkcloud.com as following:

{
  "method": "login",
  "params": {
    "appType": "Kasa_Android",
    "cloudUserName": "XXXXXXX",
    "cloudPassword": "XXXXXXX",
    "terminalUUID": "UUIDv4"
   }
}

Replace XXXXXXX with your TP-Link KASA account credentials and UUIDv4, you can generate one here, https://www.uuidgenerator.net/version4. POST request can be done in this site, https://pipedream.com/. Under destination, change GET to POST and the destination URL is https://wap.tplinkcloud.com, add a header name called Content-type with the value of application/json, under parameters, click on add body and copy the block of code above with your information, and click on Launch Request. If all went well, you should get a JSON response from the server and your TP-Link Kasa token will be in that data. You will need to change the string called "token" in the ardunio file to your account token.

### DeviceId
Secondly, to get the end point URL and the deviceID, you need do another POST request. The location will be https://wap.tplinkcloud.com/?token=ACQUIRED_TOKEN, you just need to change the last part ACQUIRED_TOKEN to the TP-Link KASA account token you got in previously. You can follow the same steps mentioned previously, but in the parameters' body, you will need to only put this:

{"method":"getDeviceList"}
You will get another JSON response from the server. Here what we are looking for if the value of "appServerURL" (which is the endpoint URL) and all of the values of "deviceId" in the list of "deviceList". In order to identify the device, in the same list element, you will see "alias" which has the name of the smart plug.

Afterwards, you only need to add the deviceId to the string called "device". Now smart plug controlling part is completed. 

### Blynk token
To connect with blynk, you need the token where you will get it in your email sent by Blynk after creating a new project. Then add the token into the BLYNK_AUTH_TOKEN in the arduino file. 

## Arduino File
I control TP-Link smart plugs according to the sensors data which are room temperature (DHT11 sensor) and human detection (PIR sensor) to be efficienct in energy usage. If the room temperature is lower than normal room temperature (20C) and human excits in the room, then our smart plug turns on otherwise turn off. Condition to control the smart plug is default by 20C, however you can update it with the user interaction by Blynk app. Here we have a button which control default mode or user mode and user desired temperature textbox. 

![IMG_20220405_211939 (1)](https://user-images.githubusercontent.com/47670208/163713791-9c22c6cc-3c40-47ad-8538-576a82f32d66.jpg)


#### Blynk app 
![Screenshot_20220405_232135 (1)](https://user-images.githubusercontent.com/47670208/163713775-ec99e055-0778-40a1-b73b-1184a2f2a9d7.jpg)


## Video Demonstration
You can watch the video demonstration here, https://www.youtube.com/watch?v=MfTWHow1oHI.


