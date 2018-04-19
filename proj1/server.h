#ifndef SERVER_H_
#define SERVER_H_
#include <vector>
#include <string>
using namespace std;
class requests{
 public:
  void parse_requests();
  
 private:
  vector<string> requests;
};


#endif
