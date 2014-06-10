#include "bot.h"
#include "cfgfile.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  IrcBot bot;
  CfgFile configFile("mybot.cfg");
  
  int botStatus = 1;			// integer used to check if we need to exit or restart the bot (0 = shutdown, 1 = restart)

  std::string host;
  std::string port;
  std::string botnick;
  std::string channel;


  cout << endl << "Welcome to my own personal IRC bot! I decided to write a bot to brush up on my C/C++, socket and IRC skills" << endl << "although bots are commonly accociated with malicious intent, I only use this for educational purposes and it should never be used in or against an environment not your own" << endl << endl << endl;


  cout << "Initialising configuration file..." << endl << endl;

  if (configFile.keyExists("host"))
  {
    host = configFile.getValueOfKey("host");
    cout << "host is: " << host << endl;
  } else
  {
    cout << "Can't find host in configuration file" << endl;
  }
 
  if (configFile.keyExists("port"))
  {
    port = configFile.getValueOfKey("port");
    cout << "port is: " << port << endl;
  }

  if (configFile.keyExists("botnick"))
  {
    botnick = configFile.getValueOfKey("botnick");
    cout << "Bot nickname will be: " << botnick << endl;
  }

  if (configFile.keyExists("channel"))
  {
    channel = configFile.getValueOfKey("channel");
    cout << "the channel will be: " << channel << endl;
  }

  cout << endl << endl << "Starting bot initialisation fase..." << endl << endl;

  while(botStatus == 1)
  {
    bot.connect(host.c_str(), port.c_str(), botnick.c_str(), channel.c_str());                            // connect bot to IRC server and C&C channel
    botStatus = bot.start();										// start listening for commands
  }

  return 0;
}
