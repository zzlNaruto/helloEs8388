/**
 ****************************************************************************************************
 * @file        key.cpp
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       KEY 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20231201
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "key.h"

/**
* @brief       初始化KEY相关IO口
* @param       无
* @retval      无
*/
void key_init(void) 
{
    /* 结合原理图设计,按键没有按下时,KEY引脚检测到的是高电平 */
    pinMode(KEY_PINBoot, INPUT_PULLUP);    /* 设置key引脚为上拉输入模式 */
    Serial.print("KEYBoot初始化KEY_PIN:");
    Serial.println(KEY_PINBoot);
}


