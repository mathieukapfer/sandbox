#include <stdint.h>

/* AT91 has these divide MCK */
const uint8_t atmel_tc_divisors[5] = { 2, 8, 32, 128, 0, };
#define NSEC_PER_SEC	1000000000L

int find_best_block(uint64_t period_ns) {
  uint64_t min, max, divisor = 0;
  int i, slowclk;
  /*
	 * Find best clk divisor:
	 * the smallest divisor which can fulfill the period_ns requirements.
	 */
	for (i = 0; i < 5; ++i) {
		if (atmel_tc_divisors[i] == 0) {
			slowclk = i;
			continue;
		}
		divisor = (uint64_t)NSEC_PER_SEC * atmel_tc_divisors[i];
		min = div_u64(divisor, rate);
		max = min << tc->tcb_config->counter_width;
		if (max >= period_ns)
			break;
	}

  return divisor;
}

int main(int argc, char *argv[]) {
  uint64_t period = -1;
  while (divisor != 0) {
    scanf("Period:%d\n", period);
    printf("Period:%d => Divisor:%d\n", period, find_best_block(divisor));
  }
  return 0;
}
