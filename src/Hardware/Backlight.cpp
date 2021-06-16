#include "Hardware/Backlight.hpp"

Backlight::Backlight(pwm_channel_t *pwm,
		uint32_t pwmFrequency, uint32_t frequency,
		uint32_t dimBrightness, uint32_t normalBrightness,
		uint32_t minDuty, uint32_t maxDuty)
{
	//assert(backlight);
	//assert(channel);
	//assert(pwmClockFrequency >= frequency);

	this->pwm = pwm;

	this->frequency = frequency;
	this->period = pwmFrequency / frequency;

	this->dimBrightness = dimBrightness;
	this->normalBrightness = normalBrightness;

	this->minDuty = minDuty;
	this->maxDuty = maxDuty;

	this->state = BacklightStateOff;

	this->pwm->ul_period = this->period;
	this->pwm->ul_duty = 0;

	// backlight pwm pin
	pio_configure(PIOB, PIO_PERIPH_A, PIO_PB1, 0);				// enable HI output to backlight, but not to piezo yet

	pwm_channel_init(PWM, this->pwm);
	pwm_channel_disable(PWM, this->pwm->channel);
}

void Backlight::SetBrightness(uint32_t brightness)
{
#define BACKLIGHT_BRIGHTNESS_MAX 100

	if (brightness == 0)
	{
		pwm_channel_disable(PWM, this->pwm->channel);
		return;
	}

	this->pwm->ul_period = this->period;
	this->pwm->ul_duty = this->minDuty + brightness * this->maxDuty / BACKLIGHT_BRIGHTNESS_MAX;
	if (this->pwm->ul_duty > this->maxDuty)
	{
		this->pwm->ul_duty = this->maxDuty;
	}

	pwm_channel_init(PWM, this->pwm);
	pwm_channel_enable(PWM, this->pwm->channel);
}

void Backlight::SetState(enum BacklightState state)
{
	//assert(backlight);
	//assert(backlight->pwm);
	//assert(backlight->maxBrightness >= brightness);

	//if (this->state == state)
		//return;

	uint32_t brightness = 100;

	switch (state)
	{
	case BacklightStateOff:
		brightness = 0;
		break;
	case BacklightStateDimmed:
		brightness = this->dimBrightness;
		break;
	case BacklightStateNormal:
		brightness = this->normalBrightness;
		break;
	default:
		// TODO throw exception
		break;
	}

	this->state = state;
	this->SetBrightness(brightness);
}

enum BacklightState Backlight::GetState()
{
	return this->state;
}

void Backlight::SetNormalBrightness(uint32_t normalBrightness)
{
	// TODO add checks?
	this->normalBrightness = normalBrightness;
}

void Backlight::SetDimBrightness(uint32_t dimBrightness)
{
	// TODO add checks?
	this->dimBrightness = dimBrightness;
}
