#ifndef _ALARM_H
#define _ALARM_H

void alarm_function();
void alarm_write_iic(SYSTEM_TIME str_time_alarm_[2],uchar alarm_show_flag_[2][7]);
void alarm_read_iic();
#endif