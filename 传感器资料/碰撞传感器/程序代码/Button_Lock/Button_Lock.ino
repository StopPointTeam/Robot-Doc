const int buttonPin = 2; //连接开关到D2。
const int ledPin = 9;    //连接LED到D9。
boolean onoff = HIGH;       //设定一个变量用于记录开关状态。
void setup(){
  pinMode(buttonPin,INPUT); //按钮端子作为输入
  pinMode(ledPin,OUTPUT);   //LED端子作为输出
}
void loop(){
  if(digitalRead(buttonPin)==HIGH)    //由于本例检测下降沿触发，所以先检测输入是否高电平，
  {
    delay(10); //然后延时一段时间，
    if(digitalRead(buttonPin)==LOW)   //然后检测是不是电平变为低了。是的话，就是刚好按钮按下了。
    {
      digitalWrite(ledPin,onoff);     //写入当前LED状态onoff，
      onoff=(!onoff);    //然后LED状态反转，以便下一次使用。
      delay(10);         //延时一段时间，防止按钮突然断开再按下。
      while(digitalRead(buttonPin)==LOW)   //判断按钮状态，如果仍然按下的话，等待松开。防止一直按住导致LED输出端连续反转
      {
        delay(1);
      }
    }
  }
}
