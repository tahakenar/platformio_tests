#include <HardwareSerial.h>
#include "main_board.h"

//TODO: Serial communication tests

#define TRUE 1
#define FALSE 0

#define STR_MSG_LEN 6
#define ARRAY_LEN 1
const float mapping_coef = 300;

/* NOTE: Change these according to the motor/encoder num.

  ARRAY_LEN: Number of actuators/encoders. # of actuators == # of encoders in that case

  STR_MSG_LEN: Total length of the string message that is used to communicate with low level controller
  
  Example: 
  (Case: ARRAY_LEN = 1)
  "S0150F" : STR_MSG_LEN: 6 -> S: Start byte, F: End byte, 0: Direction byte, 150: Mapped magnitude dataset
  (Case: ARRAY_LEN = 6)
  "S125502401123024512550240F": STR_MSG_LEN: 26
*/


/***************************************************/
/****************** CLASS DEFIN. ******************/

MainBoard board(ARRAY_LEN,STR_MSG_LEN,mapping_coef);

/***************************************************/
/****************** ROS PROCEDURE ******************/

ros::NodeHandle nh;

void commandCallback(const std_msgs::Float64MultiArray &command_msg);
void publishEncoderFeedback();
void readDataFromMCU();

std_msgs::Float64MultiArray feedback_pub_arr;

ros::Publisher feedback_pub("encoder_feedback_topic",&feedback_pub_arr);
ros::Subscriber<std_msgs::Float64MultiArray> command_sub("command_topic",&commandCallback);

/***************************************************/
/****************** SERIAL CONFIG ******************/

int incoming_byte;
String incoming_str;
char str_arr[STR_MSG_LEN+1];
int receive_cnt_flag = FALSE;

String ongoing_str;

HardwareSerial armSerial(PB11,PB10); // RX, TX

/***************************************************/
/****************** DEBUG CONFIG ******************/

std_msgs::String debug_str;
char debug_ch[50];

ros::Publisher debug_pub("debug_topic",&debug_str);

/***************************************************/

void setup(){

    nh.initNode();

    feedback_pub_arr.data = (float*)malloc(sizeof(float)*ARRAY_LEN);
    feedback_pub_arr.data_length = ARRAY_LEN;


    nh.advertise(feedback_pub);
    nh.subscribe(command_sub);

    nh.advertise(debug_pub);

    // Serial inits
    armSerial.begin(57600);

}

void loop(){

    nh.spinOnce();

    //readDataFromMCU();
    //publishEncoderFeedback();

    delay(1);
}

void commandCallback(const std_msgs::Float64MultiArray &command_msg){
    board.assignCommandArr(command_msg);
    board.multiArrToArr();
    board.generateStrMsg();
    ongoing_str = board.returnCommandStr();
    armSerial.println(ongoing_str);

    ongoing_str.toCharArray(debug_ch,50);
    debug_str.data = debug_ch;
    debug_pub.publish(&debug_str);


    nh.loginfo("New command!");

    /* TEST */
    ongoing_str[0] = 'A';
    ongoing_str[STR_MSG_LEN-1] = 'B';
    ongoing_str.toCharArray(debug_ch,50);
    debug_str.data = debug_ch;
    debug_pub.publish(&debug_str);

    board.assignStrEncFeedback(ongoing_str);
    board.parseEncoderFeedback();
    board.arrToMultiArr();
    publishEncoderFeedback();
    /* ---- */
}

void publishEncoderFeedback(){
    //board.assignFeedbackArr(feedback_pub_arr);
    feedback_pub_arr = board.returnFeedbackMultiArr();
    feedback_pub.publish(&feedback_pub_arr);
    nh.loginfo("Encoder feedback has published!");
}


void readDataFromMCU(){
     
    if (armSerial.available() > 0){
        incoming_byte = armSerial.read();
        if (incoming_byte == 'A'){
            incoming_str = "";
            incoming_str += (char) incoming_byte;
            return;
        }
        else if (receive_cnt_flag = TRUE && incoming_byte != 'B'){
            incoming_str += (char) incoming_byte;
        }
        else if (incoming_byte == 'B'){
            incoming_str += (char) incoming_byte;
            incoming_str.toCharArray(str_arr,STR_MSG_LEN+1);
            nh.loginfo(str_arr);

            board.assignStrEncFeedback(incoming_str);
            board.parseEncoderFeedback();
            publishEncoderFeedback();

            incoming_str = "";
        }   

    }
}