/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/pwm_core.h>
#ifdef AOS_COMP_DEVFS
#include <inttypes.h>
#endif

#define AOS_MAX_PERIOD (1000000000U)
#define AOS_PWM_CHECK_NULL(x) \
    do { \
        if ((x) == NULL) { \
            return -EINVAL; \
        } \
    } while (0)

aos_status_t aos_pwm_get(aos_pwm_ref_t *ref, uint32_t id)
{
    AOS_PWM_CHECK_NULL(ref);

    return aos_dev_get(ref, AOS_DEV_TYPE_PWM, id);
}

void aos_pwm_put(aos_pwm_ref_t *ref)
{
    if (ref == NULL)
        return;

    aos_dev_put(ref);
}

aos_status_t aos_pwm_set_attr(aos_pwm_ref_t *ref, aos_pwm_attr_t const *attr)
{
    aos_status_t ret;
    aos_pwm_t *pwm;
    aos_pwm_attr_t l_attr;

    AOS_PWM_CHECK_NULL(ref);
    AOS_PWM_CHECK_NULL(attr);
    pwm = aos_container_of(ref->dev, aos_pwm_t, dev);
    memcpy(&l_attr, attr, sizeof(aos_pwm_attr_t));
    if (l_attr.period > AOS_MAX_PERIOD) {
        printf("warning:period exceeds 1s\r\n");
        l_attr.period = AOS_MAX_PERIOD;
    }
    if (l_attr.duty_cycle > l_attr.period) {
        printf("warning: duty exceeds period\r\n");
        l_attr.duty_cycle = l_attr.period;
    }
    if (l_attr.period == pwm->period &&
       l_attr.duty_cycle == pwm->duty_cycle &&
       l_attr.enabled == pwm->enabled &&
       l_attr.polarity == pwm->polarity)
            return 0;
    if (pwm->ops->apply == NULL)
        return -ENOTSUP;

    aos_dev_lock(ref->dev);
    ret = pwm->ops->apply(pwm, &l_attr);
    if (!ret) {
        pwm->period = l_attr.period;
        pwm->duty_cycle = l_attr.duty_cycle;
        pwm->enabled = l_attr.enabled;
        pwm->polarity = (uint32_t)l_attr.polarity;
    }
    aos_dev_unlock(ref->dev);
    return ret;
}

aos_status_t aos_pwm_get_attr(aos_pwm_ref_t *ref, aos_pwm_attr_t *attr)
{
    aos_pwm_t *pwm;

    AOS_PWM_CHECK_NULL(ref);
    AOS_PWM_CHECK_NULL(attr);
    pwm = aos_container_of(ref->dev, aos_pwm_t, dev);
    aos_dev_lock(ref->dev);
    attr->period = pwm->period;
    attr->duty_cycle = pwm->duty_cycle;
    attr->enabled = pwm->enabled;
    attr->polarity = pwm->polarity;
    aos_dev_unlock(ref->dev);
    return 0;
}
static void dev_pwm_unregister(aos_dev_t *dev)
{
    aos_pwm_t *pwm = aos_container_of(dev, aos_pwm_t, dev);

    if (pwm->ops->unregister)
        pwm->ops->unregister(pwm);
}

static aos_status_t dev_pwm_get(aos_dev_ref_t *ref)
{
    aos_pwm_t *pwm = aos_container_of(ref->dev, aos_pwm_t, dev);

    if (!aos_dev_ref_is_first(ref))
        return 0;
    return pwm->ops->startup(pwm);
}

static void dev_pwm_put(aos_dev_ref_t *ref)
{
    aos_pwm_t *pwm = aos_container_of(ref->dev, aos_pwm_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;
    pwm->ops->shutdown(pwm);
    pwm->period = 0;
    pwm->duty_cycle = 0;
    pwm->enabled = 0;
    return;
}

static const aos_dev_ops_t dev_pwm_ops = {
    .unregister = dev_pwm_unregister,
    .get = dev_pwm_get,
    .put = dev_pwm_put,
};

#ifdef AOS_COMP_DEVFS

static aos_status_t devfs_pwm_ioctl(aos_devfs_file_t *file, int cmd, uintptr_t arg)
{
    aos_pwm_ref_t *ref = aos_devfs_file2ref(file);
    aos_status_t ret;

    switch (cmd) {
    case AOS_PWM_IOC_GET_ATTR:
        {
            aos_pwm_attr_t attr;
            if (!aos_devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            ret = aos_pwm_get_attr(ref, &attr);
            if (ret)
                break;
            if (!aos_umem_check((const void *)arg, sizeof(attr)))
                return -EFAULT;
            ret = aos_umem_copy((void *)arg, &attr, sizeof(attr)) ? -EFAULT : 0;
        }
        break;
    case AOS_PWM_IOC_SET_ATTR:
        {
            aos_pwm_attr_t attr;
            if (!aos_devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            if (!aos_umem_check((const void *)arg, sizeof(attr)))
                return -EFAULT;
            if (aos_umem_copy(&attr, (const void *)arg, sizeof(attr))) {
                ret = -EFAULT;
                break;
            }
            ret = aos_pwm_set_attr(ref, &attr);
        }
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const aos_devfs_file_ops_t devfs_pwm_ops = {
    .ioctl      = devfs_pwm_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

#endif /* AOS_COMP_DEVFS */

aos_status_t aos_pwm_register(aos_pwm_t *pwm)
{
#ifdef AOS_COMP_DEVFS
    int name_len;
#endif

    AOS_PWM_CHECK_NULL(pwm);

    /* startup/shutdown/unregister/apply/startup must be set. */
    if ((pwm->ops == NULL) || (pwm->ops->unregister == NULL) ||
        (pwm->ops->apply == NULL) || (pwm->ops->shutdown == NULL) ||
        (pwm->ops->startup == NULL))
        return -EINVAL;

    pwm->dev.type = AOS_DEV_TYPE_PWM;
    pwm->dev.ops = &dev_pwm_ops;
    pwm->enabled = 0;
    pwm->duty_cycle = 0;
    pwm->period = 0;
    pwm->polarity = 0;
#ifdef AOS_COMP_DEVFS
    aos_devfs_node_init(&pwm->dev.devfs_node);
    pwm->dev.devfs_node.ops = &devfs_pwm_ops;
    name_len = snprintf(pwm->dev.devfs_node.name, sizeof(pwm->dev.devfs_node.name), "pwm%" PRIu32, pwm->dev.id);
    if (name_len < 0 || name_len >= sizeof(pwm->dev.devfs_node.name))
        return -EINVAL;
#endif

    return aos_dev_register(&(pwm->dev));
}

aos_status_t aos_pwm_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_PWM, id);
}
