################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pc_lab2_openmp.cpp 

OBJS += \
./src/pc_lab2_openmp.o 

CPP_DEPS += \
./src/pc_lab2_openmp.d 


# Each subdirectory must supply rules for building sources it contributes
src/pc_lab2_openmp.o: ../src/pc_lab2_openmp.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/pc_lab2_openmp.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


