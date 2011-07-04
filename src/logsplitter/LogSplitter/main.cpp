#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cassert>

#include "LogSplitter/NumberedFilesSet.hpp"
#include "LogSplitter/Parser.hpp"

using namespace std;
using namespace LogSplitter;


int main(void)
{
  int ret=0;
  try
  {
    NumberedFilesSet files("thread_");

    while(true)
    {
      string line;
      // read single line
      getline(cin, line);
      if(!cin)
        break;
      // parse thread number
      const Parser parse(line);
      // append to the proper file
      // TODO
    }
    // TODO
  }
  catch(const std::exception &ex)
  {
    cerr<<"exception: "<<ex.what()<<endl;
    ret=40;
  }
  catch(...)
  {
    cerr<<"unknown expection..."<<endl;
    ret=50;
  }

  return ret;
}
