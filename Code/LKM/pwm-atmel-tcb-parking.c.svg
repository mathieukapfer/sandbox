/*
 * Copyright (C) Overkiz SAS 2012
 *
 * Author: Boris BREZILLON <b.brezillon@overkiz.com>
 * License terms: GNU General Public License (GPL) version 2
 */

/* For debug purpose */
#define DEBUG

#define LOG(level, ...)                                                 \
  {                                                                     \
    char msg[200];                                                      \
    char prefix[100];                                                   \
    snprintf(prefix, sizeof(prefix), "%s:%d:%s:", __FILE__,  __LINE__, __FUNCTION__); \
    snprintf(msg, sizeof(msg), __VA_ARGS__);                            \
    printk(level "%s%s\n", prefix, msg);                                \
  }

#define LOG_ENTER  LOG(KERN_INFO, "called");
#define LOG_INFO(...)   LOG(KERN_INFO, __VA_ARGS__)

/* CPW spec from historical driver (base on TC_TIMER_CLOCK2 = 8.0 / MCK_FREQUENCY_MHZ */
/* #define C_SMP_START_LOW        920     // Start of low level sampling period (80us) */
/* #define C_SMP_STOP_LOW         1035    // End of low level sampling period (100us) */
/* #define C_SMP_START_HIGH       920     // Start of high level sampling period (80us) */
/* #define C_SMP_STOP_HIGH        1035    // End of high level sampling period (100us) */

#if 1
#define C_SMP_START_LOW			345	// Start of low level sampling period (30us)
#define C_SMP_STOP_LOW			460	// End of low level sampling period (40us)
#define C_SMP_START_HIGH		460	// Start of high level sampling period (40us)
#define C_SMP_STOP_HIGH			575	// End of high level sampling period (50us)
#else
#define C_SMP_START_LOW			345	// Start of low level sampling period (30us)
#define C_SMP_STOP_LOW			460	// End of low level sampling period (40us)
#define C_SMP_START_HIGH		345	// Start of high level sampling period (30us)
#define C_SMP_STOP_HIGH			575	// End of high level sampling period (50us) ==> duration 20us
#endif

#include <linux/module.h>
#include <linux/init.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/atmel_tc.h>
#include <linux/pwm.h>
#include <linux/of_device.h>
#include <linux/slab.h>


#define NPWM	6

#define ATMEL_TC_ACMR_MASK	(ATMEL_TC_ACPA | ATMEL_TC_ACPC |	\
				 ATMEL_TC_AEEVT | ATMEL_TC_ASWTRG)

#define ATMEL_TC_BCMR_MASK	(ATMEL_TC_BCPB | ATMEL_TC_BCPC |	\
				 ATMEL_TC_BEEVT | ATMEL_TC_BSWTRG)

struct atmel_tcb_pwm_device {
	enum pwm_polarity polarity;	/* PWM polarity */
	unsigned div;			/* PWM clock divider */
	unsigned duty;			/* PWM duty expressed in clk cycles */
	unsigned period;		/* PWM period expressed in clk cycles */
};

struct atmel_tcb_pwm_chip {
	struct pwm_chip chip;
	spinlock_t lock;
	struct atmel_tc *tc;
	struct atmel_tcb_pwm_device *pwms[NPWM];
};

static inline struct atmel_tcb_pwm_chip *to_tcb_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct atmel_tcb_pwm_chip, chip);
}


/* Add log helper
 * Just insert DUMP_REG(x) in fct that know the chip
 * in order to log all register value of current time counter block
*/

#define DUMP_REG(chip)                          \
  LOG_INFO("Dump:");                            \
  pwm_log_reg(chip);

#define READ_AND_LOG_REG(group, reg)                              \
  val = 0;                                                        \
  offset = ATMEL_TC_REG(group, reg);                              \
  val = __raw_readl(regs + offset);                               \
  dev_info(chip->dev, "# %s(%d)@%x:%x", #reg, group, offset, val);
// LOG_INFO("# %s(%d)@%x:%x", #reg, group, offset, val);

static void pwm_log_reg(struct pwm_chip *chip) {
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tc *tc = tcbpwmc->tc;
	void __iomem *regs = tc->regs;

  unsigned val;
  unsigned offset;
  int group_index = 0;

  READ_AND_LOG_REG(group_index, BCR);
  READ_AND_LOG_REG(group_index, BMR);

  for (group_index = 0; group_index <= 2; group_index++) {
    READ_AND_LOG_REG(group_index, CMR);
    READ_AND_LOG_REG(group_index, RA);
    READ_AND_LOG_REG(group_index, RB);
    READ_AND_LOG_REG(group_index, RC);
    READ_AND_LOG_REG(group_index, SR);
    // READ_AND_LOG_REG(group_index, CCR); // write only
  }

}


static int atmel_tcb_pwm_set_polarity(struct pwm_chip *chip,
				      struct pwm_device *pwm,
				      enum pwm_polarity polarity)
{
	struct atmel_tcb_pwm_device *tcbpwm = pwm_get_chip_data(pwm);

	tcbpwm->polarity = polarity;

	return 0;
}


static int atmel_tcb_pwm_request(struct pwm_chip *chip,
				 struct pwm_device *pwm)
{
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tcb_pwm_device *tcbpwm;
	struct atmel_tc *tc = tcbpwmc->tc;
	void __iomem *regs = tc->regs;
	unsigned group = pwm->hwpwm / 2;
	// unsigned index = pwm->hwpwm % 2;
	unsigned cmr;
	int ret;

	tcbpwm = devm_kzalloc(chip->dev, sizeof(*tcbpwm), GFP_KERNEL);
	if (!tcbpwm)
		return -ENOMEM;

	ret = clk_prepare_enable(tc->clk[group]);
	if (ret) {
		devm_kfree(chip->dev, tcbpwm);
		return ret;
	}

	pwm_set_chip_data(pwm, tcbpwm);
	tcbpwm->polarity = PWM_POLARITY_NORMAL;
	tcbpwm->duty = 0;
	tcbpwm->period = 0;
	tcbpwm->div = 0;

	spin_lock(&tcbpwmc->lock);
	cmr = __raw_readl(regs + ATMEL_TC_REG(group, CMR));
	/*
	 * Get init config from Timer Counter registers if
	 * Timer Counter is already configured as a PWM generator.
	 */
  /* MATK HACK : do not take into account previous setup */
  
	/* if (cmr & ATMEL_TC_WAVE) { */
	/* 	if (index == 0) */
	/* 		tcbpwm->duty = */
	/* 			__raw_readl(regs + ATMEL_TC_REG(group, RA)); */
	/* 	else */
	/* 		tcbpwm->duty = */
	/* 			__raw_readl(regs + ATMEL_TC_REG(group, RB)); */

	/* 	tcbpwm->div = cmr & ATMEL_TC_TCCLKS; */
	/* 	tcbpwm->period = __raw_readl(regs + ATMEL_TC_REG(group, RC)); */
	/* 	cmr &= (ATMEL_TC_TCCLKS | ATMEL_TC_ACMR_MASK | */
	/* 		ATMEL_TC_BCMR_MASK); */
	/* } else */
		cmr = 0;

	cmr |= ATMEL_TC_WAVE | ATMEL_TC_WAVESEL_UP_AUTO | ATMEL_TC_EEVT_XC0;
	__raw_writel(cmr, regs + ATMEL_TC_REG(group, CMR));
	spin_unlock(&tcbpwmc->lock);

	tcbpwmc->pwms[pwm->hwpwm] = tcbpwm;

	return 0;
}

/* MATK HACK - start section */

static int atmel_tcb_pwm_request_patched(struct pwm_chip *chip,
				 struct pwm_device *pwm)
{
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tc *tc = tcbpwmc->tc;
	void __iomem *regs = tc->regs;
	unsigned group = pwm->hwpwm / 2;
  /* MATK : index should not be used - do the same on both A and B (pwm0 and pwm1) */
	// unsigned index = pwm->hwpwm % 2;

  int err;
  unsigned group_index;
  unsigned bmr, cmr;

  /* CPW driver (Control Pilote Wired)
   * ----------------------------------
   *
   * In order to implement CPW driver, as historical solution,
   * the TC BLOCK 0 is organized as follow:
   *
   *            | index 0 (TIOA)      | index 1 (TIOB)
   * --------------------------------------------------------------------
   * group TC0  | pwm0:triggers (1)   |  pwm1: CPW 1 output
   * group TC1  | pwm2:               |  pwm3: SW 2 => trigger the low part of CPW 1
   * group TC2  | pwm4:               |  pwm5: SW 1 => trigger the high part of CPW 1
   *
   *  (1) TIOA0 triggers internal CAN conversion 
   *      but also group 1 and group 2 start !
   *
   * SPEC : SW 1 generate a 10 us burst after 80 us starting a *rising* edge of TIOA, 
   *        then this signal block the high level part (12V, 9V, 6V, 3V) of CPW 1 
   *        injected to CAN and trigger again on next rising edge (FIX THIS double trigger !).
   *      : SW 2 generate a 10 us burst after 80 us starting a *falling* edge of TIOA,
   *        then this signal block the low level (-12V) injected to GPIO/DI to get
   *        the presence of -12V.
   *
   * NOTE : TIOA0 (trigger) should be the same as TIOB0 (CPW 1 output)
   */

  /* 
     Accept only pwm->hwpwm = 1 
     in order to allow the control of period and duty cycle of CPW 1
  */

  if (pwm->hwpwm != 1) {
    dev_err(chip->dev,
            "This is scpecial PWM driver dedicated for CPW - only pwm*1* is accepted\n");
    return -EINVAL;
  }

  /* continue the normal registration of pwm1 */
  err = atmel_tcb_pwm_request(chip, pwm);
  if (err) {
    return err;
  } else {
    dev_dbg(chip->dev, "registration of pwm %d done.\n", pwm->hwpwm);
  }

  /* enable clock for group 1 and 2 */
  for (group_index = 1; group_index <= 2; group_index++) {
    err = clk_prepare_enable(tc->clk[group_index]);
    if (err) {
      dev_err(chip->dev, "failed to enable clock for group %d\n", group_index);
    } else {
      dev_dbg(chip->dev, "enable clock for group %d done\n", group_index);
    }
  }

#if 0
  // TMP: setup TCO 

  // TC0 (ADC trigger connected to TIOA0, TIOA0 = TIOB0) /////////////////////////
  //     (     output connected to TIOB0)/////////////////////////////////////////
  cmr = (
              // Waveform Operating Mode / WAVSEL = 01 (up counting with auto trigger on RC)
							ATMEL_TC_TIMER_CLOCK2 | ATMEL_TC_WAVE | ATMEL_TC_WAVESEL_UP_AUTO |
							// TIOB: Enable external trigger from XC0, Event detection disabled
							ATMEL_TC_ENETRG | ATMEL_TC_EEVT_XC0 | ATMEL_TC_EEVTEDG_NONE |
							// TIOA: No software trigger action, Clear on reaching RA, set on reaching RC
							ATMEL_TC_ASWTRG_NONE | ATMEL_TC_ACPA_CLEAR | ATMEL_TC_ACPC_SET |
							// TIOB: No software trigger action, Clear on reaching RB, set on reaching RC
							ATMEL_TC_BSWTRG_NONE  | ATMEL_TC_BCPB_CLEAR | ATMEL_TC_BCPC_SET);
  __raw_writel(cmr, regs + ATMEL_TC_REG(group, CMR));
  LOG_INFO("cmr(%d):0x%x\n", group, cmr);

  /* TMP - TCO reg values */
  __raw_writel(0x3FFF, regs + ATMEL_TC_REG(group, RA));
  __raw_writel(0x3FFF, regs + ATMEL_TC_REG(group, RB));
  __raw_writel(0x5555, regs + ATMEL_TC_REG(group, RC));
#endif

  /* Configure chaining: 
     Connect TIOA0 as external event on TIO1(XC1) and TIO2(XC2) 
  */

  bmr = ATMEL_TC_TC0XC0S_NONE | ATMEL_TC_TC1XC1S_TIOA0 | ATMEL_TC_TC2XC2S_TIOA0;
	__raw_writel(bmr, regs + ATMEL_TC_REG(group, BMR));

  /* TC1 Channel Mode Register (output connected to TIOB3) */
  group_index = 1;
  cmr =
    // Waveform Operating Mode / WAVSEL = 10 (up counting) / Clock stop on reaching RC
    ATMEL_TC_TIMER_CLOCK2 | ATMEL_TC_WAVE | ATMEL_TC_WAVESEL_UP_AUTO | ATMEL_TC_CPCSTOP |
    // TIOB: Enable internal trigger from XC1=TIOA0, Event detection on falling edge
    ATMEL_TC_ENETRG | ATMEL_TC_EEVT_XC1 | ATMEL_TC_EEVTEDG_FALLING |
    // TIOB: No software trigger action, Set on reaching RB, Clear on reaching RC
    ATMEL_TC_BSWTRG_NONE | ATMEL_TC_BCPB_SET | ATMEL_TC_BCPC_CLEAR;

  __raw_writel(cmr, regs + ATMEL_TC_REG(group_index, CMR));

  /* TC1 reg values */
  __raw_writel(C_SMP_START_LOW, regs + ATMEL_TC_REG(group_index, RB));
  __raw_writel(C_SMP_STOP_LOW, regs + ATMEL_TC_REG(group_index, RC));

  /* TC2 Channel Mode Register (output connected to TIOB2) */
  group_index = 2;
	cmr =
    // Waveform Operating Mode / WAVSEL = 10 (up counting) / Clock stop on reaching RC
    ATMEL_TC_TIMER_CLOCK2 | ATMEL_TC_WAVE | ATMEL_TC_WAVESEL_UP_AUTO | ATMEL_TC_CPCSTOP |
    // TIOB: Enable external trigger from XC2, Event detection rising edge
    ATMEL_TC_ENETRG | ATMEL_TC_EEVT_XC2 | ATMEL_TC_EEVTEDG_RISING |
    // TIOB: No software trigger action, set on reaching RB, Clear on reaching RC
    ATMEL_TC_BSWTRG_NONE | ATMEL_TC_BCPB_SET | ATMEL_TC_BCPC_CLEAR;

  __raw_writel(cmr, regs + ATMEL_TC_REG(group_index, CMR));

  /* TC2 reg values */
  __raw_writel(C_SMP_START_HIGH, regs + ATMEL_TC_REG(group_index, RB));
  __raw_writel(C_SMP_STOP_HIGH, regs + ATMEL_TC_REG(group_index, RC));

#if 1
  /* Stop clock + software trigger to initialise outputs */
  for (group_index = 1; group_index <= 2; group_index++) {
  		__raw_writel(ATMEL_TC_SWTRG | ATMEL_TC_CLKDIS,
                   regs + ATMEL_TC_REG(group_index, CCR));
  }
#else

  /* TMP : enable setting with test values */ 
	/* Use software trigger to apply the new setting */
	__raw_writel(ATMEL_TC_CLKEN | ATMEL_TC_SWTRG,
		     regs + ATMEL_TC_REG(group, CCR));

  /* MATK HACK */
  /* Enable SW1 and SW2 clock  */
  for (group_index = 1; group_index <= 2; group_index++) {
  		__raw_writel(ATMEL_TC_CLKEN,
                   regs + ATMEL_TC_REG(group_index, CCR));
  }
#endif

  // log for debug
  DUMP_REG(chip);

  return 0;
}

/* MATK HACK - end of section */


static void atmel_tcb_pwm_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tcb_pwm_device *tcbpwm = pwm_get_chip_data(pwm);
	struct atmel_tc *tc = tcbpwmc->tc;

	clk_disable_unprepare(tc->clk[pwm->hwpwm / 2]);
	tcbpwmc->pwms[pwm->hwpwm] = NULL;
	devm_kfree(chip->dev, tcbpwm);
}

static void atmel_tcb_pwm_disable_patched(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tcb_pwm_device *tcbpwm = pwm_get_chip_data(pwm);
	struct atmel_tc *tc = tcbpwmc->tc;
	void __iomem *regs = tc->regs;
	unsigned group = pwm->hwpwm / 2;
  /* MATK : index should not be used - do the same on both A and B (pwm0 and pwm1) */
  // unsigned index = pwm->hwpwm % 2;
	unsigned cmr;
	enum pwm_polarity polarity = tcbpwm->polarity;

  LOG_ENTER;
	/*
	 * If duty is 0 the timer will be stopped and we have to
	 * configure the output correctly on software trigger:
	 *  - set output to high if PWM_POLARITY_INVERSED
	 *  - set output to low if PWM_POLARITY_NORMAL
	 *
	 * This is why we're reverting polarity in this case.
	 */

  /* MATK pacth: alway invert polarity regardless the duty value 
   * in order to keep 'inactive' level on disable action
   */
	// if (tcbpwm->duty == 0)
  polarity = !polarity;

	spin_lock(&tcbpwmc->lock);
	cmr = __raw_readl(regs + ATMEL_TC_REG(group, CMR));

	/* flush old setting and set the new one */
  /* MATK : index should not be used - do the same on both A and B (pwm0 and pwm1) */
	{
		cmr &= ~ATMEL_TC_ACMR_MASK;
		if (polarity == PWM_POLARITY_INVERSED)
			cmr |= ATMEL_TC_ASWTRG_CLEAR;
		else
			cmr |= ATMEL_TC_ASWTRG_SET;
	}
  {
		cmr &= ~ATMEL_TC_BCMR_MASK;
		if (polarity == PWM_POLARITY_INVERSED)
			cmr |= ATMEL_TC_BSWTRG_CLEAR;
		else
			cmr |= ATMEL_TC_BSWTRG_SET;
	}

	__raw_writel(cmr, regs + ATMEL_TC_REG(group, CMR));

	/*
	 * Use software trigger to apply the new setting.
	 * If both PWM devices in this group are disabled we stop the clock.
	 */
	if (!(cmr & (ATMEL_TC_ACPC | ATMEL_TC_BCPC))) {
    LOG_INFO("SWTRG + CLKDIS");
		__raw_writel(ATMEL_TC_SWTRG | ATMEL_TC_CLKDIS,
                 regs + ATMEL_TC_REG(group, CCR));
  } else {
    LOG_INFO("SWTRG only");
		__raw_writel(ATMEL_TC_SWTRG, regs +
                 ATMEL_TC_REG(group, CCR));
  }

  // log for debug
  DUMP_REG(chip);

	spin_unlock(&tcbpwmc->lock);
}

static int atmel_tcb_pwm_enable_patched(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tcb_pwm_device *tcbpwm = pwm_get_chip_data(pwm);
	struct atmel_tc *tc = tcbpwmc->tc;
	void __iomem *regs = tc->regs;
	unsigned group = pwm->hwpwm / 2;
  /* MATK : index should not be used - do the same on both A and B (pwm0 and pwm1) */
  // 	unsigned index = pwm->hwpwm % 2;
	u32 cmr;
	enum pwm_polarity polarity = tcbpwm->polarity;
  unsigned group_index;

  LOG_ENTER;

	/*
	 * If duty is 0 the timer will be stopped and we have to
	 * configure the output correctly on software trigger:
	 *  - set output to high if PWM_POLARITY_INVERSED
	 *  - set output to low if PWM_POLARITY_NORMAL
	 *
	 * This is why we're reverting polarity in this case.
	 */
	if (tcbpwm->duty == 0)
		polarity = !polarity;

	spin_lock(&tcbpwmc->lock);
	cmr = __raw_readl(regs + ATMEL_TC_REG(group, CMR));

	/* flush old setting and set the new one */
	cmr &= ~ATMEL_TC_TCCLKS;

  /* MATK : do the same on both A and B (pwm0 and pwm1)*/
  {
		cmr &= ~ATMEL_TC_ACMR_MASK;

		/* Set CMR flags according to given polarity */
		if (polarity == PWM_POLARITY_INVERSED)
			cmr |= ATMEL_TC_ASWTRG_CLEAR;
		else
			cmr |= ATMEL_TC_ASWTRG_SET;
	}

  {
		cmr &= ~ATMEL_TC_BCMR_MASK;
		if (polarity == PWM_POLARITY_INVERSED)
			cmr |= ATMEL_TC_BSWTRG_CLEAR;
		else
			cmr |= ATMEL_TC_BSWTRG_SET;
	}

	/*
	 * If duty is 0 or equal to period there's no need to register
	 * a specific action on RA/RB and RC compare.
	 * The output will be configured on software trigger and keep
	 * this config till next config call.
	 */
	if (tcbpwm->duty != tcbpwm->period && tcbpwm->duty > 0) {
    /* MATK : do the same on both A and B (pwm0 and pwm1)*/
		{
			if (polarity == PWM_POLARITY_INVERSED)
				cmr |= ATMEL_TC_ACPA_SET | ATMEL_TC_ACPC_CLEAR;
			else
				cmr |= ATMEL_TC_ACPA_CLEAR | ATMEL_TC_ACPC_SET;
		}
    {
			if (polarity == PWM_POLARITY_INVERSED)
				cmr |= ATMEL_TC_BCPB_SET | ATMEL_TC_BCPC_CLEAR;
			else
				cmr |= ATMEL_TC_BCPB_CLEAR | ATMEL_TC_BCPC_SET;
		}
	}

	cmr |= (tcbpwm->div & ATMEL_TC_TCCLKS);

  LOG_INFO("cmr(%d):0x%x\n", group, cmr);

	__raw_writel(cmr, regs + ATMEL_TC_REG(group, CMR));

  /* MATK : do the same on both A and B (pwm0 and pwm1)*/
  __raw_writel(tcbpwm->duty, regs + ATMEL_TC_REG(group, RA));
  __raw_writel(tcbpwm->duty, regs + ATMEL_TC_REG(group, RB));

	__raw_writel(tcbpwm->period, regs + ATMEL_TC_REG(group, RC));

	/* Use software trigger to apply the new setting */
	__raw_writel(ATMEL_TC_CLKEN | ATMEL_TC_SWTRG,
		     regs + ATMEL_TC_REG(group, CCR));

  /* MATK HACK */
  /* Enable SW1 and SW2 clock  */
  for (group_index = 1; group_index <= 2; group_index++) {
  		__raw_writel(ATMEL_TC_CLKEN,
                   regs + ATMEL_TC_REG(group_index, CCR));
  }

  // log for debug
  DUMP_REG(chip);

  spin_unlock(&tcbpwmc->lock);
	return 0;
}

static int atmel_tcb_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
				int duty_ns, int period_ns)
{
	struct atmel_tcb_pwm_chip *tcbpwmc = to_tcb_chip(chip);
	struct atmel_tcb_pwm_device *tcbpwm = pwm_get_chip_data(pwm);
	unsigned group = pwm->hwpwm / 2;
	unsigned index = pwm->hwpwm % 2;
	struct atmel_tcb_pwm_device *atcbpwm = NULL;
	struct atmel_tc *tc = tcbpwmc->tc;
	int i;
	int slowclk = 0;
	unsigned period;
	unsigned duty;
	unsigned rate = clk_get_rate(tc->clk[group]);
	unsigned long long min;
	unsigned long long max;
	unsigned long long divisor;

	/*
	 * Find best clk divisor:
	 * the smallest divisor which can fulfill the period_ns requirements.
	 */
	for (i = 0; i < 5; ++i) {
		if (atmel_tc_divisors[i] == 0) {
			slowclk = i;
			continue;
		}
		divisor = (u64)NSEC_PER_SEC * atmel_tc_divisors[i];
		min = div_u64(divisor, rate);
		max = min << tc->tcb_config->counter_width;
		if (max >= period_ns)
			break;
	}

	/*
	 * If none of the divisor are small enough to represent period_ns
	 * take slow clock (32KHz).
	 */
	if (i == 5) {
		i = slowclk;
		rate = clk_get_rate(tc->slow_clk);
		divisor = (u64)NSEC_PER_SEC;
		min = div_u64(divisor, rate);
		max = min << tc->tcb_config->counter_width;

		/* If period is too big return ERANGE error */
		if (max < period_ns)
			return -ERANGE;
	}

	duty = div_u64((u64)duty_ns*(u64)rate, divisor);
	period = div_u64((u64)period_ns*(u64)rate, divisor);

	if (index == 0)
		atcbpwm = tcbpwmc->pwms[pwm->hwpwm + 1];
	else
		atcbpwm = tcbpwmc->pwms[pwm->hwpwm - 1];

	/*
	 * PWM devices provided by TCB driver are grouped by 2:
	 * - group 0: PWM 0 & 1
	 * - group 1: PWM 2 & 3
	 * - group 2: PWM 4 & 5
	 *
	 * PWM devices in a given group must be configured with the
	 * same period_ns.
	 *
	 * We're checking the period value of the second PWM device
	 * in this group before applying the new config.
	 */

	if ((atcbpwm && atcbpwm->duty > 0 &&
			atcbpwm->duty != atcbpwm->period) &&
		(atcbpwm->div != i || atcbpwm->period != period)) {
		dev_err(chip->dev,
			"failed to configure period_ns: PWM group already configured with a different value\n");
		return -EINVAL;
	}

	tcbpwm->period = period;
	tcbpwm->div = i;
	tcbpwm->duty = duty;

	/* If the PWM is enabled, call enable to apply the new conf */
	if (pwm_is_enabled(pwm)) {
		atmel_tcb_pwm_enable_patched(chip, pwm);
  }

	return 0;
}



static const struct pwm_ops atmel_tcb_pwm_ops = {
	.request = atmel_tcb_pwm_request_patched,
	.free = atmel_tcb_pwm_free,
	.config = atmel_tcb_pwm_config,
	.set_polarity = atmel_tcb_pwm_set_polarity,
	.enable = atmel_tcb_pwm_enable_patched,
	.disable = atmel_tcb_pwm_disable_patched,
	.owner = THIS_MODULE,
};

static int atmel_tcb_pwm_probe(struct platform_device *pdev)
{
	struct atmel_tcb_pwm_chip *tcbpwm;
	struct device_node *np = pdev->dev.of_node;
	struct atmel_tc *tc;
	int err;
	int tcblock;

  LOG_ENTER;

	err = of_property_read_u32(np, "tc-block", &tcblock);
	if (err < 0) {
		dev_err(&pdev->dev,
			"failed to get Timer Counter Block number from device tree (error: %d)\n",
			err);
		return err;
	}

  LOG_INFO("tc-block:%d", tcblock);

	tc = atmel_tc_alloc(tcblock);
	if (tc == NULL) {
		dev_err(&pdev->dev, "failed to allocate Timer Counter Block\n");
		return -ENOMEM;
	}

	tcbpwm = devm_kzalloc(&pdev->dev, sizeof(*tcbpwm), GFP_KERNEL);
	if (tcbpwm == NULL) {
		err = -ENOMEM;
		dev_err(&pdev->dev, "failed to allocate memory\n");
		goto err_free_tc;
	}

	tcbpwm->chip.dev = &pdev->dev;
	tcbpwm->chip.ops = &atmel_tcb_pwm_ops;
	tcbpwm->chip.of_xlate = of_pwm_xlate_with_flags;
	tcbpwm->chip.of_pwm_n_cells = 3;
	tcbpwm->chip.base = -1;
	tcbpwm->chip.npwm = NPWM;
	tcbpwm->tc = tc;

	err = clk_prepare_enable(tc->slow_clk);
	if (err)
		goto err_free_tc;

	spin_lock_init(&tcbpwm->lock);

	err = pwmchip_add(&tcbpwm->chip);
	if (err < 0)
		goto err_disable_clk;

	platform_set_drvdata(pdev, tcbpwm);

	return 0;

err_disable_clk:
	clk_disable_unprepare(tcbpwm->tc->slow_clk);

err_free_tc:
	atmel_tc_free(tc);

	return err;
}

static int atmel_tcb_pwm_remove(struct platform_device *pdev)
{
	struct atmel_tcb_pwm_chip *tcbpwm = platform_get_drvdata(pdev);
	int err;

	clk_disable_unprepare(tcbpwm->tc->slow_clk);

	err = pwmchip_remove(&tcbpwm->chip);
	if (err < 0)
		return err;

	atmel_tc_free(tcbpwm->tc);

	return 0;
}

static const struct of_device_id atmel_tcb_pwm_dt_ids[] = {
	{ .compatible = "atmel,tcb-pwm-patched", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, atmel_tcb_pwm_dt_ids);

static struct platform_driver atmel_tcb_pwm_driver = {
	.driver = {
		.name = "atmel-tcb-pwm-patched",
		.of_match_table = atmel_tcb_pwm_dt_ids,
	},
	.probe = atmel_tcb_pwm_probe,
	.remove = atmel_tcb_pwm_remove,
};
module_platform_driver(atmel_tcb_pwm_driver);

MODULE_AUTHOR("Boris BREZILLON <b.brezillon@overkiz.com>");
MODULE_DESCRIPTION("Atmel Timer Counter Pulse Width Modulation Driver");
MODULE_LICENSE("GPL v2");
