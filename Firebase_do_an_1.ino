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

const int led_tbbt = 32; //D13  //chân led  hiển thị việc bật tắt thiết bị cảnh báo
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
  pinMode(led_tbbt,OUTPUT);
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
  digitalWrite(led_tbbt, LOW);
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
  firebase_reset(); //set lai cac gia tri cua bien ve 0 tren firebase
}

//-------------------------------hàm chính để điều khiển mạch----------------------------------------------------------------------------------------
void loop()
{
  set_biendem();      //thiet lap cac bien dem doc du lieu tu nut nhan va bien dem so lan nhan nut
  main_function();  //ham chuc nang chinh cua he thong
  firebase_tbbt();  //tbbt:thiet bi bao trom, cap nhat led chuong, btn chuong len firebase
  firebase_canhbao(); //cap nhat trang thai led canh bao, chuong len firebase
  firebase_cb2();     //cap nhat btn, gia tri cb2
  firebase_cb1();     //cap nhat btn, gia tri cb1
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
    if (btn_chuong_val == 0) //khi nhan nut gia tri la 0
      {
        count_btn_chuong += 1;  //biến đếm cộng 1 khi nhấn nút
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
    if( (count_btn_chuong %2) == 1)   //nếu biến đếm là số lẻ tức là khi nhấn nút
      {
        //setInt ham dua du lieu len firebase
        digitalWrite(led_tbbt,HIGH);  //led báo hiệu thiết bị báo trộm (thiet bi canh bao) đang bật gồm chuông và led warn
        Serial.println("Thiet bi canh bao dang bat");
        delay(20);
      }
    else   //nếu biến đếm là số chẵn
      { 
        digitalWrite(relay_chuong,LOW);
        digitalWrite(led_tbbt,LOW); 
        Serial.println("Thiet bi canh bao dang tat");
        delay(20);
      }

    
    //--------------------------------------------------den cua cam bien 1----------------------------------------------------------------------/
    //hàm cập nhật giá trị của cảm biến 1 trên serial moitor
    if( (count_btn_cb1 %2) == 1)
      {    
        digitalWrite(led_cb1,HIGH);  //led này dùng để báo hiệu là hệ thống đang nhận dữ liệu từ cảm biến 1
        Serial.print("Button cam bien 1 : ON ");
        Serial.print("      Gia tri cam bien 1 : ");
        Serial.println(cb1_val);   
        delay(20);
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
{                     
//----------------------ham chinh thuc hien cac chuc nang cua he thong----------------------------------------------------------------/
//----------------------dieu khien chuong + xet gia tri cam bien 1--------------------------------------------------------------------/
    if((count_btn_chuong %2)==1) // khi thiet bi canh bao dang duoc bat
      {                               
        // Truong hop nhan cam bien 1-------------------------------------------------------------------------------------------------/
        if ( ((count_btn_cb1 %2)==1) && ((count_btn_cb2 %2)==0) ) //khi nhấn btn cb1 
          {            
            if(cb1_val == 0)  //nếu giá trị cảm biến 1 là 0 tức là không có chuyển động
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
                  firebase_canhbao();   //hàm này dùng để cập nhật dữ liệu led cảnh báo và chuông cảnh báo lên firebase
                  for(int i = 0; i<50;i++)                          
                    {
                      digitalWrite(led_warn,HIGH);
                      delay(150);
                      digitalWrite(led_warn,LOW);
                      delay(150);  
                    }            
                  
                }
                else
                {
                  firebase_canhbao();
                }
                delay(20);
                
            }
          }   
          
          //Khi nhan gia tri cam bien 2--------------------------------------------------------------------------------------------------------/
          else if (((count_btn_cb1%2)==0) && ((count_btn_cb2%2)==1))  //khi nhấn btn cb2
          {
            // khi ko cảm biến không cảm ứng được chuyển động nào
            if (cb2_val==0)
            {     
              digitalWrite (led_warn, LOW);
              digitalWrite (relay_chuong, LOW);    
              delay (10);
            }
            else
            {         
                digitalWrite(relay_chuong,HIGH);
                relay_chuong_val = digitalRead(relay_chuong);         
                if (relay_chuong_val ==1)
                {
                  firebase_canhbao();
                  for(int i = 0; i<50;i++)                          
                      {
                        digitalWrite(led_warn,HIGH);
                        delay(150);
                        digitalWrite(led_warn,LOW);
                        delay(150);
                      }                        
                }
                else
                {
                  firebase_canhbao();
                }
                delay (20);       
            }
          }
          //nhấn cả hai cảm biến ---------------------------------------------------------------------------------------------------------------/
        else if (((count_btn_cb1%2)==1) && ((count_btn_cb2%2)==1)) //khi nhấn btn cả hai cảm biến
          {
            if ((cb1_val==1)||(cb2_val==1)) //nếu 1 trong 2 cảm biến cảm ứng được chuyển động
            {   
                digitalWrite(relay_chuong,HIGH);     // relay sẽ kích hoạt
                relay_chuong_val = digitalRead(relay_chuong);
                if (relay_chuong_val ==1)
                {
                  firebase_canhbao();
                  for(int i = 0; i<50;i++)                          
                    {
                      digitalWrite(led_warn,HIGH);
                      delay(150);
                      digitalWrite(led_warn,LOW);
                      delay(150);
                    }                        
                }     
                else
                {
                  firebase_canhbao();
                }  
                delay (20);     
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
{   //hàm này có vai trò lấy dữ liệu và cập nhật dữ liệu lên firebase
    FirebaseData count_btnchuong_fromfb;
    Firebase.getString(count_btnchuong_fromfb, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt"); //lấy chuỗi từ firebase thông qua biến Btn_tbbt
    if(count_btnchuong_fromfb.dataType() == "int")   //nếu dữ liệu từ firebase là số nguyên tức là được cập nhật từ nút của mạch
      {
        if( (count_btn_chuong %2) == 1)
          {
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 1);  //hàm dùng để đưa số nguyên 1 lên firebase vào biến Btn_tbbt
            // delay(10);
          }
        else
          {
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 0);  
            // delay(10);
          }
      }
    if(count_btnchuong_fromfb.dataType() == "string")  //nếu dữ liệu từ firebase là chuỗi tức là được cập nhật từ web người dùng
      {
        //lay du lieu bien dem so lan nhan btn tu firebase xuong de dieu khien mach

        FirebaseData count_btnchuong_fromfb;   //biến thuộc kiểu dữ liệu của firebase
        Firebase.getString(count_btnchuong_fromfb, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt"); //chuyển dữ liệu kiểu firebase thành chuỗi
        String rev_count_btnchuong = count_btnchuong_fromfb.stringData();  //khai báo biến kiểu chuỗi để nhận dữ liệu đã được chuyển thành chuỗi từ firebase
        if(rev_count_btnchuong  == "1")  //khi ấn nút trên giao diện web để bật led, lưu ý là lúc này led tắt (thiết bị cảnh báo tắt)
          {  
            //lệnh điều kiện dưới để đồng bộ dữ liệu khi điều khiển từ web và điều khiển từ mạch thực tế
            if(count_btn_chuong % 2 == 0)  //nếu biến đếm là số chẵn tức là led hiện đang tắt khi nhấn nút từ web nó sẽ cộng lên 1
              {
                count_btn_chuong = count_btn_chuong + 1; //biến đếm cộng lên 1 để bật thiết bị cảnh báo
                // delay(10); 
              }
            else     //nếu biến đếm là số lẻ tức là led hiện đang bật khi nhấn nút từ web nó sẽ giữ nguyên và thiết bị cảnh báo vẫn bật         
              {
                count_btn_chuong = count_btn_chuong ;  
                // delay(10);
              } 
            Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Btn_tbbt", 1); //lệnh cập nhật lại Btn_tbbt gửi lên firebase
          } 
        else if(rev_count_btnchuong  == "0")  //khi nhấn nút trên web để led tắt
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
        int led_tbbt_tofb = digitalRead(led_tbbt);   //tạo biến đọc giá trị từ led thiết bị báo trộm
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_thiet_bi", led_tbbt_tofb); // gửi trạng thái led lên firebase
}
void firebase_canhbao()
{   //hàm dùng để đưa trạng thái led cảnh báo và chuông lên firebase
    int led_warn_tofb = digitalRead(relay_chuong);   //tao bien dua du lieu led canh bao len firebase
    Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Led_canh_bao", led_warn_tofb);

    int alarm_tofb = digitalRead(relay_chuong);   //tao bien dua du lieu chuong len firebase
    Firebase.setInt(fbdo, "/He_thong_chong_trom/Thiet_bi_bao_trom/Chuong_canh_bao", alarm_tofb);  
}

////////////////////////////firebase cam bien 2//////////////////////////////////////////////////////////////////////
void firebase_cb2()
{
    FirebaseData count_btn2_fromfb;  //tạo biến lấy dữ liệu từ firebase
    Firebase.getString(count_btn2_fromfb, "/He_thong_chong_trom/Phong_2/Btn_cb2"); //chuyển kiểu dữ liệu của biến thành chuỗi
    if(count_btn2_fromfb.dataType() == "int")  //nếu kiểu dữ liệu biến là int tức điều khiển bằng mạch thực tế
      {
        if( (count_btn_cb2 %2) == 1) //khi nhấn btn cb2
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
    if(count_btn2_fromfb.dataType() == "string")  //nếu kiểu dữ liệu biến là string tức điều khiển bằng web
      {
        //lay du lieu bien dem so lan nhan btn tu firebase xuong de dieu khien mach
        String rev_count_btn2 = count_btn2_fromfb.stringData(); 
        if(rev_count_btn2  == "1")  //khi nhấn nút trên web để led sáng
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
        else if(rev_count_btn2  == "0")  //khi nhấn nút trên web để led tắt
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
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Cam_bien_2", cb2_val_tofb);  //đưa trạng thái cb2 lên firebase
          
        int led_cb2_tofb = digitalRead(led_cb2);   //tao bien dua du lieu led cam bien len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_2/Led_cam_bien_2", led_cb2_tofb);  //đưa trạng thái led_cb2 lên firebase
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
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Cam_bien_1", cb1_val_tofb); //đưa trạng thái cb1 lên firebase
          
        int led_cb1_tofb = digitalRead(led_cb1);   //tao bien dua du lieu led cam bien len firebase
        Firebase.setInt(fbdo, "/He_thong_chong_trom/Phong_1/Led_cam_bien_1", led_cb1_tofb); //đưa trạng thái led_cb1 lên firebase
}
        
  




