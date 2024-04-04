#include <vector>
#include <algorithm>
#include <chrono>
#include "xcl2.hpp"

#define ROWA 256
#define COLA 256
#define COLB 256
#define iter 50

void verify(float* hw_out,float* sw_result,int size);
void calcsw(float* in1,float* in2,float* sw_result,int size);

int main(int argc,char** argv){



static const int DATA_SIZE = 256;
size_t size_in_bytes = DATA_SIZE * sizeof(int);
size_t matrix_size = DATA_SIZE * DATA_SIZE;
size_t matrix_size_in_bytes = matrix_size * sizeof(int);

cl_int err;
unsigned nb;
/*std::vector<cl::Platform> platforms1;
OCL_CHECK(err, err = cl::Platform::get(&platforms1));				//get_xil_devices function explained
platforms1[1].getDevices(CL_DEVICE_TYPE_ACCELERATOR,&dev);
std::cout<<dev[0].getInfo<CL_DEVICE_NAME>()<<std::endl;*/


// xclbin specified in commandline
std::string binaryfile = argv[1];			
std::vector<cl::Device> dev;

/////Get all xilinx devices
auto devices = xcl::get_xil_devices();
for(int i = 0; i < devices.size(); i++){
	std::cout << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
}
////////Read binary file and point to its buffer
auto filebuf = xcl::read_binary_file(binaryfile,nb);
cl::Context context;
//cl::Program::Binaries bins;				//used if bins.push_back is needed
cl::Program program;
cl::CommandQueue q;
cl::Kernel kernel_matmul;
cl::Buffer buffera,bufferb,bufferout;
int rowa = ROWA;
int cola = COLA;
int colb = COLB;
float* ptr_a;
float* ptr_b;
float* ptr_out;

bool device_found = false;
cl::Program::Binaries bins{{filebuf.data(), filebuf.size()}};			
//bins.push_back({filebuf,nb});							//not working
for(unsigned int i = 0;i<devices.size();i++){
	auto device = devices[i];
	context = cl::Context(device,nullptr,nullptr,nullptr,&err);
	q = cl::CommandQueue(context,device,CL_QUEUE_PROFILING_ENABLE,&err);
	program = cl::Program(context,{device},bins,nullptr,&err);

	if(err != CL_SUCCESS){
		std::cout<<"Unable to program device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
}	
	else{
		std::cout<<"Programming of device " << device.getInfo<CL_DEVICE_NAME>() << " succesful" << std::endl;
		kernel_matmul = cl::Kernel(program,argv[2],&err);//argv[2] is the name of the kernel top function specified in the makefile
		std::cout << "The name of the kernel top function is " << kernel_matmul.getInfo<CL_KERNEL_FUNCTION_NAME>() << std::endl;
break;
}

}

buffera = cl::Buffer(context,CL_MEM_READ_ONLY,matrix_size_in_bytes,nullptr,&err);			//kernel buffer	
bufferb = cl::Buffer(context,CL_MEM_READ_ONLY,matrix_size_in_bytes,nullptr,&err);
bufferout = cl::Buffer(context,CL_MEM_WRITE_ONLY,matrix_size_in_bytes,nullptr,&err);

ptr_a = (float*) q.enqueueMapBuffer(buffera,CL_TRUE,CL_MAP_WRITE,0,matrix_size_in_bytes,nullptr,nullptr,&err);	//host pointers
ptr_b = (float*) q.enqueueMapBuffer(bufferb,CL_TRUE,CL_MAP_WRITE,0,matrix_size_in_bytes,nullptr,nullptr,&err);
ptr_out = (float*) q.enqueueMapBuffer(bufferout,CL_TRUE,CL_MAP_READ,0,matrix_size_in_bytes,nullptr,nullptr,&err);


///////// Initialise pointers //////////
for(int i = 0; i < matrix_size ; i++){
	ptr_a[i] =rand() % 256;
	ptr_b[i] = rand() % 256;
	
}
		

////// Set kernel arguments /////////
int narg=0;
kernel_matmul.setArg(narg++,buffera);						
kernel_matmul.setArg(narg++,bufferb);
kernel_matmul.setArg(narg++,bufferout);
kernel_matmul.setArg(narg++,rowa);
kernel_matmul.setArg(narg++,cola);
kernel_matmul.setArg(narg++,colb);








////// Measure time for the whole fpga execution ////////

auto fpga_begin = std::chrono::high_resolution_clock::now();

/////// Repeat multiplication for iter iterations
for(int o = 0; o < iter; o++){
//////// Transfer buffer objects to the device //////////
	q.enqueueMigrateMemObjects({buffera,bufferb},0);//CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED);

	cl::Event event;

//////// Start kernel /////////
	q.enqueueNDRangeKernel(kernel_matmul,1,1,1,nullptr,&event);		//enqueueTask is deprecated, enqueueNDRangeKernel equivalent
//q.enqueueTask(kernel_matmul);
	q.finish();

/////// send data back to host //////////
	q.enqueueMigrateMemObjects({bufferout},CL_MIGRATE_MEM_OBJECT_HOST);



	q.finish();		//wait until all previous commands have been executed

}
auto fpga_end = std::chrono::high_resolution_clock::now();



////// print hardware result //////////
	/*std::cout << "printing Hardware results" << std::endl;
	float temp = 0;
	for(int i=0;i < DATA_SIZE; i++){
		for(int j = 0; j < DATA_SIZE; j++){
			std::cout<<ptr_out[i * DATA_SIZE + j]<<" ";
}
	std::cout << std::endl;
		if(temp==colb-1){
			std::cout<<std::endl;
			temp=0;
}
		else{
			temp++;
}
		if(i==rowa*colb-1){
			std::cout<<std::endl;
			break;
}
}*/



//}





//////////// Calculate software result ////////////

auto cpu_begin = std::chrono::high_resolution_clock::now();
float host_result[matrix_size];
for(int p = 0; p < iter; p++){

calcsw(ptr_a,ptr_b,host_result,DATA_SIZE);
}
auto cpu_end = std::chrono::high_resolution_clock::now();
verify(ptr_out,host_result,DATA_SIZE);


///////print software data if needed /////////
for(int i = 0; i < DATA_SIZE; i++){
	for(int j = 0; j < DATA_SIZE; j++){
		//std::cout << host_result[i * DATA_SIZE + j] << " ";
	}
	//std::cout << std::endl;
}
q.enqueueUnmapMemObject(buffera,ptr_a,nullptr,nullptr);
q.enqueueUnmapMemObject(bufferb,ptr_b,nullptr,nullptr);
q.enqueueUnmapMemObject(bufferout,ptr_out,nullptr,nullptr);

////////// Performance output /////////////
long int total = (rowa * colb * (2 * cola - 1)) * iter * sizeof(int);
std::cout<<"Total calculations: " << total << std::endl;
std::chrono::duration<double> cpu_duration = cpu_end - cpu_begin;
std::cout << "CPU duration:" << cpu_duration.count() << " seconds" << std::endl;
std::cout << "CPU Throughput:" << (total/(float)cpu_duration.count())/(1024*1024) << " MB/s" << std::endl;

std::chrono::duration<double> fpga_duration = fpga_end - fpga_begin;
std::cout << "FPGA duration:" << fpga_duration.count() << std::endl;
std::cout << "Throughput:" << (total/fpga_duration.count())/(1024*1024) << " MB/s" << std::endl;







////////////// Alternative test way to measure fpga speed ///////////////////

cl::Event event1;
uint64_t nstimestart, nstimeend;
double fpga_exec_time_s = 0;
auto duration_nanosec = 0;

  for (int i = 0; i < iter; i++)////// execution time results for iter runs
  { 
  //q.enqueueTask(kernel_matmul, NULL, &event1));
  q.enqueueNDRangeKernel(kernel_matmul,1,1,1,nullptr,&event1);
  q.finish();
  event1.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart);
  event1.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend);
  duration_nanosec = nstimeend - nstimestart;
  fpga_exec_time_s += double((duration_nanosec * (1.0e-9))); // conversion to seconds
  }
std::cout << "alt_Fpga exec time: " << fpga_exec_time_s << " seconds" << std::endl;
std::cout << "alt_throughput:" << (total/fpga_exec_time_s)/(1024 * 1024) << " MB/s " << std::endl;
std::cout << "Speedup:" << cpu_duration.count()/fpga_exec_time_s << std::endl;






return 0;

}


void verify(float* hw_out,float* sw_result,int size){

	for(int i = 0; i < size; i++ ){
		for(int j = 0; j < size; j++){
			if(sw_result[i * size + j] != hw_out[i * size + j]) {
			        std::cout << "Mismatch: ";
			        std::cout << "-----Hardware result = " << hw_out[i * size + j];
			        std::cout << "-----Host_result = " << sw_result[i * size + j];
			        std::cout << std::endl;
			}
		}
	}
}

void calcsw(float* in1,float* in2,float* sw_result,int size){

	for(int i = 0; i < size * size; i++){
		sw_result[i] = 0;
	}

	for(int i = 0;i < size;i++){
		for(int j = 0; j < size;j++){
			sw_result[i * size + j] = 0;
			for(int z = 0; z < size; z++){
				sw_result[i * size + j] += in1[i * size + z] * in2[z * size + j];
		
			}
		}
	}






}











 
