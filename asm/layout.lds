
SECTIONS
{
    
	.real_mode 0x7c00:
    {
	  *(.text.real_mode)
	}

	.protected_mode 0x9000:
	{
		*(.text.protected_mode)
	}

    .long_mode 0x10000:
    {
		*(.text.long_mode)
	} 
}