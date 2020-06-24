/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright(c) 2020, Du Huanpeng<548708880@qq.com>
 *
 */

#include <rtthread.h>
#include <rtdevice.h>

#define RT_USING_PWM
#ifdef RT_USING_PWM


int loongson_pwm_init(void)
{
    int rc = RT_EOK;

   // rc = rt_device_pwm_register(struct rt_device_pwm *device, const char *name, const struct rt_pwm_ops *ops, const void *user_data)
    for(rc = 0; rc < 100; rc++)
        rt_kprintf("%s\n", __func__);;
    return rc;
}
INIT_DEVICE_EXPORT(loongson_pwm_init);

#endif /*RT_USING_PWM*/
