#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "JioFi2_11CDD3";
const char* password = "Root123456";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
unsigned int remoteUdpPort = 4211;
const char* remote_ip = "192.168.1.100";
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[3] ;  // a reply string to send back
int PIR_state = 0;
int flag = 0;


void setup()
{
  #define SW1 D6
  #define SW2 D2
  #define SW3 D1
  #define PIR D7
   
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  pinMode (SW1,OUTPUT);
  pinMode (SW2,OUTPUT);
  pinMode (SW3,OUTPUT);
  pinMode (PIR,INPUT);
}

void update_pir_state(int PIR_state){
  
  char payload[2] ;

  if (PIR_state == HIGH){
    payload[0] = '4';
    payload[1] = '1';
    Serial.printf("%c%c", payload[0],payload[1]);
  } else if (PIR_state == LOW){
    payload[0] = '4';
    payload[1] = '0';
    Serial.printf("%c%c", payload[0],payload[1]);
  }
  Udp.beginPacket(remote_ip,remoteUdpPort);
  Udp.write(payload,sizeof(payload));
  Udp.endPacket();
    
}

void Read_pir_data(){
  PIR_state = digitalRead(PIR);
    
  if ((PIR_state == HIGH) &&(flag == 0)){
    flag = 1;
    //Serial.printf("state = HIGH\n");
    update_pir_state(PIR_state);
  }
  else if ((PIR_state == LOW) &&(flag == 1)){
    flag = 0 ;
    //Serial.printf("state = LOW\n");
    update_pir_state(PIR_state);
  }
}


void Control_device(){
  if (!strcmp(incomingPacket, "10")){
      Serial.printf("Switch1 off");
      digitalWrite(SW1, LOW);
      replyPacket[0] = '1';
      replyPacket[1] = '3';
    }else if (!strcmp(incomingPacket, "11")){
      Serial.printf("Switch1 ON");
      digitalWrite(SW1, HIGH);
      replyPacket[0] = '1';
      replyPacket[1] = '2';
    }else if (!strcmp(incomingPacket, "20")){
      Serial.printf("Switch2 off");
      digitalWrite(SW2, LOW);
      replyPacket[0] = '2';
      replyPacket[1] = '3';
    }else if (!strcmp(incomingPacket, "21")){
      Serial.printf("Switch2 on");
      digitalWrite(SW2, HIGH);
      replyPacket[0] = '2';
      replyPacket[1] = '2';
    }else if (!strcmp(incomingPacket, "30")){
      Serial.printf("Switch3 off");
      digitalWrite(SW3, LOW);
      replyPacket[0] = '3';
      replyPacket[1] = '3';
    }else if (!strcmp(incomingPacket, "31")){
      Serial.printf("Switch3 ON");
      digitalWrite(SW3, HIGH);
      replyPacket[0] = '3';
      replyPacket[1] = '2';
    }
}
  
void loop()
{
  delay(1000);
  Read_pir_data();
  delay(1000);
  
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    Control_device();
    
    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(remote_ip,remoteUdpPort);
    Udp.write(replyPacket);
    Udp.endPacket();
  }
}
