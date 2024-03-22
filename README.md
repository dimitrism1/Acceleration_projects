






The project includes 3 kernels, matmul, matmul_it and matmul_cor. 

The matmul kernel can multiply arrays of up to 16x16 size. The host code that goes with it is hostcl.cpp and can be selected for compilation in the Makefile. 
The host code can execute the kernels many times and measure the throughput this way. The throughput for this kernel is quite low in comparison to the CPU 
through put

The matmul_it kernel iterates many times through the matrix multiplication in the kernel code instead of in the host code as the above kernel. This achieves a max higher throughput, but limits the flexibility of the matrix multiplications we can execute

To execute an acceleration project from start to finish the option make runall should be executed in the command line. It compiles and links the kernel, creates the xclbin which is then executed in the host code with the g++ compiler. If the xclbin is already built use the make run option in the command line to directly run the host code. If any change is made to the kernel code it has to be ran from the start again. 

To select whether the code will run in software emulation, hardware emulation or directly on the hardware card change the target in the makefile to sw_emu, hw_emu or hw respectively. There is a different variable for the KRNL_NAME and TOP_FUNCTION_NAME in case you want to create a second xclbin from the same kernel only slightly modified, without having to create a new HLS file and change all the Makefile variables. It makes it easier to compare slight changes in the kernel. The KRNL_NAME variable determines the name of the xclbin file that will be created and can be freely chosen. The TOP_FUNCTION_NAME variable obviously must be named after the top function of the kernel we need to use, it is then passed as an argument when the host code is executed and used in the host code to launch the kernel with OPENCL.

The alt variables are used to compile and link the second kernel, matmul_it. These variables can be used for each new kernel we want to use instead of editing the main ones. The options are the same, run_alt and runall_alt.
