#include "irc.h"
#include "sock.h"
#include "cfgfile.h"
#include "mytime.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;


void IrcClient::connect(const char *_host, const char *_port, const char *_nick, const char *_channel)
{ 
  port = _port;
  host = _host;
  nick = _nick;
  channel = _channel;

  char buf[MAXDATASIZE];						// This will hold the received socket data
  IrcMessage ircMsg; 							// used to parse received Irc messages and sending Irc messages to the server

  string parameters;							// parameters to be sent to IRC server (used by IrcMessage::send(socket, command, parameters))

  int nickadd = 2;							// integer that will be appended to a nick when it's already in use


  // Starting the session

  sockfd.link(host, atoi(port));					// Open the socket and connect to host:port

  parameters = nick; parameters += " 0 * :"; parameters += nick;	// Construct the USER command parameters
  ircMsg.send(sockfd, "USER", parameters);				// and send it to the IRC server
 
  ircMsg.send(sockfd, "NICK", nick);					// register your Nick. TODO have to do some checking to see if the server accepts my nick

  while (sockfd.in(buf)) 						// loop until no data is received anymore
  {
    ircMsg.recv(buf);							// This will parse the received message into seperate variables for easy matching
 
    if(ircMsg.command == "PING")					// IRC server sends occational PING message. client has to reply within 30 seconds otherwise will be disconnected
    { 
      ircMsg.send(sockfd, "PONG", ircMsg.parameters);			// reply to IRC server with PONG message
    }
    else if(ircMsg.command == ERR_NICKNAMEINUSE)			// if the nick is already in use we have to change our nick to be able to logon
    {
        stringstream tmpnick;						// create a temporary stringstream to try and find a free nickname. 
        tmpnick << nick << nickadd;					// add an integer (nickadd) to the nick

        ircMsg.send(sockfd, "NICK", tmpnick.str().c_str());		// try to set the nick again now we've increased the nickname trailing number

	nickadd++;							// increase the integer in case this nick is taken as well
    }
    else if(ircMsg.command == ERR_NOMOTD || ircMsg.command == RPL_ENDOFMOTD)  // if the end of the MOTD is seen or if there is no MOTD the initialiation is complete. 
    {
      ircMsg.send(sockfd, "MODE", nick);
      ircMsg.send(sockfd, "JOIN", channel);
    } 
    else if(ircMsg.command == RPL_ENDOFNAMES) 				// initialisation is complete when we've joined the channel and received the list of names
    {
      break;
    }
  }

  cout << endl << endl << "Initialisation complete, listening for commands...." << endl << endl;

// TODO check if i have to do another receive to make sure I've received everything before moving onto the main while loop?

}

int IrcClient::start()							// This function contains the main logic for the client. barebones client only handles PING/PONG and replies to PRIVMSG's
{
  char buf[MAXDATASIZE];
  IrcMessage ircMsg;

  string parameters;                                                    // parameters to be sent to IRC server
  
  while(sockfd.in(buf))
  {
    
    ircMsg.recv(buf);	                                                // This will parse the received message into seperate variables for easy matching

    if(ircMsg.command == "PING")                                        // IRC server sends occational PING message. client has to reply within 30 seconds otherwise will be disconnected
    {
      ircMsg.send(sockfd, "PONG", ircMsg.parameters);
    }

    else if(ircMsg.command == "PRIVMSG")                                // Check if we receive a PRIVMSG
    {
      parameters = channel; parameters += " You Talking to me?";
      ircMsg.send(sockfd, "PRIVMSG", parameters); 
    } 

  } // end of while loop (Bot will drop connection)

  sockfd.unlink();							// close the socket as we're no longer receiving data
  return 0;
}


IrcClient::IrcClient()
{
  // Do i need to init anything?
   
}

IrcClient::~IrcClient()
{
  sockfd.unlink();                                                      // close the socket as we're no longer receiving data
}



IrcMessage::IrcMessage()
{
}


void IrcMessage::recv(string buf)
{
  string::size_type prefixPos, commandPos, msgtargetPos;

  if(buf.find(':') == 0)                                // If the first character is : it means a prefix was supplied
  {
    prefixPos = buf.find(' ');                          // Find the first whitespace character and...
    prefix = buf.substr(0, prefixPos);                  // capture the prefix by using a substring

    if(prefix.find('!'))                                // if a ! is present it means a prefix in format nick!user@host was received
    {
      prefixNick = prefix.substr(1, prefix.find('!')-1);                // extract the username from the prefix string (omit initial : and capture till first occurance of '!')
      prefixUser = prefix.substr(prefix.find('!')+1, prefix.find('@')-1);
      prefixHost = prefix.substr(prefix.find('@')+1, prefixPos);
    } else
    {
      prefixNick = prefix;                              // set the prefixNick as prefix 
    }

    buf.erase(0, prefixPos+1);                          // Removes the prefix incl whitespace delimiter from the buf variable
  } else
  {
     prefix.clear();                                    // There is no prefix provided so make sure to empty the variables
     prefixNick.clear();
     prefixUser.clear();
     prefixHost.clear();
  }

  if(buf.find(' ') != string::npos)                     // the next whitespace character marks the beginning of the command
  {
    commandPos = buf.find(' ');
    command = buf.substr(0, commandPos);
    buf.erase(0, commandPos+1);                         // remove the command and whitespace from the buf variable
  }

  if(command == "PRIVMSG")                              // If the command is a PRIVMSG lets get the msgtarget and put it in a variable
  {
    msgtargetPos = buf.find(' ');
    msgtarget = buf.substr(0, msgtargetPos);
    buf.erase(0, msgtargetPos+1);                       // erase the msgtarget from the buffer so we're left with the remaining parameters
  }

  parameters = buf;                                     // After stripping the (optional) prefix and the command all thats left are the parameters

  if(prefix.empty())
  {
    cout << "Recv " << currentDateTime() << "   " << command << " " << parameters;
  } else
  {
    cout << "Recv " << currentDateTime() << "   " << prefixNick << " " << command << " " << parameters;
  }
} // IrcMessage Init


void IrcMessage::send(plug sockfd, std::string cmd, std::string pars)
{
  if(pars.at( pars.length() - 1) == '\n')               // if a trailing \r\n is present make sure not to add it
  {
    cout << "Sent " << currentDateTime() << "   " << cmd << " " << pars;
    sockfd << cmd.c_str() << " " << pars.c_str();
  } else                                                // else add it
  {
    cout << "Sent " << currentDateTime() << "   " << cmd << " " << pars << endl;
    sockfd << cmd.c_str() << " " << pars.c_str() << "\r\n";
  }
}

