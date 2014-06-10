#ifndef IRCBOT_H_
#define IRCBOT_H_

#include <string>
#include "sock.h"
#include "irc.h"

class IrcBot: public IrcClient 
{
public:
 
  int start();			// overloading form IrcClient::start()

};


#endif /* IRCBOT_H_ */
