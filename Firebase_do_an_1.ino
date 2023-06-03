#include <WiFi.h>
// #include <FirebaseESP32.h>

// #define FIREBASE_HOST "https://do-an-1-a1cfd-default-rtdb.firebaseio.com"
// #define FIREBASE_AUTH "P0kDsTCnEICynHE0bDUOKB1MR5KvKhPO6tbkBKPj"
// FirebaseData fbdo;

// #define WIFI_SSID "AndroidAP468B"
// #define WIFI_PASSWORD "tandaptrai"
// #define WIFI_SSID "Redmi Note 9S"
// #define WIFI_PASSWORD "vinhquang123"
//  #define WIFI_SSID "Tra sua boss"
//  #define WIFI_PASSWORD "23456789"
 #define WIFI_SSID "DangSonVinh"
 #define WIFI_PASSWORD "27051962"
// #define WIFI_SSID "Fish Farm602"
// #define WIFI_PASSWORD "anhphicubu"

WiFiClient client;
WiFiServer server(80);

//----------------------------------------thiết lập các chân và biến cho thiết bị cảnh báo----------------------------------------------
int relay_chuong = 4;//D4    //chân relay nối với chuông
int relay_chuong_val = 0;         //biến nhận giá trị digital từ relay_chuong 

int btn_chuong = 33; //D2     //chân nút nhấn điều khiển bật tắt chuông
int btn_chuong_val = 0;     //biến nhận giá trị digital từ btn 
int count_btn_chuong = 0;     //biến đếm số lần nhấn btn chuông

const int led_chuong = 32; //D13  //chân led  hiển thị việc bật tắt thiết bị cảnh báo
const int led_warn = 26; //D14   //chân led cảnh báo

//-----------------------thiết lập các chân và biến cho cảm biến 1 và btn điều khiển cb 1----------------------------------------------
int cb1 = 2; //D25            //chân cảm biến 1
int cb1_val = 0;               //biến nhận giá trị digital từ cảm biến 1

int btn_cb1 = 14; //D32        //chân btn điều khiển cảm biến 1
int btn_cb1_val = 0;          //biến nhận giá trị digital từ btn điều khiển cảm biến 1
int count_btn_cb1 = 0;        //biến đếm số lần nhấn btn điều khiển cảm biến 1       

int led_cb1 = 25; //D15        //chân led hiển thị việc nhận/ngắt dữ liệu cảm biến 1

//------------------------thiết lập các chân và biến cho cảm biến 2 và btn điều khiển cb 2----------------------------------------------
int cb2 = 15; // D26           //chân cảm biến 2
int cb2_val = 0;               //biến nhận giá trị digital từ cảm biến 2

int btn_cb2 = 12; //D33        //chân btn điều khiển cảm biến 2
int btn_cb2_val = 0;           //biến nhận giá trị digital từ cb2
int count_btn_cb2 = 0;         //biến đếm số lần nhấn btn điều khiển cảm biến 2

int led_cb2 = 13 ;//D12        //chân led hiển thị việc nhận/ngắt dữ liệu cảm biến 2

//-------------------------hàm setup các biến và các chân--------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  // Initialize the output variables as outputs
  // Cau hinh chuong + led
  pinMode(btn_chuong,INPUT_PULLUP);
  pinMode(relay_chuong, OUTPUT);
  pinMode(led_chuong,OUTPUT);
  pinMode(led_warn,OUTPUT);

  // Cau hinh cam bien 1
  pinMode(cb1, INPUT);
  pinMode(btn_cb1, INPUT_PULLUP);
  pinMode(led_cb1, OUTPUT);
  // Cau hinh cam bien 2
  pinMode(cb2, INPUT);
  pinMode(btn_cb2, INPUT_PULLUP);
  pinMode(led_cb2, OUTPUT);

  // Set outputs to LOW
  digitalWrite(relay_chuong, LOW);
  digitalWrite(led_chuong, LOW);
  digitalWrite(led_warn, LOW);

  digitalWrite(led_cb1, LOW);
  digitalWrite(led_cb2, LOW);

  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  server.begin();
  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // Firebase.reconnectWiFi(true);

  // //Set database read timeout to 1 minute (max 15 minutes)
  // Firebase.setReadTimeout(fbdo, 1000 * 60);
  // //tiny, small, medium, large and unlimited.
  // //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  // Firebase.setwriteSizeLimit(fbdo, "tiny");
}

//-------------------------------hàm chính để điều khiển mạch----------------------------------------------------------------------------------------
void loop()
{
  //---------------------------------set giá  trị biến nhận dữ liệu digital------------------------------------------------------------------------------
  //Nut nhan chuong
  btn_chuong_val = digitalRead (btn_chuong);
  relay_chuong_val = digitalRead(relay_chuong);

  //Nut nhan cb 1
  btn_cb1_val = digitalRead (btn_cb1);
  cb1_val = digitalRead(cb1);
 
  //Nut nhan cb 2
  btn_cb2_val = digitalRead (btn_cb2);
  cb2_val = digitalRead(cb2);

  //------------------------------set biến đếm số lần nhấn nút----------------------------------------------------------------------------
  ///////////set biến đếm số lần bấm btn chuong
  if (btn_chuong_val == 0) //khi nhan nut
    {count_btn_chuong += 1;}
  else
    {count_btn_chuong = count_btn_chuong;}

   ///////////set biến đếm số lần bấm btn cam bien 1
  if (btn_cb1_val == 0)
    {count_btn_cb1 += 1;}
  else
    {count_btn_cb1 = count_btn_cb1;}

 ///////////set biến đếm số lần bấm btn cam bien 2
  if (btn_cb2_val == 0)
    {count_btn_cb2 += 1;}
  else
    {count_btn_cb2 = count_btn_cb2;}

//-------------------------------------điều khiển led 3 khối: chuong, cb1, cb2---------------------------------------------------------------
    //dieu khien den cua chuong)///////////////////////////////////////////////
    if( (count_btn_chuong %2) == 1)
      {
        //setDouble ham dua du lieu len firebase
        // delay(100);
        // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", (count_btn_chuong %2));  
        digitalWrite(led_chuong,HIGH); 
        Serial.println("Thiet bi canh bao dang bat");
        delay(100);
      }
    else
      {
        // delay(100);
        // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", (count_btn_chuong %2)); 
        digitalWrite(relay_chuong,LOW);
        digitalWrite(led_chuong,LOW); 
        Serial.println("Thiet bi canh bao dang tat");
        delay(100);
      }

    // FirebaseData count_btnchuong_fromfb;
    // Firebase.getInt(count_btnchuong_fromfb, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt");
    // int rev_count_btnchuong = count_btnchuong_fromfb.intData();
    // if(rev_count_btnchuong  == 1)
    //   {    
    //     count_btn_chuong = 1; 
    //     digitalWrite(led_chuong,HIGH); 
    //     delay(100);
    //   } 
    // else
    //   {   
    //     count_btn_chuong = 0;
    //     digitalWrite(led_chuong,LOW);   
    //     delay(100);
    //   }    

    // int led_chuong_tofb = digitalRead(led_chuong);   //tao bien dua du lieu led cam bien len firebase
    // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_thiet_bi", led_chuong_tofb);
    // int led_warn_tofb = digitalRead(led_warn);   //tao bien dua du lieu led cam bien len firebase
    // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_canh_bao", led_warn_tofb);

    //den cua cam bien 1///////////////////////////////////////////////////////
    //cap nhat du lieu bien dem so lan nhan btn len firebase
    if( (count_btn_cb1 %2) == 1)
      {    
        // delay(100);
        // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", (count_btn_cb1 %2)); 
        digitalWrite(led_cb1,HIGH); 
        Serial.print("Button cam bien 1 : ON ");
        Serial.print("      Gia tri cam bien 1 : ");
        Serial.println(cb1_val);   
        delay(100);
      } 
    else
      {   
        // delay(100);
        // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", (count_btn_cb1 %2));
        digitalWrite(led_cb1,LOW);   
        Serial.print("Button cam bien 1 : OFF");
        Serial.print("      Gia tri cam bien 1 : ");
        Serial.println(cb1_val);  
        delay(100);
      }
    //lay du lieu bien dem so lan nhan btn tu firebase xuong de dieu khien mach
    // FirebaseData count_btn1_fromfb;
    // Firebase.getInt(count_btn1_fromfb, "/He_thong_chong_trom/Phong_1/Btn_cb1");
    // int rev_count_btn1 = count_btn1_fromfb.intData();
    // if(rev_count_btn1  == 1)
    //   {    
    //     count_btn_cb1 = 1; 
    //     digitalWrite(led_cb1,HIGH); 
    //     delay(100);
    //   } 
    // else
      // {   
      //   count_btn_cb1 = 0;
      //   digitalWrite(led_cb1,LOW);   
      //   delay(100);
      // }    
    

    // int cb1_val_tofb = cb1_val;   //tao bien dua du lieu cam bien len firebase
    // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_1/Cam_bien_1", cb1_val_tofb);
       
    // int led_cb1_tofb = digitalRead(led_cb1);   //tao bien dua du lieu led cam bien len firebase
    // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_1/Led_cam_bien_1", led_cb1_tofb);

    // // den cua cam bien 2/////////////////////////////////////////////////////
    if( (count_btn_cb2 %2) == 1)
     {
        // delay(100);
    //     Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", (count_btn_cb2 %2));
        digitalWrite(led_cb2,HIGH);  
        Serial.print("Button cam bien 2 : ON ");    
        Serial.print("      Gia tri cam bien 2 : ");
        Serial.println(cb2_val);
        delay(100);
     } 
    else
    {
        // delay(100);
    //     Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", (count_btn_cb2 %2));
        digitalWrite(led_cb2,LOW);     
        Serial.print("Button cam bien 2 : OFF");
        Serial.print("      Gia tri cam bien 2 : ");
        Serial.println(cb2_val);  
        delay(100);
    }

    // FirebaseData count_btn2_fromfb;
    // Firebase.getInt(count_btn2_fromfb, "/He_thong_chong_trom/Phong_2/Btn_cb2");
    // int rev_count_btn2 = count_btn2_fromfb.intData();
    // if(rev_count_btn2  == 1)
    //   {    
    //     count_btn_cb2 = 1; 
    //     digitalWrite(led_cb2,HIGH); 
    //     delay(100);
    //   } 
    // else
    //   {   
    //     count_btn_cb2 = 0;
    //     digitalWrite(led_cb2,LOW);   
    //     delay(100);
    //   }  

    // int cb2_val_tofb = cb2_val;   //tao bien dua du lieu cam bien len firebase
    // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_2/Cam_bien_2", cb2_val_tofb);
       
    // int led_cb2_tofb = digitalRead(led_cb2);   //tao bien dua du lieu led cam bien len firebase
    // Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_2/Led_cam_bien_2", led_cb2_tofb);

//----------------------dieu khien chuong + xet gia tri cam bien 1-------------------------------------------------------------
if((count_btn_chuong %2)==1) // khi thiet bi canh bao dang duoc bat
  {                               
    // Truong hop nhan cam bien 1----------------------------------------------------------------------------------------------
    if ( ((count_btn_cb1 %2)==1) && ((count_btn_cb2 %2)==0) )
      {            
        if(cb1_val == 0)
         {          
            digitalWrite(relay_chuong,LOW); // chuong tat 
            digitalWrite(led_warn,LOW);// den bao hieu tat                 
            delay (100);
         }
        else 
         { 
            digitalWrite(relay_chuong,HIGH);
            if (relay_chuong_val == 1)
             {
              for(int i = 0; i<50;i++)                          
                {
                  digitalWrite(led_warn,HIGH);
                  delay(50);
                  digitalWrite(led_warn,LOW);
                  delay(50);   
                }            
               
             }
            delay(50);
            
         }
      }   
      
      //Khi nhan gia tri cam bien 2--------------------------------------------------------------------------------------------------------
      else if (((count_btn_cb1%2)==0) && ((count_btn_cb2%2)==1))
      {
        // khi ko co trom 
        if (cb2_val==0)
        {     
          digitalWrite (led_warn, LOW);
          digitalWrite (relay_chuong, LOW);    
          delay (100);
        }
        //khi co trom
        else
        {         
            digitalWrite(relay_chuong,HIGH);
            relay_chuong_val = digitalRead(relay_chuong);         
            if (relay_chuong_val ==1)
            {
            for(int i = 0; i<50;i++)                          
                {
                  digitalWrite(led_warn,HIGH);
                  delay(50);
                  digitalWrite(led_warn,LOW);
                  delay(50);   
                }                        
            }
            delay (50);       
        }
      }
      //nhấn cả hai cảm biến ---------------------------------------------------------------------------------------------------------------
    else if (((count_btn_cb1%2)==1) && ((count_btn_cb2%2)==1)) 
      {
        if ((cb1_val==1)||(cb2_val==1)) //neu 1 trong 2 cam bien nhan ra trom
         {   
            digitalWrite(relay_chuong,HIGH);
            relay_chuong_val = digitalRead(relay_chuong);
            if (relay_chuong_val ==1)
            {
              for(int i = 0; i<50;i++)                          
                {
                  digitalWrite(led_warn,HIGH);
                  delay(50);
                  digitalWrite(led_warn,LOW);
                  delay(50);   
                }                        
            }       
            delay (50);     
         }
        else
         {              
           digitalWrite (led_warn, LOW);
           digitalWrite (relay_chuong, LOW); 
           delay (100);               
         }
      }
      else
      {
      digitalWrite(relay_chuong,LOW);   
      Serial.println("2 cam bien dang khong hoat dong hay bat no len ");
      Serial.println();
      delay(100);          
      }      
  }
else 
  {      
      Serial.println("Hay bat thiet bi canh bao len");
      Serial.println();
      delay(100);
  }


/*********************************************Firebase *****************************************************/
    

      
    




    // int Cam_bien_2 = cb2_val;
    // if(Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_2/Cam_bien_2", Cam_bien_2))  
    //   Serial.println("Upload cam bien 2 success");  
    // else
    //   Serial.println("Upload cam bien 2 fail");

    // FirebaseData ctrl_btn2_fromfb;
    // Firebase.getInt(ctrl_btn2_fromfb, "/He_thong_chong_trom/Phong_2/Led_cam_bien_2");
    // count_btn_cb2 = ctrl_btn2_fromfb.intData();
    // if( (count_btn_cb2%2) == 1)
    // {       
    //     digitalWrite(led_cb2,HIGH);      
    //     delay(100);
    // } 
    // else
    // {   digitalWrite(led_cb2,LOW);   
    //     delay(100);
    // }
    
    // int Led_cam_bien_2 = digitalRead(led_cb2);
    // if(Firebase.setDouble(fbdo, "/He_thong_chong_trom/Phong_2/Led_cam_bien_2", Led_cam_bien_2))  
    //   Serial.println("Upload led cam bien 2 success");  
    // else
    //   Serial.println("Upload led cam bien 2 fail"); 

  //   if(Firebase.setDouble(fbdo,"/cambien1/led_cb1",digitalRead(led_cb1)))  
  //     Serial.println("Upload success");  
  //   else
  //     Serial.println("Upload fail");  

  //   if(Firebase.setDouble(fbdo, "/cambien2/led_cb2",digitalRead(led_cb2)))  
  //     Serial.println("Upload success");  
  //   else
  //     Serial.println("Upload fail");  
      
  //   if(Firebase.setDouble(fbdo,"/chuong/led_chuong",digitalRead(led_chuong))) 
  //     Serial.println("Upload success");  
  //   else
  //     Serial.println("Upload fail");  

  //   if(Firebase.setDouble(fbdo, "/chuong/relay",digitalRead(relay_chuong))) 
  //     Serial.println("Upload success");  
  //   else
  //     Serial.println("Upload fail");

  //   if(Firebase.setDouble(fbdo, "/LED",digitalRead(relay_chuong))) 
  //     Serial.println("Upload success");  
  //   else
  //     Serial.println("Upload fail");
  
    // if(Firebase.getString(fbdo, "/cambien1/led_cb1"))
    // {
    //   Serial.println("Download success: " + String(fbdo.stringData()));
    //   if(fbdo.stringData() == "ON")
    //     digitalWrite(led_cb1, HIGH);
    //   else
    //     digitalWrite(led_cb1, LOW);   
    // }else {
    //   Serial.println("Download fail: " + String(fbdo.stringData())); 

    // if(Firebase.getString(fbdo, "/cambien2/led_cb2"))
    // {
    //   Serial.println("Download success: " + String(fbdo.stringData()));
    //   if(fbdo.stringData() == "ON")
    //     digitalWrite(led_cb2, HIGH);
    //   else
    //     digitalWrite(led_cb2, LOW);   
    // }
    // else 
    //   Serial.println("Download fail: " + String(fbdo.stringData()));

    // if(Firebase.getString(fbdo, "/chuong/led_chuong"))
    // {
    //   Serial.println("Download success: " + String(fbdo.stringData()));
    //   if(fbdo.stringData() == "1")
    //     digitalWrite(led_chuong, HIGH);
    //   else
    //     digitalWrite(led_chuong, LOW);   
    // }
    // else 
    //   Serial.println("Download fail: " + String(fbdo.stringData())); 
    
    // if(Firebase.getString(fbdo, "/chuong/led_warn"))
    // {
    //   Serial.println("Download success: " + String(fbdo.stringData()));
    //   if(fbdo.stringData() == "1")
    //     digitalWrite(led_chuong, HIGH);
    //   else
    //     digitalWrite(led_chuong, LOW);   
    // }
    // else 
    //   Serial.println("Download fail: " + String(fbdo.stringData())); 

    // if(Firebase.getString(fbdo, "/led_warn"))
    // {
    //   Serial.println("Download success: " + String(fbdo.stringData()));
    //   if(fbdo.stringData() == "ON")
    //     digitalWrite(led_warn, HIGH);
    //   else
    //     digitalWrite(led_warn, LOW);   
    // }
    // else 
    //   Serial.println("Download fail: " + String(fbdo.stringData())); 
    // delay(500);
}



