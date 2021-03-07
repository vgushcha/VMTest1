#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <stdexcept>
#include<unistd.h>
using namespace std;

//check correctness of the string that received from the file
void CheckString(string &s){
  if(s.size()>0){
 
     if(s.at(0)==' '){ //remove whitespaces
         do{
		 s.erase(s.begin()+0);
	 }while(s.at(0)==' ');	 
     }    
     
     stringstream stream(s);	  
    //if first symbol is not <</>> then exit 
     if (stream.peek()!=('/')){
        throw runtime_error("Error: Incorrect format in file, first symbol must be / .");
     }
   
    }else{//throw error if file is empty
      throw runtime_error("Error: File is empty");
   }
   //throw error if /dev/xxx is not available   
   if (access(s.c_str(),F_OK)==-1){
	 throw runtime_error("Error: no such disk/part: " + s);  
   }
}

//function GetLine open file and save line to the S
void GetLine(string &s){ 
  cout<<"Enter the name of the file, you would like to use:\n";	
  
  char filename[256];
  cin>>filename;
  
  ifstream input(filename);
  if(input)
  {
     getline(input,s);
  }
  else{
    throw runtime_error("Error: File is not available");
  }

  CheckString(s);//remove whitespaces from the beginning, format must be /...

}


//function Report
void GetReport(const string &s){
  string line;
  line = string ("lsblk ") + s + string (" -do TYPE,SIZE,FSTYPE,MOUNTPOINT");
  
  FILE *pipe = NULL;
  pipe = popen(line.c_str(),"r");
  if (pipe==NULL){
     throw runtime_error("Error: Cannot call system command - lsblk.\n");
  }

  char sign[2]="";
  stringstream stream;

  while (fread(sign,1,1,pipe)!=0) stream<<sign;
  fclose(pipe);
  string type,size,mountpoint,fstype;
   //skip labels
  for (int i=0; i<4; i++){
     stream>>type;
  }
  
  stream>>type; //keep type
  stream>>size; //keep size
  stream>>fstype; //keep filesystem type
  stream>>mountpoint;//keep mountpoint

  if (type=="part")
  {
     //need to check free size of the part in MB. Will use command df -BM
     line=string("df ") + s + string (" -BM");
     pipe=popen(line.c_str(),"r");

     if (pipe==NULL){
	throw runtime_error("Error: Cannot call system command - df.\n");
     }
     stream.str("");
     //read result of df
     while (fread(sign,1,1,pipe)!=0) stream<<sign;
     fclose(pipe);
     string freesize;

     //skip data in stream that is not required
     for (int i=0; i<9; i++){
           stream>>freesize;
     }
     stream>>freesize;//keep freesize
     cout<< s<< " " << type << " " << size << " "<<freesize<<" "
	  <<fstype<<" "<<mountpoint<<endl;
  }
  else{
     cout<<s<<" " << type << " " << size << endl;
  }

}



int main()
{
  string s;     //keep line from the file
  try{
     GetLine(s);
  }catch(runtime_error &e) {
     cout<<e.what()<<endl;
     cout<<"Program stopped.\n";
     return -1;
  }
  try{
     GetReport(s); //get report about disks
  } catch(runtime_error &e) {
      cout<<e.what()<<endl;
      cout<<"Program stopped.\n";
      return -1;
  }   
  return 0;
}
