RTEMS_INLINE_ROUTINE boolean _ISR_Is_in_progress( void )
{
	return (_ISR_Nest_level != 0);
}
