/*********************************************************

WiFi Electric Blanket Controler
Joel Bartlett
SparkFun Electronics
January 25, 2013

This code uses an Arduino Uno with a SparkFun WiFly shield attached
to act as a webserver cabable of controlling a servo attached
to an electric blanket controller over the web.

To use this code with Arduino 1.0.2+, it is recommended that you use
this version of the WiFLy Library:
https://github.com/jmr13031/WiFly-Shield
Many thanks to Philip Lindsay (aka Follower) for creating this library
and many thanks to jmr13031 for updating the library for Arduino 1.0+

This code borrows heavily from the code found from Upverter.com
https://gist.github.com/1201213
Many thanks to Emmanuel DeVries for doing a lot of the heavy lifting.

This also borrows from the Sparkfun Speakjet Wifly Example by Chris Taylor
https://www.sparkfun.com/tutorials/158
***********************************************************/
//I took out the credentials.h file as well as the code that 
//tells the wifly server the ssid and the parephrase since
//this info was setup manually on the WiFly shield.
//The IP address is also set maually instead of being assigned. 

#include "WiFly.h" //include the WiFly experimental library

char msg[128];//variables for reading in the html responses
int letterCount = 0; 

int blanketState;//variables for knowing the state the blanket is in
int tempState;
int tempCount;

Server server(8080);// I am on port 8080, you can just use 80 if you have no other devices hooked up to your router 

int powerRead = 2;//variables for reading and contolling the buttons on the controller
int upBut = 3;
int downBut = 4;
int powerSwitch = 5;
///////////////////////////////////////////////////////////////////
void setup() 
{
  pinMode(powerRead, INPUT);// set to inputs (High Z) so that the manual controls still work
  pinMode(powerSwitch, INPUT);
  pinMode(upBut, INPUT);
  pinMode(downBut, INPUT);
  digitalWrite(powerSwitch, HIGH);
  digitalWrite(upBut, HIGH);
  digitalWrite(downBut, HIGH);

  WiFly.begin();

  Serial.begin(9600);
  Serial.print("IP: ");
  Serial.println(WiFly.ip());//taking this line out made the  wifly hang, so I just left it in
  
  server.begin();

  if(digitalRead(powerRead) == LOW)// read the state of the blanket (on or off) upon startup to set the variable accordingly 
      blanketState = 0;
  if(digitalRead(powerRead) == HIGH)
      blanketState = 1;
  //Upon hooking up the blanket to the Arduino, it needs to be set at 5 so it and the counter are synced up    
  tempState = 1;//Medium
  tempCount = 5;//Medium
}
///////////////////////////////////////////////////////////////////
void loop() 
{

  Client client = server.available();
  //the flag dataFlag give a notification of where the actual post
  //is once we flag it we can begin recording the message
  //so we can do stuff with it later on
  
  int dataFlag = 0;
  int msgIsolator = 0;

  //int inChar; //variable for reading in data from the phone/browser 
  //boolean isParsing = false;// flag for reading in and parsing data
  
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    boolean endOfCode = true;
    char c;
    
    while (client.connected()) 
    {
      if (client.available()) 
      {
        c = client.read();
        delay(10);
        //Uncomment this line to see the HTTP respone
        //Serial.print(c);
        
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (!client.available()) {
          endOfCode = true;
        } else if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
          endOfCode = false;
        }

        //this is where the message that the user entered is going so this is where
        //we record it into a char array
        if (msgIsolator == 1) 
        {
          recordMessage(c);
          delay(100);
        }

        if ((c == '\n' && current_line_is_blank && !client.available()) || endOfCode) 
        {
          
          //As soon as the page has been refreshed the letter count is set to zero
          letterCount = 0;
          
          //Once the page has been refreshed we are no longer on the first run through
          //off the program so we set this to false
          endOfCode = false;
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
                        
          //This basically creates what the user will see on the site. 
          client.println("<html>");
          client.println("<title>Electric Blanket Control</title>");
          client.println("<h1><center>");
          client.println("<img src=\"https://dlnmh9ip6v2uc.cloudfront.net/tutorialimages/EngineeringRoundTable/electricBlanket.jpg\">");
          client.println("</center></h1>");
          
          //the form code creates the buttons to click and change the temp/blanketstate 
          client.println("<center>");
          client.println("Turn Blanket On");
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" value=\"On\" style=\"height:50px; width:150px\" >");// I made the buttons larger for smart phone screens 
          client.println("</form>");
          
          client.println("Turn Blanket Off");
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" value=\"Off\" style=\"height:50px; width:150px\" >");
          client.println("</form>");
          
          client.println("Set Blanket Temperature");
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" value=\"High\" style=\"height:50px; width:150px\" >");
          client.println("</form>");
          
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" value=\"Medium\" style=\"height:50px; width:150px\" >");
          client.println("</form>");
          
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" size=\"8\" value=\"Low\" style=\"height:50px; width:150px\" >");
          client.println("</form>");
          
          //This prints out the state of each variable on the webpage
          client.println("<h4>");
          client.print("Blanket is currently: ");
          if(blanketState == 0)
            client.println("<span style=\"color:green;\">OFF</span>");
          else
            client.println("<span style=\"color:red;\">ON</span>");
            client.println("</h4>");
          
          client.println("<h4>");
          client.print("Current Temperature: ");
          client.println(tempCount);
          if(tempState == 0)
            client.println("<span style=\"color:yellow;\">LOW</span>");//span code changes the color of the text
          else if(tempState == 1)
            client.println("<span style=\"color:orange;\">MEDIUM</span>");
          else
            client.println("<span style=\"color:red;\">HIGH</span>");
            client.println("</h4>");
          
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" size=\"8\" value=\"Reset\" style=\"height:50px; width:150px\" >");
          client.println("</form>");
          client.println("<form name=\"input\" action=\"\" method=\"post\">");
          client.println("<input type=\"submit\" name=\"%\" size=\"8\" value=\"Reload\" style=\"height:50px; width:150px\" >");
          client.println("</form>");
      
          client.println("</center>");
          client.println("</html>");
          //***NOTE: I originally added the Reload button, and it broke everything. I combined the temperature state and number into one line and 
          //took out some other html code. It then worked. This leads me to beleive that there is only so much that the WiFly can serve up at a time??*** 
          delay(500);//delay is very important 
          letterCount = 0;
          checkAction();
          break;
        }
        
        //if you want to see all data comming in and going out uncomment the line below
        //Serial.print(c);
       
        //We detect where the actual post data is in other words what the user entered
        //once we isolate it we can parse it and use it
        if (c =='%')
        {
          dataFlag = 1;
        }
          if (dataFlag == 1)
          {
            //in here goes the user entered data it follows the string "25="
            if (c == '=')
            {
              //trim the fat of the soon to be recorded char so we only get the user entered message
              msgIsolator = 1;
            }
          }
      }
    }
    
  // give the web browser time to receive the data
  delay(100);//delay is very important 
  client.flush();
  client.stop();
  }//if(client)

  //If the buttons ont he controller are pressed manually, keep track of each press. 
  if(digitalRead(upBut) == LOW && digitalRead(powerRead) == HIGH)//if the up button is pressed and the unit is on...
  {
    tempCount++;
    delay(200);//debounce
    if(tempCount > 10)//if temp count is already HIGH, stay at 10
    tempCount = 10; 
  }
  if(digitalRead(downBut) == LOW && digitalRead(powerRead) == HIGH)//if the down button is pressed and the unit is on...
  {
    tempCount--;
    delay(200);//debounce
    if(tempCount < 1)//if temp count is already LOW, stay at 1
    tempCount = 1; 
  }
  
  if(digitalRead(powerRead) == LOW)//If no power is read on the unit, it is off.
      blanketState = 0;
  if(digitalRead(powerRead) == HIGH)//If power is read on the unit, it is on.
      blanketState = 1;
  
}//end loop
///////////////////////////////////////////////////////////////////////

char recordMessage (char incomingMsg)
{
  //letterCount = 0;
  msg[letterCount] = incomingMsg;
  letterCount++;
  delay(100);
}
///////////////////////////////////////////////////////////////////////
void checkAction() 
// the first two or three letters of each message are read to determine which button was clicked on the webage
{ 
  if (msg[0] == 'H' && msg[1] == 'i' && msg[2] == 'g')// set to HIGH
  {
    if(tempCount < 10)//if in MED or LOW, go up to HIGH
    {
    pinMode(upBut, OUTPUT);//set button to output to control controller
      do
      {
        digitalWrite(upBut, LOW);//simulate button presses
        delay(100);
        digitalWrite(upBut, HIGH);
        delay(100);
        tempCount++;
      }while(tempCount < 10);
    pinMode(upBut, INPUT);//set back to INPUT to allow manual presses
    }
    
    tempState = 2;
    //The Err is set at the end of each command to prevent the regualr commands from being sent again on a browser reload 
    msg[0] = 'E';
    msg[1] = 'r';
    msg[2] = 'r';
  }
  //-------------------------------------------------------
  else if (msg[0] == 'M' && msg[1] == 'e' && msg[2] == 'd')//Set to Medium
  {
    if(tempCount < 5)//if in LOW, go up to MED
    {
    pinMode(upBut, OUTPUT);
      do
      {
        digitalWrite(upBut, LOW);
        delay(100);
        digitalWrite(upBut, HIGH);
        delay(100);
        tempCount++;
      }while(tempCount < 5);
    pinMode(upBut, INPUT);
    }
    if(tempCount > 5)//if in HIGH, go down to MED
    {
    pinMode(downBut, OUTPUT);
      do
      {
        digitalWrite(downBut, LOW);
        delay(100);
        digitalWrite(downBut, HIGH);
        delay(100);
        tempCount--;
      }while(tempCount > 5);
    pinMode(downBut, INPUT);
    }
    
    tempState = 1;
    msg[0] = 'E';
    msg[1] = 'r';
    msg[2] = 'r';
  }
  //-------------------------------------------------------
  else if (msg[0] == 'L' && msg[1] == 'o' && msg[2] == 'w')//Set to LOW
  {
    if(tempCount > 1)//if in HIGH or MED, go down to LOW
    {
    pinMode(downBut, OUTPUT);
      do
      {
        digitalWrite(downBut, LOW);
        delay(100);
        digitalWrite(downBut, HIGH);
        delay(100);
        tempCount--;
      }while(tempCount > 1);
    pinMode(downBut, INPUT);
    }

    tempState = 0;
    msg[0] = 'E';
    msg[1] = 'r';
    msg[2] = 'r';
  }
  //-------------------------------------------------------
  else if (msg[0] == 'O' && msg[1] == 'f' && msg[2] == 'f')//Set blanket to OFF
  {
    if(blanketState == 1)//only turn blanket OFF if it's already ON
    {
    pinMode(powerSwitch, OUTPUT);
    digitalWrite(powerSwitch, LOW);
    delay(100);
    digitalWrite(powerSwitch, HIGH);
    pinMode(powerSwitch, INPUT);
    }

    blanketState = 0;
    msg[0] = 'E';
    msg[1] = 'r';
    msg[2] = 'r';
  }
  //-------------------------------------------------------
  else if (msg[0] == 'O' && msg[1] == 'n')//Set blanket to ON
  {
    if(blanketState == 0)//only turn blanket ON if it's already OFF
    {
    pinMode(powerSwitch, OUTPUT);
    digitalWrite(powerSwitch, LOW);
    delay(100);
    digitalWrite(powerSwitch, HIGH);
    pinMode(powerSwitch, INPUT);
    }

    blanketState = 1;
    msg[0] = 'E';
    msg[1] = 'r';
    msg[2] = 'r';
  }
  //-------------------------------------------------------
    else if (msg[0] == 'R' && msg[1] == 'e' && msg[2] == 'l')//Reload browser
  {
    //do nothing but refresh 
    msg[0] = 'E';
    msg[1] = 'r';
    msg[2] = 'r';
  }
  //-------------------------------------------------------
  else if (msg[0] == 'R' && msg[1] == 'e' && msg[2] == 's')// Reset variables if blanket and Arduino become out of Sync
  {
      tempState = 1;//Med
      tempCount = 5;//Med
      
      msg[0] = 'E';
      msg[1] = 'r';
      msg[2] = 'r';
  }
  
 
}//end checkAction()
