export:
	@printf 'MakeExport_SOURCES $(C_SOURCES) $(ASM_SOURCES)\n'
	@printf 'MakeExport_MCU_Flags $(MCU)\n'
	@printf 'MakeExport_DEFS $(AS_DEFS) $(C_DEFS)\n'
	@printf 'MakeExport_INCLUDES $(AS_INCLUDES) $(C_INCLUDES)\n'
	@printf 'MakeExport_LDSCRIPT $(LDSCRIPT)\n'