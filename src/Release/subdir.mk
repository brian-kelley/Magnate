################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Atlas.cpp \
../Building.cpp \
../Button.cpp \
../Chunk.cpp \
../Component.cpp \
../Constants.cpp \
../Control.cpp \
../Coord.cpp \
../Cuboid.cpp \
../DirManager.cpp \
../Field.cpp \
../Label.cpp \
../Model.cpp \
../Scene.cpp \
../ScrollBlock.cpp \
../View.cpp \
../World.cpp \
../main.cpp 

OBJS += \
./Atlas.o \
./Building.o \
./Button.o \
./Chunk.o \
./Component.o \
./Constants.o \
./Control.o \
./Coord.o \
./Cuboid.o \
./DirManager.o \
./Field.o \
./Label.o \
./Model.o \
./Scene.o \
./ScrollBlock.o \
./View.o \
./World.o \
./main.o 

CPP_DEPS += \
./Atlas.d \
./Building.d \
./Button.d \
./Chunk.d \
./Component.d \
./Constants.d \
./Control.d \
./Coord.d \
./Cuboid.d \
./DirManager.d \
./Field.d \
./Label.d \
./Model.d \
./Scene.d \
./ScrollBlock.d \
./View.d \
./World.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


