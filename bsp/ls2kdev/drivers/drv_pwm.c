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


const char *cmdname[] = {
    "PWM_CMD_ENABLE",
    "PWM_CMD_DISABLE",
    "PWM_CMD_SET",
    "PWM_CMD_GET"
};


static rt_err_t loongson_ioctl(struct rt_device_pwm *device, int cmd, void *arg)
{
    rt_err_t rc;

    rt_kprintf("command:[%d-%s]\n", cmd, cmdname[cmd - PWM_CMD_ENABLE]);

    rc = RT_EOK;
    return rc;
}

struct rt_device_pwm loongson_pwm = {
    .ops = NULL,
};

struct rt_pwm_ops loongson_ops = {
    .control = loongson_ioctl,
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
