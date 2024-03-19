############################## Setting up Project Variables ##############################


##############just trying #########################
TARGET := hw
PLATFORM ?= xilinx_u200_gen3x16_xdma_1_202110_1
HOST_SRC+=src/hostalt.cpp common/xcl2.cpp
EXECUTABLE=src/hostalt
XCLBIN=krnl_src/matmul.sw_emu.xclbin
KRNL_SRC+=krnl_src/matmul_it.cpp krnl_src/matmul_it.h
KRNL_NAME=matmul_it
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
