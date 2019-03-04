/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Variable for the definition of the default graphical mode to be
 *     initialized.
 */

/*
 * @brief Allows to enable initialization of specific framebuffer driver (e.g.
 * VESA real mode) from an application by setting the value of this variable
 * to non null value in user's module. The value of this variable will be then
 * updated when linked with an application's object.
 *
 * Further the value should point to string in the following format:
 * "<resX>x<resY>[-<bpp>]" - e.g. "1024x768-32"
 * "auto" - select the graphic mode automatically
 * "none" / "off" - do not initialize the driver
 * the given parameters are used if applicable.
 *
 * Command line argument "--video=" has priority over this string if
 * it is read/implemented by the driver.
 */
extern const char * const rtems_fb_default_mode;
