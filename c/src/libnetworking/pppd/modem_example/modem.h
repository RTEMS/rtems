#ifndef _MODEM_H_
#define _MODEM_H_

void modem_reserve_resources(
								rtems_configuration_table * configuration
);

rtems_device_driver modem_initialize(
										rtems_device_major_number,
										rtems_device_minor_number,
										void *
);

rtems_device_driver modem_open(
								  rtems_device_major_number,
								  rtems_device_minor_number,
								  void *
);

rtems_device_driver modem_close(
								   rtems_device_major_number,
								   rtems_device_minor_number,
								   void *
);

rtems_device_driver modem_read(
								  rtems_device_major_number,
								  rtems_device_minor_number,
								  void *
);

rtems_device_driver modem_write(
								   rtems_device_major_number,
								   rtems_device_minor_number,
								   void *
);

rtems_device_driver modem_control(
									 rtems_device_major_number,
									 rtems_device_minor_number,
									 void *
);


#endif
