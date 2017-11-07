################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sources/board.c \
../sources/clock_config.c \
../sources/main.c \
../sources/pin_mux.c \
../sources/trcBase.c \
../sources/trcHardwarePort.c \
../sources/trcKernel.c \
../sources/trcKernelPortFreeRTOS.c \
../sources/trcUser.c 

OBJS += \
./sources/board.o \
./sources/clock_config.o \
./sources/main.o \
./sources/pin_mux.o \
./sources/trcBase.o \
./sources/trcHardwarePort.o \
./sources/trcKernel.o \
./sources/trcKernelPortFreeRTOS.o \
./sources/trcUser.o 

C_DEPS += \
./sources/board.d \
./sources/clock_config.d \
./sources/main.d \
./sources/pin_mux.d \
./sources/trcBase.d \
./sources/trcHardwarePort.d \
./sources/trcKernel.d \
./sources/trcKernelPortFreeRTOS.d \
./sources/trcUser.d 


# Each subdirectory must supply rules for building sources it contributes
sources/%.o: ../sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -Wall  -g -DDEBUG -DCPU_MK64FN1M0VLL12 -DFSL_RTOS_FREE_RTOS -DFRDM_K64F -DFREEDOM -I../CMSIS/Include -I../devices -I../drivers -I../freertos/Source/include -I../freertos/Source/portable/GCC/ARM_CM4F -I../freertos/Source -I../sources -I../utilities -std=gnu99 -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


