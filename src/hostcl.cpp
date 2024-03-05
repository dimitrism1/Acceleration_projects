#include <vector>
#include <algorithm>
#include <chrono>
#include "xcl2.hpp"


#define ROWA 30
#define COLA 2
#define COLB 2
#define iter 1

int main(int argc,char** argv){



static const int DATA_SIZE=2048;
size_t size_in_bytes=DATA_SIZE*sizeof(int);

std::cout<<"hey1"<<std::endl;
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
//cl::Program::Binaries bins;
cl::Program program;
cl::CommandQueue q;
cl::Kernel kernel_matmul;
cl::Buffer buffera,bufferb,bufferout;
size_t matrix_size=DATA_SIZE*DATA_SIZE;
size_t matrix_bytes=matrix_size*sizeof(int);
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
//std::cout<<"It works?"<<std::endl;
if(err != CL_SUCCESS){
std::cout<<"Unable to program device "<<device.getInfo<CL_DEVICE_NAME>()<<std::endl;
}
else{
std::cout<<"Programming of device "<<device.getInfo<CL_DEVICE_NAME>()<<" succesful"<<std::endl;
kernel_matmul = cl::Kernel(program,"matmul",&err);	//matmul is the name of the kernel top function
std::cout<<kernel_matmul.getInfo<CL_KERNEL_FUNCTION_NAME>()<<std::endl;
break;
}

}
buffera = cl::Buffer(context,CL_MEM_READ_ONLY,size_in_bytes,nullptr,&err);		//constructor->cant use buffera.Buffer to access member
bufferb = cl::Buffer(context,CL_MEM_READ_ONLY,size_in_bytes,nullptr,&err);
bufferout = cl::Buffer(context,CL_MEM_WRITE_ONLY,size_in_bytes,nullptr,&err);

////// Set kernel arguments /////////
int narg=0;
kernel_matmul.setArg(narg++,buffera);						//function not constructor=can use .
kernel_matmul.setArg(narg++,bufferb);
kernel_matmul.setArg(narg++,bufferout);
kernel_matmul.setArg(narg++,rowa);
kernel_matmul.setArg(narg++,cola);
kernel_matmul.setArg(narg++,colb);





ptr_a=(int*)q.enqueueMapBuffer(buffera,CL_TRUE,CL_MAP_WRITE,0,size_in_bytes,nullptr,nullptr,&err);
ptr_b=(int*)q.enqueueMapBuffer(bufferb,CL_TRUE,CL_MAP_WRITE,0,size_in_bytes,nullptr,nullptr,&err);
ptr_out=(int*)q.enqueueMapBuffer(bufferout,CL_TRUE,CL_MAP_READ,0,size_in_bytes,nullptr,nullptr,&err);

clock_t t_fpga1;
t_fpga1=clock();
const auto fpga_start{std::chrono::steady_clock::now()};
///////// Initialise pointers //////////
for(int o=0;o<iter;o++){
for(int i=0;i<DATA_SIZE;i++){
ptr_a[i]=1;//rand()%DATA_SIZE;
ptr_b[i]=1;//rand()%DATA_SIZE;


}
//////// Transfer buffer objects to the device //////////
q.enqueueMigrateMemObjects({buffera,bufferb},0);//CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED);

cl::Event event;
uint64_t timestart,timeend;
//////// Start kernel /////////
q.enqueueNDRangeKernel(kernel_matmul,1,1,1,nullptr,&event);			//enqueueTask is deprecated, enqueueNDRangeKernel equivalent
//q.enqueueTask(kernel_matmul);
q.finish();
/*event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &timestart);
event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &timeend);
auto simple_time = (timeend - timestart)*10e-09;

std::cout<<"Simple time:"<< simple_time<<" seconds"<<std::endl;*/

/////// send data back to host //////////
q.enqueueMigrateMemObjects({bufferout},CL_MIGRATE_MEM_OBJECT_HOST);



q.finish();		//wait until all previous commands have been executed

/*q.enqueueUnmapMemObject(buffera,ptr_a,nullptr,nullptr);
q.enqueueUnmapMemObject(bufferb,ptr_b,nullptr,nullptr);*/


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
}

t_fpga1=clock()-t_fpga1;
const auto fpga_end {std::chrono::steady_clock::now()};
const std::chrono::duration<double> fpga_time{fpga_end-fpga_start};


//////////// Calculate software result ////////////
clock_t t;
t=clock();
const auto cpu_start{std::chrono::steady_clock::now()};
int host_result[rowa*colb]={0};
for(int p = 0; p<iter; p++){
std::cout<<"Printing software results"<<std::endl;
for(int i=0;i<rowa*colb;i++){
	host_result[i] = 0;
}
int temph=0;
for(int i = 0;i < rowa;i++){
	for(int j = 0; j < colb;j++){
		//host_result[i*rowa + j]=0;
		for(int z = 0; z < cola; z++){
		host_result[i*colb + j]+=ptr_a[i*cola + z]*ptr_b[z*colb + j];
		//std::cout<<"host_result["<<i*rowa<<" + "<<j<<"]= "<<ptr_a[i*rowa+z]<<" "<<ptr_b[z*cola+j]<< std::endl;
		
		//std::cout<<host_result[i*rowa + j]<<" ";
		
}
		
		std::cout<<host_result[i*colb + j]<<" ";
		if(temph == colb-1) {
		std::cout<<std::endl;
		temph = 0;
		}
		else	{
		temph++;
}
}
}
}
std::cout<<std::endl;	
const auto cpu_end {std::chrono::steady_clock::now()};
t=clock()-t;
////////// Performance output /////////////
std::cout<<"CPU time first:"<<t/float(CLOCKS_PER_SEC)<<" seconds"<<std::endl;

const std::chrono::duration<double> start_time{cpu_end-cpu_start};
int total=(2*rowa*cola*colb-rowa*colb)*iter*sizeof(int);
std::cout<<"TB= "<<total<<std::endl;
std::cout<<"FPGA throughput: "<<(total/(fpga_time.count()*100))/(1024*1024)<<"MB/s "<<std::endl;
std::cout<<"CPU duration is "<<start_time.count()*100<<" Seconds"<<std::endl;
std::cout<<"FPGA duration is "<<fpga_time.count()*100<<" Seconds"<<std::endl;
std::cout<<"Speedup is "<<(double)start_time.count()/(double)fpga_time.count()<<std::endl;

clock_t t_fpga,t_fpga_start;
t_fpga_start=clock();
cl::Event event;
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
  fpga_exec_time_s += double((duration_nanosec * (1.0e-9))); // conversion to miliseconds
  }
  avg_fpga_exec_time = fpga_exec_time_s/iter;
std::cout<<"Fpga exec time: "<<fpga_exec_time_s<<" seconds"<<std::endl;
std::cout<<"throughput:"<<(total/fpga_exec_time_s)/(1024*1024)<<" MB/s "<<std::endl;
t_fpga=float(clock()-t_fpga_start);
std::cout<<"FPGA time first:"<<t_fpga/float(CLOCKS_PER_SEC)<<" seconds"<<std::endl;
std::cout<<"FPGA time second:"<<t_fpga1/float(CLOCKS_PER_SEC)<<" seconds"<<std::endl;
std::cout<<"throughput first:"<<(total/t_fpga)/(1024*1024)<<" MB/s "<<std::endl;

/*q.enqueueUnmapMemObject(buffera,ptr_a,nullptr,nullptr);
q.enqueueUnmapMemObject(bufferb,ptr_b,nullptr,nullptr);
q.enqueueUnmapMemObject(bufferout,ptr_out,nullptr,nullptr);*/
return 0;

}


