################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/can.c \
../src/main.c \
../src/spi.c \
../src/st7789.c \
../src/ui.c 

OBJS += \
./src/can.o \
./src/main.o \
./src/spi.o \
./src/st7789.o \
./src/ui.o 

C_DEPS += \
./src/can.d \
./src/main.d \
./src/spi.d \
./src/st7789.d \
./src/ui.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/can.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


