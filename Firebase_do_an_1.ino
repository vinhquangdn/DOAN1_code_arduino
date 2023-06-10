#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "https://do-an-1-a1cfd-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "P0kDsTCnEICynHE0bDUOKB1MR5KvKhPO6tbkBKPj"
FirebaseData fbdo;

// #define WIFI_SSID "AndroidAP468B"
// #define WIFI_PASSWORD "tandaptrai"
// #define WIFI_SSID "Redmi Note 9S"
// #define WIFI_PASSWORD "vinhquang123"
//  #define WIFI_SSID "Tra sua boss"
//  #define WIFI_PASSWORD "23456789"
//  #define WIFI_SSID "Q.U.Y.N.H"
//  #define WIFI_PASSWORD "TAKEITEASY"
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
const int led_warn = 25; //D14   //chân led cảnh báo


//-----------------------thiết lập các chân và biến cho cảm biến 1 và btn điều khiển cb 1----------------------------------------------
int cb1 = 2; //D25            //chân cảm biến 1
int cb1_val = 0;               //biến nhận giá trị digital từ cảm biến 1

int btn_cb1 = 14; //D32        //chân btn điều khiển cảm biến 1
int btn_cb1_val = 0;          //biến nhận giá trị digital từ btn điều khiển cảm biến 1
int count_btn_cb1 = 0;        //biến đếm số lần nhấn btn điều khiển cảm biến 1       

int led_cb1 = 26; //D15        //chân led hiển thị việc nhận/ngắt dữ liệu cảm biến 1

//------------------------thiết lập các chân và biến cho cảm biến 2 và btn điều khiển cb 2----------------------------------------------
int cb2 = 15; // D26           //chân cảm biến 2
int cb2_val = 0;               //biến nhận giá trị digital từ cảm biến 2

int btn_cb2 = 13; //D33        //chân btn điều khiển cảm biến 2
int btn_cb2_val = 0;           //biến nhận giá trị digital từ cb2
int count_btn_cb2 = 0;         //biến đếm số lần nhấn btn điều khiển cảm biến 2

int led_cb2 = 12 ;//D12        //chân led hiển thị việc nhận/ngắt dữ liệu cảm biến 2

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
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");
  firebase_reset();
}

//-------------------------------hàm chính để điều khiển mạch----------------------------------------------------------------------------------------
void loop()
{
  set_biendem();
  main_function();
  firebase_tbbt();  //tbbt:thiet bi bao trom
  firebase_cb2();
  firebase_cb1();
}

void firebase_reset()
{
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_thiet_bi", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_canh_bao", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Chuong_canh_bao", 0);  
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Cam_bien_1", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Led_cam_bien_1", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Cam_bien_2", 0);
  Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Led_cam_bien_2", 0);
}
void set_biendem()
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
      {
        count_btn_chuong += 1;
      }
    else
      {
        count_btn_chuong = count_btn_chuong;
      }

    ///////////set biến đếm số lần bấm btn cam bien 1
    if (btn_cb1_val == 0)
      {
        count_btn_cb1 += 1;
      }
    else
      {
        count_btn_cb1 = count_btn_cb1;
      }

  ///////////set biến đếm số lần bấm btn cam bien 2
    if (btn_cb2_val == 0)
      {
        count_btn_cb2 += 1;
      }
    else
      {
        count_btn_cb2 = count_btn_cb2;
      }

//-------------------------------------điều khiển led 3 khối: chuong, cb1, cb2---------------------------------------------------------------
    //dieu khien den cua chuong)///////////////////////////////////////////////
    if( (count_btn_chuong %2) == 1)
      {
        //setInt ham dua du lieu len firebase
        digitalWrite(led_chuong,HIGH); 
        Serial.println("Thiet bi canh bao dang bat");
        delay(20);
      }
    else
      {
        digitalWrite(relay_chuong,LOW);
        digitalWrite(led_chuong,LOW); 
        Serial.println("Thiet bi canh bao dang tat");
        delay(20);
      }

    
    //den cua cam bien 1///////////////////////////////////////////////////////
    //cap nhat du lieu bien dem so lan nhan btn len firebase
    if( (count_btn_cb1 %2) == 1)
      {    
        digitalWrite(led_cb1,HIGH); 
        Serial.print("Button cam bien 1 : ON ");
        Serial.print("      Gia tri cam bien 1 : ");
        Serial.println(cb1_val);   
        delay(20);
        // Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", (count_btn_cb1 %2)); 
      } 
    else
      {   
        digitalWrite(led_cb1,LOW);   
        Serial.print("Button cam bien 1 : OFF");
        Serial.print("      Gia tri cam bien 1 : ");
        Serial.println(cb1_val);  
        delay(20);
      }


    // // den cua cam bien 2/////////////////////////////////////////////////////
    if( (count_btn_cb2 %2) == 1)
     {
        digitalWrite(led_cb2,HIGH);  
        Serial.print("Button cam bien 2 : ON ");    
        Serial.print("      Gia tri cam bien 2 : ");
        Serial.println(cb2_val);
        delay(20);
     } 
    else
    {
        digitalWrite(led_cb2,LOW);     
        Serial.print("Button cam bien 2 : OFF");
        Serial.print("      Gia tri cam bien 2 : ");
        Serial.println(cb2_val);  
        delay(20);
    }
    
}

void main_function()
{                     //ham chinh thuc hien cac chuc nang cua he thong

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
                delay (10);
            }
            else 
            { 
                digitalWrite(relay_chuong,HIGH);
                relay_chuong_val = digitalRead(relay_chuong);
                if (relay_chuong_val == 1)
                {
                  for(int i = 0; i<1000;i++)                          
                    {
                      digitalWrite(led_warn,HIGH);
                      delay(20);
                      digitalWrite(led_warn,LOW);
                      delay(20);   
                    }            
                  
                }
                delay(100);
                
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
              delay (10);
            }
            //khi co trom
            else
            {         
                digitalWrite(relay_chuong,HIGH);
                relay_chuong_val = digitalRead(relay_chuong);         
                if (relay_chuong_val ==1)
                {
                for(int i = 0; i<1000;i++)                          
                    {
                      digitalWrite(led_warn,HIGH);
                      delay(20);
                      digitalWrite(led_warn,LOW);
                      delay(20);   
                    }                        
                }
                delay (100);       
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
                  for(int i = 0; i<1000;i++)                          
                    {
                      digitalWrite(led_warn,HIGH);
                      delay(20);
                      digitalWrite(led_warn,LOW);
                      delay(20);   
                    }                        
                }       
                delay (100);     
            }
            else if (((cb1_val==0)||(cb2_val==0)))
            {              
              digitalWrite (led_warn, LOW);
              digitalWrite (relay_chuong, LOW); 
              delay (10);               
            }
          }
          else
          {
          digitalWrite(relay_chuong,LOW);   
          Serial.println("2 cam bien dang khong hoat dong hay bat no len ");
          Serial.println();
          delay(10);          
          }      
      }
    else 
      {      
          Serial.println("Hay bat thiet bi canh bao len");
          Serial.println();
          delay(10);
      }
    
}

/*********************************************Firebase chuong, led cảnh báo*****************************************************/
void firebase_tbbt()
{
    FirebaseData count_btnchuong_fromfb;
    Firebase.getString(count_btnchuong_fromfb, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt");
    if(count_btnchuong_fromfb.dataType() == "int")
      {
        if( (count_btn_chuong %2) == 1)
          {
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 1); 
            // delay(10);
          }
        else
          {
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 0);
            // delay(10);
          }
      }
    if(count_btnchuong_fromfb.dataType() == "string")
      {
        //lay du lieu bien dem so lan nhan btn tu firebase xuong de dieu khien mach

        FirebaseData count_btnchuong_fromfb;
        Firebase.getString(count_btnchuong_fromfb, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt");
        String rev_count_btnchuong = count_btnchuong_fromfb.stringData();
        if(rev_count_btnchuong  == "1")
          {  
            if(count_btn_chuong % 2 == 0)
              {
                count_btn_chuong = count_btn_chuong + 1; 
                // delay(10); 
              }
            else
              {
                count_btn_chuong = count_btn_chuong ;  
                // delay(10);
              } 
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 1); 
          } 
        else if(rev_count_btnchuong  == "0")
          {   
            if(count_btn_chuong %2 == 1)
              {
                count_btn_chuong = count_btn_chuong - 1 ; 
                // delay(10);
              }
            else
              {
                count_btn_chuong = count_btn_chuong ;  
                // delay(10);
              }       
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 0); 
          }   
      }
        int led_chuong_tofb = digitalRead(led_chuong);   //tao bien dua du lieu led bao bat tat thiet bi len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_thiet_bi", led_chuong_tofb);

        int led_warn_tofb = digitalRead(relay_chuong);   //tao bien dua du lieu led canh bao len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_canh_bao", led_warn_tofb);

        int alarm_tofb = digitalRead(relay_chuong);   //tao bien dua du lieu chuong len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Chuong_canh_bao", alarm_tofb);  
}

////////////////////////////firebase cam bien 2//////////////////////////////////////////////////////////////////////
void firebase_cb2()
{
    FirebaseData count_btn2_fromfb;
    Firebase.getString(count_btn2_fromfb, "/He_thong_chong_trom/Phong_2/Btn_cb2");
    if(count_btn2_fromfb.dataType() == "int")
      {
        if( (count_btn_cb2 %2) == 1)
        {
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", 1);
            // delay(10);
        } 
        else
        {
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", 0);
            // delay(10);
        }
      }
    if(count_btn2_fromfb.dataType() == "string")
      {
        //lay du lieu bien dem so lan nhan btn tu firebase xuong de dieu khien mach
        String rev_count_btn2 = count_btn2_fromfb.stringData();
        if(rev_count_btn2  == "1")
          {    
            if(count_btn_cb2 %2 == 0)
              {
                count_btn_cb2 = count_btn_cb2 + 1;
                // delay(10);
              }
            else
              {
                count_btn_cb2 = count_btn_cb2 ;
                // delay(10);
              }
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", 1);
          } 
        else if(rev_count_btn2  == "0")
          {   
            if(count_btn_cb2 %2 == 1)
              {
                count_btn_cb2 = count_btn_cb2 - 1;
                // delay(10);
              }
            else
              {
                count_btn_cb2 = count_btn_cb2 ;
                // delay(10);
              }
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Btn_cb2", 0);
        }  
      }
        int cb2_val_tofb = cb2_val;   //tao bien dua du lieu cam bien len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Cam_bien_2", cb2_val_tofb);
          
        int led_cb2_tofb = digitalRead(led_cb2);   //tao bien dua du lieu led cam bien len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Led_cam_bien_2", led_cb2_tofb);
}
////////////////////////////firebase cam bien 1//////////////////////////////////////////////////////////////////////
void firebase_cb1()
{
    FirebaseData count_btn1_fromfb;
    Firebase.getString(count_btn1_fromfb, "/He_thong_chong_trom/Phong_1/Btn_cb1");
    if(count_btn1_fromfb.dataType() == "int")
      {
        if( (count_btn_cb1 %2) == 1)
          {    
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", 1); 
            // delay(10);
          } 
        else
          {   
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", 0);
            // delay(10);
          }
      }
    if(count_btn1_fromfb.dataType() == "string")
      {
        // //lay du lieu bien dem so lan nhan btn tu firebase xuong de dieu khien mach
        String rev_count_btn1 = count_btn1_fromfb.stringData();
        if(rev_count_btn1  == "1")
          {    
            if(count_btn_cb1 %2 == 0)
              {
                count_btn_cb1 = count_btn_cb1 + 1;
                // delay(10);
              }
            else
              {
                count_btn_cb1 = count_btn_cb1 ;
                // delay(10);
              }
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", 1); 
          } 
        else if((rev_count_btn1  == "0"))
          {   
            if(count_btn_cb1 %2 == 1)
              {
                count_btn_cb1 = count_btn_cb1 - 1;
                // delay(10);
              }
            else
              {
                count_btn_cb1 = count_btn_cb1 ;
                // delay(10);
              }
              Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Btn_cb1", 0); 
          }    
        
      }
        int cb1_val_tofb = cb1_val;   //tao bien dua du lieu cam bien len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Cam_bien_1", cb1_val_tofb);
          
        int led_cb1_tofb = digitalRead(led_cb1);   //tao bien dua du lieu led cam bien len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Led_cam_bien_1", led_cb1_tofb);
}
        
  




