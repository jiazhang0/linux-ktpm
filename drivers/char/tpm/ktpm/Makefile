#
# Makefile for kTPM driver
#

obj-$(CONFIG_TCG_KTPM) += tpm_ktpm.o

ccflags-$(CONFIG_TCG_KTPM_TPM2) += -I$(src)/tpm2

tpm_ktpm-y := ktpm.o
tpm_ktpm-$(CONFIG_TCG_KTPM_TPM2) += \
	tpm2/cmd.o \
	tpm2/startup.o \
	tpm2/testing.o \
	tpm2/pcr.o \
	tpm2/capability.o \
	tpm2/state.o
