all: I2GMM

clean depend generated realclean check-syntax $(CUSTOM_TARGETS):
	@$(MAKE) -f Makefile.I2GMM $(@)

.PHONY: I2GMM
I2GMM:
	@$(MAKE) -f Makefile.I2GMM all

project_name_list:
	@echo I2GMM
