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

struct rt_device_pwm loongson_pwm = {
    .ops = NULL,
};

struct rt_pwm_ops loongson_ops = {
    .control = NULL,
};

int loongson_pwm_init(void)
{
    int rc = RT_EOK;

    rc = rt_device_pwm_register(&loongson_pwm, "pwm0", &loongson_ops, NULL);
    rt_kprintf("Hello PWM [skeleton].\n");

    return rc;
}
INIT_DEVICE_EXPORT(loongson_pwm_init);

#endif /*RT_USING_PWM*/
