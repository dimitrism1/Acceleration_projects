#include <vector>
#include <algorithm>
#include <chrono>
#include "xcl2.hpp"


#define ROWA 16
#define COLA 16
#define COLB 16
#define iter 1
#define repcounts 2000000

//int verify(int host_result[],int kernel_result[])

int main(int argc,char** argv){



static const int DATA_SIZE=64;
size_t size_in_bytes=DATA_SIZE*sizeof(int);
size_t matrix_size=DATA_SIZE*DATA_SIZE;
size_t matrix_size_in_bytes=matrix_size*sizeof(int);

cl_int err;
unsigned nb;
/*std::vector<cl::Platform> platforms1;
OCL_CHECK(err, err = cl::Platform::get(&platforms1));				//get_xil_devices function explained
platforms1[1].getDevices(CL_DEVICE_TYPE_ACCELERATOR,&dev);
std::cout<<dev[0].getInfo<CL_DEVICE_NAME>()<<std::endl;*/


// xclbin specified in commandline
std::string binaryfile=argv[1];			
std::vector<cl::Device> dev;

/////Get all xilinx devices
auto devices = xcl::get_xil_devices();
for(int i=0;i<devices.size();i++){
	std::cout<<devices[i].getInfo<CL_DEVICE_NAME>()<<std::endl;
}
////////Read binary file and point to its buffer
auto filebuf = xcl::read_binary_file(binaryfile,nb);
cl::Context context;
//cl::Program::Binaries bins;				//used if bins.push_back is needed
cl::Program program;
cl::CommandQueue q;
cl::Kernel kernel_matmul;
cl::Buffer buffera,bufferb,bufferout;
int rowa=ROWA;
int cola=COLA;
int colb=COLB;
int* ptr_a;
int* ptr_b;
int* ptr_out;

bool device_found = false;
cl::Program::Binaries bins{{filebuf.data(), filebuf.size()}};			
//bins.push_back({filebuf,nb});							//not working
for(unsigned int i = 0;i<devices.size();i++){
	auto device = devices[i];
	context = cl::Context(device,nullptr,nullptr,nullptr,&err);
	q = cl::CommandQueue(context,device,CL_QUEUE_PROFILING_ENABLE,&err);
	program = cl::Program(context,{device},bins,nullptr,&err);

	if(err != CL_SUCCESS){
		std::cout<<"Unable to program device "<<device.getInfo<CL_DEVICE_NAME>()<<std::endl;
}	
	else{
		std::cout<<"Programming of device "<<device.getInfo<CL_DEVICE_NAME>()<<" succesful"<<std::endl;
		kernel_matmul = cl::Kernel(program,"matmul_it",&err);	//matmul is the name of the kernel top function
		std::cout<<kernel_matmul.getInfo<CL_KERNEL_FUNCTION_NAME>()<<std::endl;
break;
}

}

buffera = cl::Buffer(context,CL_MEM_READ_ONLY,matrix_size_in_bytes,nullptr,&err);			//kernel buffer	
bufferb = cl::Buffer(context,CL_MEM_READ_ONLY,matrix_size_in_bytes,nullptr,&err);
bufferout = cl::Buffer(context,CL_MEM_WRITE_ONLY,matrix_size_in_bytes,nullptr,&err);

////// Set kernel arguments /////////
int narg=0;
kernel_matmul.setArg(narg++,buffera);						
kernel_matmul.setArg(narg++,bufferb);
kernel_matmul.setArg(narg++,bufferout);
kernel_matmul.setArg(narg++,rowa);
kernel_matmul.setArg(narg++,cola);
kernel_matmul.setArg(narg++,colb);
kernel_matmul.setArg(narg++,repcounts);



ptr_a=(int*)q.enqueueMapBuffer(buffera,CL_TRUE,CL_MAP_WRITE,0,matrix_size_in_bytes,nullptr,nullptr,&err);	//host pointers
ptr_b=(int*)q.enqueueMapBuffer(bufferb,CL_TRUE,CL_MAP_WRITE,0,matrix_size_in_bytes,nullptr,nullptr,&err);
ptr_out=(int*)q.enqueueMapBuffer(bufferout,CL_TRUE,CL_MAP_READ,0,matrix_size_in_bytes,nullptr,nullptr,&err);


////// Measure time for the whole fpga execution ////////
clock_t fpga_start,fpga_clk;
fpga_start=clock();
///////// Initialise pointers //////////
	for(int i = 0; i < matrix_size ; i++){
		ptr_a[i]=1;
		ptr_b[i]=1;
}

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




////// print hardware result //////////
	std::cout<<"printing Hardware results"<<std::endl;
	int temp=0;
	for(int i=0;i<size_in_bytes;i++){

		std::cout<<ptr_out[i]<<" ";
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

}

q.enqueueUnmapMemObject(buffera,ptr_a,nullptr,nullptr);
q.enqueueUnmapMemObject(bufferb,ptr_b,nullptr,nullptr);

fpga_clk=clock()-fpga_start;
float fpga_time = (float)fpga_clk/(CLOCKS_PER_SEC);

//////////// Calculate software result ////////////
clock_t cpu_start,cpu_time;
cpu_start=clock();

int host_result[rowa*colb];
for(int p = 0; p<repcounts; p++){
	//std::cout<<"Printing software results"<<std::endl;
	for(int i=0;i<rowa*colb;i++){
		host_result[i] = 0;
}
	int temph=0;
	for(int i = 0;i < rowa;i++){
		for(int j = 0; j < colb;j++){

			for(int z = 0; z < cola; z++){
				host_result[i*colb + j]+=ptr_a[i*cola + z]*ptr_b[z*colb + j];

		
}
		
		/*std::cout<<host_result[i*colb + j]<<" ";
		if(temph == colb-1) {
		std::cout<<std::endl;
		temph = 0;
		}
		else	{
		temph++;
		}*/
}
}
}
std::cout<<std::endl;	
cpu_time=clock()-cpu_start;
////////// Performance output /////////////
auto cpu_duration = (float)cpu_time/(CLOCKS_PER_SEC);
std::cout<<"CPU duration:"<<(float)cpu_time/(CLOCKS_PER_SEC)<<" seconds"<<std::endl;
//int total=(2*rowa*cola*colb-rowa*colb)*iter*sizeof(int);
long int total = (rowa*cola*colb ) * repcounts * sizeof(int);
std::cout<<"Total calculations= "<<total<<std::endl;
std::cout << "FPGA duration:" << fpga_time << std::endl;
std::cout << "Throughput:" << (total/fpga_time)/(1024*1024) << " MB/s" << std::endl;
std::cout << "CPU Throughput:" << (total/(float)cpu_duration)/(1024*1024) << " MB/s" << std::endl;
//std::cout<<"Speedup is "<<(double)start_time.count()/(double)fpga_time.count()<<std::endl;




////////////// Alternative test way to measure fpga speed ///////////////////

//cl::Event event;
  uint64_t nstimestart, nstimeend;
  double fpga_exec_time_s=0;
  double avg_fpga_exec_time=0;
  auto duration_nanosec=0;
  for (int i = 0; i < iter; i++)//averaging execution time results for iter runs
  { 
  OCL_CHECK(err, err = q.enqueueTask(kernel_matmul, NULL, &event));
  OCL_CHECK(err, err = q.finish());
  OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(
                     CL_PROFILING_COMMAND_START, &nstimestart));
  OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(
                     CL_PROFILING_COMMAND_END, &nstimeend));
  duration_nanosec = nstimeend - nstimestart;
  fpga_exec_time_s += double((duration_nanosec * (1.0e-9))); // conversion to seconds
  }
  avg_fpga_exec_time = fpga_exec_time_s/iter;
std::cout<<"alt_Fpga exec time: "<<fpga_exec_time_s<<" seconds"<<std::endl;
std::cout<<"alt_throughput:"<<(total/fpga_exec_time_s)/(1024*1024)<<" MB/s "<<std::endl;


return 0;

}





 
