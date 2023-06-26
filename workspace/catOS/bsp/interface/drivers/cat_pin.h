/**
 * @file cat_pin.h
 * @brief 引脚和GPIO的接口文件
 * @author jiayuanwen (648137125@qq.com)
 * @version 1.0
 * @date 2023-05-27
 * Change Logs:
 * Date           Author        Notes
 * 2023-05-27     jiayuanwen     first verion
 * 
 */
#ifndef CAT_PIN_H
#define CAT_PIN_H

#include "catos_config.h"
#include "catos_types.h"

#define CAT_PIN_LOW             0x00
#define CAT_PIN_HIGH            0x01

#define CAT_PIN_MODE_OUTPUT     0x00
#define CAT_PIN_MODE_INPUT      0x01

/**
 * @brief 初始化引脚设备
 * 
 */
void cat_pin_device_init(void);

/**
 * @brief 初始化引脚
 * 
 * @param pin_num  catos引脚号
 * @param mode     模式
 * @return cat_uint8_t  CAT_EOK: 成功
 *                  CAT_ERROR: 失败
 */
cat_uint8_t cat_pin_init(cat_uint32_t pin_num, cat_uint8_t mode);

/**
 * @brief 设置引脚模式
 * 
 * @param pin_num  catos引脚号
 * @param mode     模式
 * @return cat_uint8_t 成功失败
 */
cat_uint8_t cat_pin_set_mode(cat_uint32_t pin_num, cat_uint8_t mode);

/**
 * @brief 读取引脚状态
 * 
 * @param pin_num catos引脚号
 * @return int8_t 引脚当前值
 */
cat_int8_t  cat_pin_read(cat_uint32_t pin_num);

/**
 * @brief 修改输出引脚值
 * 
 * @param pin_num   catos引脚号
 * @param val       要输出的状态
 * @return cat_uint8_t  成功失败
 */
cat_uint8_t cat_pin_write(cat_uint32_t pin_num, cat_uint8_t val);


#endif