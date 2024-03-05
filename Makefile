#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.3
#

############################## Help Section ##############################
ifneq ($(findstring Makefile, $(MAKEFILE_LIST)), Makefile)
help:
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host"
	$(ECHO) "      Command to build host application."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

endif

############################## Setting up Project Variables ##############################


##############just trying #########################
TARGET := hw
PLATFORM ?= xilinx_u200_gen3x16_xdma_1_202110_1
HOST_SRC+=src/hostcl.cpp common/xcl2.cpp
EXECUTABLE=src/host2
XCLBIN=krnl_src/matmul.sw_emu.xclbin
KRNL_SRC+=krnl_src/matmul.cpp krnl_src/matmul.h
KRNL_NAME=matmul
build:
	v++ -t $(TARGET) --platform $(PLATFORM) -c -g -k $(KRNL_NAME) -I ./src -o $(KRNL_NAME).$(TARGET).xo $(KRNL_SRC)
link:
	v++ -t $(TARGET) --platform $(PLATFORM) --link $(KRNL_NAME).$(TARGET).xo -o $(KRNL_NAME).$(TARGET).xclbin
all: build link
runall: all
	emconfigutil --platform $(PLATFORM)
	cp emconfig.json src
	cp xrt.ini src
	g++ $(HOST_SRC) -o $(EXECUTABLE) -I /opt/xilinx/xrt/include/ -I common -L /opt/xilinx/xrt/lib/ -pthread -lOpenCL -lrt -lstdc++
ifneq ($(TARGET),hw)
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin
else
	./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin	
endif
#ifeq 	($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	#XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin
#else
	#./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin
#endif
compile:
	emconfigutil --platform $(PLATFORM)
	cp emconfig.json src
	cp xrt.ini src
	g++ $(HOST_SRC) -o $(EXECUTABLE) -I /opt/xilinx/xrt/include/ -I common -L /opt/xilinx/xrt/lib/ -pthread -lOpenCL -lrt -lstdc++
	
run:	compile
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin
else
	./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin
endif
		
clean:
	rm *.log
	rm *.link_summary
	rm *.compile_summary
