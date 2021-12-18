LIBS:=${STAGING_DIR}/target-aarch64_cortex-a53_musl/root-bcm27xx/lib/
#Adding the dynamic libraries for ubox and ubus like -lubox and -lubus will solve problem for
#error api resolution in package build it should be set as dependency in package makefile but
#need to attach here to make the compilation work fine to create a executable

ubus_object: ./src/ubus_object.c
	$(CC) ${CFLAGS} -o ubus_object ./src/ubus_object.c -lubus -lubox
