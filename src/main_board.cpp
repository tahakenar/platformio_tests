#include "main_board.h"

MainBoard::MainBoard(int arr_len, int str_len, float map_coef)
{
    _array_length = arr_len;
    _str_msg_length = str_len;
    _radian_map_coef = map_coef;

    feedback_arr.data = (float*)malloc(sizeof(float)*_array_length);
    feedback_arr.data_length = _array_length;

    command_arr.data = (float*)malloc(sizeof(float)*_array_length);
    command_arr.data_length = _array_length;

    commands_to_send = (float*)malloc(sizeof(float)*_array_length);
    enc_feedback_to_send = (float*)malloc(sizeof(float)*_array_length);

}

MainBoard::~MainBoard(){
    free(commands_to_send);
    free(enc_feedback_to_send);
}

void MainBoard::assignFeedbackArr(std_msgs::Float64MultiArray &goal_arr){
    goal_arr = feedback_arr;
}

std_msgs::Float64MultiArray MainBoard::returnFeedbackMultiArr(){
    return feedback_arr;
}

void MainBoard::assignCommandArr(const std_msgs::Float64MultiArray &given_arr){
    command_arr = given_arr;
}

void MainBoard::generateStrMsg(){
    str_angle_command = "S";

    for (int i=0; i<_array_length; i++){
        str_angle_command += floatToStr(commands_to_send[i]);
    }

    str_angle_command += "F";

}

String MainBoard::floatToStr(float radian_data){
    String mapped_angle;
    if (radian_data > 0){
        mapped_angle = "1";
    }
    else{
        mapped_angle = "0";
    }
    radian_data = abs(radian_data);
    int encoded_radian = radian_data * _radian_map_coef;

    String encoded_str = String(encoded_radian);

    while (encoded_str.length() < 3){
        encoded_str = "0" + encoded_str;
    }

    mapped_angle += encoded_str;

    return mapped_angle;
}

void MainBoard::parseEncoderFeedback(){
    if (str_enc_feedback[0] == 'A' && str_enc_feedback[_str_msg_length-1] == 'B'){
        float direction;
        float angle;
        String mapped;
        int counter = 0;
        for (int i = 0; i < _array_length; i += 4){
            mapped = "";
            direction = detectDirection(str_enc_feedback[i+1]);
            mapped += str_enc_feedback[i+2] + str_enc_feedback[i+3] + str_enc_feedback[i+4];
            angle = decodeRadians(mapped);
            enc_feedback_to_send[counter] = direction*angle;
            counter++;
        }
    }

  return;
}

float MainBoard::detectDirection(char direction_byte){
    if (direction_byte == '0'){
        return -1.0;
    }
    else{
        return 1.0;
    }
}

float MainBoard::decodeRadians(String mapped_radian){
  float in_radians = mapped_radian.toFloat();

  in_radians = in_radians/_radian_map_coef;

  return in_radians;
}

void MainBoard::arrToMultiArr(){
    for (int i = 0; i < _array_length; i++){
        feedback_arr.data[i] = enc_feedback_to_send[i];
    }
}

void MainBoard::multiArrToArr(){
    for (int i = 0; i < _array_length; i++){
        commands_to_send[i] = command_arr.data[i];
    }
}

void MainBoard::assignStrEncFeedback(const String &enc_str){
    str_enc_feedback = enc_str;
}

String MainBoard::returnCommandStr(){
    return str_angle_command;
}