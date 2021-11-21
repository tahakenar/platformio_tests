#ifndef MAIN_BOARD_H
#define MAIN_BOARD_H

#include <Arduino.h>
#include <ros.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/String.h>


class MainBoard
{
private:

    int _array_length;
    int _str_msg_length;
    float _radian_map_coef;
    int _str_piece_len = 4;
    // Some predefined data. Can be changed while constructing a new member, according to the needs

    float *commands_to_send;
    float *enc_feedback_to_send;
    // Pointers to the float arrays that is used to get and send data

    String str_angle_command;
    String str_enc_feedback;
    // String vars. to communicate with low level controller

    std_msgs::Float64MultiArray feedback_arr;
    std_msgs::Float64MultiArray command_arr;
    // Arrays to publish and subscribe necessary topics using ROS interface

    
public:

    MainBoard(int arr_len, int str_len, float map_coef);
    // Constructor. Assigns necessary data and allocates memory for

    ~MainBoard();

    void assignFeedbackArr(std_msgs::Float64MultiArray &goal_arr);
    // Assigns the feedback_arr to a global variable

    void assignCommandArr(const std_msgs::Float64MultiArray &given_arr);
    // Assigns the data of the give command array to the private variable

    void generateStrMsg();
    // Generates str. message to write to the lower controller

    String floatToStr(float radian_data);
    // Converts float to String -> A helper function for MainBoard::generateStrMsg()

    void parseEncoderFeedback();
    // Parses the encoder data (String) which comes from low level controller

    float detectDirection(char direction_byte);
    // Detects the direction in the encoder data (String) -> A helper function for MainBoard::parseEncoderFeedback()

    float decodeRadians(String mapped_radian);
    // Decodes the mapped radian into real value in radians -> A helper function for MainBoard::parseEncoderFeedback()

    void arrToMultiArr();
    // Assigns the data in float array into the std_msgs::Float64MultiArray

    std_msgs::Float64MultiArray returnFeedbackMultiArr();
    // Returns _feedback_arr

    void multiArrToArr();
    // Assigns the data in the std_msgs::Float64MultiArray to the private array

    void assignStrEncFeedback(const String &enc_str);
    // Assigns the data to the private variable str_enc_feedback

    String returnCommandStr();
    // Returns command string variable

};

#endif