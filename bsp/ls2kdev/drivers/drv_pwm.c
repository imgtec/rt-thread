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

struct loongson_pwm {
    rt_uint32_t low_buffer;
    rt_uint32_t full_buffer;
    rt_uint32_t ctrl;
};

//#define LS2K_GEN_CONFIG0_REG            (LS2K_CHIP_CFG_REG_BASE + 0x420)

#define GEN_CONFIG0_REG 0xFFFFFFFFBfe10420

#define CTRL_EN      (1UL<<0)
#define CTRL_OE      (1UL<<3)
#define CTRL_SINGL   (1UL<<4)
#define CTRL_INTE    (1UL<<5)
#define CTRL_INT     (1UL<<6)
#define CTRL_RST     (1UL<<7)
#define CTRL_CAPTE   (1UL<<8)
#define CTRL_INVERT  (1UL<<9)
#define CTRL_DZONE   (1UL<<10)

rt_err_t loongson_pwm_enable(struct rt_device_pwm *device, int channel)
{
    int **priv;
    struct loongson_pwm *chip;
    volatile rt_uint64_t *config0;
    rt_uint32_t m;

    config0 = (void *)GEN_CONFIG0_REG;
    priv = device->parent.user_data;

    channel %= 4;
    chip = (void *)priv[channel];
    m = 1ULL << 12 << channel;
    *config0 |= m;

    chip->ctrl |= CTRL_EN;

//  rt_kprintf("priv: %p, %p, %p\n", priv[1], priv[2], priv[0]);
//  rt_kprintf("priv:[%p], channel:[%d]\n", device, channel);

    return RT_EOK;
}

rt_err_t loongson_pwm_disable(struct rt_device_pwm *device, int channel)
{
    struct loongson_pwm **chip;
    volatile rt_uint64_t *config0;
    rt_uint32_t m;

    config0 = (void *)GEN_CONFIG0_REG;
    chip = device->parent.user_data;

    channel %= 4; 
    chip[channel]->ctrl &= ~CTRL_EN;

    m = 1ULL << 12 << channel;
    *config0 &= ~m;

    rt_kprintf("%s: channel[%d]\n", __func__, channel);
    rt_kprintf("%p\n", chip[channel]);
    return RT_EOK;
}

rt_err_t loongson_pwm_set(struct rt_device_pwm *device, int channel, rt_uint32_t period, rt_uint32_t pulse)
{
    rt_uint32_t **priv;
    struct loongson_pwm *chip;

    priv = device->parent.user_data;
    channel %= 4;
    chip = (void *)priv[channel];

    chip->full_buffer = period;
    chip->low_buffer  = pulse;

    
    rt_kprintf("%s: channel[%d] period[%d], pulse[%d]\n", __func__, channel, period, pulse);
    rt_kprintf("%p\n", chip);

    return RT_EOK;
}

static rt_err_t loongson_pwm_ioctl(struct rt_device_pwm *device, int cmd, void *arg)
{
    rt_err_t rc;
    struct rt_pwm_configuration *cfg;

    cfg = (void *)arg;

    switch (cmd) {
    case PWM_CMD_ENABLE:
        rt_kprintf("PWM_CMD_ENABLE\n");
        rc = loongson_pwm_enable(device, cfg->channel);
        break;
    case PWM_CMD_DISABLE:
        rt_kprintf("PWM_CMD_DISABLE\n");
        rc = loongson_pwm_disable(device, cfg->channel);
        break;
    case PWM_CMD_SET:
        rt_kprintf("PWM_CMD_SET\n");
        rc = loongson_pwm_set(device, cfg->channel, cfg->period, cfg->pulse);
        break;
    case PWM_CMD_GET:
        rt_kprintf("PWM_CMD_GET\n");
        rc = RT_ENOSYS;
        break;
    default:
        rc = RT_EINVAL;
        break;
    }
    return rc;
}

struct rt_pwm_ops loongson_pwm_ops = {
    .control = loongson_pwm_ioctl,
};

struct rt_device_pwm loongson_pwm = {
    .ops = &loongson_pwm_ops,
};

#define LS2K_IO_REG_BASE                0xFFFFFFFFBf000000
/* PWM regs */
#define LS2K_PWM_REG_BASE               (LS2K_IO_REG_BASE + 0x00e02000)
#define LS2K_PWM0_REG_BASE              (LS2K_PWM_REG_BASE + 0x00)
#define LS2K_PWM1_REG_BASE              (LS2K_PWM_REG_BASE + 0x10)
#define LS2K_PWM2_REG_BASE              (LS2K_PWM_REG_BASE + 0X20)
#define LS2K_PWM3_REG_BASE              (LS2K_PWM_REG_BASE + 0X30)

int loongson_pwm_init(void)
{
    int rc = RT_EOK;
    static rt_uint32_t *priv[] = {
        (void *)LS2K_PWM0_REG_BASE,
        (void *)LS2K_PWM1_REG_BASE,
        (void *)LS2K_PWM2_REG_BASE,
        (void *)LS2K_PWM3_REG_BASE
    };

    // default GPIO input.

    rc = rt_device_pwm_register(&loongson_pwm, "pwm0", &loongson_pwm_ops, &priv);
    rt_kprintf("Hello PWM [skeleton].\n");

    return rc;
}
INIT_DEVICE_EXPORT(loongson_pwm_init);

#endif /*RT_USING_PWM*/
