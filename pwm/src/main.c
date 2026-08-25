#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

#define PWM DT_ALIAS(led)
#if !DT_NODE_EXISTS(PWM)
#error "No PWM device found"
#endif


static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(PWM);

#define STEP (pwm.period/20)

LOG_MODULE_REGISTER(pwm_demo, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("PWM Demo");

    int ret;
    uint32_t pulse_width=0;

    if (!pwm_is_ready_dt(&pwm)) {
        LOG_ERR("PWM controller device is not ready!");
        return -ENODEV;
    }
    
    while(1)
    {
        ret = pwm_set_pulse_dt(&pwm,pulse_width);
        if(ret < 0)
        {
            LOG_ERR("Failed to set pulse width");
            return ret;
        }
        pulse_width += STEP;

        if(pulse_width > pwm.period)
        {
            pulse_width = 0;
        }
        k_msleep(50);

    }
}