#ifndef LIBBSP_ARM_BEAGLE_BBB_PWM_H
#define LIBBSP_ARM_BEAGLE_BBB_PWM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief  BeagleBone Black PWM Macros.
 */
#define BBBIO_PWMSS_COUNT       3
#define BBBIO_PWMSS0           0
#define BBBIO_PWMSS1           1
#define BBBIO_PWMSS2           2

#define MUXMODE0               0
#define MUXMODE1                1
#define MUXMODE2               2
#define MUXMODE3               3
#define MUXMODE4               4
#define MUXMODE5               5
#define MUXMODE6               6
#define MUXMODE7               7

#define EPWM_GROUP1    1
#define EPWM_GROUP2    2
#define EPWM_GROUP0    0

int BBBIO_PWMSS_Setting(unsigned int PWMID , float HZ ,float dutyA ,float dutyB);
int BBBIO_PWM_Init();
void BBBIO_PWM_Release();
int BBBIO_PWMSS_Status(unsigned int PWMID);
void BBBIO_ehrPWM_Enable(unsigned int PWMSS_ID);
void BBBIO_ehrPWM_Disable(unsigned int PWMSS_ID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BBB_PWM_H */
