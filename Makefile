############################## Setting up Project Variables ##############################
#Run make run to run with the matmul kernel and iterate in the host code. Run matmul_alt to run the matmul_it kernel and iterate in the kernel code

#KRNL_NAME decides the name of the xclbin file. The top_function_name variable decides which kernel we use

##############just trying #########################
TARGET := hw
PLATFORM ?= xilinx_u200_gen3x16_xdma_1_202110_1
HOST_SRC+=src/hostcl.cpp common/xcl2.cpp
EXECUTABLE=src/host2
KRNL_SRC+=krnl_src/matmul.cpp krnl_src/matmul.h
KRNL_NAME=matmul
TOP_FUNCTION_NAME=matmul

HOST_SRC_ALT+=src/host.cpp common/xcl2.cpp 
EXECUTABLE_ALT=src/largemult
KRNL_SRC_ALT=krnl_src/large_mult.cpp 
KRNL_NAME_ALT=mmul_256_ch
TOP_FUNCTION_NAME_ALT=mmul
build:
	v++ -t $(TARGET) --platform $(PLATFORM) -c -g -k $(TOP_FUNCTION_NAME) -I ./src -o $(KRNL_NAME).$(TARGET).xo $(KRNL_SRC)
link:
	v++ -t $(TARGET) --platform $(PLATFORM) --link $(KRNL_NAME).$(TARGET).xo -o $(KRNL_NAME).$(TARGET).xclbin
all: build link
runall: all
	emconfigutil --platform $(PLATFORM)
	cp emconfig.json src
	cp xrt.ini src
	g++ $(HOST_SRC) -o $(EXECUTABLE) -I /opt/xilinx/xrt/include/ -I common -L /opt/xilinx/xrt/lib/ -pthread -lOpenCL -lrt -lstdc++
ifneq ($(TARGET),hw)
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin $(TOP_FUNCTION_NAME)
else
	./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin $(TOP_FUNCTION_NAME)	
endif

compile:
	emconfigutil --platform $(PLATFORM)
	cp emconfig.json src
	cp xrt.ini src
	g++ $(HOST_SRC) -o $(EXECUTABLE) -I /opt/xilinx/xrt/include/ -I common -L /opt/xilinx/xrt/lib/ -pthread -lOpenCL -lrt -lstdc++
	
run:	compile
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin $(TOP_FUNCTION_NAME)
else
	./$(EXECUTABLE) $(KRNL_NAME).$(TARGET).xclbin $(TOP_FUNCTION_NAME)
endif




build_alt:
	v++ -t $(TARGET) --platform $(PLATFORM) -c -g -k $(TOP_FUNCTION_NAME_ALT) -I ./src -o $(KRNL_NAME_ALT).$(TARGET).xo $(KRNL_SRC_ALT)
link_alt:
	v++ -t $(TARGET) --platform $(PLATFORM) --link $(KRNL_NAME_ALT).$(TARGET).xo -o $(KRNL_NAME_ALT).$(TARGET).xclbin
all_alt: build_alt link_alt
runall_alt: all_alt
	emconfigutil --platform $(PLATFORM)
	cp emconfig.json src
	cp xrt.ini src
	g++ $(HOST_SRC_ALT) -o $(EXECUTABLE_ALT) -I /opt/xilinx/xrt/include/ -I common -L /opt/xilinx/xrt/lib/ -pthread -lOpenCL -lrt -lstdc++
ifneq ($(TARGET),hw)
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE_ALT) $(KRNL_NAME_ALT).$(TARGET).xclbin $(TOP_FUNCTION_NAME_ALT)
else
	./$(EXECUTABLE_ALT) $(KRNL_NAME_ALT).$(TARGET).xclbin $(TOP_FUNCTION_NAME_ALT)
endif
compile_alt:
	emconfigutil --platform $(PLATFORM)
	cp emconfig.json src
	cp xrt.ini src
	g++ $(HOST_SRC_ALT) -o $(EXECUTABLE_ALT) -I /opt/xilinx/xrt/include/ -I common -L /opt/xilinx/xrt/lib/ -pthread -lOpenCL -lrt -lstdc++

run_alt:	compile_alt
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE_ALT) $(KRNL_NAME_ALT).$(TARGET).xclbin $(TOP_FUNCTION_NAME_ALT)
else
	./$(EXECUTABLE_ALT) $(KRNL_NAME_ALT).$(TARGET).xclbin $(TOP_FUNCTION_NAME_ALT)
endif
			
clean:
	rm *.log
	rm *.link_summary
	rm *.compile_summary
